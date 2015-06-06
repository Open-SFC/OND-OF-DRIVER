CM_INCLD_DIR  = $(CM_DIR)/include

INCLUDES = \
		-I $(CM_INCLD_DIR)/common \
		-I $(CM_INCLD_DIR)/infra \
		-I $(CM_INCLD_DIR)/infra/dm \
		-I $(CM_INCLD_DIR)/infra/dm/include \
		-I $(CM_DIR)/infra/dm/include \
		-I $(CM_DIR)/ldsv/include \
		-I $(CM_DIR)/infra/je/include \
		-I $(CM_DIR)/infra/transport/include \
		-I $(CM_DIR)/cli/include \
		-I $(CM_DIR)/utils/dslib/include \
                -I $(CM_DIR)/utils/netutil/include\
                -I $(CM_DIR)/utils/timer/include\
                -I $(CM_DIR)/utils/auth/include\
                -I $(CM_DIR)/lxos/include\
		-I $(CM_INCLD_DIR)/infra/je\
		-I $(CM_INCLD_DIR)/infra/transport \
		-I $(CM_INCLD_DIR)/lxos \
		-I $(CM_INCLD_DIR)/utils \
		-I $(CM_INCLD_DIR)/utils/common \
		-I $(CM_INCLD_DIR)/utils/dslib \
		-I $(CM_INCLD_DIR)/utils/netutil \
		-I $(CM_INCLD_DIR)/utils/auth\
		-I $(CM_INCLD_DIR)/utils/timerlib 

CONTRLR_DIR= $(cntrl_dir)
CONTRLR_INCLUDES = -I $(CONTRLR_DIR)/ofproto/include\
		   -I $(CONTRLR_DIR)/include\
		   -I $(CONTRLR_DIR)/ofproto/src/include\
ifneq (,$(findstring OPENFLOW_IPSEC_SUPPORT,$(CFLAGS)))
		   -I $(CONTRLR_DIR)/ofapps/ipsec/include\
endif
		   -I $(ext_dir)/urcu\
		   -I $(ext_dir)/futex


#
# Kernel Module includes
#

KSRC_DIR = $(KERNEL_PATH)

KINCLUDES +=    -I$(KSRC_DIR)/include	\
		-I$(KSRC_DIR)/include/linux	\
                -I$(KSRC_DIR)/include/net/ \
                -I$(KSRC_DIR)/include/asm/mach-default \
               -include $(KSRC_DIR)/include/linux/autoconf.h
