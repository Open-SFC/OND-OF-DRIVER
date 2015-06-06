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
# Date:   02/26/2013
# Description: 

CM_DIR = ../../../../..
include $(CM_DIR)/$(PROD_DIR)/rules.mod

MESGCLNTSYS_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/msgsys/mesgclnt/obj
MESGCLNT_OBJ_DIR = $(CM_DIR)/modules/msgsys/mesgclnt/obj

MODSET_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/obj

#
# Include Diretories
#
LOCAL_INCLUDES = \
      -I $(CM_DIR)/modules/include/msgsys/mesgclnt	\
      -I $(CM_DIR)/modules/msgsys/mesgclnt/include	\
      -I $(CM_DIR)/modules/include/lxos


#
# Program Name
#
PROG = $(MODSET_OBJ_DIR)/mesgclntsys.o

MESGCLNT_WRAP_OBJ=$(MODSET_OBJ_DIR)/mesgclntwrap.o
MESGCLNT_LIB=$(MODSET_OBJ_DIR)/lib/libucmmesgclnt.so

OBJDIR = $(MODSET_OBJ_DIR)
SOURCE = $(wildcard *.c)
OBJS = $(SOURCE:%.c=$(MODSET_OBJ_DIR)/%.o)


all : $(PROG) $(MESGCLNT_LIB)

$(PROG): $(OBJS) 
	$(LD) $(LDOPTS) $(OBJS) -o $@

$(MESGCLNT_LIB): $(MESGCLNT_WRAP_OBJ)
	$(CC) -shared -lc  $(MESGCLNT_WRAP_OBJ) -o $(MESGCLNT_LIB)


clean:
	$(RM) $(OBJS) $(PROG) $(MESGCLNT_LIB)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
