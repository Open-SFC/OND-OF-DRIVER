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
 * File name: ucmnet.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/29/2013
 * Description: This file contains functions that for converting
 *              network addresses.
 * 
 */

#ifndef _UCMNET_H_
#define _UCMNET_H_

#define CM_SOCK_CONNET_MAX_RETRIES 4
#define CM_INET6_ADDRSTRLEN  (45+1)

uint32_t cm_aton (register char * s);

char *cm_inet_ntoa (uint32_t nta, char * buf);

char *cm_inet_ntoal (uint32_t nta, char * buf);

int32_t cm_htol (char * s);

int32_t cm_itoa (uint32_t uiInt, char * cTmpBuf);

int32_t cm_atoi (const char * nptr);

int32_t cm_ip_is_net_mask (char * s);

int32_t cm_get_host_by_name(char * s, uint32_t * IpAddr);

int32_t cm_ip_is_net_addr (char * s);

int32_t cm_atoni (register char * s, uint32_t * IpAddr);

int32_t cm_val_and_conv_aton (register char * s, uint32_t * IpAddr);

int32_t cm_conv_to_pres_format(char *sequence);

int32_t cm_ipv6_presentation_to_num (unsigned char *ascii_ip_v6_addr_p,
                       struct in6_addr *bin_ip_v6_addr_p);


#endif /* _UCMNET_H_ */
