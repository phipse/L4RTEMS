#ifndef HANDLER_HEADER_H
#define HANDLER_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <rtems/l4vcpu/l4vcpu.h>
#include <rtems/l4vcpu/l4sys-vcpu.h>


void l4rtems_handler( l4_vcpu_state_t* vcpuh );
void l4rtems_setup_ipc( l4_utcb_t* utcb );
void handleIrq( l4_vcpu_state_t *vcpuh );


#ifdef __cplusplus
}
#endif


#endif // HANDLER_HEADER_H
