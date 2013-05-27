
#include <l4/sys/capability>

static L4::Cap<L4::Thread> timer_thread_cap;

void
timer_init( L4::Cap<L4::Thread> guest_cap, L4::Cap<L4::Irq> irq_cap );

#include "l4rtems_timer_IPC.h"
