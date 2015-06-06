#!/bin/bash

echo "Stopping httpd..."
killall -9 httpd 

echo "Stopping ucmldsvd..."
killall -9 ucmldsvd

echo "Stopping ucmd..."
killall -9 ucmd

if test -e ./mesgclntd
then	 
echo "Stopping message clinet"
killall -9 mesgclntd
fi

if test -e ./logcltrd
then	 
echo "Stopping log cltr daemon"
killall -9 logcltrd
fi

rm -f /ucm/debug/jedebug*

if test -e lkms/msgsys_lkm.ko
then
/sbin/rmmod msgsys_lkm
fi

