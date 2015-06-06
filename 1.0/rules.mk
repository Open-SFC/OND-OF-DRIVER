#working directory
of_dir = $(PWD)

cntrl_dir = ${of_dir}/controller
switch_dir = ${of_dir}/switch
ext_dir = ${of_dir}/opensrc
cminfra_dir = ${of_dir}/cminfra
python_bindings_dir = ${of_dir}/python-ucm

#controller directories
#========================
l3extutils_dir = ${cntrl_dir}/l3extutils
ofcli_dir = ${cntrl_dir}/ofcli
ofproto_dir = ${cntrl_dir}/ofproto
ofapps_dir = ${cntrl_dir}/ofapps
utils_dir = ${cntrl_dir}/utils
ofxml_dir = ${cntrl_dir}/xml
pethapp_dir = ${cntrl_dir}/pethapp
cntrlkern_dir = ${cntrl_dir}/kernel
CM_DM_DIR= $(ofcli_dir)
CM_INFRA_DIR= $(cminfra_dir)

ifeq ($(TARGET_PLATFORM), p2041rdb)
  LIBRARY_PATH=$(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb-tcbootstrap/usr/lib:$LIBRARY_PATH
  export LIBRARY_PATH
  OF_LIB_DIR = ${ofproto_dir}/build/lib-p2041
OF_NCURSES_LIB = -L$(OF_LIB_DIR) -lncurses
INCLUDE += -I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/include
INCLUDE += -I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/lib
INCLUDE += -I $(USDPAA_SDK_INSTALL_PATH)/build_p2041rdb_release/tmp/sysroots/p2041rdb/usr/lib/powerpc-fsl-linux/4.6.2

endif

ifeq ($(TARGET_PLATFORM), p4080ds)
  OF_LIB_DIR = ${ofproto_dir}/build/lib-p4080
  OF_NCURSES_LIB = -L$(OF_LIB_DIR) -lncurses
INCLUDE += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/include
INCLUDE += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/lib
INCLUDE += -I $(PKG_CONFIG_SYSROOT_DIR)/usr/lib/powerpc-fsl_networking-linux/4.7.2

endif

ifeq ($(TARGET_PLATFORM), t4240qds)
OF_LIB_DIR = ${ofproto_dir}/build/lib-t4240
OF_NCURSES_LIB = -L$(OF_LIB_DIR) -lncurses
INCLUDE  = -I$(SYSROOT_PATH)/usr/include
INCLUDE += -I$(SYSROOT_PATH)/usr/lib64
INCLUDE += -I$(SYSROOT_PATH)/usr/lib64/powerpc64-fsl_networking-linux/4.7.2
endif

ifeq ($(TARGET_PLATFORM), X86)
OF_LIB_DIR = ${ofproto_dir}/build/lib-x86
  INCLUDE = -I/usr/include
endif

ifeq ($(TARGET_PLATFORM), X86-64bit)
OF_LIB_DIR = ${ofproto_dir}/build/lib-x86-64bit
  INCLUDE = -I/usr/include
endif

OSPKG_BIN_DIR = $(SRC_DIR)/bin

export of_dir cntrl_dir switch_dir ext_dir cminfra_dir l3extutils_dir ofcli_dir ofproto_dir ofapps_dir utils_dir pethapp_dir cntrlkern_dir OF_LIB_DIR OF_NCURSES_LIB OSPKG_BIN_DIR OPENSSL_CRYPTO_LIB OPENSSL_SSL_LIB

ifneq ($(TARGET_PLATFORM), p2041rdb)
INCLUDE += -I $(cntrl_dir)/include\
          -I ${l3extutils_dir}/include\
          -I $(cntrl_dir)/ofapps/ipsec/include \
          -I $(cntrl_dir)/ofapps/tsc-app/src/include \
          -I $(ofproto_dir)/src/include\
          -I $(pethapp_dir)/include\
          -I $(cntrlkern_dir)/include\
          -I ${ext_dir}/openssl-1.0.0/include \
          -I ${ext_dir}/openssl-1.0.0/include/openssl\
          -I ${ext_dir}/urcu\
          -I ${ext_dir}/futex
CFLAGS += $(INCLUDE) -Wall -D_GNU_SOURCE -g -O
endif

BIN_DIR = ${ofproto_dir}/build/bin
LIBS += -L $(OF_LIB_DIR) -lurcu-mb -lurcu-common -lurcu
LIBS += -L $(OF_LIB_DIR)/apps
LIBS += -lpthread
LIBS += -lrt
LIBS += -ldl
LIBS += -rdynamic
LIBS += -lcrypto
LIBS += -lssl 

MGMT_FLAGS= -DCNTRL_OFCLI_SUPPORT

#APP_FLAGS = -DFSLX_COMMON_AND_L3_EXT_SUPPORT
APP_FLAGS = -DNICIRA_EXT_SUPPORT
#APP_FLAGS += -DOPENFLOW_IPSEC_SUPPORT
#APP_FLAGS += -DOPENFLOW_IPSEC_DEBUG_SUPPORT
#FUTEX_FLAGS = -DFUTEX_LIB_P4080_SUPPORT

APP_FLAGS += -DCNTRL_CBENCH_SUPPORT
#APP_FLAGS += -DCNTRL_APP_THREAD_SUPPORT
#APP_FLAGS += -DCNTRL_NETWORK_ELEMENT_MAPPER_SUPPORT
CFLAGS += -DCONFIG_VQA_PSP_KERNEL_PATCH
APP_FLAGS += -DCNTRL_IPV4_MULTICAST_SUPPORT
APP_FLAGS += -DCNTRL_ARP_APP_SUPPORT
CFLAGS += $(MGMT_FLAGS)
CFLAGS += $(APP_FLAGS)
CFLAGS += $(FUTEX_FLAGS)
CFLAGS += -g
#CFLAGS += -DSYSLOG_SERVER_SUPPORT
CFLAGS += -DCNTLR_CRM_SUPPORT
CFLAGS += -DOF_XML_SUPPORT
CFLAGS += -fPIC
INFRA_FLAGS += -DCNTRL_INFRA_LIB_SUPPORT
CFLAGS += $(INFRA_FLAGS)
ifneq (,$(findstring CNTRL_OFCLI_SUPPORT,$(CFLAGS)))
MGMT_LIB=-lcntrlucmcbk
endif
export MGMT_LIB 
