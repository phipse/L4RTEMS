PKGDIR?= ../..
L4DIR ?= $(PKGDIR)/../..

TARGET	= libl4rtems.o.a
LINK_INCR = y
INK_INCR_ADD_LIBS = gcc stdlib 

LINK_INCR_ONLYGLOBSYMFILE_libl4rtems.o.a = $(SRC_DIR)/global.syms

SRC_C	= l4rtems_io.c
SRC_CC  =

include $(L4DIR)/mk/lib.mk
