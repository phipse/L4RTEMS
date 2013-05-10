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
// variable in the following -- used it as irq state container
#include <rtems/score/wrapper.h>
#include <rtems/l4vcpu/handler.h>
#include <rtems/l4vcpu/l4vcpu.h>

extern sharedvars_t *sharedVariableStruct;
extern int disableCallCnt;


#define i386_disable_interrupts( _level ) \
  { \
    _level = l4vcpu_irq_disable_save( sharedVariableStruct->vcpu ); \
  }

//    printk( "irq state saved: %i, disableCallCnt: %i\n", _level, disableCallCnt ); \
    printk( "irq restored state: %x, disableCallCnt: %i\n", _level, disableCallCnt ); \
    disableCallCnt++; \
    disableCallCnt--; \

#define i386_enable_interrupts( _level )  \
  { \
    l4vcpu_irq_restore( \
      sharedVariableStruct->vcpu, \
      _level, \
      l4_utcb(), \
      l4rtems_irq_handler, \
      l4rtems_setup_ipc ); \
  }

#define i386_open_interrupts( ) \
  { \
    l4vcpu_irq_enable( \
      sharedVariableStruct->vcpu, l4_utcb(), \
      l4rtems_irq_handler, l4rtems_setup_ipc ); \
  }

    //printk( ">>>> irq opened\n");  \
    printk( ">>>> VCPU state: %x\n", (l4vcpu_irq_state_t) l4vcpu_state( sharedVariableStruct->vcpu ) ); \
    printk( "<<<< irq closed\n"); \

#define i386_close_interrupts( ) \
  { \
    l4vcpu_irq_disable( sharedVariableStruct->vcpu ); \
  }


#define i386_flash_interrupts( _level ) \
  { \
    i386_enable_interrupts( _level ); \
    i386_disable_interrupts( _level ); \
  }


#define i386_get_interrupt_level( _level ) \
  { \
    l4vcpu_irq_state_t s = (l4vcpu_irq_state_t) l4vcpu_state( sharedVariableStruct->vcpu ); \
    if( s & L4_VCPU_F_IRQ ) _level = 0; \
    else _level = 1; \
  }


#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level ) i386_enable_interrupts( _level )

#endif
#endif
