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
# Date:   02/15/2013
# Description: 

CM_DIR = ../../../../..
include $(CM_DIR)/$(PROD_DIR)/rules.mod

#LOGCLTRSYS_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/msgsys/logcltr/obj
LOGCLTR_OBJ_DIR = $(CM_DIR)/modules/msgsys/logcltr/obj

MODSET_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/obj
#
# Program Name
#
PROG = $(MODSET_OBJ_DIR)/logcltrsys.o

#LOGCLTR_DAEMON = $(MODSET_OBJ_DIR)/logcltrd

#
# Include Diretories
#
LOCAL_INCLUDES = \
      -I $(CM_DIR)/include/msgsys/logcltr	\
      -I $(CM_DIR)/msgsys/logcltr/include	\
      -I $(CM_DIR)/include/lxos

#
# Obmesgclntcts
#

#LOGCLTR_BASIC_OBJS  = $(MODSET_OBJ_DIR)/logcltrbasic.o

#OBJS_TOBE_LINKED = $(MESGCLNT_BASIC_OBJS)

LOGCLTRSYSOBJS  =  $(MODSET_OBJ_DIR)/logcltrsys.o

OBJDIR = $(MODSET_OBJ_DIR)
SOURCE = $(wildcard *.c)
OBJS = $(SOURCE:%.c=$(MODSET_OBJ_DIR)/%.o)


all : $(PROG)
 #builddaemon

$(PROG): $(OBJS) 
	$(LD) $(LDOPTS) $(OBJS) -o $@
#	$(LD) $(LDOPTS) $(OBJS) $(OBJS_TOBE_LINKED) -o $@

#builddaemon:
#	$(STRIP) $(PROG)
#	$(CC) -shared  $(PROG) -ldl -lc -o $(TRNCHL_LIB) 

clean:
	$(RM) $(OBJS) $(PROG)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
