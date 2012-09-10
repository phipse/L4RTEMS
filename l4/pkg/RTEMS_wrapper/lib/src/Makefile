PKGDIR?= ../..
L4DIR ?= $(PKGDIR)/../..

TARGET	= libl4rtems.o.a
LINK_INCR = n
LINK_INCR_ADD_LIBS = 
#LINK_INCR_ADD_LIBS = $(L4DIR)/../../build/lib/x86_686/libuc_c_minimal.a

LINK_INCR_ONLYGLOBSYMFILE_libl4rtems.o.a = $(SRC_DIR)/global.syms

SRC_C	= l4rtems_io.c
SRC_CC  =

include $(L4DIR)/mk/lib.mk
