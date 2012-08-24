/*
 * There are much nicer ways to do this, but they need deeper modifications
 * of the RTEMS source code and the BSP_output_char_function_type and 
 * BSP_polling_get_char_function_type are the provided interfaces.
 */


#include <stdio.h>
#include <l4/util/util.h>
#include <l4/util/port_io.h>
#include <l4/RTEMS_wrapper/l4rtems_io.h> 
#include <l4/RTEMS_wrapper/wrapper_1.h>

static char buf_out[256];
static int i = 0;

void
l4rtems_outch( char c )
{
  if( c == '\n' )
  {
    printf( "%s\n", buf_out);
//    fprintf( stdout, "%s\n", buf_out); 
    i = 0;
  }
  else
  {
    buf_out[i] = c;
    ++i;
  }
}


static char buf_in[256];
static int o = -1;

int
l4rtems_inch( void )
{
  if( o == -1 )
  {
    o = scanf( "%s", buf_in );
//    o = fscanf( stdin, "%s", buf_in);
    if( o == 0 )
      return 0;
    // here should be some check, to assure the validity of the input
    // if( j == EOF )
    // fprintf( stdout, "PANIC: Nothing to read\n");
  }
  else
  {
    --o;
    return buf_in[ o+1 ];
  }

  printf("PANIC: l4rtems_inch: should never be reached\n");
  l4_sleep_forever();
}



unsigned int
l4rtems_inport( unsigned int port, unsigned int size )
{// magic size numbers: 0 -> byte, 1 -> word, 2 -> long 
  switch( size )
  {
    case( 0 ): return l4util_in8( port );
    case( 1 ): return l4util_in16( port );
    case( 2 ): return l4util_in32( port );
    default: printf( "Inport: Bad size value specified: %u\n\n", size );
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
    default: printf( "Outport: Bad size value specified: %u\n\n", size );
	     l4_sleep_forever(); 
  }
}

#if 0
void
l4rtems_irq_disable_save( void )
{
 void* dummy; 
 //  saved_irq_state = l4vcpu_irq_disable_save( vcpuh );
}
#endif
