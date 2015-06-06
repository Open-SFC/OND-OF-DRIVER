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
 * File name: ucmbufdef.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: 
 * 
 */

#ifndef __CM_BUFDEF_H
#define __CM_BUFDEF_H


 inline uint32_t of_mbuf_get_32 (register char * cp)
{
	 uint32_t pp;
	 uint8_t *ctmp = (uint8_t *) cp;

	 pp = 0;

	 pp = ((*ctmp << 24) & 0xFF000000) |
			((*(ctmp + 1) << 16) & 0x00FF0000) |
			((*(ctmp + 2) << 8) & 0x0000FF00) | (*(ctmp + 3) & 0xFF);

	 return (pp);
}

 inline char *of_mbuf_put_32 (char * cp, uint32_t val)
{
	 *cp++ = (val >> 24) & 0xFF;
	 *cp++ = (val >> 16) & 0xFF;
	 *cp++ = (val >> 8) & 0xFF;
	 *cp++ = (val) & 0xFF;

	 return (cp);
}

inline char *of_mbuf_put_64 (char * cp, uint64_t val)
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

inline uint64_t of_mbuf_get_64 (register char * cp)
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


#define  OF_MODU32(a,b) (a%b)
#define  OF_DIVU32(a,b) (a/b)

 int32_t inline of_itoa(uint32_t uiInt ,char* cTmpBuf)
{
	 uint32_t uiRem,uiQuot;
	 int32_t index_i = 0,ii,jj;
	 char cBuf[32];

	 uiQuot = uiInt;
	 do
	 {
			uiRem = (uint32_t)OF_MODU32((uint32_t)uiQuot,(uint32_t)10);
			uiQuot = (uint32_t)OF_DIVU32((uint32_t)uiQuot,(uint32_t)10);
			cBuf[index_i] = uiRem+48;
			index_i++;
	 }while(uiQuot !=0);

	 for(ii=index_i-1,jj=0;ii>=0;ii--,jj++)
	 {
			cTmpBuf[jj] = cBuf[ii];
	 }
	 cTmpBuf[jj] = '\0';
	 return OF_SUCCESS;
}


 int32_t inline of_ltoa(uint64_t ulInt ,char* cTmpBuf)
{
  uint32_t uiRem;
  uint64_t ulQuot;
  int32_t index_i = 0,ii,jj;
  char cBuf[32];

  ulQuot = ulInt;
  do
  {
    uiRem = (uint64_t)OF_MODU32((uint64_t)ulQuot,(uint64_t)10);
    ulQuot = (uint64_t)OF_DIVU32((uint64_t)ulQuot,(uint64_t)10);
    cBuf[index_i] = uiRem+48;
    index_i++;
  }while(ulQuot !=0);

  for(ii=index_i-1,jj=0;ii>=0;ii--,jj++)
  {
    cTmpBuf[jj] = cBuf[ii];
  }
  cTmpBuf[jj] = '\0';
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_inet_ntoa
 * Description   :
      * Convert an internet address (in host byte order) to a dotted
      * decimal ascii string, e.g., 255.255.255.255\0
 * Input         : int32_t  -- binary format internet address
 * Output        : ASCII dotted-decimal Address
 * Return value  : char* -- dotted-decimal Address
 ***************************************************************************/
char *cm_inet_ntoa (uint32_t nta, char * buf)
{
  sprintf ((char *) buf, "%u.%u.%u.%u",
             (uint32_t) nta & 0xff,
             ((uint32_t) nta >> 8) & 0xff,
             ((uint32_t) nta >> 16) & 0xff, ((uint32_t) nta >> 24) & 0xff);
  return buf;
}

/***************************************************************************
 * Function Name : inet_ntoal
 * Description   :
      * Convert an internet address (in host byte order) to a dotted
      * decimal ascii string, e.g., 255.255.255.255\0
 * Input         : int32_t  -- binary format internet address
 * Output        : ASCII dotted-decimal Address
 * Return value  : char* -- dotted-decimal Address
 ***************************************************************************/

char *cm_inet_ntoal (uint32_t nta, char * buf)
{
  sprintf (buf, "%u.%u.%u.%u", ((uint32_t) nta >> 24) & 0xff,
             ((uint32_t) nta >> 16) & 0xff,
             ((uint32_t) nta >> 8) & 0xff, (uint32_t) nta & 0xff);
  return buf;
}

#endif
