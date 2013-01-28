
#include <string.h>
#include <rtems/l4vcpu/l4vcon.h>
/* L4RTEMS: include the wrapper.h to get the sharedVariableStruct */
#include <rtems/score/wrapper.h>


extern sharedvars_t *sharedVariableStruct;

l4_fastcall void
l4rtems_outch( const char c )
{ // buffer output until newline or null and then print to log
  static char buf_out[ 512 ];
  static int i = 0;

  if( c == '\n'  || c == 0 )
  {
    buf_out[i] = '\n';
    l4_vcon_write( sharedVariableStruct->logcap, buf_out, ++i );
    memset( buf_out, 0, i );
    i = 0;
  }
  else
  {
    buf_out[i] = c;
    ++i;
  }
  //l4_vcon_write( sharedVariableStruct->logcap, &c, 1 );
}

l4_fastcall int
l4rtems_inch( void )
{ // read one byte from the log
  char read;
  l4_vcon_read( sharedVariableStruct->logcap, &read, 1 );
  return read;
}
