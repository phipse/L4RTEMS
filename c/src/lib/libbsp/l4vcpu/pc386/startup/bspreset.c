/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: bspreset.c,v 1.1 2008/09/22 21:53:54 joel Exp $
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset(void)
{
  /* shutdown and reboot */
//  outport_byte(0x64, 0xFE);      /* use keyboard controler to do the job... */
  i386_open_interrupts( );
  while(1);
}
