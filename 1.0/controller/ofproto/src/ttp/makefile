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

TARGET = $(BIN_DIR)/ttp.o

all: $(TARGET) 


OBJS = 	$(BIN_DIR)/ttpapi.o\
	$(BIN_DIR)/sd_router_ttp.o\
	$(BIN_DIR)/dc_vs_ttp.o\
	$(BIN_DIR)/l3_fwd_ttp.o\
	$(BIN_DIR)/l2_fwd_ttp.o\
	$(BIN_DIR)/cbench_ttp.o\
	$(BIN_DIR)/ttp_init.o\
	



$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
       

#OBJS = 	$(BIN_DIR)/crmapi.o\

