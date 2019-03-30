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

#include "fw_log.h"
#include "fw_assert.h"
#include "app_hsmn.h"
#include "periph.h"
#include "System.h"
#include "SystemInterface.h"
#include "GpioInInterface.h"
#include "CompositeActInterface.h"
#include "SimpleActInterface.h"
#include "DemoInterface.h"
#include "UserLedInterface.h"
#include "AOWashingMachineInterface.h"
#include "TrafficInterface.h"
#include "LevelMeterInterface.h"
#include "SensorInterface.h"
#include "DispInterface.h"
#include "bsp.h"
#include <vector>
#include <memory>

FW_DEFINE_THIS_FILE("System.cpp")

using namespace FW;
using namespace APP;

void LCD_Config(void);

namespace APP {

static char const * const timerEvtName[] = {
    "STATE_TIMER",
    "TEST_TIMER",
};

static char const * const internalEvtName[] = {
    "DONE",
    "RESTART",
};

static char const * const interfaceEvtName[] = {
    "SYSTEM_START_REQ",
    "SYSTEM_START_CFM",
    "SYSTEM_STOP_REQ",
    "SYSTEM_STOP_CFM",
};

System::System() :
    Active((QStateHandler)&System::InitialPseudoState, SYSTEM, "SYSTEM"),
    m_stateTimer(this->GetHsm().GetHsmn(), STATE_TIMER),
    m_testTimer(this->GetHsm().GetHsmn(), TEST_TIMER) {
    SET_EVT_NAME(SYSTEM);
}

QState System::InitialPseudoState(System * const me, QEvt const * const e) {
    (void)e;
    return Q_TRAN(&System::Root);
}

QState System::Root(System * const me, QEvt const * const e) {
    QState status;

    switch (e->sig) {
    case Q_ENTRY_SIG: {
        EVENT(e);
        Periph::SetupNormal();
        // Test only

        __HAL_RCC_GPIOA_CLK_ENABLE();
        // -2- Configure IO in output push-pull mode to drive external LEDs
        GPIO_InitTypeDef  GPIO_InitStruct;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull  = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

        // Test only - Configures USER LED
        //GPIO_InitStruct.Pin = GPIO_PIN_5;
        //HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // Test only - Configures test pins.
        /*
        GPIO_InitStruct.Pin = GPIO_PIN_11;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
        */

        me->m_testTimer.Start(200, Timer::PERIODIC);

        Evt *evt = new CompositeActStartReq(COMPOSITE_ACT, SYSTEM, 0);
        Fw::Post(evt);
        evt = new SimpleActStartReq(SIMPLE_ACT, SYSTEM, 0);
        Fw::Post(evt);
        evt = new DemoStartReq(DEMO, SYSTEM, 0);
        Fw::Post(evt);
        //evt = new UserLedStartReq(USER_LED, SYSTEM, 0);
        //Fw::Post(evt);
        //evt = new UserLedStartReq(TEST_LED, SYSTEM, 0);
        //Fw::Post(evt);
        evt = new WashStartReq(AO_WASHING_MACHINE, SYSTEM, 0);
        Fw::Post(evt);
        evt = new TrafficStartReq(TRAFFIC, SYSTEM, 0);
        Fw::Post(evt);

        // Test only.
        evt = new GpioInStartReq(USER_BTN, SYSTEM, 0);
        Fw::Post(evt);

        status = Q_HANDLED();
        break;
    }
    case Q_EXIT_SIG: {
        EVENT(e);
        // Test only.
        me->m_testTimer.Stop();
        status = Q_HANDLED();
        break;
    }
    case TEST_TIMER: {
        EVENT(e);
        // Test only.
        //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
        //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
        //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);

        static int testCount = 0;
        // Delays the start of IKS01A1 IMU sensor by 200ms from power up.
        // There is a gitch at about 200ms on SDA/SCL (PB8 and PB9) of I2C1 connected to the IKS01A1 board.
        // We need to initialize the I2C1 bus after the glitch to avoid communication issues.
        // That glitch is found to be caused by the 2.8" TFT board.
        if (testCount++ == 0) {
            Evt *evt = new SensorStartReq(IKS01A1, SYSTEM, 0);
            Fw::Post(evt);
            evt = new LevelMeterStartReq(LEVEL_METER, SYSTEM, 0);
            Fw::Post(evt);
        }

        status = Q_HANDLED();
        break;
    }
    case SENSOR_START_CFM:
    case SIMPLE_ACT_START_CFM:
    case COMPOSITE_ACT_START_CFM:
    case DEMO_START_CFM:
    case USER_LED_START_CFM:
    case WASH_START_CFM:
    case TRAFFIC_START_CFM:
    case LEVEL_METER_START_CFM: {
        EVENT(e);
        return Q_HANDLED();
    }
    case GPIO_IN_START_CFM: {
        EVENT(e);
        return Q_HANDLED();
    }
    case GPIO_IN_ACTIVE_IND: {
        EVENT(e);
        Evt *evt = new UserLedPatternReq(TEST_LED, GET_HSMN(), GEN_SEQ(), 0, true);
        Fw::Post(evt);
        return Q_HANDLED();
    }
    case GPIO_IN_INACTIVE_IND: {
        EVENT(e);
        Evt *evt = new UserLedOffReq(TEST_LED, GET_HSMN(), GEN_SEQ());
        Fw::Post(evt);
        return Q_HANDLED();
    }
    default: {
        status = Q_SUPER(&QHsm::top);
        break;
    }
    }
    return status;
}


} // namespace APP
