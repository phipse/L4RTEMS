##
## $Id: Makefile.am,v 1.38 2006/01/12 09:57:43 ralf Exp $
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS= rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/i386.h \
    rtems/score/types.h rtems/score/interrupts.h rtems/score/registers.h \
    rtems/score/idtr.h rtems/score/wrapper.h
# RTEMSVCPU: Added rtems/score/wrapper.h
include_rtems_l4vcpudir = $(includedir)/rtems/l4vcpu
include_rtems_l4vcpu_HEADERS = rtems/l4vcpu/l4compiler.h rtems/l4vcpu/l4consts.h \
	rtems/l4vcpu/l4const_x86.h rtems/l4vcpu/l4int.h rtems/l4vcpu/l4ipc.h \
	rtems/l4vcpu/l4linkage.h rtems/l4vcpu/l4sys-vcpu.h rtems/l4vcpu/l4types.h \
	rtems/l4vcpu/l4utcb.h rtems/l4vcpu/l4__vcpu-arch.h \
	rtems/l4vcpu/l4vcpu_arch.h rtems/l4vcpu/l4vcpu.h \
	rtems/l4vcpu/l4err.h rtems/l4vcpu/l4int_x86.h rtems/l4vcpu/l4__l4_fpage.h \
	rtems/l4vcpu/l4__timeout.h rtems/l4vcpu/l4utcb_x86.h \
	rtems/l4vcpu/l4ipc-invoke-x86.h rtems/l4vcpu/l4ipc-l42-gcc3-nopic.h \
	rtems/l4vcpu/l4kdebug.h rtems/l4vcpu/l4thread.h rtems/l4vcpu/l4util.h \
	rtems/l4vcpu/l4reconsts.h rtems/l4vcpu/l4__kip-32bit.h rtems/l4vcpu/l4kip.h \
	rtems/l4vcpu/l4reenv.h rtems/l4vcpu/l4irq.h rtems/l4vcpu/l4kernel_object.h \
	rtems/l4vcpu/l4__kernel_object_impl.h rtems/l4vcpu/l4syscall-invoke.h \
	rtems/l4vcpu/l4vcon.h rtems/l4vcpu/handler.h 


noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
