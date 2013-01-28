

#include <rtems/l4vcpu/l4vcon.h>
/* L4RTEMS: include the wrapper.h to get the sharedVariableStruct */
#include <rtems/score/wrapper.h>


extern sharedvars_t *sharedVariableStruct;

l4_fastcall void
l4rtems_outch( const char c )
{ // write one byte long 'c' to the log
  l4_vcon_write( sharedVariableStruct->logcap, &c, 1 );
}

l4_fastcall int
l4rtems_inch( void )
{ // read one byte from the log
  char read;
  l4_vcon_read( sharedVariableStruct->logcap, &read, 1 );
  return read;
}
