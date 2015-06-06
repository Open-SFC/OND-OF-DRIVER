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
LXOS_OBJ=$(MODSET_OBJ_DIR)/lxosbasic.o


CM_LXOS_LIB=$(MODSET_OBJ_DIR)/lib/libucmlxos.so

all : $(CM_LXOS_LIB)

$(CM_LXOS_LIB):  $(LXOS_OBJ) 
	$(CC) $(LXOS_OBJ) -shared  -o $(CM_LXOS_LIB)


clean:
	$(RM) $(CM_LXOS_LIB)

include $(CM_DIR)/$(PROD_DIR)/rules.inc
