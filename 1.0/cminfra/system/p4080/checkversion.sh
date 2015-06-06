#!/bin/sh

. /$VQ_FOLDER/vq_scripts/vq_env.sh

server_ip=0
user=user
password=user123
config_dir=config
Mgmt_Cfg_dir=/VortiQa-Config
version_file=ConfigVer.txt
tmp_dir=/$FLASH_MOUNT_POINT/tmp
local_mgmver_file=MgmtCardVer.txt
cfg_zip_file=config.tgz
cfg_zip_file_bkp=config-bkp.tgz
#mount_dir=/mnt/vqfs/n710sd
mount_dir=/mnt/vqfs
VortiQa_dir=/VortiQa

getMgmtCard_IP()
{
	 #host_file=/etc/hosts
	#x=`cat $host_file | grep MGMT_IP | cut -d " " -f 1`
	 lease_file=/var/state/dhcp/dhclient.leases
   x=`cat $lease_file | grep dhcp-server-identifier | tail -1| cut -d " " -f 5 | cut -d';' -f 1`
	 if [ -z $x ]; then
			echo "No Mgmt Card IP found"
			server_ip=0
	 else
			server_ip=$x
			echo "Got Management Card's IP while checking config version: $x"
	 fi
}

getFile()
{
	 echo "INFO: Getting File $2/$3 ..."
	 ftp -ni $1 << ftp1_end
	 user $user $password
	 bi 
	 get $2/$3 $4/$5
	 bye
ftp1_end
}


getMgmtCard_IP

if [ $server_ip \= 0 ]; then
	 echo "Not checking version with Management Card"
	 exit 
fi
mkdir $tmp_dir
#rm -f $tmp_dir/$local_mgmver_file
getFile $server_ip $Mgmt_Cfg_dir $version_file $tmp_dir $local_mgmver_file >/dev/null 2>/dev/null

if [ -s $tmp_dir/$local_mgmver_file ]; then
	 echo "Management card has version"
else
	 echo "Configuration Version doesn't exist with Mgmt Card."
	 exit 
fi

version=`cat $tmp_dir/$local_mgmver_file`
local_version=`cat /$config_dir/$version_file`
echo "Management Card config Version $version"
echo "device config version $local_version"

#compare config versions
if [ $version \= $local_version ]; then
         rm -rf $tmp_dir
	 echo "This Device has same configuration version with Management Card"
else
         echo "Moved $mount_dir/$cfg_zip_file to $mount_dir/$cfg_zip_file_bkp"
         mv $mount_dir/$cfg_zip_file  $mount_dir/$cfg_zip_file_bkp
         rm -rf /$config_dir
         rm -rf $mount_dir/$config_dir
         echo "Removed /$config_dir and $mount_dir/$config_dir"
	 getFile $server_ip $Mgmt_Cfg_dir $cfg_zip_file $mount_dir $cfg_zip_file >/dev/null 2>/dev/null
	 cd $mount_dir
	 tar -xzf  $cfg_zip_file
	 mv $tmp_dir/$local_mgmver_file $mount_dir/$config_dir/$version_file
	 y=`cat $mount_dir/$config_dir/$version_file`
#	 cd /
#	 rm -rf $config_dir
#	 rm -rf $mount_dir/ucm_config
#	 mv  $mount_dir/$config_dir $mount_dir/ucm_config
     if ! [ -d /$config_dir ];then
	  cd /
      ln -s $mount_dir/$config_dir /$config_dir
	  cd -
	  echo "Created soft link for $mount_dir/$config_dir to /$config_dir. "        
	 else
      echo "/$config_dir already exists."
	 fi
         rm -rf $tmp_dir
	 cd $VQ_FOLDER
	 echo "Management Card has updated Configuration. Resetting Device with new configuration..."
	 sh /$VQ_FOLDER/reset.sh
fi

