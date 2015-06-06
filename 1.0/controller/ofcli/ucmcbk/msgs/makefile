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
# Date:   03/15/2013
# Description:

UCMCBK_DIR= $(ofcli_dir)/ucmcbk

include $(UCMCBK_DIR)/rules.mod

#
# Program Name
#
PROG = $(MODSET_CBK_OBJ_DIR)/ucmmsgstarget.o

#
# Objects
#

$(MODSET_CBK_OBJ_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)
 
OBJS += $(MODSET_CBK_OBJ_DIR)/groupcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/bucketcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/actioncbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/grpfeature.o
OBJS += $(MODSET_CBK_OBJ_DIR)/grpstats.o
OBJS += $(MODSET_CBK_OBJ_DIR)/flowmodcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/flowmatchcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/flowinstcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/flowactioncbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/flowactionlistcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/mtrcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/mtrbandcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/mtrfeature.o
OBJS += $(MODSET_CBK_OBJ_DIR)/mtrstats.o

all : $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(LDOPTS) $(OBJS) -o $@

clean:
	$(RM) $(OBJS) $(PROG)

