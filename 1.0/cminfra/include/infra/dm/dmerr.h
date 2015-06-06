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
 * File name: dmerr.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/26/2013
 * Description: This file contains error number declarations of
 *              config middle-ware data model.
 * 
 */

#ifndef CM_DM_ERR_H
#define CM_DM_ERR_H

#define CM_DM_ERR_INVALID_INPUTS        (CM_DM_ERROR_BASE + 0)
#define CM_DM_ERR_NAME_LEN_EXCEEDED     (CM_DM_ERROR_BASE + 1)
#define CM_DM_ERR_DUPLICATE_NODE        (CM_DM_ERROR_BASE + 2)
#define CM_DM_ERR_PATH_DOES_NOT_EXISTS  (CM_DM_ERROR_BASE + 3)
#define CM_DM_ERR_MEM_RESOURCES         (CM_DM_ERROR_BASE + 4)
#define CM_DM_ERR_PATH_LEN_EXCEEDS      (CM_DM_ERROR_BASE + 5)
#define CM_DM_ERR_INVALID_ELEMENT       (CM_DM_ERROR_BASE + 6)

#endif /* DM_ERR_H */
