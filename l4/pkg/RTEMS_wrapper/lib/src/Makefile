PKGDIR?= ../..
L4DIR ?= $(PKGDIR)/../..

TARGET	= libl4rtems.o.a
LINK_INCR = y 
LINK_INCR_ADD_LIBS =  
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libl4re-vfs.o.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libio-io.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libio-vbus.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/lib4re-c-util.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/lib4re-util.a
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libc_be_l4refile.a
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libuc_c.a
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libc_be_l4refile.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/lib4re-c.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/pkg/l4re/lib/src/OBJ-x86_686-l4f/rm.o
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/pkg/l4re/lib/src/OBJ-x86_686-l4f/mem_alloc.o
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/pkg/l4re/lib/src/OBJ-x86_686-l4f/dataspace.o
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/lib4re.a
LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libl4sys.a
#LINK_INCR_ADD_LIBS += $(L4DIR)/../../build/lib/x86_686/l4f/libl4util.a

LINK_INCR_ONLYGLOBSYMFILE_libl4rtems.o.a = $(SRC_DIR)/global.syms

SRC_CC	= l4rtems_io.cc
SRC_C  =

include $(L4DIR)/mk/lib.mk
