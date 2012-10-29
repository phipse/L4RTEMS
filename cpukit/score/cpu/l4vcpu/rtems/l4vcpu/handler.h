#ifndef HANDLER_HEADER_H
#define HANDLER_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <rtems/l4vcpu/l4vcpu.h>
#include <rtems/l4vcpu/l4sys-vcpu.h>


void l4rtems_handler( void );
void l4rtems_setup_ipc( l4_utcb_t* utcb );

#ifdef __cplusplus
}
#endif


#endif // HANDLER_HEADER_H
