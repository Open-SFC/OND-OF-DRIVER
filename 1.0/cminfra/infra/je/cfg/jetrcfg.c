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
 * File name: jetrcfg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/29/2013
 * Description:
*/

#ifdef CM_JE_SUPPORT

#include "cmincld.h"
#include "cmdll.h"
#include "cmdefs.h"
#include "jecfg.h"

/******************************************************************************
 * Function Name : cm_je_get_tnsprtchannel_cnf
 * Description   : This API is used to return the maximum Transport channel
 *                 count that can are allowed to configure on the Box.
 * Input params  : NONE
 * Output params : pulUCMTransChannelTableCount -- Holds the Max System count for
 *                            Transport Channels
 *                 pulUCMTransChannelTableStcCount -- Holds the Max Transport 
 *                            Channel Static count taken at the Booting time
 *
 * Return value  : NONE
 *****************************************************************************/
void cm_je_get_tnsprtchannel_cnf (uint32_t * pulUCMTransChannelTableCount,
                                    uint32_t * pulUCMTransChannelTableStcCount,
                                    uint32_t *
                                    pulUCMTransChannelMaxThresholdCnt,
                                    uint32_t *
                                    pulUCMTransChannelMinThresholdCnt,
                                    uint32_t * pulUCMTransChannelReplinishCnt)
{
  if (pulUCMTransChannelTableCount)
    *pulUCMTransChannelTableCount = UCMJE_MAX_TRANSPORT_CHANNELS;

  if (pulUCMTransChannelTableStcCount)
    *pulUCMTransChannelTableStcCount = UCMJE_MAX_STATIC_TRANSPORT_CHANNELS;

  if (pulUCMTransChannelMaxThresholdCnt)
    *pulUCMTransChannelMaxThresholdCnt =
      (UCMJE_MAX_STATIC_TRANSPORT_CHANNELS / 3);

  if (pulUCMTransChannelMinThresholdCnt)
    *pulUCMTransChannelMinThresholdCnt =
      (UCMJE_MAX_STATIC_TRANSPORT_CHANNELS / 6);

  if (pulUCMTransChannelReplinishCnt)
    *pulUCMTransChannelReplinishCnt =
      (UCMJE_MAX_STATIC_TRANSPORT_CHANNELS / 10);

  return;
}

#endif /* CM_JE_SUPPORT */
