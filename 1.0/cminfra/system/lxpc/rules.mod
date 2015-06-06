OSTYPE=lx

BUILD_MSGSYS=no
#BUILD_MSGSYS=yes

GLU_PROD_DIR=lxpc
PROD_DIR = modules/system/lxpc
CM_PROD_DIR = $(PROD_DIR)
MODSET_OBJ_DIR = $(CM_DIR)/$(PROD_DIR)/obj
MODSET_KERNOBJ_DIR = $(CM_DIR)/$(PROD_DIR)/obj/kernobj

KCFLAGS += -fno-strict-aliasing -fno-common -Wno-pointer-sign
ifeq ($(CPUARCH),POWERPC)
ifeq ($(TARGET_PLATFORM),P4080)
KCFLAGS += -ffreestanding -Os -pipe -msoft-float -freg-struct-return -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector -mspe=no -mabi=spe
endif
ifeq ($(TARGET_PLATFORM),P2020)
KCFLAGS += -ffreestanding -Os -pipe -msoft-float -freg-struct-return -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector -mspe=no -mabi=spe
endif
ifeq ($(TARGET_PLATFORM),MPC8572)
KCFLAGS += -ffreestanding -Os -pipe -mhard-float -freg-struct-return -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector -mspe=no -mabi=spe
endif
ifeq ($(TARGET_PLATFORM),MPC8548)
KCFLAGS += -ffreestanding -Os -pipe -mhard-float -freg-struct-return -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector -mspe=no -mabi=spe
endif
else
KCFLAGS += -ffreestanding -Os -pipe -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2  -march=i686 -mtune=generic -mtune=generic -ffreestanding -maccumulate-outgoing-args -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector
endif

ifeq ($(KERNEL_MAJOR_VERSION),2.6)
ifneq ($(CPUARCH),POWERPC)
LDDRVFLAGS = -m elf_i386
endif
LDDRVFLAGS = -r
else
LDDRVFLAGS = -r
endif

LINUX_OS_TYPE=ubuntu

include $(CM_DIR)/$(PROD_DIR)/includes.mk
LKMSFORMAT = ko

#
# External Programs Used By The Project
#

TARGET_ENDIANESS=BSP_LITTLE_ENDIAN

CROSS_PREFIX=
ELFOPTS = elf_i386

CC     = $(CROSS_PREFIX)gcc
AR     = $(CROSS_PREFIX)ar -crv
LD     = $(CROSS_PREFIX)ld
RANLIB = $(CROSS_PREFIX)ranlib
GDB    =$(CROSS_PREFIX)-gdb
RM     = rm -f
STRIP  = $(CROSS_PREFIX)strip -R .note -R .comment

.PHONY: exe default all
exe: all
default: all

#### Directory definitions

MODSET_OBJ_DIR=$(CM_DIR)/$(PROD_DIR)/obj
LINUX_SRC=/lib/modules/`uname -r`/build


#
# User Space Supports
#

CFLAGS += -Wformat -fno-stack-protector -Wunused -Wpointer-arith -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wstrict-prototypes
#-Wuninitialized 

CFLAGS +=  -DLINUX_SUPPORT=1
LDOPTS  = -r

#
# Supports. 
#

CFLAGS += -DCM_SUPPORT -DCM_DM_SUPPORT  -DCM_JE_SUPPORT -DCOMPILE_FOR_CM_ONLY
#CFLAGS += -DOF_CM_CP_CHANNEL_SUPPORT -DOF_UCMGLU_SUPPORT
CFLAGS += -DOF_UCMGLU_SUPPORT
CFLAGS += -DCM_TRANSPORT_CHANNEL_SUPPORT
CFLAGS += -DCM_ROLES_PERM_SUPPORT
ifeq ("$(BUILD_MSGSYS)", "no")
CFLAGS += -DVORTIQA_SYSLOG_SUPPORT
endif
#-DCM_TRCHNL_TEST_SUPPORT
#CFLAGS += -DCM_AUDIT_SUPPORT

CFLAGS += -DCM_VERSION_SUPPORT 
#CFLAGS += -DCM_EVENT_NOTIFMGR_SUPPORT 
CFLAGS += -DCM_AUTH_SUPPORT 
#CFLAGS += -DOF_CM_AUTH_PLUGIN_SUPPORT 
#-DCM_PAM_SUPPORT 
CFLAGS += -DCM_CLIENG_SUPPORT
#CFLAGS += -DCM_LDSV_SUPPORT
CFLAGS += -DOF_XML_SUPPORT
CFLAGS += -DOF_CM_SUPPORT 
#-DOF_HTTPD_SUPPORT -DOF_HTTPS_SUPPORT -DHTTPS_ENABLE
CFLAGS += -DOF_CM_HTTP_PLUGIN_SUPPORT

CFLAGS += -DOPENSSL_9_7

CFLAGS += -DCM_LOG_CLTR_SUPPORT -DCM_MESGCLNT_SUPPORT -DCM_SYSLOG_SCHEDULE

CFLAGS += -DCM_SECAPPL_DLL_SUPPORT

#
# Put only one of these OPTIM_FLAGS
#
#OPTIM_FLAGS = -O2
#OPTIM_FLAGS += -g 

OPTIM_FLAGS += -g -O

CFLAGS += $(OPTIM_FLAGS)

CFLAGS += $(INCLUDES) -fPIC
#CFLAGS += -Wall -Werror
#CFLAGS += -Werror

#
# Kernel Space Flags
#

KERNEL_MAJOR_VERSION=2.6 
KERNEL_MINOR_VERSION=27 
# uncomment if its not passed from make command line
# KERNEL_PATH=/lib/modules/2.6.27.18-standard/build
KERNEL_SYMVERS = $(KERNEL_PATH)/Module.symvers
MODULE_SYMVERS = $(PWD)/Module.symvers
MODPOST = $(KERNEL_PATH)/scripts/mod/modpost
KCFLAGS += -ffreestanding -Os -pipe -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2  -march=i686 -mtune=generic -mtune=generic -ffreestanding -maccumulate-outgoing-args -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -fno-stack-protector

LDOPTS  = -r -O2

ifeq ($(KERNEL_MAJOR_VERSION),2.6)
LDDRVFLAGS= -m elf_i386 -m elf_i386 -r
endif


ifeq ($(KERNEL_MAJOR_VERSION),2.6)
KCFLAGS += -fno-strict-aliasing -fno-common -Wno-pointer-sign #-Werror
endif

KCFLAGS += -Wall -Wuninitialized
KCFLAGS += -D__KERNEL__ -DMODULE -DBSP_LITTLE_ENDIAN
KCFLAGS +=  -DLINUX_SUPPORT=1


KCFLAGS += -DCM_MESGCLNT_SUPPORT
KCFLAGS +=$(KINCLUDES)
KCFLAGS +=$(OPTIM_FLAGS)

OPENSSL_VERSION=1.0.0
OPENSSL_VERSION_ALPHABET=
OPENSSL_NAME = openssl-$(OPENSSL_VERSION)$(OPENSSL_VERSION_ALPHABET)
OF_OPENSSL_LIBRARIES =-L$(OPENSSL_DIR) -lcrypto -lssl
OF_LIBCRYPTO = libigwcrypto.so.$(OPENSSL_VERSION)
OF_LIBSSL = libigwssl.so.$(OPENSSL_VERSION)
OF_LIBCRYPTO_LINK = libigwcrypto.so
OF_LIBSSL_LINK = libigwssl.so

