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
# Description:

include ../../../rules.mk

all: build

$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

TARGET = $(OSPKG_BIN_DIR)/controller

OBJS =  $(BIN_DIR)/main.o \
	$(BIN_DIR)/ctlrutils.o \
	$(BIN_DIR)/ctlrtest.o \
	$(BIN_DIR)/coreapp.o \
	$(BIN_DIR)/datapathrm.o \
	$(BIN_DIR)/ofview.o \
	$(BIN_DIR)/transport.o \
	$(BIN_DIR)/messages.o \
	$(BIN_DIR)/crm.o \
	$(BIN_DIR)/crmiface.o \
	$(BIN_DIR)/ttp.o \
	$(BIN_DIR)/nsrm.o \
	$(BIN_DIR)/ctlrxml.o 

ifneq (,$(findstring CNTRL_INFRA_LIB_SUPPORT,$(CFLAGS)))
else
	$(OBJS)+=$(BIN_DIR)/ctlrpeth.o \
	$(OBJS)+=$(BIN_DIR)/ofnem.o 
endif

$(TARGET) : $(OBJS) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(MGMT_LIB)


makecoreapp: 
	make -w -C $(ofproto_dir)/src/coreapp
makedprm: 
	make -w -C $(ofproto_dir)/src/dprm
makecrm: 
	make -w -C $(ofproto_dir)/src/crm
makecrmiface: 
	make -w -C $(ofproto_dir)/src/crmiface
makettp: 
	make -w -C $(ofproto_dir)/src/ttp
makensrm: 
	make -w -C $(ofproto_dir)/src/nsrm
makeview: 
	make -w -C $(ofproto_dir)/src/view
makenem: 
#	make -w -C $(ofproto_dir)/src/nem
makemessages: 
	make -w -C $(ofproto_dir)/src/messages
maketransport: 
	make -w -C $(ofproto_dir)/src/transport
copytargets: 
	$(CP) -rf $(OF_LIB_DIR)/* $(OSPKG_BIN_DIR)/lib/

build: makecoreapp makedprm makeview makenem makemessages maketransport makecrm makecrmiface makettp makensrm $(TARGET) copytargets

clean:
	make -C $(ofproto_dir)/src/coreapp clean
	make -C $(ofproto_dir)/src/dprm clean
	make -C $(ofproto_dir)/src/crm clean
	make -C $(ofproto_dir)/src/crmiface clean
	make -C $(ofproto_dir)/src/ttp clean
	make -C $(ofproto_dir)/src/nsrm clean
	make -C $(ofproto_dir)/src/messages clean
	make -C $(ofproto_dir)/src/transport clean
	make -C $(ofproto_dir)/src/view clean
#	make -C $(ofproto_dir)/src/nem clean
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	$(RM) -rf $(OSPKG_BIN_DIR)/lib/*
