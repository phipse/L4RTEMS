
#include <l4/sys/capability>

static L4::Cap<L4::Thread> timer_thread_cap;

L4::Cap<L4::Irq>
timer_init( L4::Cap<L4::Thread> guest_cap );


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef unsigned long long l4rtems_timer_t;

l4_fastcall void 
l4rtems_timer_start(  l4rtems_timer_t period,	  // between two interrupts
		      l4rtems_timer_t first );	  // first interrupt time

l4_fastcall void 
l4rtems_timer_stop( void );

#ifdef __cplusplus
}
#endif //__cplusplus
