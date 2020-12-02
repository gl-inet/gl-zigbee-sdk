# uart-test.mak
#
# Description: Makefile for EZSP-UART test programs.
#
# Copyright 2008-2010 by Ember Corporation. All rights reserved.
#

GLOBAL_BASE_DIR := ../../../../../platform/base

SHELL = /bin/sh

OPTIONS=-Wcast-align -Wformat -Wimplicit -Wimplicit-int         \
	-Wimplicit-function-declaration -Winline -Wlong-long -Wmain	  \
	-Wnested-externs -Wno-import -Wparentheses -Wpointer-arith    \
	-Wredundant-decls -Wreturn-type -Wstrict-prototypes -Wswitch  \
	-Wunused                                                      \
	-g -O0 -static -static-libgcc                                 \
	-DEZSP_HOST                                                   \
	-DEZSP_ASH                                                    \
	-DPHY_NULL                                                    \
	-DPLATFORM_HEADER=\"hal/micro/unix/compiler/gcc.h\"           \
	-DCONFIGURATION_HEADER=\"app/ezsp-host/ezsp-host-configuration.h\"

INCLUDES= -I$(GLOBAL_BASE_DIR) -I../../../ -I../../../stack

CPPFLAGS = $(OPTIONS) $(FLAGS) $(INCLUDES)

.PHONY: all

all: uart-test-1 uart-test-2 uart-test-3
	@echo All builds succeeded.

%.d: %.c
	@set -e; $(CC) -MM $(CPPFLAGS) $<				                            \
                | sed 's^\($(*F)\)\.o[ :]*^$(@D)/\1.o $@ : ^g' > $@;	\
		echo 'created dependency file $@';			                          \
		[ -s $@ ] || rm -f $@

ASH_FILES =                                            \
        ash-host.c                                     \
        ash-host-ui.c                                  \
        $(GLOBAL_BASE_DIR)/hal/micro/generic/ash-common.c        \
        $(GLOBAL_BASE_DIR)/hal/micro/generic/system-timer.c      \
        ../../../app/util/ezsp/ezsp-enum-decode.c                \
        $(GLOBAL_BASE_DIR)/hal/micro/generic/crc.c               \
        ../../../app/util/gateway/backchannel-stub.c

EZSP_FILES =                                        \
        ../ezsp-host-io.c                           \
        ../ezsp-host-queues.c                       \
        ../ezsp-host-ui.c                           \
        ../../util/ezsp/secure-ezsp-stub.c          \
        ../../util/ezsp/ezsp.c                      \
        ../../util/ezsp/ezsp-callbacks.c            \
        ../../util/ezsp/ezsp-frame-utilities.c      \
        ../../util/ezsp/serial-interface-uart.c

TEST_FILES =                                        \
        uart-test-1.c                               \
        uart-test-2.c                               \
        uart-test-3.c

ifneq ($(MAKECMDGOALS),clean)
-include $(TEST_FILES:.c=.d)
-include $(ASH_FILES:.c=.d)
-include $(EZSP_FILES:.c=.d)
endif

uart-test-1:                                        \
              uart-test-1.o                         \
              $(ASH_FILES:.c=.o)                    \
              $(EZSP_FILES:.c=.o)                    
	$(CC) -g $(OPTIONS) $^ -o $@
	@set -e; echo ' '; echo '$@ build success'

uart-test-2:                                        \
              uart-test-2.o                         \
              $(ASH_FILES:.c=.o)                    \
              $(EZSP_FILES:.c=.o)                    
	$(CC) -g $(OPTIONS) $^ -o $@
	@set -e; echo ' '; echo '$@ build success'

uart-test-3:                                        \
              uart-test-3.o                         \
              $(ASH_FILES:.c=.o)                    \
              $(EZSP_FILES:.c=.o)                    
	$(CC) -g $(OPTIONS) $^ -o $@
	@set -e; echo ' '; echo '$@ build success'

clean:
	rm -f uart-test-1  uart-test-1.exe
	rm -f uart-test-2  uart-test-2.exe
	rm -f uart-test-3  uart-test-3.exe
	rm -f $(ASH_FILES:.c=.o) $(ASH_FILES:.c=.d)
	rm -f $(EZSP_FILES:.c=.o) $(EZSP_FILES:.c=.d)
	rm -f $(TEST_FILES:.c=.o) $(TEST_FILES:.c=.d)

all: uart-test-1 uart-test-2 uart-test-3
