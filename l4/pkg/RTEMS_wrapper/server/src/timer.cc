#include <l4/RTEMS_wrapper/timer.h>
#include <cstdio>

#include <l4/re/util/cap_alloc>
#include <l4/re/env>
#include <l4/util/util.h>
#include <l4/sys/factory>
#include <l4/sys/scheduler>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static timerVars tv;

bool
l4rtems_timerIsInit()
{
  return tv.init;
}


  
static void 
l4rtems_timer( )
{ /* This function triggers an IRQ to test the IRQ entry capability of the running 
     rtems guest application. The timer resolution is milliseconds.
     If no parameter is set, the default period is 1ms. */

  printf( "Hello timer\n" );
  
  while(1)
  {
    timerIRQ->trigger();
    l4_sleep( tv.period );
  }
}



void
l4rtems_timerInit(L4::Cap<L4::Thread> thread_cap)
{
  tv.thread_cap = thread_cap;
  tv.timerIsOn = false;

  // create new IRQ
  timerIRQ = L4Re::Util::cap_alloc.alloc<L4::Irq>();
  L4Re::Env::env()->factory()->create_irq( timerIRQ );
  
  // create timer thread
  l4_addr_t kumem = (l4_addr_t)l4re_env()->first_free_utcb;
  l4re_env()->first_free_utcb += L4_UTCB_OFFSET;
  l4_utcb_t *utcb_time = (l4_utcb_t *)kumem;

  tv.attr_time.pager( L4::cap_reinterpret_cast<L4::Thread>( L4Re::Env::env()->rm() ) );
  tv.attr_time.exc_handler( L4Re::Env::env()->main_thread() );
  tv.attr_time.bind( utcb_time, L4Re::This_task);

  tv.timer->control( tv.attr_time );
  tv.timer->ex_regs( (l4_umword_t) l4rtems_timer,
		  (l4_umword_t) tv.timer_stack + sizeof(tv.timer_stack),
		  0 );
}



bool
l4rtems_timerIsOn()
{
  return tv.timerIsOn;
}


void
l4rtems_timerOn()
{
  // schedule timer thread
  tv.timerIsOn = true;
  L4Re::Env::env()->scheduler()->run_thread( tv.timer, l4_sched_param(4) ); 
}


void
l4rtems_timerOff()
{
  // deschedule timer thread;
  tv.timerIsOn = false;
  // How to stop/pause the thread?

}



void
l4rtems_setTimerPeriod( unsigned long per )
{
  tv.period = per;
}


#ifdef __cplusplus
}
#endif
