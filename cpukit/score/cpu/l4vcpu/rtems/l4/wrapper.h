
/*
 * This is my first attempt to create a wrapper for RTEMS to get it on L4 via 
 * the vCPU interface.
 *
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef L4RTEMS_WRAPPER
#define L4RTEMS_WRAPPER

static void
irq_start();

static void
irq_pending();

static void
starter();

unsigned long
load_elf( char *name, unsigned long *initial_sp );

/* provides access to the vcpu irq interface  */

extern unsigned long __attribute__((weak))
l4rtems_port_irq_in( unsigned long _port);

extern void
l4rtems_port_irq_out( unsigned long _port, unsigned long _value)
__attribute__((weak));



/* IRQ Handling */

  
static void
l4rtems_irq_disable( void );


static void
l4rtems_irq_disable_save( void );


static void
l4rtems_irq_enable( void );


static void
l4rtems_irq_restore( void );


#endif /* !L4RTEMS_WRAPPER */
