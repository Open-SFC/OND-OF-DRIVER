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
 * File name: dmparse.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description: This file contains source code implementing 
 *              configuration middle-ware data model tree. 
*/

#ifdef CM_SUPPORT
#ifdef CM_DM_SUPPORT

#include "dmincld.h"

/***************************************************************************
 * Function Name : cmi_dm_get_path_token
 * Description   : This function extracts the first token from a path string
 *                 and updates the index to point to the rest of the string.
 *                 Using this we can successively parse through an entire
 *                 path string.  
 *
 * Input         : char* path_string_p -- A pointer to the path string 
 *                      to be tokenized
 *                 uint32_t uiMaxTokenLen -- Output buffer's max length.
 *                 uint32_t* pIndex  -- A pointer the index in the remaining 
                        path string
 * Output        : char* pToken-- Return the parsed token as a pointer 
 * Return value  : OF_SUCCESS -- If the pathstring is not completely parsed.
 *                              otherwise return OF_FAILURE;
 ***************************************************************************/
int32_t cmi_dm_get_path_token (char * path_string_p,
                            char * pToken,
                            uint32_t uiMaxTokenLen, uint32_t * pIndex)
{
  uint32_t uiTmpIndex;
  uint32_t uiStrLen;
  uint32_t ii, jj = 0;
	unsigned char bInstStart = FALSE, bInstEnd = FALSE;

  if ((path_string_p == NULL) || (pIndex == NULL) || (pToken == NULL))
  {
    return UCMDM_PATH_PARSING_COMPLETED;
  }

  uiTmpIndex = *pIndex;

  uiStrLen = of_strlen (path_string_p);
  if (uiTmpIndex >= uiStrLen)
  {
    pToken[0] = '\0';
    return UCMDM_PATH_PARSING_COMPLETED;
  }

	for (ii = uiTmpIndex; ii < uiStrLen; ii++)
	{
		 if (jj >= uiMaxTokenLen)
		 {
				pToken[0] = '\0';
				return UCMDM_PATH_PARSING_COMPLETED;
		 }

		 if((path_string_p[ii]=='{') && ii==uiTmpIndex)
		 {
				bInstStart = TRUE;
		 }

		 if(path_string_p[ii] == '}')
		 {
				if (bInstStart == TRUE)
					 bInstEnd = TRUE;
		 }
		 if (path_string_p[ii] == '.')
		 {
				if(((bInstStart == TRUE) && (bInstEnd == TRUE)) ||
						((bInstStart == FALSE) && (bInstEnd == FALSE))	 )
				{
				pToken[jj] = '\0';
				*pIndex = ii + 1;
				break;
				}
		 }
		 /*  uiTmpIndex is begining of new token
			* tokenize at '{' if it is not the first position
			*/
		 if ((path_string_p[ii] == '{') && (ii != uiTmpIndex))
		 {
				pToken[jj] = '\0';
				*pIndex = ii;
				break;
		 }

		 pToken[jj++] = path_string_p[ii];
		 *pIndex = ii + 1;
	}

  if (*pIndex >= uiStrLen)
  {
    pToken[jj] = '\0';
    return UCMDM_PATH_PARSING_COMPLETED;
  }

  return UCMDM_PATH_PARSING_REMAINING;
}

#endif /* CM_DM_SUPPORT */
#endif /* CM_SUPPORT */
