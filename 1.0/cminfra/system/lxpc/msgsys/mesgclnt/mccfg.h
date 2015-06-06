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
 * File name: mccfg.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/26/2013
 * Description: This file contains code for getting the configuration
 *              for syslog buffer pool.
*/

#ifndef __MCCFG_H__
#define __MCCFG_H__

void UCMSysLogGetPoolInfo(uint32_t *pulMaxBufCnt,
                             uint32_t *pulMaxStcBufCnt);

void UCMMSGSGetMaxIssuesPerMSGId(uint32_t *pulMaxIssueCnt);

void UCMMSGSGetDefaultThrtParams(uint32_t *pulThrtCnt_p,uint32_t *pulThrtTime_p);


#endif /* __MCCFG_H__ */
