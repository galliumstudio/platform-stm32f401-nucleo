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
#include "fw_assert.h"
#include "periph.h"

FW_DEFINE_THIS_FILE("periph.cpp")

namespace APP {

TimHal Periph::m_timHalStor[MAX_TIM_COUNT];
TimHalMap Periph::m_timHalMap(m_timHalStor, ARRAY_COUNT(m_timHalStor), TimHal(NULL, NULL));

TIM_HandleTypeDef Periph::m_tim1Hal;
TIM_HandleTypeDef Periph::m_tim2Hal;
TIM_HandleTypeDef Periph::m_tim3Hal;
// Add more HAL handles here.

// Setup common peripherals for normal power mode.
// These common peripherals are shared among different HW blocks and cannot be setup individually
// USART2 - TX PA.2 DMA1 Request 6 Channel 4
//          RX PA.3 DMA1 Request 5 Channel 4
// USART1 - TX PA.9 DMA2 Request 7 Channel 4
//          RX PA.10 DMA2 Request 5 Channel 4
// USER_LED (LED2) - PA.5 PWM TIM2 Channel 1
// TEST_LED - PC.7 PWM TIM3 Channel 2
// BUTTON - PC.13
// Ili9341 SPI1 - SCK PA.5, MISO PA.6, MOSI PA.7, CS PB.6 D/CX PC.7
//                TX DMA2 Request 3 Channel 3
//                RX DMA2 Request 2 Channel 3
// IKS01A1 I2C1 - SCL PB.8, SDA PB.9
//              - TX DMA1 Request 7 Channel 0 (not used)
//              - RX DMA1 Request 0 Channel 1 (not used)
// IKS01A1 ACCEL GYRO INT - PB.5
// IKS01A1 MAG INT/DRDY - PC.1 (INT) PC.0 (DRDY)
// IKS01A1 HUMID TEMP DRDY - PB.10
// IKS01A1 PRESS INT - PB.4
// **TEST PIN 1 - PC.7 (used as TEST_LED)
// **TEST PIN 2 - PB.6
// TIM2 configuration:
// APB1CLK = HCLK -> TIM1CLK = HCLK = SystemCoreClock (See "clock tree" and "timer clock" in ref manual.)
#define TIM2CLK             (SystemCoreClock)   // 84MHz
#define TIM2_COUNTER_CLK    (21000000)          // 21MHz
#define TIM2_PWM_FREQ       (21000)             // 21kHz

// TIM3 configuration:
// APB1CLK = HCLK -> TIM1CLK = HCLK = SystemCoreClock (See "clock tree" and "timer clock" in ref manual.)
#define TIM3CLK             (SystemCoreClock)   // 84MHz
#define TIM3_COUNTER_CLK    (21000000)          // 21MHz
#define TIM3_PWM_FREQ       (21000)             // 21kHz

void Periph::SetupNormal() {
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

    // Initialize TIM2 for PWM (shared by USER_LED/LED2...).
    HAL_StatusTypeDef status;
    m_tim2Hal.Instance = TIM2;
    m_tim2Hal.Init.Prescaler = (TIM2CLK / TIM2_COUNTER_CLK) - 1;
    m_tim2Hal.Init.Period = (TIM2_COUNTER_CLK / TIM2_PWM_FREQ) - 1;
    m_tim2Hal.Init.ClockDivision = 0;
    m_tim2Hal.Init.CounterMode = TIM_COUNTERMODE_UP;
    m_tim2Hal.Init.RepetitionCounter = 0;
    status = HAL_TIM_PWM_Init(&m_tim2Hal);
    FW_ASSERT(status == HAL_OK);
    // Add timHandle to map.
    SetHal(TIM2, &m_tim2Hal);

    // Initialize TIM3 for PWM (shared by TEST_LED...).
    m_tim3Hal.Instance = TIM3;
    m_tim3Hal.Init.Prescaler = (TIM3CLK / TIM3_COUNTER_CLK) - 1;
    m_tim3Hal.Init.Period = (TIM3_COUNTER_CLK / TIM3_PWM_FREQ) - 1;
    m_tim3Hal.Init.ClockDivision = 0;
    m_tim3Hal.Init.CounterMode = TIM_COUNTERMODE_UP;
    m_tim3Hal.Init.RepetitionCounter = 0;
    status = HAL_TIM_PWM_Init(&m_tim3Hal);
    FW_ASSERT(status == HAL_OK);
    // Add timHandle to map.
    SetHal(TIM3, &m_tim3Hal);
}

// Setup common peripherals for low power mode.
void Periph::SetupLowPower() {
    // TBD.
}

// Reset common peripherals to startup state.
void Periph::Reset() {
    HAL_TIM_PWM_DeInit(&m_tim1Hal);
    __HAL_RCC_TIM3_CLK_DISABLE();
    __HAL_RCC_TIM2_CLK_DISABLE();
    __HAL_RCC_DMA2_CLK_DISABLE();
    __HAL_RCC_DMA1_CLK_DISABLE();
    __GPIOC_CLK_DISABLE();
    __GPIOB_CLK_DISABLE();
    __GPIOA_CLK_DISABLE();
    // TBD.
}

} // namespace APP
