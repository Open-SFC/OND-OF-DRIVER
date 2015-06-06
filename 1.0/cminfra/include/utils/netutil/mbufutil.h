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
 * File name: mbufutil.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains the buffer management scheme related
 *              global and local functions.
 * 
 */

uint16_t of_mbuf_get_16 (register char * cp);

uint32_t of_mbuf_get_32 (register char * cp);

uint64_t of_mbuf_get_64 (register char * cp);

char *of_mbuf_put_16 (char * cp, uint16_t val);

char *of_mbuf_put_32 (char * cp, uint32_t val);

char *of_mbuf_put_64 (char * cp, uint64_t val);
