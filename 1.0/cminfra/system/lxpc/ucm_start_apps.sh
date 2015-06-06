#!/bin/bash

sysctl -w net.core.wmem_default=65535

INST_DIR=ucm

if [ `grep -c "\/${INST_DIR}\/lib" /etc/ld.so.conf` -eq 0 ]
then
     echo "/${INST_DIR}/lib" >>/etc/ld.so.conf
  fi
ldconfig


test  -c /dev/msgsysdrv       || /bin/mknod /dev/msgsysdrv      c 126 0

if test -e lkms/msgsys_lkm.ko
then
/sbin/insmod lkms/msgsys_lkm.ko
fi

echo "Running LDSV..."
/$INST_DIR/ucmldsvd &

if test -e /$INST_DIR/mesgclntd
then	 
echo "Running Message Client..."
/$INST_DIR/mesgclntd &
fi

if test -e /$INST_DIR/logcltrd
then	 
echo "Running Log Cltr Daemon..."
/$INST_DIR/logcltrd &
fi

echo "Running UCM..."
/$INST_DIR/ucmd  -f &

echo "Running HTTPD..."
/$INST_DIR/httpd &

