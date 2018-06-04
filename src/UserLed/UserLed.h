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

#ifndef USER_LED_H
#define USER_LED_H

#include "qpcpp.h"
#include "fw_active.h"
#include "fw_timer.h"
#include "fw_evt.h"
#include "app_hsmn.h"
#include "LedPattern.h"

using namespace QP;
using namespace FW;

namespace APP {

class UserLed : public Active {
public:
    UserLed(Hsmn hsmn, char const *name);

protected:
    static QState InitialPseudoState(UserLed * const me, QEvt const * const e);
    static QState Root(UserLed * const me, QEvt const * const e);
        static QState Stopped(UserLed * const me, QEvt const * const e);
        static QState Started(UserLed * const me, QEvt const * const e);
            static QState Idle(UserLed * const me, QEvt const * const e);
            static QState Active(UserLed * const me, QEvt const * const e);
                static QState Repeating(UserLed * const me, QEvt const * const e);
                static QState Once(UserLed * const me, QEvt const * const e);

    void InitGpio();
    void DeInitGpio();
    void ConfigPwm(uint32_t levelPermil = 500);
    void StartPwm(TIM_HandleTypeDef *hal);
    void StopPwm(TIM_HandleTypeDef *hal);

    typedef struct {
        Hsmn hsmn;
        GPIO_TypeDef *port;
        uint16_t pin;
        bool activeHigh;
        uint32_t mode;
        uint32_t pull;
        uint32_t af;
        TIM_TypeDef *pwmTimer;
        uint32_t pwmChannel;
        bool pwmComplementary;
        LedPatternSet const &patternSet;
    } Config;
    static Config const CONFIG[];

    Config const *m_config;
    LedPattern const *m_currPattern;
    uint32_t m_intervalIndex;
    bool m_isRepeat;
    Timer m_intervalTimer;

#define USER_LED_TIMER_EVT \
    ADD_EVT(INTERVAL_TIMER)

#define USER_LED_INTERNAL_EVT \
    ADD_EVT(DONE) \
    ADD_EVT(NEXT_INTERVAL) \
    ADD_EVT(LAST_INTERVAL)

#undef ADD_EVT
#define ADD_EVT(e_) e_,

    enum {
        USER_LED_TIMER_EVT_START = TIMER_EVT_START(USER_LED),
        USER_LED_TIMER_EVT
    };
    enum {
        USER_LED_INTERNAL_EVT_START = INTERNAL_EVT_START(USER_LED),
        USER_LED_INTERNAL_EVT
    };
};

} // namespace APP

#endif // USER_LED_H

