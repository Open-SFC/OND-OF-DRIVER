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
# Date:   02/15/2013
# Description: compilation rules file for modsets

DEPS = $(OBJS:%.o=%.d)

$(OBJDIR)/%.o: %.c
	$(CC) -c -MMD -MP -MF $(MODSET_OBJ_DIR)/$*.d $(CFLAGS) $(LOCAL_INCLUDES) $<  -o $@

#	$(CC) -c -MMD -MP -MF $(MODSET_OBJ_DIR)/$*.d $(CFLAGS) $(LOCAL_INCLUDES) $<  -o $@

$(MODSET_KERNOBJ_DIR)/%.o: %.c
	$(CC) -c -MMD -MP -MF $(MODSET_KERNOBJ_DIR)/$*.d $(KCFLAGS) $(LOCAL_INCLUDES) $<  -o $@



ifneq "$(DEPS)"  ""
-include $(DEPS)
endif

