
/*
 * This is my first attempt to create a wrapper for RTEMS to get it on L4 via 
 * the vCPU interface.
 *
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

#if L4RTEMS
  #include <rtems/l4vcpu/l4vcpu.h>
#else
  #include <l4/sys/vcpu.h>
#endif

typedef struct guestHostShare
{
  l4_vcpu_state_t *vcpu; 
} sharedvars_t;


void
irq_start( void );

void
irq_pending( void );

void
starter( void );

unsigned long
load_elf( char *name, unsigned long *initial_sp );

/* provides access to the vcpu irq interface  */

unsigned long __attribute__((weak))
l4rtems_port_irq_in( unsigned long _port);

void
l4rtems_port_irq_out( unsigned long _port, unsigned long _value)
__attribute__((weak));


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
unsigned int
l4rtems_inport( unsigned int port, unsigned int size );


// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
void
l4rtems_outport( unsigned int port, unsigned int value, unsigned int size );

/* IRQ Handling */

  
void
l4rtems_irq_disable( void );


void
l4rtems_irq_disable_save( void );


void
l4rtems_irq_enable( void );


void
l4rtems_irq_restore( void );


#endif /* !L4RTEMS_WRAPPER */
