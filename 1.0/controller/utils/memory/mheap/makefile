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

include ../../../../rules.mk

$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/infra

TARGET_LIB = $(SHARED_LIB)/s02-of-cntrl-mheap-infra.so

TARGET = $(BIN_DIR)/ofmheap.o

ifneq (,$(findstring CNTRL_INFRA_LIB_SUPPORT,$(CFLAGS)))
all: $(TARGET) $(TARGET_LIB) 
else
all: $(TARGET)
endif

OBJS = 	$(BIN_DIR)/mheapinit.o\
	$(BIN_DIR)/mheapapi.o

$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

$(TARGET_LIB) : $(TARGET)
	$(CC) -shared -o $@ $(TARGET) -lm

clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(TARGET)
	$(RM) $(OBJS)
       


