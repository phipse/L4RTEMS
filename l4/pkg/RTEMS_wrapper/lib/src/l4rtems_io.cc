/*
 * There are much nicer ways to do this, but they need deeper modifications
 * of the RTEMS source code and the BSP_output_char_function_type and 
 * BSP_polling_get_char_function_type are the provided interfaces.
 */


#include <l4/util/util.h>
#include <l4/util/port_io.h>
//#include <contrib/libio-io/l4/io/io.h>
#include <l4/RTEMS_wrapper/l4rtems_wrapper.h>
#include <l4/sys/irq>
#include <l4/sys/vcpu.h>
#include <l4/sys/thread>
#include <l4/sys/factory>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/util/atomic.h>
#include <l4/re/c/log.h>
#include <l4/sys/kdebug.h>
#include <string.h>

#if 0
extern sharedvars_t* sharedVariableStruct;

void
l4rtems_outch( char c )
{
  const int j = 512;
  static char buf_out[j];
  static int i = 0;

  if( c == '\n'  || c == 0 )
  {
    buf_out[i] = '\n';
    l4_vcon_write( sharedVariableStruct->logcap, buf_out, ++i );
//    l4re_log_print_srv( sharedVariableStruct->logcap, buf_out );
    memset( buf_out, 0, j );
    i = 0;
  }
  else
  {
    buf_out[i] = c;
    ++i;
  }
}


int
l4rtems_inch( void )
{
  const int buffsize = 512;
  static char buff[ buffsize ];
  static int read = 0;
  static int charptr = 0;
  static int flag = 0;

  if( flag != 0  && charptr == read )
  {
    read = l4_vcon_read( sharedVariableStruct->logcap, buff, buffsize );
    if( read <=  buffsize )
      flag = 0;
    charptr = 0;
  }
  

  if( read == 0 || charptr == read )
  {
    read = l4_vcon_read( sharedVariableStruct->logcap, buff, buffsize );
    charptr = 0;
    if( read < 0 )
      return read;
    else if( read > buffsize )
      flag++;
  }

  return buff[charptr++];

}
#endif // #if 0



unsigned int
l4rtems_inport( unsigned int port, unsigned int size )
{// magic size numbers: 0 -> byte, 1 -> word, 2 -> long 
  switch( size )
  {
    case( 0 ): return l4util_in8( port );
    case( 1 ): return l4util_in16( port );
    case( 2 ): return l4util_in32( port );
    default:// printf( "Inport: Bad size value specified: %u\n\n", size );
	     l4_sleep_forever(); 
	     return -1;
  }
}



void
l4rtems_outport( unsigned int port, unsigned int value, unsigned int size )
{// magic size numbers: 0 -> byte, 1 -> word, 2 -> long
  switch( size )
  {
    case( 0 ): l4util_out8( value, port ); break;
    case( 1 ): l4util_out16( value, port ); break;
    case( 2 ): l4util_out32( value, port ); break;
    default: //printf( "Outport: Bad size value specified: %u\n\n", size );
	     l4_sleep_forever(); 
  }
}


