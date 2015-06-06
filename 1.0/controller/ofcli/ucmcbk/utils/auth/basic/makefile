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

VOR_CODE_BASE_DIR = ../../../../..

include $(VOR_CODE_BASE_DIR)/$(UCMCBK_SYS_DIR)/rules.mod


#TARGET = $(MODSET_OBJ_DIR)/uauthusrplg.o


CM_AUTH_UDB_UTILITIES_LIB=$(MODSET_CBK_OBJ_DIR)/lib/libucmauthudbutils.so
OBJS = $(MODSET_CBK_OBJ_DIR)/usrplgn.o


all: $(CM_AUTH_UDB_UTILITIES_LIB)

#$(TARGET): $(OBJS)
#	$(LD) $(LDOPTS) $(OBJS) -o $(TARGET)

$(CM_AUTH_UDB_UTILITIES_LIB): $(OBJS)
		$(CC) -shared -o $(CM_AUTH_UDB_UTILITIES_LIB) $(OBJS) -o $@


clean:
	$(RM) $(OBJS)
	$(RM) $(CM_AUTH_UDB_UTILITIES_LIB)
	$(RM) $(TARGET)

include $(VOR_CODE_BASE_DIR)/$(UCMCBK_SYS_DIR)/rules.inc

