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
 * File name: dgstmd5.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: 
 * 
 */

void CalculateResponse( unsigned char *passwd,
               unsigned long len,  
    		         unsigned char *realm,
    		         unsigned char *nonce,
    		         unsigned char *ncvalue,
    		         unsigned char *cnonce,
    		         char *qop,
    		         unsigned char *digesturi,
    		         unsigned char *authorization_id,
                         unsigned char  *result,
                         unsigned char  *pusername
			);
char *hmacmd5 (char *text,unsigned long tl,char *key,unsigned long kl);
