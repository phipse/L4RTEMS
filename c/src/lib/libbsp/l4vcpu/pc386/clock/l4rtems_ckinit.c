/*
 *  Clock Tick Device Driver
 *
 *  History:
 *    + Original driver was go32 clock by Joel Sherrill
 *    + go32 clock driver hardware code was inserted into new
 *      boilerplate when the pc386 BSP by:
 *        Pedro Miguel Da Cruz Neto Romano <pmcnr@camoes.rnl.ist.utl.pt>
 *        Jose Rufino <ruf@asterix.ist.utl.pt>
 *    + Reworked by Joel Sherrill to use clock driver template.
 *      This removes all boilerplate and leave original hardware
 *      code I developed for the go32 BSP.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: ckinit.c,v 1.24 2010/03/10 17:16:01 joel Exp $
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bspopts.h>
#include <libcpu/cpuModel.h>

volatile uint32_t pc386_microseconds_per_isr;
volatile uint32_t pc386_isrs_per_tick;
uint32_t pc386_clock_click_count;


/* this driver may need to count ISRs per tick */
#define CLOCK_DRIVER_ISRS_PER_TICK pc386_isrs_per_tick

/* if so, the driver may use the count in Clock_driver_support_at_tick */
#ifdef CLOCK_DRIVER_ISRS_PER_TICK
extern volatile uint32_t Clock_driver_isrs;
#endif

/*
 *  Hooks which get swapped based upon which nanoseconds since last
 *  tick method is preferred.
 */
void     (*Clock_driver_support_at_tick)(void) = NULL;
uint32_t (*Clock_driver_nanoseconds_since_last_tick)(void) = NULL;


void Clock_driver_support_at_tick_empty(void)
{
}

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = NULL; \
  } while(0)

extern volatile uint32_t Clock_driver_isrs;
#if 0
uint32_t bsp_clock_nanoseconds_since_last_tick_tsc(void)
{
  /******
   * Get nanoseconds using Pentium-compatible TSC register
   ******/

  uint64_t                 diff_nsec;

  diff_nsec = rdtsc() - pc586_tsc_at_tick;

  /*
   * At this point, with a hypothetical 10 GHz CPU clock and 100 Hz tick
   * clock, diff_nsec <= 27 bits.
   */
  diff_nsec *= pc586_nanoseconds_per_tick; /* <= 54 bits */
  diff_nsec /= pc586_tsc_per_tick;

  if (diff_nsec > pc586_nanoseconds_per_tick)
    /*
     * Hmmm... Some drift or rounding. Pin the value to 1 nanosecond before
     * the next tick.
     */
    /*    diff_nsec = pc586_nanoseconds_per_tick - 1; */
    diff_nsec = 12345;

  return (uint32_t)diff_nsec;
}

uint32_t bsp_clock_nanoseconds_since_last_tick_i8254(void)
{
  // RTEMSVCPU: use wrapper interface to get passed nanoseconds
  // e.g. L4_EXTERNAL_FUNC(l4rtems_nanoseconds_since_last_tick)
  // uint32_t nanosecs = l4rtems_nanoseconds_since_last_tick();
  // return naosecs;

  /******
   * Get nanoseconds using 8254 timer chip
   ******/

  uint32_t                 usecs, clicks, isrs;
  uint32_t                 usecs1, usecs2;
  uint8_t                  lsb, msb;
  rtems_interrupt_level    level;

  /*
   * Fetch all the data in an interrupt critical section.
   */
  rtems_interrupt_disable(level);
    READ_8254(lsb, msb);
    isrs = Clock_driver_isrs;
  rtems_interrupt_enable(level);

  /*
   *  Now do the math
   */
  /* convert values read into counter clicks */
  clicks = ((msb << 8) | lsb);

  /* whole ISRs we have done since the last tick */
  usecs1 = (pc386_isrs_per_tick - isrs - 1) * pc386_microseconds_per_isr;

  /* the partial ISR we in the middle of now */
  usecs2 = pc386_microseconds_per_isr - TICK_TO_US(clicks);

  /* total microseconds */
  usecs = usecs1 + usecs2;
  #if 0
    printk( "usecs1=%d usecs2=%d ", usecs1, usecs2 );
    printk( "maxclicks=%d clicks=%d ISRs=%d ISRsper=%d usersPer=%d usecs=%d\n",
    pc386_clock_click_count, clicks,
    Clock_driver_isrs, pc386_isrs_per_tick,
    pc386_microseconds_per_isr, usecs );
  #endif

  /* return it in nanoseconds */
  return usecs * 1000;

}

#endif //if 0

volatile uint32_t Clock_driver_ticks;

void l4rtems_clockisr( void )
{
  // Increment accurate counter
  Clock_driver_ticks += 1;
  rtems_clock_tick();

  if( (Clock_driver_ticks % 10) == 0 )
    printk( "clock interrupt \n" );
}

void Clock_driver_support_initialize_hardware(void)
{ // just aquire the IRQ and use the 8254 behaviour. No TSC support for now!
  
  // initialize clock tick counter
  Clock_driver_ticks = 0;
  // use l4rtems_timer as clock device
  rtems_status_code sc = rtems_interrupt_handler_install( BSP_PERIODIC_TIMER,
							"l4rtems_timer",
							RTEMS_INTERRUPT_UNIQUE,
							l4rtems_clockisr, 
							0 );
  if( sc != RTEMS_SUCCESSFUL )
  {
    printk( "Unable to install l4rtems_timer interrupt handler\n" );
    rtems_fatal_error_occurred(1);
  }
  else
  {
    printk( "Successfully installed l4rtems_timer interrupt handler\n" );
  }


}

#define Clock_driver_support_shutdown_hardware() \
  do { \
    rtems_interrupt_handler_remove( BSP_PERIODIC_TIMER, l4rtems_clockisr, 0); \
  } while (0)

#include "../../../shared/clockdrv_shell.h"

