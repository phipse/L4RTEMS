/**
 * \internal
 * \file
 * \brief   Kernel Interface Page (KIP).
 * \ingroup l4_kip_api
 */
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>,
 *               Björn Döbel <doebel@os.inf.tu-dresden.de>,
 *               Frank Mehnert <fm3@os.inf.tu-dresden.de>,
 *               Torsten Frenzel <frenzel@os.inf.tu-dresden.de>,
 *               Martin Pohlack <mp26@os.inf.tu-dresden.de>,
 *               Lars Reuther <reuther@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
#pragma once

#include "l4types.h"

/**
 * \brief L4 Kernel Interface Page.
 * \ingroup l4_kip_api
 */
typedef struct l4_kernel_info_t
{
  /* offset 0x00 */
  l4_uint32_t            magic;               /**< Kernel Info Page
					       **  identifier ("L4�K").
					       **/
  l4_uint32_t            version;             ///< Kernel version
  l4_uint8_t             offset_version_strings; ///< offset to version string
  l4_uint8_t             fill0[3];            ///< reserved \internal
  l4_uint8_t             kip_sys_calls;       ///< pointer to system calls
  l4_uint8_t             fill1[3];            ///< reserved \internal

  /* the following stuff is undocumented; we assume that the kernel
     info page is located at offset 0x1000 into the L4 kernel boot
     image so that these declarations are consistent with section 2.9
     of the L4 Reference Manual */

  /* offset 0x10 */
  /* Kernel debugger */
  l4_umword_t            init_default_kdebug; ///< Kdebug init function
  l4_umword_t            default_kdebug_exception; ///< Kdebug exception handler
  l4_umword_t            scheduler_granularity; ///< for rounding time slices
  l4_umword_t            default_kdebug_end;  ///< default_kdebug_end

  /* offset 0x20 */
  /* Sigma0 */
  l4_umword_t            sigma0_esp;          ///< Sigma0 start stack pointer
  l4_umword_t            sigma0_eip;          ///< Sigma0 instruction pointer
  l4_umword_t            _res01[2];           ///< reserved \internal

  /* offset 0x30 */
  /* Sigma1 */
  l4_umword_t            sigma1_esp;          ///< Sigma1 start stack pointer
  l4_umword_t            sigma1_eip;          ///< Sigma1 instruction pointer
  l4_umword_t            _res02[2];           ///< reserved \internal

  /* offset 0x40 */
  /* Root task */
  l4_umword_t            root_esp;            ///< Root task stack pointer
  l4_umword_t            root_eip;            ///< Root task instruction pointer
  l4_umword_t            _res03[2];           ///< reserved \internal

  /* offset 0x50 */
  /* L4 configuration */
  l4_umword_t            l4_config;           /**< L4 kernel configuration.
					       **
					       ** Values:
					       **  - bits 0-7: set the number
					       **    of page table entries to
					       **    allocate
					       **  - bits 8-15: set the number
					       **    of mapping nodes.
					       **/
  l4_umword_t            mem_info;            ///< memory information
  l4_umword_t            kdebug_config;       /**< Kernel debugger config.
					       **
					       **  Values:
					       **  - bits 0-7: set the number
					       **    of pages to allocate for
					       **    the trace buffer
					       **  - bit 8: if set to 1, the
					       **    kernel enters kdebug
					       **    before starting the root
					       **    task
					       **  - bits 16-19: set the port
					       **    speed to use with serial
					       **    line (1..115.2KBd,
					       **    2..57.6KBd, 3..38.4KBd,
					       **    6..19.2KBd, 12..9.6KBD)
					       **  - bits 20-31: set the I/O
					       **    port to use with serial
					       **    line, 0 indicates that no
					       **    serial output should be
					       **    used
					       **/
  l4_umword_t            kdebug_permission;   /**< Kernel debugger permissions.
					       **
					       **  Values:
					       **  - bits 0-7: if 0 all tasks
					       **    can enter the kernel
					       **    debugger, otherwise only
					       **    tasks with a number lower
					       **    the set value can enter
					       **    kdebug, other tasks will be
					       **    shut down.
					       **  - bit 8: if set, kdebug may
					       **    display mappings
					       **  - bit 9: if set, kdebug may
					       **    display user registers
					       **  - bit 10: if set, kdebug may
					       **    display user memory
					       **  - bit 11: if set, kdebug may
					       **    modify memory, registers,
					       **    mappings and tcbs
					       **  - bit 12: if set, kdebug may
					       **    read/write I/O ports
					       **  - bit 13: if set, kdebug may
					       **    protocol page faults and
					       **    IPC
					       **/

  /* offset 0x60 */
  l4_umword_t            total_ram;           ///< Size of RAM in bytes
  l4_umword_t            processor_info;      ///< CPU info
  l4_umword_t            _res04[14];          ///< reserved \internal

  /* offset 0xA0 */
  volatile l4_cpu_time_t clock;               ///< L4 system clock (�s)
  l4_umword_t            _res05[2];           ///< reserved \internal

  /* offset 0xB0 */
  l4_umword_t            frequency_cpu;       ///< CPU frequency in kHz
  l4_umword_t            frequency_bus;       ///< Bus frequency

  /* offset 0xB8 */
  l4_umword_t            _res06[10];          ///< reserved \internal

  /* offset 0xE0 */
  l4_umword_t		 user_ptr;            ///< user_ptr
  l4_umword_t		 vhw_offset;          ///< offset to vhw structure
  l4_umword_t            _res07[2];

  /* offset 0xF0 */
  struct l4_kip_platform_info    platform_info;
} l4_kernel_info_t;
