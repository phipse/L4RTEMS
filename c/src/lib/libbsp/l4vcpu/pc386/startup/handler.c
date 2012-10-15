#define DEBUG 0

#include <rtems/l4vcpu/handler.h>
#include <rtems/l4vcpu/l4thread.h>
#include <rtems/l4vcpu/l4util.h>


#include <stdio.h>

  
void
handler_prolog( void )
{ /* handler_prolog: recover the saved host fs & ds */
#if DEBUG
  printf( "fs: %x\n", fs);
  enter_kdebug( "handler prolog" );
#endif
  asm volatile( 
      " mov %0, %%es \t\n"
      " mov %0, %%ds \t\n"
      " mov %1, %%fs \t\n"
      : : "r"(ds),"r"(fs) );
}



void
handler( void )
{ /* This handler is invoked, if the guest system suffers a page fault or an
     interrupt. The handler checks the entry reason, and replies with an 
     apropriate action. */
  printf("Hello Handler\n");
#if DEBUG
  //l4vcpu_print_state( vcpuh, "State: ");
  enter_kdebug("handler entry");
#endif
  handler_prolog();
  vcpuh->state &= L4_VCPU_F_EXCEPTIONS;
  printf("vcpu->ip: %lx\n", vcpuh->r.ip);
  
  /* checking the entry reason */
  if( l4vcpu_is_page_fault_entry( vcpuh ) )
  { /* This shouldn't be happening */
    printf("page fault entry\n");
    l4_umword_t pfnum = vcpuh->r.pfa;
    printf("pfa: %lx\n",pfnum);
    /* where do I get the snd_base? */
//    vcpu_task->map( L4Re::This_task, 
//	l4_fpage( pfnum, L4_PAGESIZE, L4_FPAGE_RWX ),
//	 0);
    printf("Handler: Page mapped\n");
    /* copied out of vcpu example: why state add? */
    vcpuh->saved_state |= L4_VCPU_F_PAGE_FAULTS ;
    printf("Handler: saved_state added\n");
  }
  else if( l4vcpu_is_irq_entry( vcpuh ) )
  {
    printf("irq entry!\n" );
    switch( vcpuh->i.label )
    {
      case(9000):
	   printf("Triggered Timer IRQ\n");
	   break;
      default:
	   printf("Unrecognized IRQ\n");
	   printf("irq: %lx \n", vcpuh->i.label );
	   //call RTEMS bsp_interrupt_handler_dispatch( vcpu->i()->label() );
    }
  }
  else
  {
    //l4vcpu_print_state( vcpuh, "Handler Unknown Entry:");
  }

//  vcpu->print_state("resume ");
  l4_cap_idx_t self = L4_INVALID_CAP;
  l4_thread_vcpu_resume_commit( self, l4_thread_vcpu_resume_start() );

  printf("hier gehts nicht weiter\n");
}
