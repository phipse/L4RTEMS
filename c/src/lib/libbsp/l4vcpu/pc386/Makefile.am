##
## $Id: Makefile.am,v 1.58 2011/03/16 20:05:12 joel Exp $
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# FIXME: We must not include *.cfg
include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

CLEANFILES =

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

SUBDIRS = . tools

include_bsp_HEADERS = ../../l4vcpu/shared/irq/irq.h \
    ../../l4vcpu/shared/irq/irq_asm.h ../../l4vcpu/shared/comm/tty_drv.h \
    ../../shared/include/irq-generic.h \
    ../../shared/include/irq-info.h 

include_HEADERS += include/crt.h 
nodist_include_HEADERS += ../../shared/include/coverhd.h 


noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

# RTEMSVCPU: No need for 16bit
#EXTRA_DIST = start/start16.S
#start16.$(OBJEXT): start/start16.S
#	$(CPPASCOMPILE) $(AM_CPPFLAGS) -DHEADERADDR=$(HEADERADDR) -o $@ -c $<

#start16-elf32.$(OBJEXT): start16.$(OBJEXT)
#	$(LD) -N -T $(top_srcdir)/startup/linkcmds -Ttext $(START16ADDR) -e start16 -nostdlib \
#	  --oformat=elf32-i386 \
#	-o $@ $<

#start16.bin: start16-elf32.$(OBJEXT)
#	$(OBJCOPY) -O binary $< $@
#CLEANFILES += start16.bin

#project_lib_DATA += start16.bin

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES = 

# clock
libbsp_a_SOURCES += clock/l4rtems_ckinit.c clock/todcfg.c ../../shared/tod.c \
    ../../../shared/clockdrv_shell.h 

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = console/keyboard.h console/kd.h console/ps2_drv.h

include_HEADERS += ../../l4vcpu/shared/comm/i386_io.h
# console
libbsp_a_SOURCES += console/l4rtems_console.c console/l4rtems_inoutchar.c \
	console/inch.c console/outch.c \
    console/defkeymap.c console/fb_vga.c console/keyboard.c \
    console/pc_keyb.c console/ps2_mouse.c \
    console/vgainit.c console/vt.c console/videoAsm.S \
    console/kbd_parser.c console/serial_mouse_config.c \
    ../../l4vcpu/shared/comm/uart.c ../../l4vcpu/shared/comm/tty_drv.c

# gdb
libbsp_a_SOURCES += ../../l4vcpu/shared/comm/i386-stub.c \
    ../../l4vcpu/shared/comm/i386-stub-glue.c \
    ../../l4vcpu/shared/comm/gdb_glue.c

# gnat
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c

include_HEADERS += ../../l4vcpu/shared/pci/pcibios.h
# pci
libbsp_a_SOURCES += ../../l4vcpu/shared/pci/pcibios.c \
    ../../l4vcpu/shared/pci/pcibios.h

include_HEADERS += ../../l4vcpu/shared/comm/uart.h
# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c startup/bspgetworkarea.c \
    ../../shared/bsppretaskinghook.c startup/bspstart.c startup/bspcmdline.c \
    ../../shared/bspclean.c startup/bspreset.c ../../l4vcpu/shared/irq/idt.c \
    ../../l4vcpu/shared/irq/irq.c ../../l4vcpu/shared/irq/irq_init.c \
    ../../shared/bootcard.c ../../shared/bspinit.c ../../shared/sbrk.c \
    startup/ldsegs.S ../../l4vcpu/shared/irq/irq_asm.S \
    ../../shared/src/irq-generic.c \
    ../../shared/src/irq-legacy.c \
    ../../shared/src/irq-info.c \
    ../../shared/src/irq-shell.c \
    ../../shared/src/irq-server.c \
    startup/handler.c startup/l4lib.h

# timer
libbsp_a_SOURCES += \ # timer/timer.c timer/timerisr.S \
	timer/l4rtems_timer.c timer/l4rtems_timerisr.S

# ide
libbsp_a_SOURCES += ide/idecfg.c ide/ide.c

if HAS_SMP
libbsp_a_SOURCES += ../../shared/smp/getcpuid.c ../../shared/smp/smp_stub.c
endif

if HAS_NETWORKING
ne2000_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += ne2000.rel
ne2000_rel_SOURCES = ne2000/ne2000.c
ne2000_rel_CPPFLAGS = $(AM_CPPFLAGS) $(ne2000_CPPFLAGS)
ne2000_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
include_HEADERS += include/wd80x3.h

wd8003_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += wd8003.rel
wd8003_rel_SOURCES = wd8003/wd8003.c include/wd80x3.h
wd8003_rel_CPPFLAGS = $(AM_CPPFLAGS) $(wd8003_CPPFLAGS)
wd8003_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if HAS_NETWORKING
3c509_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += 3c509.rel
3c509_rel_SOURCES = 3c509/3c509.c 3c509/3c509.h 3c509/elink.c 3c509/elink.h
3c509_rel_CPPFLAGS = $(AM_CPPFLAGS) $(3c509_CPPFLAGS)
3c509_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/page.rel \
    ../../../libcpu/@RTEMS_CPU@/score.rel
# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
libbsp_a_LIBADD += ne2000.rel wd8003.rel 3c509.rel
endif

EXTRA_DIST = HOWTO README.dec21140 STATUS times_i486dx times_p5

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
