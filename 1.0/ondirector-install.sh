#!/bin/bash

TARGET_DIR=/ondirector
TARGET_LIB_DIR=$TARGET_DIR/lib
TARGET_BIN_DIR=$TARGET_DIR/bin
TARGET_HTML_DIR=$TARGET_DIR/html

rm -rf $TARGET_DIR
mkdir $TARGET_DIR
mkdir  $TARGET_LIB_DIR
mkdir  $TARGET_BIN_DIR
mkdir  $TARGET_HTML_DIR
mkdir $TARGET_DIR/debug
mkdir $TARGET_DIR/conf
mkdir $TARGET_DIR/app-conf
mkdir $TARGET_DIR/certs

cp ondirector-start.sh $TARGET_DIR/
cp ondirector-stop.sh $TARGET_DIR/
cp -rf bin/lib/*  $TARGET_LIB_DIR/
cp -f certs/*.pem  $TARGET_DIR/certs/
cp -rf conf/*  $TARGET_DIR/conf/
cp -rf html/*  $TARGET_DIR/html/
cp -f bin/controller  $TARGET_BIN_DIR
cp -f bin/ofcli  $TARGET_BIN_DIR
cp -f bin/ofucmd  $TARGET_BIN_DIR
cp -f bin/ofldsvd  $TARGET_BIN_DIR
#cp -f bin/httpd  $TARGET_BIN_DIR

echo "Completed installing ON Director to the folder /ondirector"
echo "To start ON Director, execute ondirector-start.sh in /ondirector"
