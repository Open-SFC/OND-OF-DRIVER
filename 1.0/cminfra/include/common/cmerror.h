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
 * File name: ucmerror.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This header file contains error macro bases for
 *              all the UCM components.
 * 
 */

#ifndef CM_ERROR_H
#define CM_ERROR_H

#define CM_ERROR_BASE          1000

#define CM_JE_ERROR_BASE      (CM_ERROR_BASE + 0)
#define CM_DM_ERROR_BASE       (CM_ERROR_BASE + 1000)
#define CM_LDSV_ERROR_BASE     (CM_ERROR_BASE + 2000)
#define CM_LOG_CLTR_ERROR_BASE     (CM_ERROR_BASE + 3000)
#define CM_LIB_TIMER_ERR_BASE (CM_ERROR_BASE + 4000)
#define CM_COMMON_ERROR_BASE	(CM_ERROR_BASE + 5000)

#define CM_SUCCESS_BASE          10000
#define CM_JE_SUCCESS_BASE      (CM_SUCCESS_BASE + 0)

#define CM_COMMON_NO_KEY_FOR_TABLE (CM_COMMON_ERROR_BASE + 1)
#define CM_COMMON_DM_JE_NO_KEY_FOR_TABLE (CM_COMMON_ERROR_BASE + 2)
#endif
