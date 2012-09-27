/*
 * There are much nicer ways to do this, but they need deeper modifications
 * of the RTEMS source code and the BSP_output_char_function_type and 
 * BSP_polling_get_char_function_type are the provided interfaces.
 */


#include <l4/util/util.h>
#include <l4/util/port_io.h>
#include <l4/RTEMS_wrapper/wrapper_1.h>
#include <l4/sys/irq>
#include <l4/sys/vcpu.h>
#include <l4/sys/thread>
#include <l4/sys/factory>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/util/atomic.h>

extern sharedvars_t* sharedVariableStruct;

void
l4rtems_outch( char c )
{
  static char buf_out[256];
  static int i = 0;

  if( c == '\n'  || c == 0 )
  {
    buf_out[i] = 0;
    sprintf( sharedVariableStruct->buff_out , "%s", buf_out ); 
    sharedVariableStruct->outready = true;
    // Spin while the output hasn't been written
//    while( sharedVariableStruct->outready )
    // sth goes wrong, the loop spinns and spinns and spinns
    i = 0;
  }
  else
  {
    buf_out[i] = c;
    ++i;
  }
}


// here is some bogus going on with the returntype and in the loop.
// FIXME
int
l4rtems_inch( void )
{
  static char buf_in[256];
  static int o = -1;

  if( o == -1 )
  {
//    o = fscanf( std, "%s", buf_in);
    if( o == 0 )
      return 0;
    // here should be some check, to assure the validity of the input
    // if( j == EOF )
    // fprintf( fout, "PANIC: Nothing to read\n");
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

using namespace L4;
using namespace L4Re;



bool
l4rtems_requestIrq( unsigned irqNbr )
{ /* Create IRQ object and attach it to the requested irqNbr. Then store the
     irqNbr and the capability in a map. */

  // request new capability and create IRQ
/*  Cap<Irq> newIrq = L4Re::Util::cap_alloc.alloc<Irq>();
  if( !newIrq.is_valid() )
  {
    sprintf( fout, "newIrq cap invalid!\n\n" );
    flag = true;
    return false;
  }

  l4_msgtag_t err = Env::env()->factory()->create_irq( newIrq );
  if( err.has_error() )
  {
    sprintf( fout, "create_irq failed! Flags: %x \n\n", err.flags() );
    flag = true;
    return false;
  }

  // attach vcpu thread to the IRQ
  err = newIrq->attach( irqNbr, Cap<Thread>(_vcpu_cap) );
  if( err.has_error() )
  {
    sprintf( fout, "IRQ attach failed! Flags: %x \n\n", err.flags() );
    flag = true;
    return false;
  }
  */
  return true; 
}



void
l4rtems_detachIrq( unsigned irqNbr )
{
  // Detach from irqNbr
  l4_msgtag_t err = l4_irq_detach( _vcpu_cap );
  if( err.has_error() )
  {
//    printk( "detachIrq:: Detatch failed! Flags: %x\n\n", 
//	err.flags() );
    return;
  }
}
