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

TRCHNL_BASIC_OBJ=$(MODSET_OBJ_DIR)/transbasic.o
TRCHNL_PORT_OBJ=$(MODSET_OBJ_DIR)/transport.o
JE_WRAP_OBJ=$(MODSET_OBJ_DIR)/jewrap.o
DM_WRAP_OBJ=$(MODSET_OBJ_DIR)/dmwrap.o
LDSV_WRAP_OBJ=$(MODSET_OBJ_DIR)/ldsvwrap.o


CM_TRCHNL_LIB=$(MODSET_OBJ_DIR)/lib/libucmtrchl.so

all : $(CM_TRCHNL_LIB)

$(CM_TRCHNL_LIB): $(TRCHNL_BASIC_OBJ) $(JE_WRAP_OBJ) $(DM_WRAP_OBJ) $(LDSV_WRAP_OBJ) 
	$(CC) -shared -o $(CM_TRCHNL_LIB) $(TRCHNL_BASIC_OBJ) $(JE_WRAP_OBJ) $(DM_WRAP_OBJ) $(LDSV_WRAP_OBJ) -o $@


clean:
	$(RM) $(CM_TRCHNL_LIB)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
