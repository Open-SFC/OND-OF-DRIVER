# Copyright  2012, 2013  Freescale Semiconductor
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.

# File name:  makefile
# Author: Freescale Semiconductor    
# Date:   02/23/2013
# Description: Makefile for compiling UCM lib timer files

CM_DIR = ../../../../..
include $(CM_DIR)/$(PROD_DIR)/rules.mod

#
# Directories
#

#
# Program Name
#
ifeq ("$(OSTYPE)", "lx")
PROG = $(MODSET_OBJ_DIR)/lib/libucmtmr.so
endif


#
# Objects
#
UCMLIBTMROBJS =				\
		timerlibbasic.o	\
		timerlibport.o

OBJS = $(UCMLIBTMROBJS:%=$(MODSET_OBJ_DIR)/%)

#OBJS = $(SOURCE:%.c=$(MODSET_OBJ_DIR)/%.o)

.PHONY: all
.PHONY: clean

all : $(PROG)

ifeq ("$(OSTYPE)", "lx")
$(PROG): $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lc
endif

clean:
	$(RM) $(PROG)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
