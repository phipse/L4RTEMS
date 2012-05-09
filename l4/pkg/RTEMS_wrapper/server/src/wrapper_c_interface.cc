/*
 * As RTEMS is written in C, there is the need to have an interface, which 
 * provides the necessary C++ functionality, for a C programm.
 *
 * Contact: philipp.eppelt@mailbox.tu-dresden.de
 */ 
/*
#include <l4/RTEMS_wrapper/wrapper_1.h>


// provides access to the vcpu irq interface  

static void
irq_disable_c( void )
{
  l4vcpu_irq_disable( getVcpu() );
}

static l4vcpu_irq_state_t saved_irq_state;

static void
irq_disable_save_c( void )
{
  saved_irq_state = l4vcpu_irq_disable_save( getVcpu() );
}

static void
irq_enable_c( void )
{
  l4vcpu_irq_enable( getVcpu(), l4_utcb(), (l4vcpu_event_hndl_t) handler, 
      (l4vcpu_setup_ipc_t) irq_start );
}

static void
irq_restore_c( void )
{
  l4vcpu_irq_restore( getVcpu(), saved_irq_state, l4_utcb(), 
      (l4vcpu_event_hndl_t) handler, (l4vcpu_setup_ipc_t) irq_start );
}
*/
