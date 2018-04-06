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

#ifndef WIFI_ST_INTERFACE_H
#define WIFI_ST_INTERFACE_H

#include "fw_def.h"
#include "fw_evt.h"
#include "app_hsmn.h"

using namespace QP;
using namespace FW;

namespace APP {

#define WIFI_ST_INTERFACE_EVT \
    ADD_EVT(WIFI_ST_START_REQ) \
    ADD_EVT(WIFI_ST_START_CFM) \
    ADD_EVT(WIFI_ST_STOP_REQ) \
    ADD_EVT(WIFI_ST_STOP_CFM) \
    ADD_EVT(WIFI_ST_INTERACTIVE_ON_REQ) \
    ADD_EVT(WIFI_ST_INTERACTIVE_ON_CFM) \
    ADD_EVT(WIFI_ST_INTERACTIVE_OFF_REQ) \
    ADD_EVT(WIFI_ST_INTERACTIVE_OFF_CFM)

#undef ADD_EVT
#define ADD_EVT(e_) e_,

enum {
    WIFI_ST_INTERFACE_EVT_START = INTERFACE_EVT_START(WIFI_ST),
    WIFI_ST_INTERFACE_EVT
};

enum {
    WIFI_ST_REASON_UNSPEC = 0,
};

class WifiStStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 300
    };
    WifiStStartReq(Hsmn to, Hsmn from, Sequence seq, Hsmn ifHsmn) :
        Evt(WIFI_ST_START_REQ, to, from, seq),  m_ifHsmn(ifHsmn) {}
    Hsmn GetIfHsmn() const { return m_ifHsmn; }
private:
    Hsmn m_ifHsmn;      // HSMN of the interface active object.
};

class WifiStStartCfm : public ErrorEvt {
public:
    WifiStStartCfm(Hsmn to, Hsmn from, Sequence seq,
                   Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(WIFI_ST_START_CFM, to, from, seq, error, origin, reason) {}
};

class WifiStStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 200
    };
    WifiStStopReq(Hsmn to, Hsmn from, Sequence seq) :
        Evt(WIFI_ST_STOP_REQ, to, from, seq) {}
};

class WifiStStopCfm : public ErrorEvt {
public:
    WifiStStopCfm(Hsmn to, Hsmn from, Sequence seq,
                   Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(WIFI_ST_STOP_CFM, to, from, seq, error, origin, reason) {}
};

class WifiStInteractiveOnReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    WifiStInteractiveOnReq(Hsmn to, Hsmn from, Sequence seq, Hsmn consoleOutIfHsmn) :
        Evt(WIFI_ST_INTERACTIVE_ON_REQ, to, from, seq),
        m_consoleOutIfHsmn(consoleOutIfHsmn) {}
    Hsmn GetConsoleOutIfHsmn() const { return m_consoleOutIfHsmn; }
private:
    Hsmn m_consoleOutIfHsmn;      // HSMN of the console output interface.
};

class WifiStInteractiveOnCfm : public ErrorEvt {
public:
    // In case of error outIfHsmn will be set to HSM_UNDEF.
    WifiStInteractiveOnCfm(Hsmn to, Hsmn from, Sequence seq, Hsmn outIfHsmn,
                           Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(WIFI_ST_INTERACTIVE_ON_CFM, to, from, seq, error, origin, reason),
        m_outIfHsmn(outIfHsmn) {}
    Hsmn GetOutIfHsmn() const { return m_outIfHsmn; }
private:
    Hsmn m_outIfHsmn;      // HSMN of this object's own output interface.
};

class WifiStInteractiveOffReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    WifiStInteractiveOffReq(Hsmn to, Hsmn from, Sequence seq) :
        Evt(WIFI_ST_INTERACTIVE_OFF_REQ, to, from, seq) {}
};

class WifiStInteractiveOffCfm : public ErrorEvt {
public:
    WifiStInteractiveOffCfm(Hsmn to, Hsmn from, Sequence seq,
                   Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(WIFI_ST_INTERACTIVE_OFF_CFM, to, from, seq, error, origin, reason) {}
};

} // namespace APP

#endif // WIFI_ST_INTERFACE_H
