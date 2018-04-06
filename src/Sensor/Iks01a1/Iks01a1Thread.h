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

#ifndef IKS01A1_THREAD_H
#define IKS01A1_THREAD_H

#include "qpcpp.h"
#include "fw_xthread.h"
#include "fw_region.h"
#include "fw_evt.h"
#include "app_hsmn.h"
#include "Iks01a1.h"
#include "Iks01a1AccelGyro.h"
#include "Iks01a1Mag.h"
#include "Iks01a1HumidTemp.h"
#include "Iks01a1Press.h"
#include "Iks01a1Config.h"

using namespace QP;
using namespace FW;

namespace APP {

class Iks01a1Thread : public XThread {
public:
    Iks01a1Thread();

    // Only supports single instance.
    static I2C_HandleTypeDef *GetHal() { return &m_hal; }
    static void SignalI2cSem() { m_i2cSem.signal(); }
    static bool I2cWriteInt(uint16_t devAddr, uint16_t memAddr, uint8_t *buf, uint16_t len);
    static bool I2cReadInt(uint16_t devAddr, uint16_t memAddr, uint8_t *buf, uint16_t len);

protected:
    void OnRun();

    static Iks01a1Config const CONFIG[];

    Iks01a1Config const *m_config;
    static I2C_HandleTypeDef m_hal;     // Only support single instance.
    static QXSemaphore m_i2cSem;        // Only support single instance.
                                        // Binary semaphore to siganl I2C read/write completion.
    Iks01a1 m_iks01a1;
    Iks01a1AccelGyro m_iks01a1AccelGyro;
    Iks01a1Mag m_iks01a1Mag;
    Iks01a1HumidTemp m_iks01a1HumidTemp;
    Iks01a1Press m_iks01a1Press;
};

} // namespace APP

#endif // IKS01A1_THREAD_H
