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

#ifndef USER_LED_INTERFACE_H
#define USER_LED_INTERFACE_H

#include "fw_def.h"
#include "fw_evt.h"
#include "app_hsmn.h"

using namespace QP;
using namespace FW;

namespace APP {

#define USER_LED_INTERFACE_EVT \
    ADD_EVT(USER_LED_START_REQ) \
    ADD_EVT(USER_LED_START_CFM) \
    ADD_EVT(USER_LED_STOP_REQ) \
    ADD_EVT(USER_LED_STOP_CFM) \
    ADD_EVT(USER_LED_PATTERN_REQ) \
    ADD_EVT(USER_LED_PATTERN_CFM) \
    ADD_EVT(USER_LED_OFF_REQ) \
    ADD_EVT(USER_LED_OFF_CFM)

#undef ADD_EVT
#define ADD_EVT(e_) e_,

enum {
    USER_LED_INTERFACE_EVT_START = INTERFACE_EVT_START(USER_LED),
    USER_LED_INTERFACE_EVT
};

enum {
    USER_LED_REASON_UNSPEC = 0,
    USER_LED_REASON_INVALID_PATTERN,
};

class UserLedStartReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedStartReq(Hsmn to, Hsmn from, Sequence seq) :
        Evt(USER_LED_START_REQ, to, from, seq) {}
};

class UserLedStartCfm : public ErrorEvt {
public:
    UserLedStartCfm(Hsmn to, Hsmn from, Sequence seq, Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(USER_LED_START_CFM, to, from, seq, error, origin, reason) {}
};

class UserLedStopReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedStopReq(Hsmn to, Hsmn from, Sequence seq) :
        Evt(USER_LED_STOP_REQ, to, from, seq) {}
};

class UserLedStopCfm : public ErrorEvt {
public:
    UserLedStopCfm(Hsmn to, Hsmn from, Sequence seq, Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(USER_LED_STOP_CFM, to, from, seq, error, origin, reason) {}
};

class UserLedPatternReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedPatternReq(Hsmn to, Hsmn from, Sequence seq, uint32_t patternIndex, bool isRepeat = false) :
        Evt(USER_LED_PATTERN_REQ, to, from, seq), m_patternIndex(patternIndex), m_isRepeat(isRepeat) {}
    uint32_t GetPatternIndex() const { return m_patternIndex; }
    bool IsRepeat() const { return m_isRepeat; }
private:
    uint32_t m_patternIndex;
    bool m_isRepeat;
};

class UserLedPatternCfm : public ErrorEvt {
public:
    UserLedPatternCfm(Hsmn to, Hsmn from, Sequence seq, Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(USER_LED_PATTERN_CFM, to, from, seq, error, origin, reason) {}
};

class UserLedOffReq : public Evt {
public:
    enum {
        TIMEOUT_MS = 100
    };
    UserLedOffReq(Hsmn to, Hsmn from, Sequence seq) :
        Evt(USER_LED_OFF_REQ, to, from, seq) {}
};

class UserLedOffCfm : public ErrorEvt {
public:
    UserLedOffCfm(Hsmn to, Hsmn from, Sequence seq, Error error, Hsmn origin = HSM_UNDEF, Reason reason = 0) :
        ErrorEvt(USER_LED_OFF_CFM, to, from, seq, error, origin, reason) {}
};

} // namespace APP

#endif // USER_LED_INTERFACE_H
