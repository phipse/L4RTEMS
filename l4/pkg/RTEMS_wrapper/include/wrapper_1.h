
/*
 * This is my first attempt to create a wrapper for RTEMS to get it on L4 via 
 * the vCPU interface.
 * !Header file!
 *
 * Contact: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

/* C includes */
#include <l4/sys/vcpu.h>
#include <l4/vcpu/vcpu.h>
#include <l4/sys/utcb.h>
#include <l4/sys/types.h>

static l4_vcpu_state_t *vcpuh;

static void handler();

/* setup_user_state:
 * save the current fs, ds and set the vcpu ss */
static void
setup_user_state( l4_vcpu_state_t *vcpu);

/* handler_prolog:
 * recover the saved host fs & ds */
static void
handler_prolog();

static void
irq_start();

static void
irq_pending();

static void
starter();

l4_umword_t
load_elf( char *name, l4_umword_t *initial_sp );

/* provides access to the vcpu irq interface  */

extern l4_umword_t  __attribute__((weak)) 
l4rtems_port_irq_in( l4_umword_t _port);

extern void
l4rtems_port_irq_out( l4_umword_t _port, l4_umword_t _value)
__attribute__((weak));

static void
l4rtems_irq_disable( void )
{
l4vcpu_irq_disable( vcpuh );
}

static l4vcpu_irq_state_t saved_irq_state;

static void
l4rtems_irq_disable_save( void )
{
saved_irq_state = l4vcpu_irq_disable_save( vcpuh );
}

static void
l4rtems_irq_enable( void )
{
l4vcpu_irq_enable( vcpuh, l4_utcb(), (l4vcpu_event_hndl_t) handler, 
    (l4vcpu_setup_ipc_t) irq_start );
}

static void
l4rtems_irq_restore( void )
{
l4vcpu_irq_restore( vcpuh, saved_irq_state, l4_utcb(), 
    (l4vcpu_event_hndl_t) handler, (l4vcpu_setup_ipc_t) irq_start );
}

#endif /* !L4RTEMS_WRAPPER */
