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

#include <math.h>
#include "app_hsmn.h"
#include "fw_log.h"
#include "fw_assert.h"
#include "DispInterface.h"
#include "SensorInterface.h"
#include "WifiInterface.h"
#include "LevelMeterInterface.h"
#include "LevelMeter.h"

FW_DEFINE_THIS_FILE("LevelMeter.cpp")

namespace APP {

#undef ADD_EVT
#define ADD_EVT(e_) #e_,

static char const * const timerEvtName[] = {
    "LEVEL_METER_TIMER_EVT_START",
    LEVEL_METER_TIMER_EVT
};

static char const * const internalEvtName[] = {
    "LEVEL_METER_INTERNAL_EVT_START",
    LEVEL_METER_INTERNAL_EVT
};

static char const * const interfaceEvtName[] = {
    "LEVEL_METER_INTERFACE_EVT_START",
    LEVEL_METER_INTERFACE_EVT
};

LevelMeter::LevelMeter() :
    Active((QStateHandler)&LevelMeter::InitialPseudoState, LEVEL_METER, "LEVEL_METER"),
    m_accelGyroPipe(m_accelGyroStor, ACCEL_GYRO_PIPE_ORDER),
    m_stateTimer(GetHsm().GetHsmn(), STATE_TIMER),
    m_reportTimer(GetHsm().GetHsmn(), REPORT_TIMER) {
    SET_EVT_NAME(LEVEL_METER);
}

QState LevelMeter::InitialPseudoState(LevelMeter * const me, QEvt const * const e) {
    (void)e;
    return Q_TRAN(&LevelMeter::Root);
}

QState LevelMeter::Root(LevelMeter * const me, QEvt const * const e) {
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
            return Q_TRAN(&LevelMeter::Stopped);
        }
        case LEVEL_METER_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new LevelMeterStartCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_STATE, GET_HSMN());
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case LEVEL_METER_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&LevelMeter::Stopping);
        }
    }
    return Q_SUPER(&QHsm::top);
}

QState LevelMeter::Stopped(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case LEVEL_METER_STOP_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            Evt *evt = new LevelMeterStopCfm(req.GetFrom(), GET_HSMN(), req.GetSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case LEVEL_METER_START_REQ: {
            EVENT(e);
            Evt const &req = EVT_CAST(*e);
            me->GetHsm().SaveInSeq(req);
            return Q_TRAN(&LevelMeter::Starting);
        }
    }
    return Q_SUPER(&LevelMeter::Root);
}

QState LevelMeter::Starting(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = LevelMeterStartReq::TIMEOUT_MS;
            FW_ASSERT(timeout > DispStartReq::TIMEOUT_MS);
            FW_ASSERT(timeout > SensorAccelGyroOnReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new DispStartReq(ILI9341, GET_HSMN(), GEN_SEQ());
            me->GetHsm().SaveOutSeq(*evt);
            Fw::Post(evt);
            evt = new SensorAccelGyroOnReq(IKS01A1_ACCEL_GYRO, GET_HSMN(), GEN_SEQ(), &me->m_accelGyroPipe);
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
        case DISP_START_CFM:
        case SENSOR_ACCEL_GYRO_ON_CFM: {
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
                evt = new LevelMeterStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(),
                                            failed.GetError(), failed.GetOrigin(), failed.GetReason());
            } else {
                evt = new LevelMeterStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_TIMEOUT, GET_HSMN());
            }
            Fw::Post(evt);
            return Q_TRAN(&LevelMeter::Stopping);
        }
        case DONE: {
            EVENT(e);
            Evt *evt = new LevelMeterStartCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&LevelMeter::Started);
        }
    }
    return Q_SUPER(&LevelMeter::Root);
}

QState LevelMeter::Stopping(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            uint32_t timeout = LevelMeterStopReq::TIMEOUT_MS;
            FW_ASSERT(timeout > DispStopReq::TIMEOUT_MS);
            FW_ASSERT(timeout > SensorAccelGyroOffReq::TIMEOUT_MS);
            me->m_stateTimer.Start(timeout);
            me->GetHsm().ResetOutSeq();
            Evt *evt = new DispStopReq(ILI9341, GET_HSMN(), GEN_SEQ());
            me->GetHsm().SaveOutSeq(*evt);
            Fw::Post(evt);
            evt = new SensorAccelGyroOffReq(IKS01A1_ACCEL_GYRO, GET_HSMN(), GEN_SEQ());
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
        case LEVEL_METER_STOP_REQ: {
            EVENT(e);
            me->GetHsm().Defer(e);
            return Q_HANDLED();
        }
        case DISP_STOP_CFM:
        case SENSOR_ACCEL_GYRO_OFF_CFM: {
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
            Evt *evt = new LevelMeterStopCfm(me->GetHsm().GetInHsmn(), GET_HSMN(), me->GetHsm().GetInSeq(), ERROR_SUCCESS);
            Fw::Post(evt);
            return Q_TRAN(&LevelMeter::Stopped);
        }
    }
    return Q_SUPER(&LevelMeter::Root);
}

QState LevelMeter::Started(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            me->m_reportTimer.Start(REPORT_TIMEOUT_MS, Timer::PERIODIC);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            me->m_reportTimer.Stop();
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&LevelMeter::Normal);
        }
        case REPORT_TIMER: {
            EVENT(e);
            // Default to zero.
            AccelGyroReport report;
            me->m_avgReport = report;
            int32_t count = 0;
            while (me->m_accelGyroPipe.GetUsedCount()) {
                me->m_accelGyroPipe.Read(&report, 1);
                //LOG("%d %d %d", report.m_aX, report.m_aY, report.m_aZ);
                me->m_avgReport.m_aX += report.m_aX;
                me->m_avgReport.m_aY += report.m_aY;
                me->m_avgReport.m_aZ += report.m_aZ;
                //LOG("%d, %d, %d", me->m_avgReport.m_aX, me->m_avgReport.m_aY, me->m_avgReport.m_aZ);
                count++;
            }
            if (count) {
                me->m_avgReport.m_aX /= count;
                me->m_avgReport.m_aY /= count;
                me->m_avgReport.m_aZ /= count;
            }
            LOG("(count = %d) %d, %d, %d", count, me->m_avgReport.m_aX, me->m_avgReport.m_aY, me->m_avgReport.m_aZ);
            Evt *evt = new Evt(REDRAW, GET_HSMN());
            me->PostSync(evt);
            // Send to server.
            char buf[50];
            snprintf(buf, sizeof(buf), "%d %d %d\n\r", (int)me->m_avgReport.m_aX, (int)me->m_avgReport.m_aY, (int)me->m_avgReport.m_aZ);
            evt = new WifiSendReq(WIFI_ST, GET_HSMN(), 0, buf);
            Fw::Post(evt);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&LevelMeter::Root);
}

QState LevelMeter::Normal(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case REDRAW: {
            EVENT(e);
            return Q_TRAN(&LevelMeter::Redrawing);
        }
    }
    return Q_SUPER(&LevelMeter::Started);
}

QState LevelMeter::Redrawing(LevelMeter * const me, QEvt const * const e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            EVENT(e);
            Evt *evt = new DispDrawBeginReq(ILI9341, GET_HSMN(), GEN_SEQ());
            Fw::Post(evt);
            char buf[30];
            volatile const float PI = 3.14159265;
            volatile const float G = 1000;
            volatile float x = me->m_avgReport.m_aX;
            volatile float y = me->m_avgReport.m_aY;
            volatile float z = me->m_avgReport.m_aZ;
            /*
            float pitch = atan(y/sqrt((x*x) + (z*z))) * 180/PI;
            float roll  = atan(x/sqrt((y*y) + (z*z))) * 180/PI;
            */
            volatile float pitch = atan(x/sqrt((y*y) + (z*z))) * 180/PI;
            volatile float roll  = atan(y/sqrt((x*x) + (z*z))) * 180/PI;
            //PRINT("%f %f %f\n\r", x, y, z);
            //PRINT("pitch=%06.2f, roll=%06.2f\n\r", pitch, roll);
            snprintf(buf, sizeof(buf), "P= %06.2f", pitch);
            evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 30, COLOR24_RED, COLOR24_WHITE, 4);
            Fw::Post(evt);
            snprintf(buf, sizeof(buf), "R= %06.2f", roll);
            evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 90, COLOR24_BLUE, COLOR24_WHITE, 4);
            Fw::Post(evt);

            // Alternative methods.
            //PRINT("(2) %f %f %f %f %f %f %f\n\r", x, y, x/G, y/G, asin(x/G), asin(y/G), asin(1.007000));
            if (x > 0) {
                x = LESS(x, G);
            } else {
                x = GREATER(x, -G);
            }
            if (y > 0) {
                y = LESS(y, G);
            } else {
                y = GREATER(y, -G);
            }
            /*
            pitch = asin(y/G) * 180/PI;
            roll = asin(x/G) * 180/PI;
            */
            pitch = asin(x/G) * 180/PI;
            roll = asin(y/G) * 180/PI;
            //PRINT("(2) pitch=%06.2f, roll=%06.2f\n\r", pitch, roll);
            snprintf(buf, sizeof(buf), "P= %06.2f", pitch);
            evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 150, COLOR24_RED, COLOR24_WHITE,4);
            Fw::Post(evt);
            snprintf(buf, sizeof(buf), "R= %06.2f", roll);
            evt = new DispDrawTextReq(ILI9341, GET_HSMN(), buf, 10, 210, COLOR24_BLUE, COLOR24_WHITE,4);
            Fw::Post(evt);
            evt = new DispDrawEndReq(ILI9341, GET_HSMN(), GEN_SEQ());
            Fw::Post(evt);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            EVENT(e);
            return Q_HANDLED();
        }
        case DISP_DRAW_END_CFM: {
            EVENT(e);
            return Q_TRAN(&LevelMeter::Normal);
        }
        case REDRAW: {
            EVENT(e);
            LOG("Discarded");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&LevelMeter::Started);
}

/*
QState LevelMeter::MyState(LevelMeter * const me, QEvt const * const e) {
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
            return Q_TRAN(&LevelMeter::SubState);
        }
    }
    return Q_SUPER(&LevelMeter::SuperState);
}
*/

} // namespace APP
