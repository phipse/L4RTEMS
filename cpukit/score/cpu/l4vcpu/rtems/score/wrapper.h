
/*
 * This is my first attempt to create a wrapper for RTEMS to get it on L4 via 
 * the vCPU interface.
 *
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

#ifdef __cplusplus
extern "C" {
#endif

  #include <rtems/l4vcpu/l4vcpu.h>
  #include <rtems/l4vcpu/l4reenv.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct guestHostShare
{
  l4_vcpu_state_t   *vcpu;    // pointer to vcpu struct
  l4_cap_idx_t	    logcap;   // capability for the log to use print
  unsigned long	    ufs, uds; // user/host fs ds
  l4re_env_t*	    l4re_env; // l4re environment pointer
  unsigned long	    (*external_resolver)(void); //funcpointer external resolver
} sharedvars_t;

//global vars
static l4_cap_idx_t _vcpu_cap;

void
l4rtems_outch( char c );

int
l4rtems_inch( void );

void
starter( void );

unsigned long
load_elf( char *name, unsigned long *initial_sp );



bool
l4rtems_serve_requestIrq( unsigned irqNbr );
void
l4rtems_serve_detachIrq( unsigned irqNbr );

// programmable periodic timer interrupt
void l4rtems_timer( unsigned long period );


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
unsigned int
l4rtems_inport( unsigned int port, unsigned int size );


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
void
l4rtems_outport( unsigned int port, unsigned int value, unsigned int size );

#ifdef __cplusplus
}
#endif

#endif /* !L4RTEMS_WRAPPER */
