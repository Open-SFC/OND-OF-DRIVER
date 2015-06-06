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

CM_DIR = ../../../..
include $(CM_DIR)/$(PROD_DIR)/rules.mod


MODSET_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/obj
DSLIB_OBJ=$(MODSET_OBJ_DIR)/dslibbasic.o
NETUTIL_OBJ=$(MODSET_OBJ_DIR)/netutilbasic.o
TIMER_OBJ=$(MODSET_OBJ_DIR)/timerbasic.o
UTIL_CMN_OBJ=$(MODSET_OBJ_DIR)/utilcmnbasic.o
#TMRLIB_SYS_DIR=$(CM_DIR)/modules/system/lxpc/utils/timerlib
TMRLIB_SYS_DIR=$(CM_DIR)/modules/utils/timerlib

ifneq (,$(findstring OF_CM_AUTH_PLUGIN_SUPPORT,$(CFLAGS)))
AUTH_OBJ=$(MODSET_OBJ_DIR)/usrauth.o
else
AUTH_OBJ=$(MODSET_OBJ_DIR)/authlibbasic.o
endif


CM_UTILITIES_LIB=$(MODSET_OBJ_DIR)/lib/libucmutils.so

ifneq (,$(findstring OF_CM_AUTH_PLUGIN_SUPPORT,$(CFLAGS)))
CM_AUTH_UTILITIES_LIB=$(MODSET_OBJ_DIR)/lib/authplugin/libucmauthutils.so
else
CM_AUTH_UTILITIES_LIB=$(MODSET_OBJ_DIR)/lib/libucmauthutils.so
endif

CM_UTIL_CMN_LIB=$(MODSET_OBJ_DIR)/lib/libucmutilcmn.so

all : $(CM_UTILITIES_LIB) $(CM_AUTH_UTILITIES_LIB)  $(CM_UTIL_CMN_LIB) makeucmtmrlib

$(CM_UTILITIES_LIB):  $(DSLIB_OBJ) $(NETUTIL_OBJ) $(TIMER_OBJ)
	$(CC) -shared -o $(CM_UTILITIES_LIB) $(DSLIB_OBJ) $(NETUTIL_OBJ) $(TIMER_OBJ) -o $@

$(CM_UTIL_CMN_LIB): $(UTIL_CMN_OBJ) 
	$(CC) -shared -o $(CM_UTIL_CMN_LIB) $(UTIL_CMN_OBJ) -o $@

makeucmtmrlib:
	make -C $(TMRLIB_SYS_DIR)

$(CM_AUTH_UTILITIES_LIB): $(AUTH_OBJ)
	$(CC) -shared -o $(CM_AUTH_UTILITIES_LIB) $(AUTH_OBJ) -o $@

clean:
	$(RM) $(CM_UTILITIES_LIB) $(CM_UTIL_CMN_LIB) 
	$(RM) $(CM_AUTH_UTILITIES_LIB)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
