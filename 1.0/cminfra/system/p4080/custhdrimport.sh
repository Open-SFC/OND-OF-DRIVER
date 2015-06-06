#!/bin/sh

. /$VQ_FOLDER/vq_scripts/vq_env.sh

server_ip=0
user=user
password=user123
Mgmt_Cfg_dir=/VortiQa-CustHdr
custhdr_file=custhdr.xml
tmp_dir=/$FLASH_MOUNT_POINT
local_custhdr_file=custhdr.xml
VortiQa_dir=/VortiQa

getMgmtCard_IP()
{
	 #host_file=/etc/hosts
	#x=`cat $host_file | grep MGMT_IP | cut -d " " -f 1`
	 lease_file=/var/state/dhcp/dhclient.leases
   x=`cat $lease_file | grep dhcp-server-identifier | cut -d " " -f 5 | cut -d';' -f 1`
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
	 echo "Not getting custom header file from  Management Card"
	 exit 
fi

getFile $server_ip $Mgmt_Cfg_dir $custhdr_file $tmp_dir $local_custhdr_file >/dev/null 2>/dev/null
echo "file importing"
if [ -s $tmp_dir/$local_custhdr_file ]; then
	 echo "Custom Header file is successfully imported."
         cp $tmp_dir/$local_custhdr_file $VortiQa_dir/
else
	 echo "Custom Header file import failed."
	 exit 
fi
