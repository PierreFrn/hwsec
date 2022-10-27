# MASTER-ONLY: DO NOT MODIFY THIS FILE

#
# Copyright (C) Telecom Paris
# 
# This file must be used under the terms of the CeCILL. This source
# file is licensed as described in the file COPYING, which you should
# have received as part of this distribution. The terms are also
# available at:
# http://www.cecill.info/licences/Licence_CeCILL_V1.1-US.txt
#

SHELL		:= /bin/bash

CC		:= gcc
CFLAGS		:= -Wall -c -O3
INCLUDES	:= -I.
LD		:= gcc
LDFLAGS		:=
LIBS		:= -lm
OBJS		:= $(patsubst %.c,%.o,$(wildcard *.c))
DATA		:= pa.hws
KEY		:= pa.key
EXTRADATA	:= average.cmd average.dat dpa.cmd dpa.dat

.PHONY: help clean

define HELP_message
Usage: make [GOAL]

goals:
	help		print this message
	pa		build attacker
	clean		delete generated files
endef
export HELP_message

help::
	@printf '%s\n' "$$HELP_message"

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

pa: pa.o des.o utils.o traces.o pcc.o

pa:
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean::
	rm -f $(OBJS) $(EXTRADATA) pa

