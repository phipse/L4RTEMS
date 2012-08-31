
/*
 * This is my first attempt to create a wrapper for RTEMS to get it on L4 via 
 * the vCPU interface.
 *
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

#include <rtems/l4vcpu/l4vcpu.h>

typedef struct guestHostShare
{
  l4_vcpu_state_t *vcpu; 
// framebuffer console stuff - atm unneccessary
//  unsigned long *bitmapBaseAddr;
//  unsigned long ioCrtBaseAddr;
//  unsigned long linesPerPage;
//  unsigned long columnsPerPage;
} sharedvars_t;


void
l4rtems_outch( char c );

void
starter( void );

unsigned long
load_elf( char *name, unsigned long *initial_sp );


// programmable periodic timer interrupt
void l4rtems_timer( unsigned long period );


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
unsigned int
l4rtems_inport( unsigned int port, unsigned int size );


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
void
l4rtems_outport( unsigned int port, unsigned int value, unsigned int size );

#endif /* !L4RTEMS_WRAPPER */
