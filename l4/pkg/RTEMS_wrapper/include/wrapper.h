/*
 * Purpose: Header file for all non shared functions in the L4RTEMS_wrapper.
 *
 * Maintainer: philipp.eppelt@mailbox.tu-dresden.de
 * 
 */

#ifndef WRAPPER
#define WRAPPER

#ifdef __cplusplus
extern "C" {
#endif

void
startTimerService( void );

void
starter( void );

unsigned long
load_elf( char *name, unsigned long *initial_sp );


#ifdef __cplusplus
}
#endif

#endif /* !WRAPPER */
