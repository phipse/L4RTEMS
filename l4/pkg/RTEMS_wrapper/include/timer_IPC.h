
#include <l4/sys/capability>

static L4::Cap<L4::Thread> timer_thread_cap;

void
timer_init( L4::Cap<L4::Thread> guest_cap, L4::Cap<L4::Irq> irq_cap );


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

enum class Protos : long
{
  L4RTEMS_PROTO_TIMER = 31L,
};


enum class Timer_ops
{
  L4RTEMS_TIMER_START = 119UL,
  L4RTEMS_TIMER_STOP = 120UL,
};

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
#endif //__cplusplus
