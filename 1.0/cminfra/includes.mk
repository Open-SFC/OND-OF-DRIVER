CM_INCLD_DIR  = $(CM_DIR)/include

INCLUDES = \
		-I $(CM_INCLD_DIR)/common \
		-I $(CM_INCLD_DIR)/infra \
		-I $(CM_INCLD_DIR)/infra/dm \
		-I $(CM_INCLD_DIR)/infra/dm/include \
		-I $(cminfra_dir)/infra/dm/include \
		-I $(cminfra_dir)/ldsv/include \
		-I $(cminfra_dir)/include/common \
		-I $(cminfra_dir)/infra/je/include \
		-I $(cminfra_dir)/infra/transport/include \
		-I $(cminfra_dir)/cli/include \
		-I $(cminfra_dir)/utils/dslib/include \
                -I $(cminfra_dir)/utils/netutil/include\
                -I $(cminfra_dir)/utils/timer/include\
                -I $(cminfra_dir)/utils/auth/include\
                -I $(cminfra_dir)/lxos/include\
		-I $(CM_INCLD_DIR)/infra/je\
		-I $(CM_INCLD_DIR)/infra/transport \
		-I $(CM_INCLD_DIR)/lxos \
		-I $(CM_INCLD_DIR)/utils \
		-I $(CM_INCLD_DIR)/utils/common \
		-I $(CM_INCLD_DIR)/utils/dslib \
		-I $(CM_INCLD_DIR)/utils/netutil \
		-I $(CM_INCLD_DIR)/utils/auth\
		-I $(cminfra_dir)/http/cfg \
		-I $(cminfra_dir)/http/include \
		-I $(cminfra_dir)/http/include/lx \
		-I $(cminfra_dir)/http/include/httpscrt \
		-I $(ext_dir)/openssl-1.0.0/include/openssl \
		-I $(CM_INCLD_DIR)/utils/timerlib
ifeq ($(TARGET_PLATFORM), p2041rdb)
    INCLUDES += \
		-I $(switch_dir)/include \
		-I $(switch_mgmt_dir)/includes/global\
		-I $(switch_dir)/src/network/ofagent/include\
		-I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/include \
		-I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/lib \
		-I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/lib/powerpc-fsl-linux/4.6.2
endif
ifeq ($(TARGET_PLATFORM), p4080ds)
    INCLUDES += \
  		-I$(SYSROOT_PATH)/usr/include \
  		-I$(SYSROOT_PATH)/usr/lib \
  		-I$(SYSROOT_PATH)/usr/lib/powerpc-fsl_networking-linux/4.7.2 \
  		-I$(SYSROOT_PATH)/usr/lib/powerpc-fsl_networking-linux/4.6.2
endif
ifeq ($(TARGET_PLATFORM), t4240qds)
    INCLUDES += \
  		-I$(SYSROOT_PATH)/usr/include \
  		-I$(SYSROOT_PATH)/usr/lib64 \
  		-I$(SYSROOT_PATH)/usr/lib64/powerpc64-fsl_networking-linux/4.7.2
endif

CFLAGS += $(INCLUDES)
