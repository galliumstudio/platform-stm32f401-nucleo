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

#include "app_hsmn.h"
#include "fw_log.h"
#include "fw_assert.h"
#include "x_nucleo_iks01a1.h"
#include "Iks01a1.h"

FW_DEFINE_THIS_FILE("SensorIo.cpp")

using namespace APP;

// I2C interface functions required by ST IKS01A1 BSP.
extern "C" uint8_t Sensor_IO_Write(void *handle, uint8_t memAddr, uint8_t *pBuffer, uint16_t nBytes) {
    bool result = true;
    DrvContextTypeDef *ctx = (DrvContextTypeDef *)handle;
    switch(ctx->who_am_i) {
        case IKS01A1_LPS25HB_WHO_AM_I:
        case IKS01A1_LIS3MDL_WHO_AM_I:
        case IKS01A1_HTS221_WHO_AM_I: {
            // Enable I2C multi-bytes write
            if (nBytes > 1) {
                memAddr |= 0x80;
            }
            result = Iks01a1::I2cWriteInt(ctx->address, memAddr, pBuffer, nBytes);
            break;
        }
        case IKS01A1_LPS22HB_WHO_AM_I: {
            // I2C multi-bytes write not supported for LPS22HB.
            for (uint16_t i = 0; i < nBytes; i++ ) {
                result = Iks01a1::I2cWriteInt(ctx->address, (memAddr + i), &pBuffer[i], 1);
                if (!result) {
                    break;
                }
            }
            break;
        }
        case IKS01A1_LSM6DS0_WHO_AM_I:
        case IKS01A1_LSM6DS3_WHO_AM_I:
        case IKS01A2_LSM6DSL_WHO_AM_I: {
            result = Iks01a1::I2cWriteInt(ctx->address, memAddr, pBuffer, nBytes);
            break;
        }
        default: {
            FW_ASSERT(0);
            break;
        }
    }
    return result ? 0 : 1;
}

extern "C" uint8_t Sensor_IO_Read(void *handle, uint8_t memAddr, uint8_t *pBuffer, uint16_t nBytes) {
    bool result = true;
    DrvContextTypeDef *ctx = (DrvContextTypeDef *)handle;
    switch(ctx->who_am_i) {
        case IKS01A1_LPS25HB_WHO_AM_I:
        case IKS01A1_LIS3MDL_WHO_AM_I:
        case IKS01A1_HTS221_WHO_AM_I: {
            // Enable I2C multi-bytes read
            if (nBytes > 1) {
                memAddr |= 0x80;
            }
            result = Iks01a1::I2cReadInt(ctx->address, memAddr, pBuffer, nBytes);
            break;
        }
        case IKS01A1_LPS22HB_WHO_AM_I: {
            // I2C multi-bytes read not supported for LPS22HB.
            for (uint16_t i = 0; i < nBytes; i++ ) {
                result = Iks01a1::I2cReadInt(ctx->address, (memAddr + i), &pBuffer[i], 1);
                if (!result) {
                    break;
                }
            }
            break;
        }
        case IKS01A1_LSM6DS0_WHO_AM_I:
        case IKS01A1_LSM6DS3_WHO_AM_I:
        case IKS01A2_LSM6DSL_WHO_AM_I: {
            result = Iks01a1::I2cReadInt(ctx->address, memAddr, pBuffer, nBytes);
            break;
        }
        default: {
            FW_ASSERT(0);
            break;
        }
    }
    return result ? 0 : 1;
}

extern "C" DrvStatusTypeDef Sensor_IO_Init(void) {
    // Dummy. I2C bus initialization handled by the active object directly.
    return COMPONENT_OK;
}
extern "C" DrvStatusTypeDef LSM6DS0_Sensor_IO_ITConfig(void) {
    // Dummy. Interrupt handled by GpioIn HSM.
    return COMPONENT_OK;
}
extern "C" DrvStatusTypeDef LSM6DS3_Sensor_IO_ITConfig(void) {
    // Dummy. Interrupt handled by GpioIn HSM.
    return COMPONENT_OK;
}
extern "C" DrvStatusTypeDef LSM6DSL_Sensor_IO_ITConfig(void) {
    // Dummy. Interrupt handled by GpioIn HSM.
    return COMPONENT_OK;
}
extern "C" DrvStatusTypeDef LPS22HB_Sensor_IO_ITConfig(void) {
    // Dummy. Interrupt handled by GpioIn HSM.
    return COMPONENT_OK;
}
