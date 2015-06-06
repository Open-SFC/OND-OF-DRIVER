CM_INCLD_DIR  = $(CM_DIR)/modules/include
CM_OPEN_SRC_DIR  = $(CM_DIR)/opensrc
OF_SRC_DIR=$(CM_DIR)/..
OF_OPENSSL_SRC_DIR=/root/OF/Apr02/othersrc/opensrc/openssl-1.0.0/include
OF_OPENSSL_SRC_KER_DIR=$(OF_SRC_DIR)/modules/modopsrc/kernopsl/include

INCLUDES = \
		-I $(CM_INCLD_DIR)/common \
		-I $(CM_INCLD_DIR)/infra \
		-I $(CM_INCLD_DIR)/infra/dm \
		-I $(CM_INCLD_DIR)/infra/je \
		-I $(CM_INCLD_DIR)/infra/transport \
		-I $(OF_OPENSSL_SRC_DIR) \
		-I $(OF_OPENSSL_SRC_KER_DIR) \
		-I $(CM_OPEN_SRC_DIR)/zlib/include \
		-I $(CM_INCLD_DIR)/lxos \
		-I $(CM_INCLD_DIR)/utils \
		-I $(CM_INCLD_DIR)/utils/common \
		-I $(CM_INCLD_DIR)/utils/dslib \
		-I $(CM_INCLD_DIR)/utils/netutil \
		-I $(CM_INCLD_DIR)/utils/auth\
		-I $(CM_INCLD_DIR)/utils/timerlib \
		-I $(CM_INCLD_DIR)/mgmteng/statscltr

#
# Kernel Module includes
#

KSRC_DIR = $(KERNEL_PATH)

KINCLUDES +=    -I$(KSRC_DIR)/include	\
								-I$(KSRC_DIR)/include/linux	\
                -I$(KSRC_DIR)/include/net/ \
                -I$(KSRC_DIR)/include/asm/mach-default \
               -include $(KSRC_DIR)/include/linux/autoconf.h
