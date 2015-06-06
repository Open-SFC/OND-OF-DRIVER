#!/bin/sh

server_ip=0
user=user
password=user123
config_dir=/config
Mgmt_Cfg_dir=/VortiQa-Config
config_zip_file=config.tgz
config_backupzip_file=config-backup.tgz
version_file=ConfigVer.txt

getMgmtCard_IP()
{
	 host_file=/etc/hosts
	 x=`cat $host_file | grep MGMT_IP | cut -d " " -f 1`
	 if [ -z $x ]; then
			echo "No Mgmt Card IP found"
	 else
			server_ip=$x
			echo "Got Management Card's IP: $x"
	 fi
}

putFile()
{
	 echo "Copying File  ..."
	 ftp -ni $server_ip << ftp1_end
	 user $user $password
	 bi
	 cd $Mgmt_Cfg_dir
	 delete $config_backupzip_file 
	 delete $version_file 
	 rename $config_zip_file $config_backupzip_file
	 put $config_dir/$version_file $version_file 
	 put /$config_zip_file $config_zip_file 
	 bye
ftp1_end
}

getMgmtCard_IP
putFile

	 exit 0
