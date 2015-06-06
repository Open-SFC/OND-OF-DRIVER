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
 * File name: trace.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/23/2013
 * Description: 
 * 
 */

#ifndef _TRACE_H
#define _TRACE_H

#define Trace(id, level, format,args...)
#define Trace_1(id, level, format,args...)
#define Trace_2(id, level, format,args...)
#define Trace_3(id, level, format,args...)
#define Trace_4(id, level, format,args...)
enum{
	 TRACE_SEVERE,
	 TRACE_WARNING,
	 TRACE_INFO
}Trace_e;

#endif
