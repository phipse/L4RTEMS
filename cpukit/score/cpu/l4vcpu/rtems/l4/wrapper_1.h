
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

#if L4RTEMS
  #include <rtems/l4vcpu/l4vcpu.h>
#else
  #include <l4/sys/vcpu.h>
#endif
#include <stdio.h>
#include <stdbool.h>


typedef struct guestHostShare
{
  l4_vcpu_state_t *vcpu;  // pointer to vcpu struct
  unsigned buff_size;	  // size of the I/O buffers
  char* buff_in;	  // pointer to input buffer
  unsigned* inready;	  // buff_in ready flag
  l4_cap_idx_t logcap;	  // capability for the log to use print
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


bool l4rtems_requestIrq( unsigned irqNbr );

void l4rtems_detachIrq( unsigned irqNbr );

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
