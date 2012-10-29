#define DEBUG 0

#include <rtems/l4vcpu/handler.h>
#include <rtems/l4vcpu/l4thread.h>
#include <rtems/l4vcpu/l4util.h>
#include <rtems/score/wrapper.h>
#include <bsp/irq-generic.h>

#include <stdio.h>



static unsigned long fs, ds;
static l4_vcpu_state_t *vcpuh;
extern sharedvars_t* sharedVariableStruct;
  
void
handler_prolog( void )
{ /* handler_prolog: recover the saved host fs & ds */
  fs = sharedVariableStruct->ufs;
  ds = sharedVariableStruct->uds;
#if !DEBUG
  printk( "fs: %x\n", fs);
//  enter_kdebug( "handler prolog" );
#endif
  asm volatile( 
      " mov %0, %%es \t\n"
      " mov %0, %%ds \t\n"
      " mov %1, %%fs \t\n"
      : : "r"(ds),"r"(fs) );
}



void
l4rtems_handler( l4_vcpu_state_t* vcpuh )
{ /* This handler is invoked, if the guest system suffers a page fault or an
     interrupt. The handler checks the entry reason, and replies with an 
     apropriate action. */
  printk("Hello Handler\n");
  if( vcpuh == NULL )
    vcpuh = sharedVariableStruct->vcpu;
#if DEBUG
  //l4vcpu_print_state( vcpuh, "State: ");
  enter_kdebug("handler entry");
#endif
  handler_prolog();
  vcpuh->state &= L4_VCPU_F_EXCEPTIONS;
  printk("vcpu->trapno: %lx\n", vcpuh->r.trapno);
  
  /* checking the entry reason */
  if( l4vcpu_is_page_fault_entry( vcpuh ) )
  { /* This shouldn't be happening */
    printk("page fault entry\n");
    l4_umword_t pfnum = vcpuh->r.pfa;
    printk("pfa: %lx\n",pfnum);
    /* where do I get the snd_base? */
//    vcpu_task->map( L4Re::This_task, 
//	l4_fpage( pfnum, L4_PAGESIZE, L4_FPAGE_RWX ),
//	 0);
    printk("Handler: Page mapped\n");
    /* copied out of vcpu example: why state add? */
    vcpuh->saved_state |= L4_VCPU_F_PAGE_FAULTS ;
    printk("Handler: saved_state added\n");
  }
  else if( l4vcpu_is_irq_entry( vcpuh ) )
  {
//    vcpuh->saved_state |= L4_VCPU_F_IRQ;
    printk("irq entry!\n" );
    switch( vcpuh->i.label )
    {
      case(9000):
	   printk("Triggered Timer IRQ\n");
	   break;
      default:
	   printk("Unrecognized IRQ\n");
	   printk("irq: %lx \n", vcpuh->i.label );
	   bsp_interrupt_handler_dispatch( vcpuh->i.label );
    }
  }
  else
  {
    //l4vcpu_print_state( vcpuh, "Handler Unknown Entry:");
    printk( "unknown entry reason! \n" );
  }

//  vcpu->print_state("resume ");
  l4_cap_idx_t self = L4_INVALID_CAP;
  l4_thread_vcpu_resume_commit( self, l4_thread_vcpu_resume_start() );

  printk("this should NOT be reached: handler.c\n");
}


void
handleIrq( l4_vcpu_state_t *vcpuh )
{
    printk("irq entry!\n" );
    switch( vcpuh->i.label )
    {
      case(9000):
	   printk("Triggered Timer IRQ\n");
	   break;
      default:
	   printk("Unrecognized IRQ\n");
	   printk("irq: %lx \n", vcpuh->i.label );
	   bsp_interrupt_handler_dispatch( vcpuh->i.label );
    }
}


void
l4rtems_setup_ipc( l4_utcb_t* utcb )
{
  (void*) utcb;
  return;  
}
