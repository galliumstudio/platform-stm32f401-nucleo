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

#include "qpcpp.h"
#include "fw_active.h"
#include "fw.h"
#include "fw_assert.h"

FW_DEFINE_THIS_FILE("fw.cpp")

using namespace QP;

namespace FW {

HsmAct Fw::m_hsmActStor[MAX_HSM_COUNT];
HsmActMap Fw::m_hsmActMap(m_hsmActStor, ARRAY_COUNT(m_hsmActStor), HsmAct(NULL, NULL));
uint32_t Fw::m_evtPoolSmall[ROUND_UP_DIV_4(EVT_SIZE_SMALL * EVT_COUNT_SMALL)];
uint32_t Fw::m_evtPoolMedium[ROUND_UP_DIV_4(EVT_SIZE_MEDIUM * EVT_COUNT_MEDIUM)];
uint32_t Fw::m_evtPoolLarge[ROUND_UP_DIV_4(EVT_SIZE_LARGE * EVT_COUNT_LARGE)];

void Fw::Init() {
    // Initialize QP. It must be done before BspInit() since the latter may enable
    // SysTick which will cause the scheduler to run.
    QF::init();
    QF::poolInit(m_evtPoolSmall, sizeof(m_evtPoolSmall), EVT_SIZE_SMALL);
    QF::poolInit(m_evtPoolMedium, sizeof(m_evtPoolMedium), EVT_SIZE_MEDIUM);
    QF::poolInit(m_evtPoolLarge, sizeof(m_evtPoolLarge), EVT_SIZE_LARGE);
    // Any necessary framework initialization will be placed here.
    // ...
    // Initialize BSP include HAL.
    BspInit();
}

// By design there is no "Remove" function. All HSM's are to be created and added
// to the framework during system initialization (in main()).
// As a result there is no need to implement critical sections in other API functions.
void Fw::Add(Hsmn hsmn, Hsm *hsm, QActive *container) {
    FW_ASSERT(hsmn != HSM_UNDEF && hsm && container);
    QF_CRIT_STAT_TYPE crit;
    QF_CRIT_ENTRY(crit);
    HsmAct *existing = m_hsmActMap.GetByIndex(hsmn);
    FW_ASSERT(existing && (existing->GetKey() == NULL));
    m_hsmActMap.Put(hsmn, HsmAct(hsm, container));
    QF_CRIT_EXIT(crit);
}

// If the HSM to post to is invalid, e.g. HSM_UNDEF, the event will be discarded.
void Fw::Post(Evt const *e) {
    FW_ASSERT(e);
    QActive *act = m_hsmActMap.GetByIndex(e->GetTo())->GetValue();
    if (act) {
        act->post_(e, 0);
    } else {
        QF::gc(e);
    }
}

// Allow HSM_UNDEF which returns NULL.
Hsm *Fw::GetHsm(Hsmn hsmn) {
    return m_hsmActMap.GetByIndex(hsmn)->GetKey();
}

// Allow HSM_UNDEF which returns NULL.
QActive *Fw::GetContainer(Hsmn hsmn) {
    return m_hsmActMap.GetByIndex(hsmn)->GetValue();
}

} // namespace FW
