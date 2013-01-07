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


L4_EXTERNAL_FUNC(l4rtems_timerIsOn);
L4_EXTERNAL_FUNC(l4rtems_timerOn);
L4_EXTERNAL_FUNC(l4rtems_timerOff);
L4_EXTERNAL_FUNC(l4rtems_setTimerPeriod);

#ifdef __cplusplus
}
#endif


#endif // L4RTEMS_TIMER_H
