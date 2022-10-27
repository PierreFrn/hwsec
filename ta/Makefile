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
CFLAGS		:= -Wall -c
INCLUDES	:= -I.
LD		:= gcc
LDFLAGS		:=
LIBS		:= -lm
OBJS		:= $(patsubst %.c,%.o,$(wildcard *.c))
DATA		:= ta.dat
KEY		:= ta.key

.PHONY: help clean

define HELP_message
Usage: make [GOAL]

goals:
	help		print this message
	ta		build attacker
	target		build target of attack
	clean		delete generated files
endef
export HELP_message

help::
	@printf '%s\n' "$$HELP_message"

target.o des.o rdtsc_timer.o utils.o: CFLAGS += -O3
p.o: CFLAGS += -O0

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

target: target.o des.o rdtsc_timer.o utils.o p.o
ta: ta.o des.o utils.o pcc.o

target ta:
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean::
	rm -f $(OBJS) $(DATA) $(KEY) target ta

