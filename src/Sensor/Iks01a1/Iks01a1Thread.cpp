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
#include "Iks01a1Thread.h"

FW_DEFINE_THIS_FILE("Iks01a1Thread.cpp")

namespace APP {

// Define I2C and interrupt configurations.
Iks01a1Config const Iks01a1Thread::CONFIG[] = {
    { IKS01A1, I2C1, I2C1_EV_IRQn, I2C1_EV_PRIO, I2C1_ER_IRQn, I2C1_ER_PRIO,    // I2C INT
      GPIOB, GPIO_PIN_8, GPIO_PIN_9, GPIO_AF4_I2C1,                                 // I2C SCL SDA
      DMA1_Stream7, DMA_CHANNEL_1, DMA1_Stream7_IRQn, DMA1_STREAM7_PRIO,            // TX DMA
      DMA1_Stream0, DMA_CHANNEL_1, DMA1_Stream0_IRQn, DMA1_STREAM0_PRIO,            // RX DMA
      ACCEL_GYRO_INT, MAG_INT, MAG_DRDY, HUMID_TEMP_DRDY, PRESS_INT
    }
};
I2C_HandleTypeDef Iks01a1Thread::m_hal;   // Only support single instance.
QXSemaphore Iks01a1Thread::m_i2cSem;      // Only support single instance.

bool Iks01a1Thread::I2cWriteInt(uint16_t devAddr, uint16_t memAddr, uint8_t *buf, uint16_t len) {
    if (HAL_I2C_Mem_Write_IT(&m_hal, devAddr, memAddr, I2C_MEMADD_SIZE_8BIT, buf, len) != HAL_OK) {
        return false;
    }
    return m_i2cSem.wait(BSP_MSEC_TO_TICK(1000));
}

bool Iks01a1Thread::I2cReadInt(uint16_t devAddr, uint16_t memAddr, uint8_t *buf, uint16_t len) {
    // Test only
    //HAL_I2C_Mem_Read(&m_hal, devAddr, memAddr, I2C_MEMADD_SIZE_8BIT, buf, len, 10000);
    //return true;

    // Gallium - test only
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    if (HAL_I2C_Mem_Read_IT(&m_hal, devAddr, memAddr, I2C_MEMADD_SIZE_8BIT, buf, len) != HAL_OK) {
        return false;
    }
    // Gallium - test only
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

    // Test only
    //return m_i2cSem.wait(BSP_MSEC_TO_TICK(1000));
    bool result = m_i2cSem.wait(BSP_MSEC_TO_TICK(1000));
    // Gallium - test only
    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    if (!result) {
        return false;
    }
    return true;
}

Iks01a1Thread::Iks01a1Thread() :
    m_config(&CONFIG[0]),
    m_iks01a1(m_config, m_hal), m_iks01a1AccelGyro(m_config->accelGyroIntHsmn, m_hal), m_iks01a1Mag(m_config->magIntHsmn, m_config->magDrdyHsmn, m_hal),
    m_iks01a1HumidTemp(m_config->humidTempDrdyHsmn, m_hal), m_iks01a1Press(m_config->pressIntHsmn, m_hal) {
    m_i2cSem.init(0,1);
    }

void Iks01a1Thread::OnRun() {
    m_iks01a1.Init(this);
    m_iks01a1AccelGyro.Init(this);
    m_iks01a1Mag.Init(this);
    m_iks01a1HumidTemp.Init(this);
    m_iks01a1Press.Init(this);
}

} // namespace APP
