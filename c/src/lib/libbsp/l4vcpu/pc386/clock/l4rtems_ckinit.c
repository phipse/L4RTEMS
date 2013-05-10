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

#define DEBUG 0
#define FIRST_TICK_OFFSET 0

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
volatile uint32_t Clock_driver_ticks;
extern rtems_configuration_table Configuration;



void l4rtems_clockisr( void )
{
  // Increment accurate counter
  Clock_driver_ticks += 1;
  rtems_clock_tick();
#if DEBUG
  if( (Clock_driver_ticks % 10) == 0 )
    printk( "clock interrupt nb: %u\n", Clock_driver_ticks );
#endif
}



unsigned int 
l4rtems_clock_nanoseconds_since_last_tick( void )
{
  /* Reads the current clock and computes the difference since last tick.
   * As it is possible to delay the first tick by not a full period, we have to
   * take this into account.*/

  printk( "nanoseconds invoked\n");
  uint32_t currentTime = l4rtems_timer_read();
  uint32_t tickPeriod = rtems_configuration_get_microseconds_per_tick();
  uint32_t msSinceTick = (currentTime - FIRST_TICK_OFFSET) % tickPeriod;
  return msSinceTick*1000; 
}



void Clock_driver_support_initialize_hardware(void)
{ 
  
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
    printk( "Starting the clock with period: %u ms \n", rtems_configuration_get_microseconds_per_tick() );
    l4rtems_timer_start( rtems_configuration_get_microseconds_per_tick(), FIRST_TICK_OFFSET );
  }

  Clock_driver_nanoseconds_since_last_tick = l4rtems_clock_nanoseconds_since_last_tick;

}





#define Clock_driver_support_shutdown_hardware() \
  do { \
    rtems_interrupt_handler_remove( BSP_PERIODIC_TIMER, l4rtems_clockisr, 0); \
  } while (0)

#include "../../../shared/clockdrv_shell.h"

