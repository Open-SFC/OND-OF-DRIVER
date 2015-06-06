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
 * File name: flowtblstats.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Flow Table Statistics.
 * 
 */

#ifdef OF_CM_SUPPORT

#include "cbcmninc.h"


struct cm_dm_app_callbacks UCMFlowStatsCallbacks = 
{
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   FLOWTBL_GetFirstNStats,
   FLOWTBL_GetNextNStats,
   FLOWTBL_GetExactStats,
   NULL,
   NULL
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t  of_switch_flowstats_ucmcbk_init(void)
{
   int32_t return_value;

   return_value = cm_register_app_callbacks(CM_ON_DIRECTOR_SWITCH_FLOWSTATS_APPL_ID,
                                               &UCMFlowStatsCallbacks);
   if(return_value != OF_SUCCESS)
   {
      cntrlrucm_debugmsg("Flow stats Objects CBK registration failed");
      return OF_FAILURE;
   }

   return OF_SUCCESS;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Function Name:
 * Description:
 * Input:
 * Output:
 * Result:
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32_t FLOWTBL_GetFirstNStats(struct cm_array_of_iv_pairs * pKeysArr,
      uint32_t * pCount,
      struct cm_array_of_iv_pairs ** ppArrayofIvPairArr)
{
  return OF_SUCCESS;
}

#endif /*OF_CM_SUPPORT */
