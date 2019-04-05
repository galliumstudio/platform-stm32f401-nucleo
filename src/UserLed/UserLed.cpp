/*******************************************************************************
 * Copyright (C) Gallium Studio LLC. All rights reserved.
 *
 * This program is open source software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, this program may be distributed and modified under the
 * terms of Gallium Studio LLC commercial licenses, which expressly supersede
 * the GNU General Public License and are specifically designed for licensees
 * interested in retaining the proprietary status of their code.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contact information:
 * Website - https://www.galliumstudio.com
 * Source repository - https://github.com/galliumstudio
 * Email - admin@galliumstudio.com
 ******************************************************************************/

#include "app_hsmn.h"
#include "fw_log.h"
#include "fw_assert.h"
#include "fw_active.h"
#include "UserLedInterface.h"
#include "UserLed.h"
#include "LedPattern.h"
#include "periph.h"

FW_DEFINE_THIS_FILE("UserLed.cpp")

namespace APP {

#undef ADD_EVT
#define ADD_EVT(e_) #e_,

static char const * const timerEvtName[] = {
    "USER_LED_TIMER_EVT_START",
    USER_LED_TIMER_EVT
};

static char const * const internalEvtName[] = {
    "USER_LED_INTERNAL_EVT_START",
    USER_LED_INTERNAL_EVT
};

static char const * const interfaceEvtName[] = {
    "USER_LED_INTERFACE_EVT_START",
    USER_LED_INTERFACE_EVT
};

// Define LED configurations.
// Set pwmTimer to NULL if PWM is not supported for an LED (no brightness control).
// If pwmTimer is NULL, af and pwmChannel are don't-care, and mode must be OUTPUT_PP or OUTPUT_OD.
UserLed::Config const UserLed::CONFIG[] = {
    { USER_LED, GPIOA, GPIO_PIN_5, true, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2, TIM2, TIM_CHANNEL_1, false, TEST_LED_PATTERN_SET },
    { TEST_LED, GPIOC, GPIO_PIN_7, true, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF2_TIM3, TIM3, TIM_CHANNEL_2, false, TEST_LED_PATTERN_SET },
    // Add more LED here.
};

// Corresponds to what was done in _msp.cpp file.
void UserLed::InitGpio() {
    // Clock has been initialized by System via Periph.
    GPIO_InitTypeDef gpioInit;
    gpioInit.Pin = m_config->pin;
    gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpioInit.Mode = m_config->mode;
    gpioInit.Pull = m_config->pull;
    gpioInit.Alternate = m_config->af;
    if (m_config->pwmTimer) {
        FW_ASSERT((m_config->mode == GPIO_MODE_AF_PP) || (m_config->mode == GPIO_MODE_AF_OD));
    }
    else {
        FW_ASSERT((m_config->mode == GPIO_MODE_OUTPUT_PP) || (m_config->mode == GPIO_MODE_OUTPUT_OD));
    }
    HAL_GPIO_Init(m_config->port, &gpioInit);
}

void UserLed::DeInitGpio() {
    if (m_config->pwmTimer) {
        StopPwm(Periph::GetHal(m_config->pwmTimer));
    }
    HAL_GPIO_DeInit(m_config->port, m_config->pin);
}

void UserLed::ConfigPwm(uint32_t levelPermil) {
    // If PWM is not supported, turn off GPIO if level = 0; turn on GPIO if level > 0.
    if (m_config->pwmTimer == NULL) {
        if (levelPermil == 0) {
            HAL_GPIO_WritePin(m_config->port, m_config->pin, m_config->activeHigh ? GPIO_PIN_RESET : GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(m_config->port, m_config->pin, m_config->activeHigh ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
        return;
    }
    // Here PWM is supported.
    FW_ASSERT(levelPermil <= 1000);
    if (!m_config->activeHigh) {
        levelPermil = 1000 - levelPermil;
    }
    // Base PWM timer has been initialized by System via Periph.
    TIM_HandleTypeDef *hal = Periph::GetHal(m_config->pwmTimer);
    StopPwm(hal);
    TIM_OC_InitTypeDef timConfig;
    timConfig.OCMode       = TIM_OCMODE_PWM1;
    timConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    timConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    timConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    timConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    timConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
    timConfig.Pulse        = (hal->Init.Period + 1) * levelPermil / 1000;
    HAL_StatusTypeDef status = HAL_TIM_PWM_ConfigChannel(hal, &timConfig, m_config->pwmChannel);
    FW_ASSERT(status== HAL_OK);
    StartPwm(hal);
}

void UserLed::StartPwm(TIM_HandleTypeDef *hal) {
    FW_ASSERT(hal);
    HAL_StatusTypeDef status;
    if (m_config->pwmComplementary) {
        status = HAL_TIMEx_PWMN_Start(hal, m_config->pwmChannel);
    } else {
        status = HAL_TIM_PWM_Start(hal, m_config->pwmChannel);
    }
    FW_ASSERT(status == HAL_OK);
}


void UserLed::StopPwm(TIM_HandleTypeDef *hal) {
    FW_ASSERT(hal);
    HAL_StatusTypeDef status;
    if (m_config->pwmComplementary) {
        status = HAL_TIMEx_PWMN_Stop(hal, m_config->pwmChannel);
    } else {
        status = HAL_TIM_PWM_Stop(hal, m_config->pwmChannel);
    }
    FW_ASSERT(status == HAL_OK);
}

UserLed::UserLed(Hsmn hsmn, char const *name) :
    FW::Active((QStateHandler)&UserLed::InitialPseudoState, hsmn, name),
    m_config(NULL), m_currPattern(NULL), m_intervalIndex(0), m_isRepeat(false),
    m_intervalTimer(GetHsm().GetHsmn(), INTERVAL_TIMER) {
    SET_EVT_NAME(USER_LED);
    uint32_t i;
    for (i = 0; i < ARRAY_COUNT(CONFIG); i++) {
        if (CONFIG[i].hsmn == GetHsm().GetHsmn()) {
            m_config = &CONFIG[i];
            break;
        }
    }
    FW_ASSERT(i < ARRAY_COUNT(CONFIG));
}

QState UserLed::InitialPseudoState(UserLed * const me, QEvt const * const e) {
    (void)e;
    return Q_TRAN(&UserLed::Root);
}

QState UserLed::Root(UserLed * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&UserLed::Stopped);
        }
        case USER_LED_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_STATE, GET_HSMN());
            Fw::Post(evt);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState UserLed::Stopped(UserLed * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case USER_LED_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case USER_LED_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(),ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&UserLed::Started);
        }
    }
    return Q_SUPER(&UserLed::Root);
}

QState UserLed::Started(UserLed * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            me->InitGpio();
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            me->DeInitGpio();
            return Q_HANDLED();
        }
        //case Q_INIT_SIG: {
        //    return Q_TRAN(&UserLed::Idle);
        //}
        case USER_LED_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new UserLedStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&UserLed::Stopped);
        }
    }
    return Q_SUPER(&UserLed::Root);
}

/*
QState UserLed::MyState(UserLed * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&UserLed::SubState);
        }
    }
    return Q_SUPER(&UserLed::SuperState);
}
*/

} // namespace APP
