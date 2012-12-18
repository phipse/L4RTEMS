PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

TARGET		= $(PKGNAME) 

# list your .c or .cc files here
SRC_C		= res.c
SRC_CC          = wrapper_1.cc timer.cc 

# list requirements of your program here
REQUIRES_LIBS   = libvcpu stdlibs libstdc++ libio-io 

include $(L4DIR)/mk/prog.mk

res.o: func_list.h

func_list.h: $(L4DIR)/../../build/bin/x86_686/l4f/myTest.exe
	$(Q)$(OBJCOPY) -j .data.l4externals.str -O binary $< $@.tmp
	$(Q)cat $@.tmp
	$(Q)perl -p -e 's/(.+?)\0/EF($$1)\n/g' $@.tmp > $@
