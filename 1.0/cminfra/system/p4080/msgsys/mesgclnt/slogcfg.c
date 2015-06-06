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
 * File name: slogcfg.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains code for getting the configuration
 *              for syslog buffer pool.
*/

#ifdef CM_MESGCLNT_SUPPORT
#include "cmincld.h"
//#include "basic_tunable.h"

/*
** The following are only initial values
** we should be able to calculate these
** values based on the various modules present
** in the build 
 */
#define CM_SYSLOG_BUF_MAX_TOTAL_COUNT 256
#define CM_SYSLOG_BUF_MAX_STATIC_COUNT 128

/*
 *  * Maximum number of Issue Ids per Message Id.
 *   */
#define CM_MSGS_MAX_ISSUEID_PER_MSGID 20
/* 
 *  * Default Throttling count
 *   */
#define CM_MSGS_THROTT_COUNT 1

/* 
 *  * Default Throttling time in seconds
 *   */
#define CM_MSGS_THROTT_TIME 300

/*
 *  * Maximum Threshold Q length.
 *   */
#define CM_MSGS_MAX_Q_THRESHOLD     75

/*
 *  * Maximum Threshold time in minutes.
 *   */
#define CM_MSGS_MAX_TIME_THRESHOLD  60 

/****************************************************************************
 * FunctionName  : UCMSysLogGetPoolInfo
 * Description   : Maximum number of Syslog Buffers and 
	*                 Maximum number of Static Buffers Info.
 * Input         : NONE
 * Output        : Get Maximum Counts
 * Return value  : NONE
 ***************************************************************************/
void UCMSysLogGetPoolInfo(uint32_t *pulMaxBufCnt,
                             uint32_t *pulMaxStcBufCnt)
{
  if(pulMaxBufCnt)
  {
    *pulMaxBufCnt   = CM_SYSLOG_BUF_MAX_TOTAL_COUNT;
  }

  if(pulMaxStcBufCnt)
  {
    *pulMaxStcBufCnt = CM_SYSLOG_BUF_MAX_STATIC_COUNT;
  }
  return;
}

/****************************************************************************
 * FunctionName  : UCMMSGSGetMaxIssuesPerMSGId
 * Description   : Maximum number of Issues per messags ID.
 * Input         : NONE
 * Output        : Get Maximum issue count
 * Return value  : NONE
 ***************************************************************************/
void UCMMSGSGetMaxIssuesPerMSGId(uint32_t *pulMaxIssueCnt)
{
  if(pulMaxIssueCnt)
  {
    *pulMaxIssueCnt   = CM_MSGS_MAX_ISSUEID_PER_MSGID;
  }

  return;
}

/****************************************************************************
 * FunctionName  : UCMMSGSGetDefaultThrtParams
 * Description   : Get Default Throttling count and time.
 * Input         : NONE
 * Output        : Throttling count and time.
 * Return value  : NONE
 ***************************************************************************/
void UCMMSGSGetDefaultThrtParams(uint32_t *pulThrtCnt_p,uint32_t *pulThrtTime_p)
{
  if(pulThrtCnt_p)
  {
    *pulThrtCnt_p   = CM_MSGS_THROTT_COUNT;
  }
  if(pulThrtTime_p)
  {
    *pulThrtTime_p   = CM_MSGS_THROTT_TIME;
  }

  return;
}
#if 0
/****************************************************************************
 * FunctionName  : UCMMSGSGetMaxThresQandTime
 * Description   : Maximum Threshold Q length and Threshold time.
 * Input         : NONE
 * Output        : Threshold Q and time
 * Return value  : NONE
 ***************************************************************************/
void UCMMSGSGetMaxThresQandTime(uint16_t *pusMaxThresQLen, uint16_t *pusMaxThresTime)
{
  if(pusMaxThresQLen)
  {
    *pusMaxThresQLen   = CM_MSGS_MAX_Q_THRESHOLD;
  }
  
  if(pusMaxThresTime)
  {
    *pusMaxThresTime = CM_MSGS_MAX_TIME_THRESHOLD;
  }

  return;
}
#endif
#endif /*CM_MESGCLNT_SUPPORT*/
