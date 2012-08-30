/*
 * videoAsm.S  - This file contains code for displaying cursor on the console
 *
 * Copyright (C) 1998  valette@crf.canon.fr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: videoAsm.S,v 1.3 2004/04/21 10:42:44 ralf Exp $
 */

	.file "videoAsm.s"

#include <crt.h>

	.text
	.align 4
    	.globl  wr_cursor	/* Move cursor position */

/*
 *   	void wr_cursor(newPosition, ioBaseAddr)
 */

/* RTEMSVCPU: rewrite to adapt the code to long datatypes */
wr_cursor:	pushl	%ecx
		movl	8(%esp), %ecx		/* Get new cursor position */
/*		movb	$CC_CURSHIGH, %al */	/* Cursor high location */
		mov	$CC_CURSHIGH, %eax
		movl	12(%esp), %edx		/* Get IO base address */
/* -- snip: asm code for long instructions -- */		
		outb	(%dx)
		inc	%edx
		mov	%ecx, %eax
		outb	(%dx)
		dec	%edx
		mov	$CC_CURSLOW, %eax
		outb	(%dx)
		inc	%edx
		mov	%ecx, %eax
		outb	(%dx)
		pop	%ecx
		ret
/* -- snip: old ascm code for short -- */
		outb	(%dx)
		incw	%dx			/* Program Data register */
		movb	%ch, %al
		outb	(%dx)			/* Update high location cursor */
		decw	%dx			/* Program Index Register */
		movb	$CC_CURSLOW, %al	/* Cursor low location */
		outb	(%dx)
		incw	%dx			/* Program Data Register */
		movb	%cl, %al
		outb	(%dx)			/* Update low location cursor */
		popl	%ecx
		ret
