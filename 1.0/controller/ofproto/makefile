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

include ../../rules.mk

all: build

maketest: 
	make -w -C $(ofproto_dir)/test
makesrc: 
	make -w -C $(ofproto_dir)/src

build: maketest makesrc

clean:
	make -C $(ofproto_dir)/src clean
	make -C $(ofproto_dir)/test clean
