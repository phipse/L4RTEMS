#ifndef L4RTEMS_TIMER_H
#define L4RTEMS_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#if 0
enum class Protos : long
{
  L4RTEMS_PROTO_TIMER = 31L,
};


enum class Timer_ops
{
  L4RTEMS_TIMER_START = 119UL,
  L4RTEMS_TIMER_STOP = 120UL,
};
#endif

typedef unsigned long long l4rtems_timer_t;

l4_fastcall void 
l4rtems_timer_start(  l4rtems_timer_t period,	  // between two interrupts
		      l4rtems_timer_t first );	  // first interrupt time

l4_fastcall void 
l4rtems_timer_stop( void );


l4_fastcall unsigned int 
l4rtems_timer_read( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* L4RTEMS_TIMER_H */
