#ifndef HANDLER_HEADER_H
#define HANDLER_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <rtems/l4vcpu/l4vcpu.h>
#include <rtems/l4vcpu/l4sys-vcpu.h>



static unsigned long fs, ds;
static l4_vcpu_state_t *vcpuh;

void handler( void );


#ifdef __cplusplus
}
#endif


#endif // HANDLER_HEADER_H
