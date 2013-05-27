/*
 * Purpose: This file is shared between L4 and RTEMS and is providing access to
 *	    host functionaly.
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * Date:    27/05/2013
 */


#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

#ifdef __cplusplus
extern "C" {
#endif

#if L4RTEMS
  #include <rtems/l4vcpu/l4vcpu.h>
  #include <rtems/l4vcpu/l4reenv.h>
#else
  #include <l4/sys/vcpu.h>
  #include <l4/re/env.h>
#endif

typedef struct guestHostShare
{
  l4_vcpu_state_t   *vcpu;    // pointer to vcpu struct
  l4_cap_idx_t	    logcap;   // capability for the log to use print
  unsigned long	    ufs, uds; // user/host fs ds
  l4re_env_t*	    l4re_env; // l4re environment pointer
  unsigned long	    (*external_resolver)(void); //funcpointer external resolver
} sharedvars_t;


// console services
l4_fastcall void
l4rtems_outch( char c );

l4_fastcall int
l4rtems_inch( void );

// irq services
l4_fastcall bool
l4rtems_requestIrq( unsigned irqNbr );

l4_fastcall void
l4rtems_detachIrq( unsigned irqNbr );

// programmable periodic timer interrupt
l4_fastcall void 
l4rtems_timer( unsigned long period );

// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
l4_fastcall unsigned int
l4rtems_inport( unsigned int port, unsigned int size );

// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
l4_fastcall void
l4rtems_outport( unsigned int port, unsigned int value, unsigned int size );


#ifdef __cplusplus
}
#endif

#endif /* !L4RTEMS_WRAPPER */
