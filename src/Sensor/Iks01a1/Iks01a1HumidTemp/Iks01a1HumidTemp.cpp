/*******************************************************************************
 * Copyright (C) 2018 Gallium Studio LLC (Lawrence Lo). All rights reserved.
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
#include "GpioInInterface.h"
#include "SensorHumidTempInterface.h"
#include "Iks01a1HumidTemp.h"
#include "x_nucleo_iks01a1_temperature.h"

FW_DEFINE_THIS_FILE("Iks01a1HumidTemp.cpp")

namespace APP {

Iks01a1HumidTemp::Iks01a1HumidTemp(Hsmn drdyHsmn, I2C_HandleTypeDef &hal) :
    SensorHumidTemp((QStateHandler)&Iks01a1HumidTemp::InitialPseudoState, IKS01A1_HUMID_TEMP, "IKS01A1_HUMID_TEMP"),
    m_drdyHsmn(drdyHsmn), m_hal(hal), m_stateTimer(GetHsm().GetHsmn(), STATE_TIMER), m_handle(NULL) {
}

QState Iks01a1HumidTemp::InitialPseudoState(Iks01a1HumidTemp * const me, QEvt const * const e) {
    (void)e;
    return Q_TRAN(&Iks01a1HumidTemp::Root);
}

QState Iks01a1HumidTemp::Root(Iks01a1HumidTemp * const me, QEvt const * const e) {
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
            return Q_TRAN(&Iks01a1HumidTemp::Stopped);
        }
        case SENSOR_HUMID_TEMP_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new SensorHumidTempStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_STATE, GET_HSMN());
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case SENSOR_HUMID_TEMP_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&Iks01a1HumidTemp::Stopping);
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState Iks01a1HumidTemp::Stopped(Iks01a1HumidTemp * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case SENSOR_HUMID_TEMP_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new SensorHumidTempStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case SENSOR_HUMID_TEMP_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&Iks01a1HumidTemp::Starting);
        }
    }
    return Q_SUPER(&Iks01a1HumidTemp::Root);
}

QState Iks01a1HumidTemp::Starting(Iks01a1HumidTemp * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = SensorHumidTempStartReq::TIMEOUT_MS;
            FW_ASSERT(timeout > GpioInStartReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new GpioInStartReq(me->m_drdyHsmn, GET_HSMN(), GEN_SEQ());
            me->GetHsm().SaveOutSeq(*evt);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            me->m_stateTimer.Stop();
            me->GetHsm().ClearInSeq();
            return Q_HANDLED();
        }
        case GPIO_IN_START_CFM: {
            EVENT(e);
            ErrorEvt const &cfm = ERROR_EVT_CAST(*e);
            bool allReceived;
            if (!me->GetHsm().HandleCfmRsp(cfm, allReceived)) {
                Evt *evt = new Failed(GET_HSMN(), cfm.GetError(), cfm.GetOrigin(), cfm.GetReason());
                me->PostSync(evt);
            } else if (allReceived) {
                Evt *evt = new Evt(DONE, GET_HSMN());
                me->PostSync(evt);
            }
            return Q_HANDLED();
        }
        case FAILED:
        case STATE_TIMER: {
            EVENT(e);
            Evt *evt;
            if (e->sig == FAILED) {
                ErrorEvt const &failed = ERROR_EVT_CAST(*e);
                evt = new SensorHumidTempStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(),
                                            failed.GetError(), failed.GetOrigin(), failed.GetReason());
            } else {
                evt = new SensorHumidTempStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_TIMEOUT, GET_HSMN());
            }
            Fw::Post(evt);
            return Q_TRAN(&Iks01a1HumidTemp::Stopping);
        }
        case DONE: {
            EVENT(e);
            Evt *evt = new SensorHumidTempStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&Iks01a1HumidTemp::Started);
        }
    }
    return Q_SUPER(&Iks01a1HumidTemp::Root);
}

QState Iks01a1HumidTemp::Stopping(Iks01a1HumidTemp * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = SensorHumidTempStopReq::TIMEOUT_MS;
            FW_ASSERT(timeout > GpioInStopReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new GpioInStopReq(me->m_drdyHsmn, GET_HSMN(), GEN_SEQ());
            me->GetHsm().SaveOutSeq(*evt);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            me->m_stateTimer.Stop();
            me->GetHsm().ClearInSeq();
            me->GetHsm().Recall();
            return Q_HANDLED();
        }
        case SENSOR_HUMID_TEMP_STOP_REQ: {
            EVENT(e);
            me->GetHsm().Defer(e);
            return Q_HANDLED();
        }
        case GPIO_IN_STOP_CFM: {
            EVENT(e);
            ErrorEvt const &cfm = ERROR_EVT_CAST(*e);
            bool allReceived;
            if (!me->GetHsm().HandleCfmRsp(cfm, allReceived)) {
                Evt *evt = new Failed(GET_HSMN(), cfm.GetError(), cfm.GetOrigin(), cfm.GetReason());
                me->PostSync(evt);
            } else if (allReceived) {
                Evt *evt = new Evt(DONE, GET_HSMN());
                me->PostSync(evt);
            }
            return Q_HANDLED();
        }
        case FAILED:
        case STATE_TIMER: {
            EVENT(e);
            FW_ASSERT(0);
            // Will not reach here.
            return Q_HANDLED();
        }
        case DONE: {
            EVENT(e);
            Evt *evt = new SensorHumidTempStopCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&Iks01a1HumidTemp::Stopped);
        }
    }
    return Q_SUPER(&Iks01a1HumidTemp::Root);
}

QState Iks01a1HumidTemp::Started(Iks01a1HumidTemp * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            DrvStatusTypeDef status = BSP_TEMPERATURE_Init(HTS221_T_0, &me->m_handle);
            FW_ASSERT(status == COMPONENT_OK);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            DrvStatusTypeDef status = BSP_TEMPERATURE_DeInit(&me->m_handle);
            FW_ASSERT(status == COMPONENT_OK);
            return Q_HANDLED();
        }
        // Test only.
        case GPIO_IN_ACTIVE_IND:
        case GPIO_IN_INACTIVE_IND: {
            EVENT(e);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Iks01a1HumidTemp::Root);
}

/*
QState Iks01a1HumidTemp::MyState(Iks01a1HumidTemp * const me, QEvt const * const e) {
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
            return Q_TRAN(&Iks01a1HumidTemp::SubState);
        }
    }
    return Q_SUPER(&Iks01a1HumidTemp::SuperState);
}
*/

} // namespace APP
