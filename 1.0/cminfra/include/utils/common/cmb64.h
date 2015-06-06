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
 * File name: ucmb64.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

#ifndef _CM_BASE64_H_
#define _CM_BASE64_H_


void cm_base_64_encode(const unsigned char *in, int32_t inlen,unsigned char *out, int32_t *outlen);

int32_t cm_base_64_decode(unsigned char *pIn, int32_t iInlen,unsigned char *pOut, int32_t *pOutlen);


#endif  /*#ifndef _CM_BASE64_H__*/
