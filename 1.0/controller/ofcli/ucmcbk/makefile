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

include ../../../rules.mk
UCMCBK_DIR= ../../ofcli/ucmcbk
include $(UCMCBK_DIR)/rules.mod

OF_MOD_DIR = $(VOR_CODE_BASE_DIR)/$(VOR_MOD_DIR)

#include $(VOR_CODE_BASE_DIR)/$(VOR_SYS_DIR)/rules.mod

CM_INFRA_CBK_TARGET = $(MODSET_CBK_OBJ_DIR)/ucmcbktarget.o
CM_CBK_OBJS=$(MODSET_CBK_OBJ_DIR)/ucmreg.o
CM_CBK_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmdpcbk.o
CM_CBK_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmcntlrcomm.o

CNTRLR_CM_LIB=$(MODSET_CBK_OBJ_DIR)/lib/libcntrlucmcbk.so
CM_CNTRLR_OBJS=$(MODSET_CBK_OBJ_DIR)/cntrlrucmtarget.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmglutils.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmdprmcbk.o
#CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmnemcbk.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmstatscbk.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)//ucmmsgstarget.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmcrmcbk.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmtrnsprtcbk.o
CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmnsrmcbk.o
#CM_CNTRLR_OBJS+=$(MODSET_CBK_OBJ_DIR)/ucmtsccbk.o

all: makereg makedp makecomm makegutil makecntlrucm makecbks $(CM_INFRA_CBK_TARGET) $(CNTRLR_CM_LIB) copytargets
clean: cleanreg cleandp cleancomm cleangutil cleancntlrucm cleancbks cleantargets

makereg:
	make -w -C ucm 

makedp:
	make -w -C dp 

makecomm:
	make -w -C comm 

makecbks:
	make -w -C dprm 
	make -w -C crm 
	make -w -C stats 
	make -w -C msgs 
	make -w -C transport
	make -w -C nsrm 
#	make -w -C tsc 
#	make -w -C nem 

makecntlrucm:
	make -w -C cntlrucm 

makegutil:
	make -w -C utils

$(CM_INFRA_CBK_TARGET): $(CM_CBK_OBJS)
	$(LD) $(LDOPTS) $(CM_CBK_OBJS) -o $(CM_INFRA_CBK_TARGET)

$(CNTRLR_CM_LIB): $(CM_CNTRLR_OBJS)
	    $(CC) -shared -o $(CNTRLR_CM_LIB) $(CM_CNTRLR_OBJS)  -o $@

copytargets:
	cp $(CM_INFRA_CBK_TARGET)  $(ofcli_dir)/obj
	cp $(CNTRLR_CM_LIB)  $(OF_LIB_DIR)

cleanreg:
	make -w -C ucm clean

cleandp:
	make -w -C dp clean

cleancomm:
	make -w -C comm clean

cleancbks:
	make -w -C dprm clean
	make -w -C crm clean
	make -w -C stats clean
	make -w -C msgs clean 
#	make -w -C nem clean 
	make -w -C transport clean
	make -w -C nsrm clean 
#	make -w -C tsc clean 

cleancntlrucm:
	make -w -C cntlrucm clean

cleangutil:
	make -w -C utils clean

cleantargets:
	rm -f $(CM_INFRA_CBK_TARGET)
	rm -f $(CNTRLR_CM_LIB)

cleandeps:
	$(RM) obj/*.d 
