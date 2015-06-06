#!/bin/bash

pkill  controller
pkill -9 ofucmd
pkill -9 ofldsvd
pkill -9 ofcli
pkill -9 ofucmd
pkill -9 ofldsvd
pkill -9 ofcli
sleep 3

ps -aef|grep controller
ps -aef|grep ucmd
ps -aef|grep ofldsvd
ps -aef|grep ofcli
