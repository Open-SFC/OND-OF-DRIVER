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
 * File name: mesgprod.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/26/2013
 * Description: 
*/

#ifdef CM_MESGCLNT_SUPPORT
#include "cmincld.h"
#include "ucmatmic.h"
#include "mesggdef.h"

#ifdef CM_SYSLOG_SCHEDULE
 UCMLogSchedule_t ConfigSchedule = {
                                          CM_LOG_SCHEDULE_HOURLY
                                          };
#endif

#if 0
TODO something
void UCMLogWrite(char* Buf)
{
#ifdef OF_HTTPD_SUPPORT
  LogWrite(Buf);
#endif /* OF_HTTPD_SUPPORT */
}
#endif

#endif /* CM_MESGCLNT_SUPPORT */

