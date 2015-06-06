#!/bin/bash

#source rules.mod

BUILD_MSGSYS="no"

installlog=/root/log
installerrlog=/root/errlog
Sucmpkg="startucmpkg"
Kucmpkg="stopucmpkg"
CAT=cat
CLEAR=clear

INST_DIR=ucm
DEBUG_DIR=debug
CONFIG_DIR=config
DEFAULT_CONFIG_DIR=config-default
IGD_DIR=igd
VSG_DIR=vsggeneral
FIREWALL_DIR=firewall
IPDB_DIR=netobj
IPS_DIR=ips
IPSEC_DIR=ipsec
BOOT_DIR=bootflag
KERNELVERSION=`uname -r`
KERNEL_MAJOR_VERSION=`uname -r | cut -d. -f1,2`

test -z "$welcome" && {
$CLEAR
echo "--------------------------------------------------------------------------------"
echo -e "                    \033[32mUCM\033[00m INSTALLATION"
echo "--------------------------------------------------------------------------------"
$CAT << EOF
Welcome to UCM Installation !!!

This will install UCM in '/ucm' directory on the system.
[You need root privileges for the installation to complete]

EOF
echo -n "Do you want to continue[y/n]" 
read n < /dev/tty
if [ "$n" = "n" -o "$n" = "N" ]; then
	   exit
	else
		 if [ "$n" = "y" -o "$n" = "Y" ]; then
				  echo""
			 else
					echo "Wrong Option"
					exit
			 fi
		fi
 }


rm -rf /$INST_DIR
mkdir /$INST_DIR
mkdir /$INST_DIR/$DEBUG_DIR
mkdir /$INST_DIR/$CONFIG_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$FIREWALL_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$IPDB_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$IPS_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$IPSEC_DIR
mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$BOOT_DIR/



mkdir  /$INST_DIR/lib
if [ $BUILD_MSGSYS = "yes" ]
then
mkdir  /$INST_DIR/lib/mesgclnt
mkdir  /$INST_DIR/lib/logcltr
mkdir  /$INST_DIR/lkms
fi
mkdir  /$INST_DIR/lib/applications
mkdir  /$INST_DIR/lib/htplugin
mkdir  /$INST_DIR/lib/authplugin
mkdir  /$INST_DIR/lib/cp



cp -f obj/ucmd /$INST_DIR/
cp -f obj/tcdmn /$INST_DIR/
cp -f obj/cli /$INST_DIR/
cp -f obj/ucmldsvd /$INST_DIR/
cp -f obj/httpd /$INST_DIR/
if [ $BUILD_MSGSYS = "yes" ]
then
cp -f obj/mesgclntd /$INST_DIR/
cp -f obj/logcltrd /$INST_DIR/
cp -f obj/kernobj/msgsys_lkm.ko /$INST_DIR/lkms
fi
cp -f obj/lib/lib* /$INST_DIR/lib/
cp -f obj/lib/applications/lib* /$INST_DIR/lib/applications/
cp -f obj/lib/htplugin/lib* /$INST_DIR/lib/htplugin/
cp -f obj/lib/authplugin/libucmauthudbutils.so /$INST_DIR/lib/authplugin/
cp -f obj/lib/cp/lib* /$INST_DIR/lib/cp/
if [ $BUILD_MSGSYS = "yes" ]
then
cp -f obj/lib/mesgclnt/lib* /$INST_DIR/lib/mesgclnt/
cp -f obj/lib/logcltr/lib* /$INST_DIR/lib/logcltr/
fi

cp -f obj/lib/applications/lib* /$INST_DIR/lib/applications/
cp -f obj/lib/cp/lib* /$INST_DIR/lib/cp/
cp -rf ../html /$INST_DIR/
cp -rf certs /$INST_DIR/
cp -f config/vsg.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR
cp -f config/acl.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$FIREWALL_DIR
cp -f config/servicedb.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$IPDB_DIR
cp -f config/algptmap.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR/$IPDB_DIR
cp -f config/assoc.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_DIR
cp -f config/bootflag.xml /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/
#rm -rf /$INST_DIR/$CONFIG_DIR/$DEFAULT_CONFIG_DIR/CVS


chmod +x ucm_start_apps.sh
chmod +x ucm_stop_apps.sh
#if [ $BUILD_MSGSYS = "yes" ]; then 
#echo "BUILD_MSGSYS=yes" > ucm_start_apps.sh
#else
#echo "BUILD_MSGSYS=no" > ucm_start_apps.sh
#fi

cp -f ucm_start_apps.sh /$INST_DIR/
cp -f ucm_stop_apps.sh /$INST_DIR/
cp -f obj/lib/libigwssl.so* /$INST_DIR/lib
cp -f obj/lib/libigwcrypto.so* /$INST_DIR/lib
cp -f ../../../opensrc/zlib/lib/libigwzlib.a /$INST_DIR/lib

## Check if the dynamic library path is added to ld.so.conf 
## and add it if not available
if [ `grep -c "\/${INST_DIR}\/lib" /etc/ld.so.conf` -eq 0 ]
then
	   echo "/${INST_DIR}/lib" >>/etc/ld.so.conf
	fi
ldconfig
cp -f obj/lib/mesgclnt/libucm_logmfcbk.so /igateway/lib/syslog/
cp -f obj/lib/logcltr/libucm_logdbcbk.so /igateway/lib/logdb/
