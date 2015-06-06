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
 * File name: ucmnet.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: This file contains functions that for converting
 *              network addresses.
 * 
 */

#include "cmincld.h"
#include "cmutil.h"

int32_t validate_string_with_out_xml_chars(char* name_p, int32_t iLen);
/***************************************************************************
 * Function Name : cm_aton
 * Description   :
     * Convert Internet address in ascii dotted-decimal format (44.0.0.1)
     * to binary IP address
 * Input         : char*   -- dotted-decimal address to be converted
                                 to binary format
 * Output        : Binary Address
 * Return value  : Binary Address or OF_FAILURE
 ***************************************************************************/
uint32_t cm_aton (register char * s)
{
  uint32_t n;

  register int32_t i;
  int32_t k;

  n = 0;
  if (s == NULLCHAR)
    return OF_FAILURE;

  for (i = 24; i >= 0; i -= 8)
  {
    /* Skip any leading stuff (e.g., spaces, '[') */
    while (*s != '\0' && !isdigit (*s))
      s++;

    if (*s == '\0')
      return OF_FAILURE;

    k = (int32_t) atoi (s);
    if (k < 0 || 255 < k)
      return OF_FAILURE;

    n |= k << i;
    if (((s = of_strchr (s, '.')) == NULLCHAR) && (i > 0))
      return OF_FAILURE;

    if (i == 0 && (s != NULLCHAR))
      return OF_FAILURE;

    s++;
  }

  return n;
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

/***************************************************************************
 * Function Name : cm_htol
 * Description   :
     * Convert hex-ascii string to long integer
 * Input         : char*  -- Hex ASCII string
 * Output        : None
 * Return value  : int32_t   -- long integer corr to string
 ***************************************************************************/

int32_t cm_htol (char * s)
{
  int32_t ret;
  char c;

  ret = 0;
  while ((c = *s++) != '\0')
  {
    c &= 0x7f;
    if (c == 'x')
      continue;                 /* Ignore 'x', e.g., '0x' prefixes */

    if (c >= '0' && c <= '9')
      ret = ret * 16 + (c - '0');
    else if (c >= 'a' && c <= 'f')
      ret = ret * 16 + (10 + c - 'a');
    else if (c >= 'A' && c <= 'F')
      ret = ret * 16 + (10 + c - 'A');
    else
      break;
  }

  return ret;
}

/***************************************************************************
 * Function Name : cm_itoa
 * Description   :
 * Converts the integer to string
 * Input         : Integer to be converted
 * Output        : character buffer containing the string
 * Return value  : int32_t      -- buffer containing address and port
 ***************************************************************************/

int32_t cm_itoa (uint32_t uiInt, char * cTmpBuf)
{
  uint32_t uiRem, uiQuot;
  int32_t index_i = 0, ii, jj;
  char cBuf[32];
  if (uiInt < 0)
    return OF_FAILURE;

  uiQuot = uiInt;
  do
  {
    uiRem = (uint32_t) CM_MODU32 ((uint32_t) uiQuot, (uint32_t) 10);
    uiQuot = (uint32_t) CM_DIVU32 ((uint32_t) uiQuot, (uint32_t) 10);
    cBuf[index_i] = uiRem + 48;
    index_i++;
  } while (uiQuot != 0);
  for (ii = index_i - 1, jj = 0; ii >= 0; ii--, jj++)
  {
    cTmpBuf[jj] = cBuf[ii];
  }
  cTmpBuf[jj] = '\0';
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_atoi
 * Description   :
 * Converts the string to integer
 * Input         : String to be converted
 * Return value  : int32_t      -- Converted integer.
 ***************************************************************************/
int32_t cm_atoi (const char * nptr)
{
  return (int32_t) atoi (nptr);
}

/***************************************************************************
 * Function Name : cm_ip_is_net_mask
 * Description   :
 * Input         : 
 * Return value  : 
 ***************************************************************************/
int32_t cm_ip_is_net_mask (char * s)
{
  uint32_t i, l;
  uint32_t nmask;
  char *Temp;

  Temp = s;
  if (cm_ip_is_net_addr (Temp) == FALSE)
    return FALSE;
  if (cm_atoni (s, &nmask) == OF_SUCCESS)
  {
    i = (uint32_t) nmask;
    l = (~i) + 1;
    if (l & (~i))
      return FALSE;
    else
      return TRUE;
  }
  return FALSE;
}

/***************************************************************************
 * Function Name : cm_get_host_by_name
 * Description   :
 * Input         : char*  -- pointer to string to be evaluated
 * Output        : None
 * Return value  : OF_SUCCESS if valid host name; OF_FAILURE otherwise
 ***************************************************************************/

int32_t cm_get_host_by_name(char * s, uint32_t * IpAddr)
{
  struct hostent *pHostAddr;
  /*struct in_addr *address;*/
  uint32_t addr = 0;

  pHostAddr = gethostbyname (s);
  if(!pHostAddr)
  {
    return OF_FAILURE;
  }
  /*address = (struct in_addr *)pHostAddr->h_addr;
  addr = ntohl(address->s_addr);*/
  addr = *(uint32_t *)pHostAddr->h_addr;
  addr = ntohl(addr);

  *IpAddr = addr;

  return OF_SUCCESS;
}
/***************************************************************************
 * Function Name : IpIsNetAddr
 * Description   :
 * Input         : char*  -- pointer to string to be evaluated
 * Output        : None
 * Return value  : int16_t -- TRUE if IP addr; FALSE otherwise
 ***************************************************************************/

int32_t cm_ip_is_net_addr (char * s)
{
  char c;
  char prev = '#';
  int8_t sNumOfDots = 0;
  int8_t sNumOfDigits = 0;
  unsigned char bFourthByte = FALSE;

  if (s == NULLCHAR)
    return FALSE;             /* Can't happen */
  c = *s;
  if (c == '.')
    return FALSE;
  while ((c = *s++) != '\0')
  {
    if (!isdigit ((int32_t) c) && c != '.')
      return FALSE;

    switch (prev)
    {
      case '.':
        switch (c)
        {
          case '.':
            return FALSE;
          default:
            break;
        }
        break;
      default:
        break;
    }

    if (sNumOfDots == 3)
      bFourthByte = TRUE;

    sNumOfDigits++;
    if (c == '.')
    {
      sNumOfDigits = 0;
      sNumOfDots++;
    }
    if (sNumOfDigits > 3)
    {
       return FALSE;
    }
    prev = c;
  }

  if ((sNumOfDots == 3) && bFourthByte)
    return TRUE;
  else
    return FALSE;
}

/***************************************************************************
 * Function Name : cm_atoni
 * Description   :
     * Convert Internet address in ascii dotted-decimal format (44.0.0.1)
     * to binary IP address
 * Input         : char*   -- dotted-decimal address to be converted
                                 to binary format
 * Output        : uint32_tr*   -- pointer to uint32_tess where the coverted
                                 address is to be placed.
 * Return value  : OF_SUCCESS or OF_FAILURE
 ***************************************************************************/

int32_t cm_atoni (register char * s, uint32_t * IpAddr)
{
  uint32_t n;

  register int32_t i;
  int32_t k;

  n = 0;
  if (s == NULLCHAR)
    return OF_FAILURE;

  for (i = 24; i >= 0; i -= 8)
  {
    /* Skip any leading stuff (e.g., spaces, '[') */
    while (*s != '\0' && !isdigit (*s))
      s++;

    if (*s == '\0')
    {
      return OF_FAILURE;
    }

    k = (int32_t) of_atoi (s);
    if (k < 0 || 255 < k)
    {
      return OF_FAILURE;
    }

    n |= k << i;
    if (((s = of_strchr (s, '.')) == NULLCHAR) && (i > 0))
      return OF_FAILURE;

    if (i == 0 && (s != NULLCHAR))
      return OF_FAILURE;

    s++;
  }
  *IpAddr = n;
  return OF_SUCCESS;
}

/***************************************************************************
 * Function Name : cm_val_and_conv_aton
 * Description   :
 * Validates the Internet address in ascii dotted-decimal format (44.0.0.1) 
 * string and converts to binary IP address
 * Input         : char*   -- dotted-decimal address to be converted
                                 to binary format
 * Output        : uint32_tr*   -- pointer to uint32_tess where the coverted
                                 address is to be placed.
 * Return value  : OF_SUCCESS or OF_FAILURE
 ***************************************************************************/

int32_t cm_val_and_conv_aton (register char * s, uint32_t * IpAddr)
{
  uint32_t n;

  register int32_t i;
  int32_t k;

  if (cm_ip_is_net_addr (s) == FALSE)
  {
    if(cm_get_host_by_name (s, IpAddr) != OF_SUCCESS)
    {
      return OF_FAILURE;
    }
    return OF_SUCCESS;
  }

  n = 0;
  if (s == NULLCHAR)
    return OF_FAILURE;

  for (i = 24; i >= 0; i -= 8)
  {
    /* Skip any leading stuff (e.g., spaces, '[') */
    while (*s != '\0' && !isdigit (*s))
      s++;

    if (*s == '\0')
      return OF_FAILURE;

    k = (int32_t) atoi (s);
    if (k < 0 || 255 < k)
      return OF_FAILURE;

    n |= k << i;
    if (((s = of_strchr (s, '.')) == NULLCHAR) && (i > 0))
      return OF_FAILURE;

    if (i == 0 && (s != NULLCHAR))
      return OF_FAILURE;

    s++;
  }
  *IpAddr = n;
  return OF_SUCCESS;
}


int32_t cm_conv_to_pres_format(char *sequence)
{

    char ucBuf[CM_INET6_ADDRSTRLEN];
    char *pcTemp=NULL,*pTemp=NULL;
    char *result;
    struct in6_addr ia;
    int32_t i=1;


    pTemp=pcTemp = (char *)of_malloc(CM_INET6_ADDRSTRLEN);
    if(!pTemp)
       return OF_FAILURE;

    of_strcpy(pcTemp,sequence);

    of_memset(ucBuf,0,CM_INET6_ADDRSTRLEN);
    /* After every 4 characters in the input string ,place a ':' to form a new
 *      * string that is ready for input to inet_pton */
    of_strncat(ucBuf,pcTemp,4);
    pcTemp+=4;
    while(i<8)
    {
      of_strcat(ucBuf,":");
      of_strncat(ucBuf,pcTemp,4);
      pcTemp+=4;
      i++;
    }
    of_free(pTemp);
    if((inet_pton(AF_INET6,ucBuf,&ia))<=0)
    {
      return OF_FAILURE;
    }
    /* Converting from binary to presentation form */
    result =(char *)inet_ntop(AF_INET6,&ia,sequence,CM_INET6_ADDRSTRLEN);
    if(!result)
    {
#ifdef LX_DEBUG
       printf("\n Conversion to Presentation foramt failed\n");
#endif
       return OF_FAILURE;
    }
   return OF_SUCCESS;
}/* end ConvToPresFormat */


int32_t cm_ipv6_presentation_to_num (unsigned char *ascii_ip_v6_addr_p,
                       struct in6_addr *bin_ip_v6_addr_p)
{
   struct in6_addr  in6;

   if(inet_pton(AF_INET6,ascii_ip_v6_addr_p,&in6) == 0)
      return OF_FAILURE;

   of_memcpy(bin_ip_v6_addr_p->s6_addr,in6.s6_addr ,CM_IPV6_ADDR_LEN );

   return OF_SUCCESS;
}/* end IGWIPv6PresentationToNum */

int32_t validate_string_with_out_xml_chars(char* name_p, int32_t iLen)
{
  int32_t ii, iSlen;

  if(name_p == NULL)
     return OF_FAILURE;
  for(iSlen = 0;  ((name_p[iSlen] != '\0') && (iSlen < iLen)); iSlen++);

  if(iSlen == 0)
    return OF_FAILURE;
  for(ii=0; ii < iSlen; ii++)
  {
    if( !isalnum (name_p[ii]) ||
        (name_p[ii] == '<') || (name_p[ii] == '>') ||
        (name_p[ii] == '/') || (name_p[ii] == '"') || (name_p[ii] == '!'))
    {
      return OF_FAILURE;  /* INVALID NAME*/
    }
  }
  return OF_SUCCESS;
}
