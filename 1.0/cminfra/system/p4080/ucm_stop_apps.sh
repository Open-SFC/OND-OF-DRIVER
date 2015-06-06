#!/bin/bash

echo "Stopping icsd..."
killall -9 csd 

echo "Stopping httpd..."
killall -9 httpd 

echo "Stopping ucmldsvd..."
killall -9 ucmldsvd

echo "Stopping ucmd..."
killall -9 ucmd


echo "Stopping message client"
killall -9 mesgclntd


echo "Stopping log cltr daemon"
killall -9 logcltrd

echo "Stopping stats cltr daemon"
killall -9 ucmstatcltrd 

rm -f /VortiQa/ucm/debug/jedebug*
rm -f /VortiQa/ucm/debug/csddebug*


