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

#include "fw_hsm.h"
#include "fw_inline.h"
#include "fw_assert.h"

FW_DEFINE_THIS_FILE("fw_hsm.cpp")

using namespace QP;

namespace FW {

char const * const Hsm::m_builtinEvtName[] = {
    "NULL",
    "ENTRY",
    "EXIT",
    "INIT"
};

char const Hsm::m_undefName[] = "UNDEF";

void Hsm::Init(QActive *container) {
    m_deferEQueue.Init(container, m_deferQueueStor, ARRAY_COUNT(m_deferQueueStor));
    m_reminderQueue.init(m_reminderQueueStor, ARRAY_COUNT(m_reminderQueueStor));
}

char const *Hsm::GetBuiltinEvtName(QP::QSignal signal) {
    if (signal < Q_USER_SIG) {
        return m_builtinEvtName[signal];
    }
    return m_undefName;
}

char const *Hsm::GetEvtName(QSignal signal) const {
    uint32_t index;
    if (signal < Q_USER_SIG) {
        return GetBuiltinEvtName(signal);
    } else if (IS_TIMER_EVT(signal)) {
        index = GET_TIMER_EVT_INDEX(signal);
        if (m_timerEvtName && (index < m_timerEvtCount)) {
            return m_timerEvtName[index];
        }
    } else if (IS_INTERNAL_EVT(signal)) {
        index = GET_INTERNAL_EVT_INDEX(signal);
        if (m_internalEvtName && (index < m_internalEvtCount)) {
            return m_internalEvtName[index];
        }
    } else {
        index = GET_INTERFACE_EVT_INDEX(signal);
        if (m_interfaceEvtName && (index < m_interfaceEvtCount)) {
            return m_interfaceEvtName[index];
        }
    }
    return m_undefName;
}

bool Hsm::MatchOutSeq(Hsmn hsmn, Sequence seqToMatch, bool autoClear) {
    if (hsmn == HSM_UNDEF) {
        return false;
    }
    HsmnSeq *hsmnSeq = m_outHsmnSeqMap.GetByKey(hsmn);
    if (hsmnSeq == NULL) {
        return false;
    }
    if (hsmnSeq->GetValue() != seqToMatch) {
        return false;
    }
    // Clear entry once matched.
    if (autoClear) {
        *hsmnSeq = m_outHsmnSeqMap.GetUnusedKv();
    }
    return true;
}

// @description Standard handling of a confirmation or response event.
// @param[in] e - Event to handle.
// @param[out] allReceived -  Set to true if all expected confirmation/response have been received without errors.
// @return Handling status - true if the event was handled without error. This includes the event being ignored
//                           due to sequence number mismatch.
//                           false if the event matches sequence number and reports an error.
bool Hsm::HandleCfmRsp(ErrorEvt const &e, bool &allReceived) {
    allReceived = false;
    if (MatchOutSeq(e)) {
        if (e.GetError() != ERROR_SUCCESS) {
            return false;
        }
        if(IsOutSeqAllCleared()) {
            allReceived = true;
        }
    }
    return true;
}

void Hsm::DispatchReminder() {
    while (QEvt const *reminder = m_reminderQueue.get()) {
        m_qhsm->QHsm::dispatch(reminder);
        // A reminder event must be dynamic and is garbage collected after being processed.
        FW_ASSERT(QF_EVT_POOL_ID_(reminder) != 0);
        // A reminder event must be an internal or interface event (but not a timer event).
        FW_ASSERT(IS_EVT_HSMN_VALID(reminder->sig) && (!IS_TIMER_EVT(reminder->sig)));
        FW_ASSERT(EVT_CAST(*reminder).GetTo() == m_hsmn);
        QF::gc(reminder);
    }
}

} // namespace FW

