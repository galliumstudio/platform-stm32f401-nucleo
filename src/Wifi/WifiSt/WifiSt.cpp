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

#include <cstdio>
#include "app_hsmn.h"
#include "fw_log.h"
#include "fw_assert.h"
#include "fw_pipe.h"
#include "UartActInterface.h"
#include "UartOutInterface.h"
#include "UartInInterface.h"
#include "UartAct.h"
#include "WifiInterface.h"
#include "WifiSt.h"

FW_DEFINE_THIS_FILE("WifiSt.cpp")

namespace APP {

void WifiSt::Write(char *const atCmd) {
    FW_ASSERT(atCmd);
    FW_ASSERT(m_outIfHsmn != HSM_UNDEF);
    bool status;
    // @todo - Check result against atCmd length.
    uint32_t result = m_outFifo.Write(reinterpret_cast<uint8_t const *>(atCmd), strlen(atCmd), &status);
    if (status) {
        Evt *evt = new Evt(UART_OUT_WRITE_REQ, m_outIfHsmn);
        Fw::Post(evt);
    }
}

WifiSt::WifiSt() :
    Wifi((QStateHandler)&WifiSt::InitialPseudoState, WIFI_ST, "WIFI_ST"),
        m_ifHsmn(HSM_UNDEF), m_outIfHsmn(HSM_UNDEF), m_consoleOutIfHsmn(HSM_UNDEF),
        m_outFifo(m_outFifoStor, OUT_FIFO_ORDER),
        m_inFifo(m_inFifoStor, IN_FIFO_ORDER),
        m_stateTimer(GetHsm().GetHsmn(), STATE_TIMER) {
}

QState WifiSt::InitialPseudoState(WifiSt * const me, QEvt const * const e) {
    (void)e;
    return Q_TRAN(&WifiSt::Root);
}

QState WifiSt::Root(WifiSt * const me, QEvt const * const e) {
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
            return Q_TRAN(&WifiSt::Stopped);
        }
        case WIFI_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new WifiStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_STATE, GET_HSMN());
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case WIFI_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&WifiSt::Stopping);
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState WifiSt::Stopped(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case WIFI_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new WifiStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case WIFI_START_REQ: {
            EVENT(e);
            WifiStartReq const &req = static_cast<WifiStartReq const &>(*e);
            // Remember the assigned interface active object.
            me->m_ifHsmn = req.GetIfHsmn();
            LOG("ifHsmn = %d", me->m_ifHsmn);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&WifiSt::Starting);
        }
    }
    return Q_SUPER(&WifiSt::Root);
}

QState WifiSt::Starting(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = WifiStartReq::TIMEOUT_MS;
            FW_ASSERT(timeout > UartActStartReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new UartActStartReq(me->m_ifHsmn, GET_HSMN(), GEN_SEQ(), &me->m_outFifo, &me->m_inFifo);
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
        case UART_ACT_START_CFM: {
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
                evt = new WifiStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(),
                                            failed.GetError(), failed.GetOrigin(), failed.GetReason());
            } else {
                evt = new WifiStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_TIMEOUT, GET_HSMN());
            }
            Fw::Post(evt);
            return Q_TRAN(&WifiSt::Stopping);
        }
        case DONE: {
            EVENT(e);
            Evt *evt = new WifiStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&WifiSt::Started);
        }
    }
    return Q_SUPER(&WifiSt::Root);
}

QState WifiSt::Stopping(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = WifiStopReq::TIMEOUT_MS;
            FW_ASSERT(timeout > UartActStopReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new UartActStopReq(me->m_ifHsmn, GET_HSMN(), GEN_SEQ());
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
        case WIFI_STOP_REQ: {
            EVENT(e);
            me->GetHsm().Defer(e);
            return Q_HANDLED();
        }
        case UART_ACT_STOP_CFM: {
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
            Evt *evt = new WifiStopCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&WifiSt::Stopped);
        }
    }
    return Q_SUPER(&WifiSt::Root);
}

QState WifiSt::Started(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            me->m_outIfHsmn = UartAct::GetUartOutHsmn(me->m_ifHsmn);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&WifiSt::Normal);
        }
    }
    return Q_SUPER(&WifiSt::Root);
}

QState WifiSt::Normal(WifiSt * const me, QEvt const * const e) {
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
            return Q_TRAN(&WifiSt::Disconnected);
        }
        case WIFI_INTERACTIVE_ON_REQ: {
            EVENT(e);
            WifiInteractiveOnReq const &req = static_cast<WifiInteractiveOnReq const &>(*e);
            me->m_consoleOutIfHsmn = req.GetConsoleOutIfHsmn();
            FW_ASSERT(me->m_consoleOutIfHsmn != HSM_UNDEF);
            Evt *evt = new WifiInteractiveOnCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), me->m_outIfHsmn, ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&WifiSt::Interactive);
        }
        case WIFI_CONNECT_REQ: {
            EVENT(e);
            WifiConnectReq const &req = static_cast<WifiConnectReq const &>(*e);
            LOG("Connecting to %s:%d", req.GetDomain(), req.GetPort());
            char cmd[100];
            snprintf(cmd, sizeof(cmd), "at+s.sockon=%s,%d,,t\n\r", req.GetDomain(), req.GetPort());
            me->Write(cmd);
            return Q_TRAN(&WifiSt::Connected);
        }
        case WIFI_DISCONNECT_REQ: {
            EVENT(e);
            char cmd[100];
            snprintf(cmd, sizeof(cmd), "at+s.sockc=%d\n\r", 0);
            me->Write(cmd);
            return Q_TRAN(&WifiSt::Disconnected);
        }
    }
    return Q_SUPER(&WifiSt::Started);
}

QState WifiSt::Disconnected(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&WifiSt::Normal);
}

QState WifiSt::Connected(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case WIFI_SEND_REQ: {
            EVENT(e);
            WifiSendReq const &req = static_cast<WifiSendReq const &>(*e);
            char cmd[150];
            char const *data = req.GetData();
            LOG("Send '%s'", data);
            snprintf(cmd, sizeof(cmd), "at+s.sockw=0,%d\n\r%s", strlen(data), data);
            me->Write(cmd);
            return Q_HANDLED();
        }
        case UART_IN_DATA_IND: {
            char buf[100];
            while(uint32_t len = me->m_inFifo.Read(reinterpret_cast<uint8_t *>(buf), sizeof(buf)-1)) {
                FW_ASSERT(len < sizeof(buf));
                buf[len] = 0;
                LOG("Received: %s", buf);
                if (strstr(buf, "+WIND:55")) {
                    char cmd[100];
                    snprintf(cmd, sizeof(cmd), "at+s.sockr=0,\n\r");
                    me->Write(cmd);
                }
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&WifiSt::Normal);
}

QState WifiSt::Interactive(WifiSt * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            Log::AddInterface(me->m_outIfHsmn, &me->m_outFifo, UART_OUT_WRITE_REQ, false);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            Log::RemoveInterface(me->m_outIfHsmn);
            return Q_HANDLED();
        }
        case WIFI_INTERACTIVE_OFF_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new WifiInteractiveOffCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&WifiSt::Normal);
        }
        case UART_IN_DATA_IND: {
            char buf[100];
            while(uint32_t len = me->m_inFifo.Read(reinterpret_cast<uint8_t *>(buf), sizeof(buf))) {
                Log::Write(me->m_consoleOutIfHsmn, buf, len);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&WifiSt::Started);
}

/*
QState WifiSt::MyState(WifiSt * const me, QEvt const * const e) {
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
            return Q_TRAN(&WifiSt::SubState);
        }
    }
    return Q_SUPER(&WifiSt::SuperState);
}
*/

} // namespace APP
