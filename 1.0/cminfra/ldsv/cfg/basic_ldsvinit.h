/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name:   basic_ldsvinit.h
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: LDSV engine specific local structures and definitions. 
*/

#ifndef CM_LDSV_SYSLDEF_H
#define CM_LDSV_SYSLDEF_H

#ifdef OF_CM_CNTRL_SUPPORT
#define PERSISTENT_TRANSFERLIST_DIR "/usr/local/ond_of_driver/1.0/conf/"
#define PERSISTENT_TRANSFERLIST_DEFAULT_DIR "/usr/local/ond_of_driver/1.0/conf/"
#endif

#ifdef OF_CM_SWITCH_CBK_SUPPORT 
#define PERSISTENT_TRANSFERLIST_DIR "/psp/conf/"
#define PERSISTENT_TRANSFERLIST_DEFAULT_DIR "/psp/conf/"
#endif

#define CM_CONFIGSYNC_CONFIG_VER_FILE  "ConfigVer.txt"
#define SAVE_CONFIG_TO_FLASH_DIR "/mnt/vqfs/"

#endif

