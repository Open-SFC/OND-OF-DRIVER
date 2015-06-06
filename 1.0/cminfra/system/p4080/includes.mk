
CM_INCLD_DIR  = $(CM_DIR)/modules/include
CM_SYSINCLD_DIR  = $(CM_DIR)/modules/system/p4080
#OPENSSL_DIR=$(OPENSRC_BASE_DIR)/openssl-1.0.0
OPENSSL_DIR=$(OPENSRC_BASE_DIR)/xorp-1.8.5

INCLUDES = \
		-I $(CM_INCLD_DIR)/common \
		-I $(CM_INCLD_DIR)/infra \
		-I $(CM_INCLD_DIR)/infra/dm \
		-I $(CM_INCLD_DIR)/infra/je \
		-I $(CM_INCLD_DIR)/infra/transport \
		-I$(OPENSSL_DIR)\
		-I$(OPENSSL_DIR)/include \
		-I $(CM_INCLD_DIR)/lxos \
		-I $(CM_INCLD_DIR)/utils \
		-I $(CM_INCLD_DIR)/utils/common \
		-I $(CM_INCLD_DIR)/utils/dslib \
		-I $(CM_INCLD_DIR)/utils/netutil \
		-I $(CM_INCLD_DIR)/utils/auth \
		-I $(CM_DIR)/../../othersrc-MCUTM/opensrc/zlib/zlib1_2_3/include \
		-I $(CM_SYSINCLD_DIR)/mgmteng/ldsv\
		-I $(CM_INCLD_DIR)/utils/timerlib \
		-I $(CM_INCLD_DIR)/mgmteng/statscltr


