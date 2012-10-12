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
#include <l4/re/c/log.h>
#include <string.h>

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
    l4re_log_print_srv( sharedVariableStruct->logcap, buf_out );
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
  static char buf_in;
  int o = 0;
  char* sharedBuf = sharedVariableStruct->buff_in;
  unsigned* inflag = sharedVariableStruct->inready;
  return 64;

  if( inflag )
  {
    o = sscanf( &buf_in, "%c", sharedBuf );
    buf_in = 'f';
    l4util_xchg32( inflag, false );

    if( o == 1 )
      return buf_in;
    else
      return buf_in;
      asm __volatile__ ( " mov %0, %%eax \t\n"
//	  "mov %1, %%ebx \t\n"
	  "ud2 \t\n" : :"r"(o), "r"(buf_in) : );

  }
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
