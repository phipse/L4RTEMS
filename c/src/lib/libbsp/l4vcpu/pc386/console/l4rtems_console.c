
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/termiostypes.h>
#include <uart.h>
#include <libcpu/cpuModel.h>

#include <rtems/mw_uid.h>
#include <rtems/mouse_parser.h>
#include <rtems/keyboard.h>

#include <rtems/score/wrapper.h>
#include <rtems/l4vcpu/l4vcon.h>

extern sharedvars_t *sharedVariableStruct;



#if (USE_COM1_AS_CONSOLE == 1)
int BSPConsolePort = BSP_UART_COM1;
int BSPPrintkPort  = BSP_UART_COM1;
#else
int BSPConsolePort = BSP_CONSOLE_PORT_CONSOLE;
int BSPPrintkPort  = BSP_CONSOLE_PORT_CONSOLE;
#endif


/* printk support */
// RTEMSVPCU: adjusted the IO functions
BSP_output_char_function_type BSP_output_char =
                       (BSP_output_char_function_type) l4rtems_outch; //_IBMPC_outch;

BSP_polling_getchar_function_type BSP_poll_char = 
			(BSP_polling_getchar_function_type) l4rtems_inch; //BSP_wait_polled_input;
  
  

void 
BSP_console_select( void )
{
  printk( "console_select called\n" );
}
  
  
rtems_device_driver
console_initialize(rtems_device_major_number major,
                   rtems_device_minor_number minor,
                   void                      *arg)
{
  rtems_status_code status;
  // RTEMSVCPU: driver seems to work

  /* Initialize the KBD interface */
  kbd_init();

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();

  /*
   *  The video was initialized in the start.s code and does not need
   *  to be reinitialized.
   */

  if(BSPConsolePort == BSP_CONSOLE_PORT_CONSOLE)
    {
      /* Install keyboard interrupt handler */
// replace with rtems_interrupt_handler_install
      status = rtems_interrupt_handler_install( BSP_KEYBOARD, 
						"l4 keyboard",
						RTEMS_INTERRUPT_UNIQUE,
						keyboard_interrupt,
						0 );

      if( status != RTEMS_SUCCESSFUL )
      {
	printk("Error installing keyboard interrupt handler!\n");
	printk("Status %i\n", status );
	rtems_fatal_error_occurred(status);
      }

      status = rtems_io_register_name("/dev/console", major, 0);
      if (status != RTEMS_SUCCESSFUL)
	{
	  printk("Error registering console device!\n");
	  rtems_fatal_error_occurred(status);
	}
      printk("Initialized console on port CONSOLE\n\n");
    }
  else
    {
      printk( "oops, we don't have UART! Configuration wrong!\n" );
#if 0
      /*
       * Do device-specific initialization
       */
      /* BSPCmdBaud-8-N-1 */
      BSP_uart_init(BSPConsolePort, BSPCmdBaud, CHR_8_BITS, 0, 0, 0);

      /* Set interrupt handler */
      if(BSPConsolePort == BSP_UART_COM1)
   	{
	     console_isr_data.name = BSP_UART_COM1_IRQ;
        console_isr_data.hdl  = BSP_uart_termios_isr_com1;

   	}
      else
	   {
          assert(BSPConsolePort == BSP_UART_COM2);
          console_isr_data.name = BSP_UART_COM2_IRQ;
          console_isr_data.hdl  = BSP_uart_termios_isr_com2;
    	}
      status = BSP_install_rtems_irq_handler(&console_isr_data);

      if (!status){
	  printk("Error installing serial console interrupt handler!\n");
	  rtems_fatal_error_occurred(status);
      }
      /*
       * Register the device
       */
      status = rtems_io_register_name ("/dev/console", major, 0);
      if (status != RTEMS_SUCCESSFUL)
	{
	  printk("Error registering console device!\n");
	  rtems_fatal_error_occurred (status);
	}

      if(BSPConsolePort == BSP_UART_COM1)
	{
	  printk("Initialized console on port COM1 %d-8-N-1\n\n", BSPCmdBaud);
	}
      else
	{
	  printk("Initialized console on port COM2 %d-8-N-1\n\n", BSPCmdBaud);
	}
#endif
  }

  if(BSPPrintkPort != BSP_CONSOLE_PORT_CONSOLE && BSPPrintkPort != BSP_UART_COM1)
    {
      printk("illegal assignement of printk channel");
      rtems_fatal_error_occurred (status);
    }

  return RTEMS_SUCCESSFUL;
} /* console_initialize */

  
  
/*-------------------------------------------------------------------------+
| Console device driver OPEN entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_open(rtems_device_major_number major,
                rtems_device_minor_number minor,
                void                      *arg)
{
#if 0
  rtems_status_code              status;
  static rtems_termios_callbacks cb =
  {
    NULL,	              /* firstOpen */
    console_last_close,       /* lastClose */
    NULL,          /* pollRead */
    BSP_uart_termios_write_com1, /* write */
    conSetAttr,	              /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    1		              /* outputUsesInterrupts */
  };

  if(BSPConsolePort == BSP_CONSOLE_PORT_CONSOLE)
    {

      /* Let's set the routines for termios to poll the
       * Kbd queue for data
       */
      cb.pollRead = kbd_poll_read;
      cb.outputUsesInterrupts = 0;
      /* write the "echo" if it is on */
      cb.write = ibmpc_console_write;

      cb.setAttributes = NULL;
      ++console_open_count;
      status = rtems_termios_open (major, minor, arg, &cb);
      if(status != RTEMS_SUCCESSFUL)
      {
         printk("Error openning console device\n");
	 printk("status: %i\n", status );
      }
      return status;
    }

  if(BSPConsolePort == BSP_UART_COM2)
    {
      cb.write = BSP_uart_termios_write_com2;
    }

  cb.firstOpen = ser_console_first_open;

  status = rtems_termios_open (major, minor, arg, &cb);

  if(status != RTEMS_SUCCESSFUL)
    {
      printk("Error openning console device\n");
      return status;
    }
#endif
  return RTEMS_SUCCESSFUL;
}

/*-------------------------------------------------------------------------+
| Console device driver CLOSE entry point
+--------------------------------------------------------------------------*/
rtems_device_driver
console_close(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                      *arg)
{
   return RTEMS_SUCCESSFUL;
   //rtems_termios_close (arg);
} /* console_close */

/*-------------------------------------------------------------------------+
| Console device driver READ entry point.
+--------------------------------------------------------------------------+
| Read characters from the I/O console. We only have stdin.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_read(rtems_device_major_number major,
             rtems_device_minor_number minor,
             void                      *arg)
{
 return rtems_termios_read( arg );
} /* console_read */

/*-------------------------------------------------------------------------+
| Console device driver WRITE entry point.
+--------------------------------------------------------------------------+
| Write characters to the I/O console. Stderr and stdout are the same.
+--------------------------------------------------------------------------*/
rtems_device_driver
console_write(rtems_device_major_number major,
              rtems_device_minor_number minor,
              void                    * arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  char                  *buffer  = rw_args->buffer;
  int                    maximum  = rw_args->count;

  l4_vcon_write( sharedVariableStruct->logcap, buffer, maximum );
#if 0
  if(BSPConsolePort != BSP_CONSOLE_PORT_CONSOLE)
    {
      return rtems_termios_write (arg);
    }

  /* write data to VGA */
  ibmpc_console_write( minor, buffer, maximum );
#endif
  rw_args->bytes_moved = maximum;
  return RTEMS_SUCCESSFUL;
} /* console_write */

/*
 * Handle ioctl request.
 */
rtems_device_driver
console_control(rtems_device_major_number major,
		rtems_device_minor_number minor,
		void                      * arg
)
{
#if 0
	rtems_libio_ioctl_args_t *args = arg;
	switch (args->command)
	{
	   default:
      if( vt_ioctl( args->command, (unsigned long)args->buffer ) != 0 )
          return rtems_termios_ioctl (arg);
		break;

      case MW_UID_REGISTER_DEVICE:
      printk( "SerialMouse: reg=%s\n", args->buffer );
      register_kbd_msg_queue( args->buffer, 0 );
		break;

      case MW_UID_UNREGISTER_DEVICE:
      unregister_kbd_msg_queue( 0 );
		break;
   }
 	args->ioctl_return = 0;
#endif
  printk( "console_control called\n");
  return RTEMS_SUCCESSFUL;
}
 
