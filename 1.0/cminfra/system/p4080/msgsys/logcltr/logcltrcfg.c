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
 * File name: logcltrcfg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains code for getting the configuration
 *              for logcltr monitoring timer.
*/

#ifdef CM_LOG_CLTR_SUPPORT
#include "cmincld.h"
//#include "basic_tunable.h"
//#include "logcltrgdef.h"
//#include "logcltrgif.h"

#define CM_LOG_CLTR_MONITOR_TMR_IN_SECS  120  
/****************************************************************************
 * FunctionName  : UCMLogCltrGetMonitorInfo
 * Description   : LogCltr Monitoring period
 * Input         : NONE
 * Output        : Get monitoring period 
 * Return value  : NONE
 ***************************************************************************/
void UCMLogCltrGetMonitorInfo(uint32_t *pulMonTmr_p)
{
  if(pulMonTmr_p)
  {
    *pulMonTmr_p = CM_LOG_CLTR_MONITOR_TMR_IN_SECS;
  }
  return;
}

#endif /* CM_LOG_CLTR_SUPPORT */
