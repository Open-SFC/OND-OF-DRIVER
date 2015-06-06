UCMCBK_INCLD_DIR = $(ofcli_dir)/ucmcbk/includes
UCMINF_INCLD_DIR = $(cminfra_dir)/include

CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/common
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/infra
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/infra/dm
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/infra/je
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/infra/transport
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/utils
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/utils/dslib
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/utils/auth
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/utils/netutil
CM_LINCLUDES  +=  -I $(UCMINF_INCLD_DIR)/lxos
CM_LINCLUDES  +=  -I $(UCMCBK_INCLD_DIR)/global
CM_LINCLUDES  +=  -I $(UCMCBK_INCLD_DIR)/generated
CM_LINCLUDES  +=  -I $(cminfra_dir)/infra/je/include
CM_LINCLUDES  +=  -I $(ofcli_dir)/ucmcbk/dp/include
CM_LINCLUDES  +=  -I $(ofcli_dir)/ucmcbk/cntlrucm/include
CM_LINCLUDES  +=  -I $(ofcli_dir)/ucmcbk/comm/include
CM_LINCLUDES  +=  -I $(ofcli_dir)/ucmcbk/dprm/include
CM_LINCLUDES  +=  -I $(ofcli_dir)/ucmcbk/crm/include

CONTRLR_DIR= $(cntrl_dir)
CONTRLR_INCLUDES = -I $(CONTRLR_DIR)/ofproto/include\
		   -I $(CONTRLR_DIR)/include\
		   -I $(CONTRLR_DIR)/ofproto/src/include\
		   -I $(CONTRLR_DIR)/l3extutils/include\
		   -I $(CONTRLR_DIR)/ofapps/ipsec/include\
		   -I $(ext_dir)/openssl-1.0.0/include \
		   -I $(ext_dir)/openssl-1.0.0/include/openssl \
		   -I $(ext_dir)/urcu\
		   -I $(ext_dir)/futex

LOCAL_INCLUDES += $(CM_LINCLUDES)
LOCAL_INCLUDES += $(CONTRLR_INCLUDES)
