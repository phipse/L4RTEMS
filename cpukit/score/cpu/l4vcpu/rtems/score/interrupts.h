/**
 * @file rtems/score/interrupts.h
 */

/*
 *  i386 interrupt macros.
 *
 *  Formerly contained in and extracted from libcpu/i386/cpu.h
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: interrupts.h,v 1.7 2011/02/11 09:14:20 ralf Exp $
 *
 *  Applications must not include this file directly.
 */

#ifndef _RTEMS_SCORE_INTERRUPTS_H
#define _RTEMS_SCORE_INTERRUPTS_H

#ifndef ASM

struct 	__rtems_raw_irq_connect_data__;

typedef void (*rtems_raw_irq_hdl)		(void);
typedef void (*rtems_raw_irq_enable)		(const struct __rtems_raw_irq_connect_data__*);
typedef void (*rtems_raw_irq_disable)		(const struct __rtems_raw_irq_connect_data__*);
typedef int  (*rtems_raw_irq_is_enabled)	(const struct __rtems_raw_irq_connect_data__*);

/*
 *  Interrupt Level Macros
 */

// RTEMSVCPU: 19/05/2012
// lots of unused variable errors in other files, caused by unused _level
// variable in the following
#include  <rtems/score/wrapper.h>
#include <rtems/l4vcpu/l4vcpu.h>

l4vcpu_irq_state_t l4rtems_vcpu_irq_state;
extern sharedvars_t *sharedVariableStruct;

#define i386_disable_interrupts( _level ) \
  { l4rtems_vcpu_irq_state = \
      l4vcpu_irq_disable_save( sharedVariableStruct->vcpu ); \
  }

#define i386_enable_interrupts( _level )  \
  { l4vcpu_irq_restore( \
      sharedVariableStruct->vcpu, l4rtems_vcpu_irq_state, l4_utcb(), \
      NULL, NULL); \
  }

#define i386_flash_interrupts( _level ) \
  { \
    __asm__ volatile ( "push %0 ; \
                    popf ; \
                    cli" \
                    : : "rm" ((_level)) : "cc" \
    ); \
  }

#define i386_get_interrupt_level( _level ) \
  do { \
    register uint32_t   _eflags; \
    \
    __asm__ volatile ( "pushf ; \
                    pop %0" \
                    : "=rm" ((_eflags)) \
    ); \
    \
    _level = (_eflags & EFLAGS_INTR_ENABLE) ? 0 : 1; \
  } while (0)

#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level ) i386_enable_interrupts( _level )

#endif
#endif
