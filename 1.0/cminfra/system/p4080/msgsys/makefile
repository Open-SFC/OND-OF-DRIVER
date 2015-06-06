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
# Description: To build UCM infrastructure

CM_DIR = ../../../..

include $(CM_DIR)/$(PROD_DIR)/rules.mod

CM_SYS_DIR=$(CM_DIR)/modules/system/p4080
#CM_SYS_DIR=$(CM_DIR)/$(PROD_DIR)

CM_MSGSYS_SYS_DIR = $(CM_SYS_DIR)/msgsys


CM_LOGCLTR_DIR        =  $(CM_MSGSYS_SYS_DIR)/logcltr
CM_MESGCLNT_DIR        =  $(CM_MSGSYS_SYS_DIR)/mesgclnt

all: makemsgsys

makemsgsys:
	make -C $(CM_LOGCLTR_DIR) 
	make -C $(CM_MESGCLNT_DIR) 

clean: cleanmsgsys

cleanmsgsys:
	make -C $(CM_LOGCLTR_DIR)  PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_MESGCLNT_DIR) PROD_DIR=$(CM_PROD_DIR) clean
