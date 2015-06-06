
# Copyright  2012, 2013  Freescale Semiconductor
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.

# File name:  makefile
# Author: Freescale Semiconductor    
# Description:

include ../rules.mk

all: build

makeutils:
	make -w -C $(utils_dir)
makeapps:
	make -w -C $(ofapps_dir)
makecli:
#	make -w -C $(ofcli_dir)
makel3utils:
	make -w -C $(l3extutils_dir)
makeofproto: 
	make -w -C $(ofproto_dir)
makepethapp: 
#	make -w -C $(pethapp_dir)
makekern: 
#	make -w -C $(cntrlkern_dir)
makexml: 
	make -w -C $(ofxml_dir)
        
build: makeutils makecli makel3utils makepethapp makexml makeofproto makekern
#build: makeutils makeapps makel3utils makeofproto

clean:
	make -C $(l3extutils_dir) clean
	make -C $(ofproto_dir) clean
	make -C $(utils_dir) clean
	make -C $(ofcli_dir) clean
#	make -C $(pethapp_dir) clean
#	make -C $(cntrlkern_dir) clean
	make -C $(ofxml_dir) clean

