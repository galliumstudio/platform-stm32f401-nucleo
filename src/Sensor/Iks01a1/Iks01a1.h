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

#ifndef IKS01A1_H
#define IKS01A1_H

#include "qpcpp.h"
#include "fw_region.h"
#include "fw_timer.h"
#include "fw_evt.h"
#include "app_hsmn.h"
#include "Iks01a1Config.h"

using namespace QP;
using namespace FW;

namespace APP {

class Iks01a1 : public Region {
public:
    Iks01a1(Iks01a1Config const *config, I2C_HandleTypeDef &hal);

protected:
    static QState InitialPseudoState(Iks01a1 * const me, QEvt const * const e);
    static QState Root(Iks01a1 * const me, QEvt const * const e);
        static QState Stopped(Iks01a1 * const me, QEvt const * const e);
        static QState Starting(Iks01a1 * const me, QEvt const * const e);
        static QState Stopping(Iks01a1 * const me, QEvt const * const e);
        static QState Started(Iks01a1 * const me, QEvt const * const e);

    void InitI2c();
    void DeInitI2c();
    bool InitHal();

    Iks01a1Config const *m_config;
    I2C_HandleTypeDef &m_hal;
    DMA_HandleTypeDef m_txDmaHandle;
    DMA_HandleTypeDef m_rxDmaHandle;
    Hsmn m_client;
    Timer m_stateTimer;

#define IKS01A1_TIMER_EVT \
    ADD_EVT(STATE_TIMER)

#define IKS01A1_INTERNAL_EVT \
    ADD_EVT(START) \
    ADD_EVT(DONE) \
    ADD_EVT(FAILED)

#undef ADD_EVT
#define ADD_EVT(e_) e_,

    enum {
        IKS01A1_TIMER_EVT_START = TIMER_EVT_START(IKS01A1),
        IKS01A1_TIMER_EVT
    };

    enum {
        IKS01A1_INTERNAL_EVT_START = INTERNAL_EVT_START(IKS01A1),
        IKS01A1_INTERNAL_EVT
    };

    class Failed : public ErrorEvt {
    public:
        Failed(Hsmn hsmn, Error error, Hsmn origin, Reason reason) :
            ErrorEvt(FAILED, hsmn, hsmn, 0, error, origin, reason) {}
    };
};

} // namespace APP

#endif // IKS01A1_H
