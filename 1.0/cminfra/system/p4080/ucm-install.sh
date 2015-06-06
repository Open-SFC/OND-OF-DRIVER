#!/bin/bash
installlog=/root/log
installerrlog=/root/errlog
Sucmpkg="startucmpkg"
Kucmpkg="stopucmpkg"

CAT=cat
CLEAR=clear

#INST_DIR=$LTIB_INSTALL_PATH/rootfs/ucm
INST_DIR=$1
PRODUCT_TYPE=$2
echo "Target Dir=$1"
#exit 0
DEBUG_DIR=debug
CONFIG_DIR=config
DEFAULT_CONFIG_DIR=config-default
IGD_DIR=igd
SYSTEM_SETTINGS_DIR=systemsettings
GENERAL_DIR=general
VSG_GENERAL_DIR=vsggeneral
FREESCALE_DIR=Freescale
VSG_FREESCALE_DIR=vsgFreescale
FIREWALL_DIR=firewall
TIMEOUTS_DIR=timeouts
IPDB_DIR=netobj
PLATFORM_SERVICES_DIR=platformservices
MESSAGE_CLIENT_DIR=messageclient
ZONES_DIR=zones
IPS_DIR=ips
IPSEC_DIR=ipsec
KERNELVERSION=`uname -r`
KERNEL_MAJOR_VERSION=`uname -r | cut -d. -f1,2`


#test -z "$welcome" && {
#$CLEAR
#echo "--------------------------------------------------------------------------------"
#echo -e "                    \033[32mUCM\033[00m INSTALLATION"
#echo "--------------------------------------------------------------------------------"
#$CAT << EOF
#Welcome to UCM Installation !!!

#This will install UCM for P4080 in current user's ($INST_DIR) directory on the system.

#EOF
#echo -n "Do you want to continue[y/n]" 
#read n < /dev/tty
#if [ "$n" = "n" -o "$n" = "N" ]; then
#           echo "update the \$INST_DIR"
#	   exit
#	else
#		 if [ "$n" = "y" -o "$n" = "Y" ]; then
#				  echo""
#			 else
#					echo "Wrong Option"
#					exit
#			 fi
#		fi
# }


rm -rf $INST_DIR
mkdir $INST_DIR
mkdir $INST_DIR/$DEBUG_DIR
mkdir $INST_DIR/$CONFIG_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$SYSTEM_SETTINGS_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$FIREWALL_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$FIREWALL_DIR/$TIMEOUTS_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$IPDB_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$PLATFORM_SERVICES_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$MESSAGE_CLIENT_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$ZONES_DIR
#mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$IPS_DIR
#mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$IPSEC_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$FIREWALL_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$IPDB_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$PLATFORM_SERVICES_DIR
mkdir $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$MESSAGE_CLIENT_DIR
#mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$IPS_DIR
#mkdir /$INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$IPSEC_DIR



mkdir  $INST_DIR/lib
mkdir  $INST_DIR/lib/applications
mkdir  $INST_DIR/html
mkdir  $INST_DIR/lib/mesgclnt
mkdir  $INST_DIR/lib/logcltr
mkdir  $INST_DIR/lib/htplugin
mkdir  $INST_DIR/lib/authplugin


cp -f obj/ucmd $INST_DIR/
cp -f obj/csd $INST_DIR/
cp -f obj/cli $INST_DIR/
cp -f obj/ucmldsvd $INST_DIR/
cp -f obj/ucmstatcltrd $INST_DIR/
if [ $PRODUCT_TYPE \= "SPUTM" -o $PRODUCT_TYPE \= "ITCL_PCIe" ];then
cp -f obj/httpd $INST_DIR/
fi
#cp -f obj/tcdmn /$INST_DIR/
#cp -f obj/mesgclntd /$INST_DIR/
#cp -f obj/logcltrd /$INST_DIR/
cp -f obj/lib/lib* $INST_DIR/lib/
cp -f obj/lib/applications/*.so $INST_DIR/lib/applications/
cp -f tpl/libedit.so $INST_DIR/lib/libedit.so.0
cp -f tpl/libpq.so.5 $INST_DIR/lib/
#cp -f obj/lib/mesgclnt/lib* /$INST_DIR/lib/mesgclnt/
#cp -f obj/lib/logcltr/lib* /$INST_DIR/lib/logcltr/
#cp -f obj/lib/htplugin/lib* $INST_DIR/lib/htplugin
cp -f obj/lib/authplugin/lib* $INST_DIR/lib/authplugin
if [ $PRODUCT_TYPE \= "SPUTM" -o $PRODUCT_TYPE \= "ITCL_PCIe" ];then
cp -f ../html/* $INST_DIR/html/
mkdir  $INST_DIR/html/images
cp -f ../html/images/* $INST_DIR/html/images/
mkdir  $INST_DIR/html/doc
cp -f ../html/doc/* $INST_DIR/html/doc/
if [ $PRODUCT_TYPE \= "SPUTM" ];then
cp -f ../html/sputm/* $INST_DIR/html/.
fi
fi
if [ $PRODUCT_TYPE \= "SPUTM" -o $PRODUCT_TYPE \= "ITCL_PCIe" ];then
cp -f ../html/pci/welcome.htm $INST_DIR/html/
fi
#cp -rf certs $INST_DIR/
if [ $PRODUCT_TYPE \= "ITCL" ];then
tar -zcf html.tgz ../html/*MetaData.xml ../html/menu.xml ../html/doc
cp -f html.tgz $INST_DIR/
fi

cp -f config/vsg.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR
cp -f config/$SYSTEM_SETTINGS_DIR/datetime.xml  $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$SYSTEM_SETTINGS_DIR

cp -f config/$GENERAL_DIR/acl.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$FIREWALL_DIR
cp -f config/$GENERAL_DIR/$FIREWALL_DIR/$TIMEOUTS_DIR/fwsrvtimeout.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$FIREWALL_DIR/$TIMEOUTS_DIR
cp -f config/$GENERAL_DIR/servicedb.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$IPDB_DIR
cp -f config/$GENERAL_DIR/algptmap.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$IPDB_DIR
cp -f config/$GENERAL_DIR/$PLATFORM_SERVICES_DIR/ipreassembly.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$PLATFORM_SERVICES_DIR
cp -f config/$GENERAL_DIR/$MESSAGE_CLIENT_DIR/mesgclient.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$MESSAGE_CLIENT_DIR
cp -f config/$GENERAL_DIR/$ZONES_DIR/zoneinfo.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_GENERAL_DIR/$ZONES_DIR

cp -f config/$FREESCALE_DIR/acl.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$FIREWALL_DIR
cp -f config/$FREESCALE_DIR/servicedb.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$IPDB_DIR
cp -f config/$FREESCALE_DIR/algptmap.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$IPDB_DIR
#cp -f config/$FREESCALE_DIR/$PLATFORM_SERVICES_DIR/ipreassembly.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$PLATFORM_SERVICES_DIR
#cp -f config/$FREESCALE_DIR/$MESSAGE_CLIENT_DIR/mesgclient.xml $INST_DIR/$DEFAULT_CONFIG_DIR/$IGD_DIR/$VSG_FREESCALE_DIR/$MESSAGE_CLIENT_DIR


chmod +x ucm_start_apps.sh
chmod +x ucm_stop_apps.sh
cp -f ucm_start_apps.sh $INST_DIR/
cp -f ucm_stop_apps.sh $INST_DIR/
cp -f CreateVer.sh $INST_DIR/
cp -f checkversion.sh $INST_DIR/
cp -f custhdrimport.sh $INST_DIR/
cp -f exportcfg.sh $INST_DIR/

## Check if the dynamic library path is added to ld.so.conf 
## and add it if not available
#if [ `grep -c "\/${INST_DIR}\/lib" /etc/ld.so.conf` -eq 0 ]
#then
#	   echo "/${INST_DIR}/lib" >>/etc/ld.so.conf
#	fi
#ldconfig
