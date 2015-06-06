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
# Date:   02/28/2013
# Description: Makefile to build UCM Management Engines

CM_DIR = ../../../..

include $(CM_DIR)/$(PROD_DIR)/rules.mod

CM_SYS_DIR=$(CM_DIR)/$(PROD_DIR)
CM_MGMTENG_SYS_DIR = $(CM_SYS_DIR)/mgmteng


CM_HTTP_DIR      =  $(CM_MGMTENG_SYS_DIR)/http
CM_CLI_DIR       =  $(CM_MGMTENG_SYS_DIR)/cli
CM_LDSV_DIR      =  $(CM_MGMTENG_SYS_DIR)/ldsv
ifneq (,$(findstring CM_STATS_COLLECTOR_SUPPORT,$(CFLAGS)))
CM_STATCLTR_DIR  =  $(CM_MGMTENG_SYS_DIR)/statcltr
endif

all: makemgmteng

makemgmteng:
	make -C $(CM_HTTP_DIR) 
#	make -C $(CM_CLI_DIR) 
#	make -C $(CM_LDSV_DIR) 
ifneq (,$(findstring CM_STATS_COLLECTOR_SUPPORT,$(CFLAGS)))
	make -C $(CM_STATCLTR_DIR)
endif

clean: cleanmgmteng

cleanmgmteng:
	make -C $(CM_HTTP_DIR) clean
#	make -C $(CM_CLI_DIR) clean
#	make -C $(CM_LDSV_DIR) clean
ifneq (,$(findstring CM_STATS_COLLECTOR_SUPPORT,$(CFLAGS)))
	make -C $(CM_STATCLTR_DIR) clean
endif
