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
 * File name: mbufutil.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: This file contains the buffer management scheme, mbuf, related
 *              global and local functions.
 * 
 */

#include "cmincld.h"

/*
 ******************** Function Definition Section **********************
*/

/***************************************************************************
 * Function Name : of_mbuf_get_16
 * Description   :
     * Machine-independent, alignment insensitive network-to-host short
     * conversion
 * Input         : Pointer to the character buffer
 * Output        : None
 * Return value  : The 16-bit integer in host format
 ***************************************************************************/
uint16_t of_mbuf_get_16 (register char * cp)
{
  uint16_t pp;
  uint8_t *ctmp = (uint8_t *) cp;

  pp = 0;
  pp = ((*ctmp << 8) & 0xFF00) | (*(ctmp + 1) & 0xFF);
                           /** SRINI SEP 11 **/

  return (pp);
}

/***************************************************************************
 * Function Name : of_mbuf_get_32
 * Description   :
     * Machine-independent, alignment insensitive network-to-host long
     * conversion
 * Input         : Pointer to the character buffer
 * Output        : None
 * Return value  : The 32-bit integer in host format
 ***************************************************************************/
uint32_t of_mbuf_get_32 (register char * cp)
{
  uint32_t pp;
  uint8_t *ctmp = (uint8_t *) cp;

  pp = 0;

  pp = ((*ctmp << 24) & 0xFF000000) |
    ((*(ctmp + 1) << 16) & 0x00FF0000) |
    ((*(ctmp + 2) << 8) & 0x0000FF00) | (*(ctmp + 3) & 0xFF);

  return (pp);
}

uint64_t of_mbuf_get_64 (register char * cp)
{
  uint32_t pp1, pp2;
  uint64_t ulResult;

  pp1 =pp2= 0;
  ulResult=0;
  pp1= of_mbuf_get_32(cp);
  pp2= of_mbuf_get_32(cp+4);

  ulResult= pp1 & 0xFFFFFFFF;
	ulResult = ulResult << 32;
  ulResult |= (pp2 & 0xFFFFFFFF);

  return (ulResult);
}
/***************************************************************************
 * Function Name : of_mbuf_put_16
 * Description   :
     * Machine-independent, alignment insensitive host-to-network short
     * conversion
 * Input         : uint16_t  -- 16-bit integer 
 * Output        : char*  -- Updated buffer
 * Return value  : char*  -- buffer updated with the bytes of the integer
 ***************************************************************************/
char *of_mbuf_put_16 (char * cp, uint16_t val)
{
  *cp++ = (val >> 8) & 0xFF;
  *cp++ = val & 0xFF;
  return (cp);
}

/***************************************************************************
 * Function Name : of_mbuf_put_32
 * Description   :
     * Machine-independent, alignment insensitive host-to-network long
     * conversion
 * Input         : uint32_t  -- 32-bit integer 
 * Output        : char*  -- Updated buffer
 * Return value  : char*  -- buffer updated with the bytes of the integer
 ***************************************************************************/
char *of_mbuf_put_32 (char * cp, uint32_t val)
{
  *cp++ = (val >> 24) & 0xFF;
  *cp++ = (val >> 16) & 0xFF;
  *cp++ = (val >> 8) & 0xFF;
  *cp++ = (val) & 0xFF;

  return (cp);
}
char *of_mbuf_put_64 (char * cp, uint64_t val)
{

	char *pTmp;
	uint32_t pp1, pp2;

	pTmp=cp;
	pp1 = (val & 0xFFFFFFFF00000000)>>32 ;
	pp2 = (val & 0x00000000FFFFFFFF); 
	pTmp = of_mbuf_put_32(pTmp, pp1);
	pTmp = of_mbuf_put_32(pTmp, pp2);

	return (pTmp);
}
