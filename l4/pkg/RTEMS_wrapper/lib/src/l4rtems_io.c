/*
 * There are much nicer ways to do this, but they need deeper modifications
 * of the RTEMS source code and the BSP_output_char_function_type and 
 * BSP_polling_get_char_function_type are the provided interfaces.
 */


#include <stdio.h>
#include <l4/util/util.h>
#include <l4/RTEMS_wrapper/l4rtems_io.h> 

static char buf_out[256];
static int i = 0;

void
l4rtems_outch( char c )
{
  if( c == '\n' )
  {
    fprintf( stdout, "%s\n", buf_out); 
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
    o = fscanf( stdin, "%s", buf_in);
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
