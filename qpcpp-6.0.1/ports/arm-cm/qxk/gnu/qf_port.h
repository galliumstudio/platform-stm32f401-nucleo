/// @file
/// @brief QF/C++ port to ARM Cortex-M, dual-mode QXK kernel, GNU-ARM toolset
/// @cond
///***************************************************************************
/// Last Updated for Version: 6.0.0
/// Date of the Last Update:  2017-10-12
///
///                    Q u a n t u m     L e a P s
///                    ---------------------------
///                    innovating embedded systems
///
/// Copyright (C) Quantum Leaps, LLC. All rights reserved.
///
/// This program is open source software: you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as published
/// by the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// Alternatively, this program may be distributed and modified under the
/// terms of Quantum Leaps commercial licenses, which expressly supersede
/// the GNU General Public License and are specifically designed for
/// licensees interested in retaining the proprietary status of their code.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program. If not, see <http://www.gnu.org/licenses/>.
///
/// Contact information:
/// https://state-machine.com
/// mailto:info@state-machine.com
///***************************************************************************
/// @endcond

#ifndef qf_port_h
#define qf_port_h

// The maximum number of active objects in the application, see NOTE1
#define QF_MAX_ACTIVE           32

// The maximum number of system clock tick rates
#define QF_MAX_TICK_RATE        2

// QF interrupt disable/enable and log2()...
#if (__ARM_ARCH == 6) // Cortex-M0/M0+/M1(v6-M, v6S-M)?

    // Cortex-M0/M0+/M1(v6-M, v6S-M) interrupt disabling policy, see NOTE2
    #define QF_INT_DISABLE()    __asm volatile ("cpsid i")
    #define QF_INT_ENABLE()     __asm volatile ("cpsie i")

    // QF critical section (save and restore interrupt status), see NOTE6
    #define QF_CRIT_STAT_TYPE   uint32_t
    #define QF_CRIT_ENTRY(primask_) do { \
        __asm volatile ("mrs %0,PRIMASK" : "=r" (primask_) ::); \
        __asm volatile ("cpsid i" :: "" (primask_) : ); \
    } while (0)
    #define QF_CRIT_EXIT(primask_) \
        __asm volatile ("msr PRIMASK,%0" :: "r" (primask_) : )

    // CMSIS threshold for "QF-aware" interrupts, see NOTE2,4
    #define QF_AWARE_ISR_CMSIS_PRI 0

#else // Cortex-M3/M4/M7

    // Cortex-M3/M4/M7 interrupt disabling policy, see NOTE3,4
    #define QF_INT_DISABLE() __asm volatile (\
        "cpsid i\n\t" "msr BASEPRI,%0\n\t" "cpsie i" :: "r" (QF_BASEPRI) : )
    #define QF_INT_ENABLE()  __asm volatile (\
        "msr BASEPRI,%0" :: "r" (0) : )

    // QF critical section (save and restore interrupt status), see NOTE5,6
    #define QF_CRIT_STAT_TYPE   uint32_t
    #define QF_CRIT_ENTRY(basepri_) do { \
        __asm volatile ("mrs %0,BASEPRI" : "=r" (basepri_) :: ); \
        __asm volatile ("cpsid i" :: "" (basepri_) : ); \
        __asm volatile ("msr BASEPRI,%0" :: "r" (QF_BASEPRI) : ); \
        __asm volatile ("cpsie i" :: "" (basepri_) : ); \
    } while (0)
    #define QF_CRIT_EXIT(basepri_) \
        __asm volatile ("msr BASEPRI,%0" :: "r" (basepri_) : )

    // BASEPRI threshold for "QF-aware" interrupts, see NOTE3.
    // CAUTION: keep in synch with the value defined in "qk_port.s"
    #define QF_BASEPRI          (0xFFU >> 2)

    /* CMSIS threshold for "QF-aware" interrupts, see NOTE5 */
    #define QF_AWARE_ISR_CMSIS_PRI (QF_BASEPRI >> (8 - __NVIC_PRIO_BITS))

    // Cortex-M3/M4/M7 provide the CLZ instruction for fast LOG2
    #define QF_LOG2(n_) (static_cast<uint_fast8_t>(32U - __builtin_clz(n_)))

#endif

#define QF_CRIT_EXIT_NOP()      __asm volatile ("isb")

#include "qep_port.h" // QEP port
#include "qxk_port.h" // QXK dual-mode kernel port
#include "qf.h"       // QF platform-independent public interface
#include "qxthread.h" // QXK extended thread

//****************************************************************************
// NOTE1:
// The maximum number of active objects QF_MAX_ACTIVE can be increased
// up to 64, if necessary. Here it is set to a lower level to save some RAM.
//
// NOTE2:
// On Cortex-M0/M0+/M1 (architecture v6-M, v6S-M), the interrupt disabling
// policy uses the PRIMASK register to disable interrupts globally. The
// QF_AWARE_ISR_CMSIS_PRI level is zero, meaning that all interrupts are
// "QF-aware".
//
// NOTE3:
// On Cortex-M3/M4/M7, the interrupt disable/enable policy uses the BASEPRI
// register (which is not implemented in Cortex-M0/M0+/M1) to disable
// interrupts only with priority lower than the threshold specified by the
// QF_BASEPRI macro. The interrupts with priorities above QF_BASEPRI (i.e.,
// with numerical priority values lower than QF_BASEPRI) are NOT disabled in
// this method. These free-running interrupts have very low ("zero") latency,
// but they are not allowed to call any QF services, because QF is unaware
// of them ("QF-unaware" interrutps). Consequently, only interrupts with
// numerical values of priorities eqal to or higher than QF_BASEPRI
// ("QF-aware" interrupts ), can call QF services.
//
// NOTE4:
// The QF_AWARE_ISR_CMSIS_PRI macro is useful as an offset for enumerating
// the "QF-aware" interrupt priorities in the applications, whereas the
// numerical values of the "QF-aware" interrupts must be greater or equal to
// QF_AWARE_ISR_CMSIS_PRI. The values based on QF_AWARE_ISR_CMSIS_PRI can be
// passed directly to the CMSIS function NVIC_SetPriority(), which shifts
// them by (8 - __NVIC_PRIO_BITS) into the correct bit position, while
// __NVIC_PRIO_BITS is the CMSIS macro defining the number of implemented
// priority bits in the NVIC. Please note that the macro QF_AWARE_ISR_CMSIS_PRI
// is intended only for applications and is not used inside the QF port, which
// remains generic and not dependent on the number of implemented priority bits
// implemented in the NVIC.
//
// NOTE5:
// The selective disabling of "QF-aware" interrupts with the BASEPRI register
// has a problem on ARM Cortex-M7 core r0p1 (see ARM-EPM-064408, errata
// 837070). The workaround recommended by ARM is to surround MSR BASEPRI with
// the CPSID i/CPSIE i pair, which is implemented in the QF_INT_DISABLE()
// macro. This workaround works also for Cortex-M3/M4 cores.
//
// NOTE6:
// The critical section for Cortex-M0(+)/M1 (ARMv6-M architecture) is
// specifically defined to avoid the GNU-ARM bug 1722849 (see
// https://bugs.launchpad.net/gcc-arm-embedded/+bug/1722849 ). Specifically,
// the in-line assembly for disabling of interrupts with "cpsid i" has an
// artificial dependency on the (primask_) argument. This ensures that
// the GNU-ARM compiler will evaluate the preceeding "mrs %0,PRIMASK"
// __before__ evaluating "cpsid i".
//

#endif // qf_port_h
