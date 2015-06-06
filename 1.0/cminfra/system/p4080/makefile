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
# Description: Master makefile to build UCM

OF_MOD_DIR= ../../../../igateway/
CM_DIR=$(OF_MOD_DIR)/modules/../../ucm
CM_DIR=../../..

#CM_GLUE_PATH = $(OF_MOD_DIR)/../../ucmcbk
#CM_GLUE_PROD_DIR = $(CM_GLUE_PATH)/control_plane/modules/system/ENTR-SPUTM/lx-bin
include rules.mod

include defs.system
#all:  copysputmlibs makeucm makemgmt makemsgsys makesys  $(CLI) $(HTTPD)  $(UCMLDSVD) $(TRCHNLD) $(UCMD) $(CSD) $(STATCLTRD)
all:  makeucm makemgmt makemsgsys makesys  $(CLI) $(HTTPD)  $(UCMLDSVD) $(TRCHNLD) $(CSD) $(STATCLTRD) $(UCMD)
#all: makedmtool makeglenv makeaplcmp makeucm makemgmt makemsgsys makesys  $(CLI) $(HTTPD) $(LOGCLTRLIB) $(MESGCLNTLIB) $(MESGCLNTD) $(LOGCLTRD) $(UCMLDSVD) $(TRCHNLD) $(UCMD) 

copysputmlibs:
	cp $(OF_CP_SYS_DIR)/VortiQa/lib/*.so $(MODSET_CM_OBJ_DIR)/../tpl/.
ifeq ($(PRODUCT_TYPE),SPUTM)
	cp $(OF_CP_SYS_DIR)/obj/igwbnbasic.o $(MODSET_CM_OBJ_DIR)/../tpl/.
	cp $(OF_CP_SYS_DIR)/obj/igwbnsys.o $(MODSET_CM_OBJ_DIR)/../tpl/.
	cp $(OF_CP_SYS_DIR)/obj/crptopsslglue.o $(MODSET_CM_OBJ_DIR)/../tpl/.
	cp $(OF_CP_SYS_DIR)/obj/pkdrvbasic.o $(MODSET_CM_OBJ_DIR)/../tpl/.
	cp $(OF_CP_SYS_DIR)/obj/scepcmn.o $(MODSET_CM_OBJ_DIR)/../tpl/.
endif
	cp $(OF_CP_MOD_DIR)/PlatFrmSrvs_FinalObjs/lib/*.so $(MODSET_CM_OBJ_DIR)/../tpl/.
makeucm:
	make -C $(CM_INFRA_DIR) PROD_DIR=$(CM_PROD_DIR) 
	make -C $(CM_UTIL_DIR) PROD_DIR=$(CM_PROD_DIR) 
	make -C $(CM_LXOS_DIR) PROD_DIR=$(CM_PROD_DIR)
ifeq ($(UCMCONFIGSYNC), TRUE)
	make -C $(CM_CONFIGSYNC_DIR) PROD_DIR=$(CM_PROD_DIR)
endif 

makemgmt:
	make -C $(CM_MGMT_DIR) PROD_DIR=$(CM_PROD_DIR) 

makesys:
	make -w -C $(CM_SYS_DIR)/infra  PROD_DIR=$(CM_PROD_DIR)
	make -w -C $(CM_SYS_DIR)/mgmteng  PROD_DIR=$(CM_PROD_DIR)
	make -w -C $(CM_SYS_DIR)/utils  PROD_DIR=$(CM_PROD_DIR)
	make -w -C $(CM_SYS_DIR)/lxos  PROD_DIR=$(CM_PROD_DIR)

makeglue:
	make -C $(CM_GLUE_PROD_DIR) -f makefile all

makedmtool:
	ant -f $(CM_DM_TOOL_BIN_DIR)/build.xml -DPROD_DIR=$(CM_PROD_DIR) -DCM_PROJECT_PATH_FORANT=$(CM_PROJECT_PATH_FORANT) -DGLU_PROD_DIR=$(GLU_PROD_DIR)
	echo "var defaultHost = \"localhost\";" > ../html/host.js
ifeq ($(PRODUCT_TYPE),ITCM)
	echo "var defaultHostType = \"Cluster\";" >> ../html/host.js
else
	echo "var defaultHostType = \"SingleDevice\";" >> ../html/host.js
endif

makemsgsys:
	make -C $(CM_MSGSYS_DIR) PROD_DIR=$(CM_PROD_DIR)

$(TRCHNLD): $(TRCHNL_PORT_OBJ) 
	$(CC) $(LDFLAGS) -lpthread  $(MGMTLIBFLAGS) $(UCMLIBFLAGS) $(TRCHNL_PORT_OBJ) -lm -o	$(TRCHNLD) -ldl

$(CLI):$(CLI_BASIC_OBJ) $(CLI_PORT_OBJ)
	$(CC) $(LDFLAGS) $(DYNAMICFLAG) $(LDREADLINEFLAGS) -lpthread $(MGMTLIBFLAGS) $(UCMLIBFLAGS) $(LDAUTHLIB) $(CLI_BASIC_OBJ) $(CLI_PORT_OBJ) -lm $(OF_OPENSSL_LIBRARIES) $(NCURSES_LIB_FLAGS) -ldl -o $(CLI)

ifneq (,$(findstring CM_STATS_COLLECTOR_SUPPORT,$(CFLAGS)))
$(STATCLTRD): $(STATSCLTR_BASIC_OBJ) $(STATSCLTR_USPACE_OBJ)
	$(CC) $(LDFLAGS)   -lpthread $(MGMTLIBFLAGS) $(UCMLIBFLAGS) $(STATSCLTR_BASIC_OBJ) $(STATSCLTR_USPACE_OBJ) $(UCMSTATCLTRFLAGS) -lm $(ITCL_FLAGS) $(LIBPSP)  -o $(STATCLTRD) -ldl
endif

$(LOGCLTRLIB): $(LOGCLTR_CM_OBJ)
	$(CC) $(LDFLAGS) $(LDLOGCLTRFLAGS) $(UCMLIBFLAGS) $(LOGCLTR_CM_OBJ) -o $(LOGCLTRLIB)

$(LOGCLTRD): $(LOGCLTR_BASIC_OBJ) $(LOGCLTR_PORT_OBJ) $(LOGCLTR_SYS_OBJ)
	$(CC) $(LDFLAGS) $(LOGCLTR_BASIC_OBJ) $(LOGCLTR_PORT_OBJ) $(LOGCLTR_SYS_OBJ) $(LOGCLTRDYNLIB) -lm -o $(LOGCLTRD)

$(MESGCLNTLIB): $(MESGCLNTLIB_CM_OBJ)
	$(CC) $(LDFLAGS) $(UCMLIBFLAGS) $(MESGCLNTLIB_CM_OBJ) $(LDMESGCLNTFLAGS) -o $(MESGCLNTLIB)

$(MESGCLNTD): $(MESGCLNT_BASIC_OBJ) $(MESGCLNT_PORT_OBJ)  $(MSGCLIENT_OBJS) $(MESGCLNT_SYS_OBJ)
	$(CC) $(LDFLAGS) -lpthread $(MSGCLIENT_OBJS) $(MESGCLNT_BASIC_OBJ) $(MESGCLNT_PORT_OBJ)  $(MESGCLNT_SYS_OBJ) $(MSGSYSDYNLIB) -lm -o $(MESGCLNTD)

$(HTTPD): $(HTTP_SYS_OBJ) $(HTTP_WRAP_OBJ) $(HTTP_USPACE_OBJ) $(HTTPS_BASIC_OBJ) $(HTTP_BASIC_OBJ) $(TIMER_LIB_OBJ) $(DM_BASIC_OBJ) 
	$(CC) $(LDFLAGS) -rdynamic -lpthread $(UCMUTILSCMNLIB) $(UCMLIBFLAGS) $(UCMHTTPOPENSRC) $(MGMTLIBFLAGS) $(LDAUTHLIB) $(HTTP_WRAP_OBJ) $(HTTP_USPACE_OBJ) $(HTTP_SYS_OBJ) $(HTTPS_BASIC_OBJ) $(HTTP_BASIC_OBJ) $(TIMER_LIB_OBJ) $(DM_BASIC_OBJ) -lm $(OF_OPENSSL_LIBRARIES) -o $(HTTPD)

$(UCMLDSVD):$(LDSV_BASIC_OBJ) $(LDSV_USPACE_OBJ) 
	$(CC) $(LDFLAGS)  -lpthread $(MGMTLIBFLAGS) $(UCMLIBFLAGS) $(UCMUTILSCMNLIB) $(DM_BASIC_OBJ) $(IGWFLAGS) $(LDSV_BASIC_OBJ) $(LDSV_USPACE_OBJ) -lm  $(ITCL_FLAGS) $(LIBPSP) -o $(UCMLDSVD)

$(CSD):$(CSD_BASIC_OBJ) $(CSD_USPACE_OBJ) 
	$(CC) $(LDFLAGS) $(UCMLIBFLAGS) $(UCMTRCHLLIB) $(CSD_BASIC_OBJ) $(CSD_USPACE_OBJ) -lm -lpthread -ldl $(ITCL_FLAGS) $(LIBPSP)  -o $(CSD)

$(DRIVER): $(DRIVER_BASIC_OBJS)  
	$(LD) $(LDFLAGS) $(LDDRVFLAGS) $(DRIVER_BASIC_OBJS)  -o $(DRIVER)
ifeq ($(KERNEL_MAJOR_VERSION),2.6)
	$(MODPOST) -m -a -i $(KERNEL_SYMVERS) -I $(MODULE_SYMVERS) -o $(MODULE_SYMVERS) -w  $(DRIVER)
	$(CC) -c $(KMODLDFLAGS) $(KCFLAGS) -o  $(DRIVER_2_6_MOD_O) $(DRIVER_2_6_MOD_C)
	$(LD) $(LDDRVFLAGS) -o $(DRIVER_2_6) $(DRIVER) $(DRIVER_2_6_MOD_O)
endif

#include $(OF_CP_SYS_DIR)/rules.mod
$(UCMD):$(JE_BASIC_OBJ) $(JE_USPACE_OBJ) $(DM_BASIC_OBJ) $(JE_SYS_OBJ) $(LOGCLTR_WRAP_FLAG) $(IGWUCMCBKLIB) 
	$(CC) $(LDFLAGS) -rdynamic $(LOGCLTR_WRAP_FLAG) $(DM_BASIC_OBJ) $(IGWFLAGS) $(IGWUCMCBKLIB) $(UCMLIBFLAGS) $(UCMTRCHLLIB) $(JE_BASIC_OBJ) $(JE_USPACE_OBJ) $(JE_SYS_OBJ) $(UCMSTATCLTRFLAGS) $(UCMAPPLIB) -lm -lc -lpq -lmip -ldl $(LIBPSP) $(ITCL_FLAGS)  -o $(UCMD)

clean: cleanucm cleanmgmt cleanmsgsys cleansys cleanjedmn cleantr cleancli cleanldsvdmn cleanhttp  cleanmsgclntd cleanlogcltrd cleanlogcltrlib cleanmsgclntlib cleandriver cleanglenv cleancsd cleandep cleanstatcltrd

deploy:
	echo $(PROD_DIR)
	sh ucm-install.sh $(OF_DIR)/$(ITCL_PROD_DIR)/igw_itcl_pkg/ucm 	ITCL_PCIe
	#sh ucm-install.sh $(OF_DIR)/$(ITCL_PROD_DIR)/igw_itcl_pkg/ucm 	SPUTM

cleanjedmn:
	$(RM) $(UCMD)

cleantr:
	$(RM) $(TRCHNLD)

cleancli:
	$(RM) $(CLI)

cleanldsvdmn:
	$(RM) $(UCMLDSVD)

cleanhttp:
	$(RM) $(HTTPD)

cleancsd:
	$(RM) $(CSD)

cleanstatcltrd:
	$(RM) $(STATCLTRD)

cleandep:
	$(RM) obj/*.d

cleanmsgclntd:
	make -C $(CM_MSGSYS_DIR) PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(MESGCLNTSYS_DIR) PROD_DIR=$(CM_PROD_DIR) clean
	$(RM) $(MESGCLNTD)

cleanlogcltrd:
	make -C $(LOGCLTRSYS_DIR) PROD_DIR=$(CM_PROD_DIR) clean
	$(RM) $(LOGCLTRD)

cleanlogcltrlib:
	$(RM) $(LOGCLTRLIB)

cleanmsgclntlib:
	$(RM) $(MESGCLNTLIB)

cleanucm:
	make -C $(CM_INFRA_DIR) PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_UTIL_DIR) PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_LXOS_DIR) PROD_DIR=$(CM_PROD_DIR) clean
ifeq ($(UCMCONFIGSYNC),TRUE)
	make -C $(CM_CONFIGSYNC_DIR) PROD_DIR=$(CM_PROD_DIR) clean
endif

cleanmgmt:
	make -C $(CM_MGMT_DIR) PROD_DIR=$(CM_PROD_DIR) clean

cleanmsgsys:
	make -C $(CM_MSGSYS_DIR) PROD_DIR=$(CM_PROD_DIR) clean

cleansys:
	make -C $(CM_SYS_DIR)/infra  PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_SYS_DIR)/utils  PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_SYS_DIR)/lxos  PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_SYS_DIR)/mgmteng  PROD_DIR=$(CM_PROD_DIR) clean
	make -C $(CM_SYS_DIR)/msgsys  PROD_DIR=$(CM_PROD_DIR) clean

cleandmtool:
	ant -f $(CM_DM_TOOL_BIN_DIR)/build.xml clean
	$(RM) $(CM_CBK_LOCAL_DIR)/$(CM_CBK_TARGET)
	$(RM) ../html/host.js
#	$(RM) $(UCMGL_PATH)/common/ucm/include/ucmgl/*.h
	$(RM) $(CM_DIR)/../ucmcbk/modules/includes/generated/*.h
	#make -w -C $(UCMGL_PATH)/modules/system/lxpc clean

cleanglenv:
	$(RM) $(UCMGL_PATH)/modules/basic_include
	$(RM) $(UCMGL_PATH)/modules/sysbasic_include
	$(RM) $(UCMGL_PATH)/modules/templates
	$(RM) $(UCMGL_PATH)/modules/include/ucm

cleandriver:
	$(RM)	$(DRIVER)
ifeq ($(KERNEL_MAJOR_VERSION),2.6)
	  $(RM) $(DRIVER_2_6) $(DRIVER_2_6_MOD_C) $(DRIVER_2_6_MOD_O) ./Module.symvers
endif


