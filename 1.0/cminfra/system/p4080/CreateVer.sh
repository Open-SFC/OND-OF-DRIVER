#!/bin.sh
#. /$VQ_FOLDER/scripts/vq_scripts/vq_env.sh
. /igateway/scripts/vq_scripts/vq_env.sh
#PROD=iTCM
#version_file=VortiQa-$PROD-version.txt
#CFG_FOLDER=/config
ucmldsvcfgversionfile=ConfigVer.txt

if ! [ -d /$FLASH_MOUNT_POINT/$CFG_FOLDER ]; then
mkdir /$FLASH_MOUNT_POINT/$CFG_FOLDER
echo "Created /$FLASH_MOUNT_POINT/$CFG_FOLDER for UCM configuration "
fi

if ! [ -d /$CFG_FOLDER ]; then
echo "Creating soft link for /$CFG_FOLDER with /$FLASH_MOUNT_POINT/$CFG_FOLDER"
cd /
ln -s /$FLASH_MOUNT_POINT/$CFG_FOLDER /$CFG_FOLDER
cd -
else
echo "/$CFG_FOLDER already exists"
fi


if ! [ -f /$CFG_FOLDER/$ucmldsvcfgversionfile ]; then 
x=`cat $VQ_FOLDER/$version_file | grep "Config Version" | cut -d ":" -f 2`
x=`echo $x | sed s/" "/""/g`
echo $x >>/$CFG_FOLDER/$ucmldsvcfgversionfile
fi
