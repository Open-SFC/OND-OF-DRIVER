#!/bin/bash
export TARGET_PLATFORM=X86
export SRC_DIR=$PWD
#export JAVA_HOME=/projects/vortiqabuilds_nbk/SDN/jdk1.7.0_17
export JAVA_HOME=/usr/lib/jvm/java-6-openjdk-i386/jre
export PATH=$PATH:$JAVA_HOME/bin
export CLASSPATH=.:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar
export TARGET_HOST=
export CROSS_COMPILE=
export CC=gcc
export CCLD=gcc
export CP=cp
export BSP_DIR=/
export LTIB_DIR=/
export KSRC=/lib/modules/`uname -r`/build
export ARCH=x86
