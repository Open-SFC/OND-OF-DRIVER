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
 * File name: jecfg.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/26/2013
 * Description: 
*/

#ifndef _JECFG_H_
#define _JECFG_H_

#define UCMJE_MAX_CONFIG_SESSIONS     20 
#define UCMJE_MAX_STATIC_CONFIG_SESSIONS (UCMJE_MAX_CONFIG_SESSIONS/2)

#define  UCMJE_MAX_TRANSPORT_CHANNELS 20
#define UCMJE_MAX_STATIC_TRANSPORT_CHANNELS (UCMJE_MAX_TRANSPORT_CHANNELS/2)

#define CM_JE_MAX_INSTANCEID_VALUE 10000


void cm_je_get_cfgsession_cnf(uint32_t * pulUCMCfgSessionTableCount,
                         uint32_t * pulUCMCfgSessionTableStcCount,
                         uint32_t * pulUCMCfgSessionMaxThresholdCnt,
                         uint32_t * pulUCMCfgSessionMinThresholdCnt,
                         uint32_t * pulUCMCfgSessionReplinishCnt);



void cm_je_get_cfgcache_cnf(uint32_t * pulUCMCfgCacheTableCount,
                         uint32_t * pulUCMCfgCacheTableStcCount,
                         uint32_t * pulUCMCfgCacheMaxThresholdCnt,
                         uint32_t * pulUCMCfgCacheMinThresholdCnt,
                         uint32_t * pulUCMCfgCacheReplinishCnt);


void cm_je_get_cfgrequest_cnf(uint32_t * pulUCMCfgRequestTableCount,
                         uint32_t * pulUCMCfgRequestTableStcCount,
                         uint32_t * pulUCMCfgRequestMaxThresholdCnt,
                         uint32_t * pulUCMCfgRequestMinThresholdCnt,
                         uint32_t * pulUCMCfgRequestReplinishCnt);

void cm_je_get_tnsprtchannel_cnf(uint32_t * pulUCMTransChannelTableCount,
                         uint32_t * pulUCMTransChannelTableStcCount,
                         uint32_t * pulUCMTransChannelMaxThresholdCnt,
                         uint32_t * pulUCMTransChannelMinThresholdCnt,
                         uint32_t * pulUCMTransChannelReplinishCnt);

void JEInstanceIDGeneratorGetCfgInfo(uint32_t *puiMaxRuleIdValue_p);
#endif /* _JECFG_H_ */
