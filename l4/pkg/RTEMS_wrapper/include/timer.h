#ifndef L4RTEMS_TIMER_H
#define L4RTEMS_TIMER_H


#include <l4/sys/irq>
#include <l4/sys/thread>


#ifdef __cplusplus
extern "C" {
#endif // cplusplus


static L4::Cap<L4::Irq> timerIRQ;

struct timerVariables
{
  bool init = false;
  int period;
  char timer_stack[8<<10];
  bool timerIsOn;
  L4::Cap<L4::Thread> thread_cap;
  L4::Cap<L4::Thread> timer;
  L4::Thread::Attr attr_time;
};
typedef struct timerVariables timerVars;

bool l4rtems_timerIsInit();
void l4rtems_timerInit(L4::Cap<L4::Thread> thread_cap );
bool l4rtems_timerIsOn();
void l4rtems_timerOn();
void l4rtems_timerOff();
void l4rtems_setTimerPeriod( unsigned long per );


#ifdef __cplusplus
}
#endif // cplusplus

#endif // L4RTEMS_TIMER_H
