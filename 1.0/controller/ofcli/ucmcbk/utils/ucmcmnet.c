#ifdef OF_CM_SUPPORT

#include "cmincld.h"
#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"

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
    if (((s = strchr (s, '.')) == NULLCHAR) && (i > 0))
      return OF_FAILURE;

    if (i == 0 && (s != NULLCHAR))
      return OF_FAILURE;

    s++;
  }
  *IpAddr = n;
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
    if (((s = strchr (s, '.')) == NULLCHAR) && (i > 0))
      return OF_FAILURE;

    if (i == 0 && (s != NULLCHAR))
      return OF_FAILURE;

    s++;
  }
  *IpAddr = n;
  return OF_SUCCESS;
}
#endif

