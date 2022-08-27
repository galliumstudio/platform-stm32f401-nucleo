/*******************************************************************************
 * Copyright (C) 2018 Gallium Studio LLC (Lawrence Lo). All rights reserved.
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

#include <string.h>
#include "fw_log.h"
#include "fw_assert.h"
#include "Console.h"
#include "ConsoleInterface.h"
#include "UartInInterface.h"
#include "WifiInterface.h"
#include "WifiStCmd.h"

FW_DEFINE_THIS_FILE("WifiStCmd.cpp")

namespace APP {

static CmdStatus Test(Console &console, Evt const *e) {
    switch (e->sig) {
        case Console::CONSOLE_CMD: {
            console.PutStr("WifiStCmd Test\n\r");
            break;
        }
    }
    return CMD_DONE;
}

static CmdStatus Conn(Console &console, Evt const *e) {
    Hsm &hsm = console.GetHsm();
    switch (e->sig) {
        case Console::CONSOLE_CMD: {
            Console::ConsoleCmd const &ind = static_cast<Console::ConsoleCmd const &>(*e);
            if (ind.Argc() < 3) {
                console.PutStr("wifi conn <host> <port>\n\r");
                return CMD_DONE;
            }
            char const *host = ind.Argv(1);
            uint16_t port = STRING_TO_NUM(ind.Argv(2), 0);
            Evt *evt = new WifiConnectReq(WIFI_ST, hsm.GetHsmn(), hsm.GenSeq(), host, port);
            Fw::Post(evt);
            break;
        }
        // @todo - Wait for cfm.
    }
    //return CMD_CONTINUE;
    return CMD_DONE;
}

static CmdStatus Disc(Console &console, Evt const *e) {
    Hsm &hsm = console.GetHsm();
    switch (e->sig) {
        case Console::CONSOLE_CMD: {
            Console::ConsoleCmd const &ind = static_cast<Console::ConsoleCmd const &>(*e);
            Evt *evt = new WifiDisconnectReq(WIFI_ST, hsm.GetHsmn(), hsm.GenSeq());
            Fw::Post(evt);
            break;
        }
        // @todo - Wait for cfm.
    }
    //return CMD_CONTINUE;
    return CMD_DONE;
}

static CmdStatus Send(Console &console, Evt const *e) {
    Hsm &hsm = console.GetHsm();
    switch (e->sig) {
        case Console::CONSOLE_CMD: {
            Console::ConsoleCmd const &ind = static_cast<Console::ConsoleCmd const &>(*e);
            if (ind.Argc() < 2) {
                console.PutStr("wifi send <text>\n\r");
                return CMD_DONE;
            }
            char const *text = ind.Argv(1);
            Evt *evt = new WifiSendReq(WIFI_ST, hsm.GetHsmn(), hsm.GenSeq(), text);
            Fw::Post(evt);
            break;
        }
        // @todo - Wait for cfm.
    }
    //return CMD_CONTINUE;
    return CMD_DONE;
}

static CmdStatus Interact(Console &console, Evt const *e) {
    enum {
        STATE_WAIT,
        STATE_READY
    };
    enum {
        VAR_STATE,
        VAR_WIFI_OUT_IF_HSMN,
    };
    uint32_t &state = console.Var(0);
    uint32_t &wifiOutIfHsmn = console.Var(1);
    Hsm &hsm = console.GetHsm();
    switch (e->sig) {
        case Console::CONSOLE_CMD: {
            console.PutStr("Enter ST WiFi Interactive mode. Hit CTRL-C to exit\n\r");
            Evt *evt = new Evt(CONSOLE_RAW_ENABLE_REQ, hsm.GetHsmn(), hsm.GetHsmn(), hsm.GenSeq());
            Fw::Post(evt);
            evt = new WifiInteractiveOnReq(WIFI_ST, hsm.GetHsmn(), hsm.GenSeq(), console.GetOutIfHsmn());
            Fw::Post(evt);
            wifiOutIfHsmn = HSM_UNDEF;
            state = STATE_WAIT;
            break;
        }
        case WIFI_INTERACTIVE_ON_CFM: {
            if (state == STATE_WAIT) {
                WifiInteractiveOnCfm const &cfm = static_cast<WifiInteractiveOnCfm const &>(*e);
                wifiOutIfHsmn = cfm.GetOutIfHsmn();
                FW_ASSERT(wifiOutIfHsmn != HSM_UNDEF);
                state = STATE_READY;
            }
            break;
        }
        case UART_IN_DATA_IND: {
            char c;
            while(console.GetInFifo().Read(reinterpret_cast<uint8_t *>(&c), 1)) {
                if (c == CTRLC) {
                    // Exit interactive mode.
                    Evt *evt = new WifiInteractiveOffReq(WIFI_ST, hsm.GetHsmn(), hsm.GenSeq());
                    Fw::Post(evt);
                    console.PutStr("Exit ST WiFi interactive mode\n\r");
                    return CMD_DONE;
                }
                if (state == STATE_READY) {
                   FW_ASSERT(wifiOutIfHsmn != HSM_UNDEF);
                    // Write to wifi output interface.
                    Log::PutChar(wifiOutIfHsmn, c);
                }
            }
            break;
        }
    }
    return CMD_CONTINUE;
}

static CmdStatus List(Console &console, Evt const *e);
static CmdHandler const cmdHandler[] = {
    { "test",       Test,       "Test function", 0 },
    { "conn",       Conn,       "Connect to host", 0 },
    { "disc",       Disc,       "Disconnect", 0 },
    { "send",       Send,       "Send", 0 },
    { "interact",   Interact,   "Interactive mode", 0 },
    { "?",          List,       "List commands", 0 },
};

static CmdStatus List(Console &console, Evt const *e) {
    return console.ListCmd(e, cmdHandler, ARRAY_COUNT(cmdHandler));
}

CmdStatus WifiStCmd(Console &console, Evt const *e) {
    return console.HandleCmd(e, cmdHandler, ARRAY_COUNT(cmdHandler));
}

}
