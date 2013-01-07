#ifndef L4RTEMS_TIMER_H
#define L4RTEMS_TIMER_H


#include <rtems/l4vcpu/l4irq.h>
#include <rtems/l4vcpu/l4thread.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

bool l4rtems_timerIsOn();
void l4rtems_timerOn();
void l4rtems_timerOff();
void l4rtems_setTimerPeriod( unsigned long per );



#ifdef __cplusplus
}
#endif


#endif // L4RTEMS_TIMER_H
