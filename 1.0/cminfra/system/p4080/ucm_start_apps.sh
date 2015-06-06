#!/bin/bash

sysctl -w net.core.wmem_default=65535

#INST_DIR=`pwd`
INST_DIR=/igateway/ucm
#cp /igateway/ucm/lib/* /igateway/lib/

export LD_LIBRARY_PATH=/psp/lib:/igateway/lib:/igateway/ucm/lib

echo "INST_DIR is ........$INST_DIR....."

#kill the default http server on p4080 (boa)
killall boa
killall -9 httpd

#cp -f /$INST_DIR/lib/lib* /usr/lib
#ls $PWD/lib/ | while read i
#do
 # test -f /usr/lib/$i || ln -s $PWD/lib/$i /usr/lib/$i
#done

if [ -f $INST_DIR/csd ]; then 
echo "Running CSD..."
rm -f $INST_DIR/debug/csddebug*
$INST_DIR/csd &
fi

echo "Running LDSV..."
rm -f $INST_DIR/debug/ldsvdebug*
$INST_DIR/ucmldsvd &

# Following mesgclntd, logcltrd and httpd can be enabled 
# at the time of support.

#echo "Running Message Cliet..."
#$INST_DIR/mesgclntd &

#echo "Running Log Cltr Daemon..."
#$INST_DIR/logcltrd & 
sleep 2
echo "Running UCM..."
rm -f $INST_DIR/debug/jedebug*
$INST_DIR/ucmd &

if [ -f $INST_DIR/ucmstatcltrd ]; then 
echo "Running stats collector daemon"
$INST_DIR/ucmstatcltrd &
fi

if [ -f $INST_DIR/httpd ]; then 
echo "Running HTTPD..."
$INST_DIR/httpd &
fi
