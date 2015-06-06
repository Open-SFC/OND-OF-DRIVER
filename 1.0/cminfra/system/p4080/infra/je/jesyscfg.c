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
 * File name: jesyscfg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/26/2013
 * Description: 
*/

#ifdef CM_JE_SUPPORT
#include "cmincld.h"
#include "cmdll.h"
#include "jecfg.h"

/******************************************************************************
 * Function Name : cm_je_get_cfgsession_cnf
 * Description   : This API is used to return the maximum Transport channel
 *                 count that can are allowed to configure on the Box.
 * Input params  : NONE
 * Output params : pulUCMCfgSessionTableCount -- Holds the Max System count for
 *                            Transport Channels
 *                 pulUCMCfgSessionTableStcCount -- Holds the Max Transport 
 *                            Channel Static count taken at the Booting time
 *
 * Return value  : NONE
 *****************************************************************************/
void cm_je_get_cfgsession_cnf (uint32_t * pulUCMCfgSessionTableCount,
                              uint32_t * pulUCMCfgSessionTableStcCount,
                              uint32_t * pulUCMCfgSessionMaxThresholdCnt,
                              uint32_t * pulUCMCfgSessionMinThresholdCnt,
                              uint32_t * pulUCMCfgSessionReplinishCnt)
{
  if (pulUCMCfgSessionTableCount)
    *pulUCMCfgSessionTableCount = UCMJE_MAX_CONFIG_SESSIONS;

  if (pulUCMCfgSessionTableStcCount)
    *pulUCMCfgSessionTableStcCount = UCMJE_MAX_STATIC_CONFIG_SESSIONS;

  if (pulUCMCfgSessionMaxThresholdCnt)
    *pulUCMCfgSessionMaxThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 3);

  if (pulUCMCfgSessionMinThresholdCnt)
    *pulUCMCfgSessionMinThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 6);

  if (pulUCMCfgSessionReplinishCnt)
    *pulUCMCfgSessionReplinishCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 10);

  return;
}

/******************************************************************************
 * Function Name : cm_je_get_cfgcache_cnf
 * Description   : This API is used to return the maximum Transport channel
 *                 count that can are allowed to configure on the Box.
 * Input params  : NONE
 * Output params : pulUCMCfgCacheTableCount -- Holds the Max System count for
 *                            Transport Channels
 *                 pulUCMCfgCacheTableStcCount -- Holds the Max Transport 
 *                            Channel Static count taken at the Booting time
 *
 * Return value  : NONE
 *****************************************************************************/
void cm_je_get_cfgcache_cnf (uint32_t * pulUCMCfgCacheTableCount,
                            uint32_t * pulUCMCfgCacheTableStcCount,
                            uint32_t * pulUCMCfgCacheMaxThresholdCnt,
                            uint32_t * pulUCMCfgCacheMinThresholdCnt,
                            uint32_t * pulUCMCfgCacheReplinishCnt)
{
  if (pulUCMCfgCacheTableCount)
    *pulUCMCfgCacheTableCount = UCMJE_MAX_CONFIG_SESSIONS;

  if (pulUCMCfgCacheTableStcCount)
    *pulUCMCfgCacheTableStcCount = UCMJE_MAX_STATIC_CONFIG_SESSIONS;

  if (pulUCMCfgCacheMaxThresholdCnt)
    *pulUCMCfgCacheMaxThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 3);

  if (pulUCMCfgCacheMinThresholdCnt)
    *pulUCMCfgCacheMinThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 6);

  if (pulUCMCfgCacheReplinishCnt)
    *pulUCMCfgCacheReplinishCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 10);

  return;
}

/******************************************************************************
 * Function Name : cm_je_get_cfgrequest_cnf
 * Description   : This API is used to return the maximum Transport channel
 *                 count that can are allowed to configure on the Box.
 * Input params  : NONE
 * Output params : pulUCMCfgRequestTableCount -- Holds the Max System count for
 *                            Transport Channels
 *                 pulUCMCfgRequestTableStcCount -- Holds the Max Transport 
 *                            Channel Static count taken at the Booting time
 *
 * Return value  : NONE
 *****************************************************************************/
void cm_je_get_cfgrequest_cnf (uint32_t * pulUCMCfgRequestTableCount,
                              uint32_t * pulUCMCfgRequestTableStcCount,
                              uint32_t * pulUCMCfgRequestMaxThresholdCnt,
                              uint32_t * pulUCMCfgRequestMinThresholdCnt,
                              uint32_t * pulUCMCfgRequestReplinishCnt)
{
  if (pulUCMCfgRequestTableCount)
    *pulUCMCfgRequestTableCount = UCMJE_MAX_CONFIG_SESSIONS;

  if (pulUCMCfgRequestTableStcCount)
    *pulUCMCfgRequestTableStcCount = UCMJE_MAX_STATIC_CONFIG_SESSIONS;

  if (pulUCMCfgRequestMaxThresholdCnt)
    *pulUCMCfgRequestMaxThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 3);

  if (pulUCMCfgRequestMinThresholdCnt)
    *pulUCMCfgRequestMinThresholdCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 6);

  if (pulUCMCfgRequestReplinishCnt)
    *pulUCMCfgRequestReplinishCnt = (UCMJE_MAX_STATIC_CONFIG_SESSIONS / 10);

  return;
}

/***************************************************************************
 ** Function Name :
 ** Description   :
 ** Input         : NONE
 ** Output        : NONE
 ** Return value  :
 ****************************************************************************/
void JEInstanceIDGeneratorGetCfgInfo (uint32_t * puiMaxRuleIdValue_p)
{
  if (puiMaxRuleIdValue_p)
    *puiMaxRuleIdValue_p = CM_JE_MAX_INSTANCEID_VALUE;
}

#endif /*CM_JE_SUPPORT */
