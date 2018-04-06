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

#ifndef IKS01A1_CONFIG_H
#define IKS01A1_CONFIG_H

#include "bsp.h"
#include "fw_def.h"

using namespace QP;
using namespace FW;

namespace APP {

class Iks01a1Config {
public:
    // Key
    Hsmn hsmn;
    I2C_TypeDef *i2c;
    // I2c interrupt parameters.
    IRQn_Type i2cEvIrq;
    uint32_t i2cEvPrio;
    IRQn_Type i2cErIrq;
    uint32_t i2cErPrio;

    // SCL/SDA parameters.
    GPIO_TypeDef *i2cPort;
    uint32_t sclPin;
    uint32_t sdaPin;
    uint32_t i2cAf;

    // I2C Tx DMA parameters.
    DMA_Stream_TypeDef *txDmaStream;
    uint32_t txDmaCh;
    IRQn_Type txDmaIrq;
    uint32_t txDmaPrio;

    // I2C Rx DMA parameters.
    DMA_Stream_TypeDef *rxDmaStream;
    uint32_t rxDmaCh;
    IRQn_Type rxDmaIrq;
    uint32_t rxDmaPrio;

    // Hsmn of GpioIn regions for interrupt pins.
    Hsmn accelGyroIntHsmn;
    Hsmn magIntHsmn;
    Hsmn magDrdyHsmn;
    Hsmn humidTempDrdyHsmn;
    Hsmn pressIntHsmn;
};

} // namespace APP

#endif // IKS01A1_CONFIG_H
