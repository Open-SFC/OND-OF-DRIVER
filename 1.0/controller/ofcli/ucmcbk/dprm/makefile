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
PROG = $(MODSET_CBK_OBJ_DIR)/ucmdprmcbk.o

#
# Objects
#

$(MODSET_CBK_OBJ_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)
 
OBJS = $(MODSET_CBK_OBJ_DIR)/switchcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/domaincbk.o
#OBJS += $(MODSET_CBK_OBJ_DIR)/dprmtblcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/datapathcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/portcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/dpattributecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/switchattributecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/domainattributecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/portattributecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/eventscbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/asyncmsgcfgcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/channelscbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/auxchannels.o
OBJS += $(MODSET_CBK_OBJ_DIR)/tblfeaturescbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ipoptionscbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/cntlrrolecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/swinfocbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/sslcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ssladdnlcacertcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/tracecbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/viewcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/nscbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/mempoolstats.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ttpcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ttptblcbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ttpdomaincbk.o
OBJS += $(MODSET_CBK_OBJ_DIR)/ttpmatchcbk.o

all : $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(LDOPTS) $(OBJS) -o $@

clean:
	$(RM) $(OBJS) $(PROG)

