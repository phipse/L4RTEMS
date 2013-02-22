/*  
 *  Date: 07/01/2012
 *  Maintainer: Philipp Eppelt
 *  Contact: philipp.eppelt@mailbox.tu-dresden.de
 *
 *  Purpose: RTEMS timer interface for the l4rtems_timer.
 */


#include <rtems/l4vcpu/timer.h>

#include <stdlib.h>
#include <unistd.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <libcpu/cpuModel.h>
#include <rtems/score/wrapper.h>




volatile uint32_t         Ttimer_val;
extern void l4rtems_timerisr( void );

static int
timerIsOn( const rtems_raw_irq_connect_data* unused )
{
  return false;
}

static void
timerOn( const rtems_raw_irq_connect_data* used ) 
{
  l4rtems_timer_start( 10ULL, Ttimer_val );
}

static void
timerOff( const rtems_raw_irq_connect_data* unused )
{
  l4rtems_timer_stop();
}


static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
  l4rtems_timerisr,
  timerOn,
  timerOff,
  timerIsOn
};

static bool init = false;

extern void 
rtems_irq_prologue_0(void);

void 
l4rtems_timer_exit( void )
{
  i386_delete_idt_entry (&timer_raw_irq_data);
  timerOff( NULL );
}

void 
l4rtems_timerInit( void )
{
  printk( "Timer init called\n");
  if( !init )
  {
    init = true;
  
    rtems_raw_irq_connect_data raw_irq_data = {
      BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
      rtems_irq_prologue_0,
      NULL,
      NULL,
      NULL
    };

    i386_delete_idt_entry (&raw_irq_data);

    atexit(l4rtems_timer_exit);            /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("raw handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }
  }

  /* wait for ISR to be called at least once */
  Ttimer_val = 0;
  while(Ttimer_val == 0)
    continue;
  printk( "Timer initialized!" );
  Ttimer_val = 0;
}


/*
 *  loop which waits at least timeToWait ms
 */
void Wait_X_ms( unsigned int timeToWait)
{
  usleep(timeToWait);
}
