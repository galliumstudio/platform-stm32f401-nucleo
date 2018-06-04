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

#ifndef HSM_NUM_H
#define HSM_NUM_H

#include "fw_def.h"

namespace APP {

#define APP_HSM \
    ADD_HSM(SYSTEM, 1) \
    ADD_HSM(CONSOLE, 2) \
    ADD_HSM(CMD_INPUT, 2) \
    ADD_HSM(CMD_PARSER, 2) \
    ADD_HSM(UART_ACT, 2) \
    ADD_HSM(UART_IN, 2) \
    ADD_HSM(UART_OUT, 2) \
    ADD_HSM(WIFI_ST, 1) \
    ADD_HSM(SENSOR, 1) \
    ADD_HSM(SENSOR_ACCEL_GYRO, 1) \
    ADD_HSM(SENSOR_HUMID_TEMP, 1) \
    ADD_HSM(SENSOR_MAG, 1) \
    ADD_HSM(SENSOR_PRESS, 1) \
    ADD_HSM(GPIO_IN_ACT, 1) \
    ADD_HSM(GPIO_IN, 6) \
    ADD_HSM(DEMO, 1) \
    ADD_HSM(USER_LED, 2) \
    ADD_HSM(AO_WASHING_MACHINE, 1) \
    ADD_HSM(TRAFFIC, 1) \
    ADD_HSM(LAMP, 2) \
    ADD_HSM(SIMPLE_ACT, 1) \
    ADD_HSM(SIMPLE_REG, 1) \
    ADD_HSM(COMPOSITE_ACT, 1) \
    ADD_HSM(COMPOSITE_REG, 4)

#define ALIAS_HSM \
    ADD_ALIAS(CONSOLE_UART2,    CONSOLE+0) \
    ADD_ALIAS(CONSOLE_UART1,    CONSOLE+1) \
    ADD_ALIAS(CMD_INPUT_UART2,  CMD_INPUT+0) \
    ADD_ALIAS(CMD_INPUT_UART1,  CMD_INPUT+1) \
    ADD_ALIAS(CMD_PARSER_UART2, CMD_PARSER+0) \
    ADD_ALIAS(CMD_PARSER_UART1, CMD_PARSER+1) \
    ADD_ALIAS(UART2_ACT, UART_ACT+0) \
    ADD_ALIAS(UART1_ACT, UART_ACT+1) \
    ADD_ALIAS(UART2_IN,  UART_IN+0) \
    ADD_ALIAS(UART1_IN,  UART_IN+1) \
    ADD_ALIAS(UART2_OUT, UART_OUT+0) \
    ADD_ALIAS(UART1_OUT, UART_OUT+1) \
    ADD_ALIAS(IKS01A1,            SENSOR) \
    ADD_ALIAS(IKS01A1_ACCEL_GYRO, SENSOR_ACCEL_GYRO) \
    ADD_ALIAS(IKS01A1_HUMID_TEMP, SENSOR_HUMID_TEMP) \
    ADD_ALIAS(IKS01A1_MAG,        SENSOR_MAG) \
    ADD_ALIAS(IKS01A1_PRESS,      SENSOR_PRESS) \
    ADD_ALIAS(USER_BTN,        GPIO_IN+0) \
    ADD_ALIAS(ACCEL_GYRO_INT,  GPIO_IN+1) \
    ADD_ALIAS(MAG_INT,         GPIO_IN+2) \
    ADD_ALIAS(MAG_DRDY,        GPIO_IN+3) \
    ADD_ALIAS(HUMID_TEMP_DRDY, GPIO_IN+4) \
    ADD_ALIAS(PRESS_INT,       GPIO_IN+5) \
    ADD_ALIAS(TEST_LED,        USER_LED+1) \
    ADD_ALIAS(LAMP_NS, LAMP+0) \
    ADD_ALIAS(LAMP_EW, LAMP+1) \
    ADD_ALIAS(COMPOSITE_REG0, COMPOSITE_REG+0) \
    ADD_ALIAS(COMPOSITE_REG1, COMPOSITE_REG+1) \
    ADD_ALIAS(COMPOSITE_REG2, COMPOSITE_REG+2) \
    ADD_ALIAS(COMPOSITE_REG3, COMPOSITE_REG+3)

#undef ADD_HSM
#undef ADD_ALIAS
#define ADD_HSM(hsmn_, count_) hsmn_, hsmn_##_COUNT = count_, hsmn_##_LAST = hsmn_ + count_ - 1,
#define ADD_ALIAS(alias_, to_) alias_ = to_,

enum {
    APP_HSM_START = FW::HSM_UNDEF,
    APP_HSM
    HSM_COUNT,
    ALIAS_HSM
};

// Higher value corresponds to higher priority.
// The maximum priority is defined in qf_port.h as QF_MAX_ACTIVE (32)
enum
{
    PRIO_IKS01A1        = 16, //31,
    PRIO_UART2_ACT      = 30,
    PRIO_UART1_ACT      = 29,
    PRIO_CONSOLE_UART2  = 28,
    PRIO_CONSOLE_UART1  = 27,
    PRIO_SYSTEM         = 26,
    PRIO_WIFI_ST        = 20,
    PRIO_GPIO_IN_ACT    = 31, //16,
    PRIO_DEMO           = 10,
    PRIO_USER_LED       = 9,
    PRIO_TEST_LED       = 8,
    PRIO_AO_WASHING_MACHINE = 7,
    PRIO_TRAFFIC        = 6,
    PRIO_SIMPLE_ACT     = 5,
    PRIO_COMPOSITE_ACT  = 4
};

} // namespace APP

#endif // HSM_NUM_H
