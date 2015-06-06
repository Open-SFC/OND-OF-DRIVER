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
 * File name: ucmdb.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: UCM userdb module for PAM Authentication.
 * 
 */

#ifdef CM_PAM_SUPPORT
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ucmdb.h"

/******* M A C R O S ***********************************/

#define READ_LINE_LEN 100
#define OF_SUCCESS     0
#define OF_FAILURE     1

/********************************************************/


struct cm_role_db *pRoleDbHead = NULL;

/******* F U N C T I O N   P R O T O T Y P E S **********/
char* cm_strp_brk(char* cs,char* ct);
char* cm_str_tok(char *s, char *ct);
char *cm_trim_string (char * string_p);
void  cm_tok_array(char *str, char *delims, char *resultArry[], int *cnt);
void  add_record(char *resultArry[]);

/********************************************************/

int cm_load_user_db(char *file_name_p)
{
  char aReadLine[READ_LINE_LEN];
  
  char *temp_string_p = NULL;
  char *string_p = NULL;
  FILE *pFile = NULL;
  char  *resultArry[100];
  int  count = 0;
  int  cnt = 0;
  char *delims =":";

  memset(resultArry,0, sizeof(resultArry));

  if ((pFile = fopen (file_name_p, "r")) == NULL)
  {
    UCMROLEDBDEBUG ("File Not Found.");
    return OF_FAILURE;
  }
  memset (aReadLine, 0, sizeof(aReadLine));

  while((string_p = fgets (aReadLine, READ_LINE_LEN, pFile)) != NULL)
  {
    temp_string_p = cm_trim_string (string_p);
    if(strlen(temp_string_p) > 1)
    {
      memset(resultArry,0, sizeof(resultArry));
      cm_tok_array(temp_string_p, delims, resultArry, &cnt);
      add_record(resultArry);
    }
    UCMROLEDBDEBUG("\ntemp_string_p 2= %s\n",temp_string_p);
    memset (aReadLine, 0, sizeof(aReadLine));
  }
  UCMROLEDBDEBUG("\n*** 2:: Total lines=%d \n", count);
  fclose (pFile);
  return OF_SUCCESS;
}

void  add_record(char *resultArry[])
{
  struct cm_role_db  *temp = NULL;
  int len = 0;

  temp = pRoleDbHead;
  
  if(pRoleDbHead==NULL)
  {
    pRoleDbHead=calloc(1, sizeof(struct cm_role_db));
    if(!pRoleDbHead)
    {
      UCMROLEDBDEBUG(" %s()::Failed to allocate memory \n", __FUNCTION__);
      return;
    }
    temp = pRoleDbHead;
  }
  else
  {
    while((temp->next_p)!=NULL)
    {
      temp=temp->next_p;
    }
    temp->next_p = calloc(1, sizeof(struct cm_role_db));
    if(!temp->next_p)
    {
      UCMROLEDBDEBUG(" %s()::Failed to allocate memory \n", __FUNCTION__);
      return;
    }
    temp=temp->next_p;
  }
  //copy User Name;
  len = strlen(resultArry[0]);
  temp->user_name_p = (char*)calloc(1, len + 1);
  if(temp->user_name_p == NULL)    
  {
    /* Free*/
    free(temp);
    return;      
  }
  memcpy(temp->user_name_p, resultArry[0], len);

  //copy Password MD5 sum
  len = strlen(resultArry[1]);
  temp->pPwdSum = (char*)calloc(1, len + 1);
  if(temp->pPwdSum == NULL)    
  {
    /* Free*/
    free(temp->user_name_p);
    free(temp);
    return;      
  }
  memcpy(temp->pPwdSum, resultArry[1], len);

  //copy Role Name
  len = strlen(resultArry[2]);
  temp->role_name_p = (char*)calloc(1, len + 1);
  if(temp->role_name_p == NULL)    
  {
    /* Free*/
    free(temp->user_name_p);
    free(temp->pPwdSum);
    free(temp);
    return;      
  }
  memcpy(temp->role_name_p, resultArry[2], len);

  temp->next_p  = NULL;
} // add_record

char * get_role_info(const char *pUser, char *pMd5Sum)
{
  struct cm_role_db  *temp;

  temp = pRoleDbHead;

  //while(temp->next_p!=NULL)
  while(temp !=NULL)
  {
    if((strcmp(temp->user_name_p, pUser) == 0) 
         && (strcmp(temp->pPwdSum, pMd5Sum) == 0))
      return(temp->role_name_p);
    temp=temp->next_p;
  }
  return(NULL);
}

void clean_role_info()
{
  
  struct cm_role_db  *temp = NULL;
  struct cm_role_db  *delNode = NULL;

  UCMROLEDBDEBUG("\n**** %s():: invoked... \n", __FUNCTION__);
  temp = pRoleDbHead;

  while(temp->next_p!=NULL)
  {
    free(temp->user_name_p);
    free(temp->pPwdSum);
    free(temp->role_name_p);
    delNode = temp;
    temp=temp->next_p;
    free(delNode);
    delNode = NULL;
  }
    pRoleDbHead = NULL;
  return;
}

void  cm_tok_array(char *str, char *delims,
                      char *resultArry[], int *cnt)
{
  char *result = NULL;
  int ii = 0,x =0;

  result = cm_str_tok( str, delims );

  while( result != NULL ) 
  {
      resultArry[x] = result;
      x++;
      ii++;

      result = cm_str_tok( NULL, delims );
  }

  UCMROLEDBDEBUG(" %s()::ii = %d \n", __FUNCTION__, ii);
  for(x = 0; x < ii; x++)
  {
    UCMROLEDBDEBUG(" %s()::resultArry = %s \n", __FUNCTION__, resultArry[x]);
  }
  *cnt = ii; 
}
/**************************************************************************
 Function Name : cm_trim_string
 Input Args    : 
                string_p: String to remove leading and trailing spaces.
 Output Args   : none
 Description   : This method is used to remove triling and leading spaces.
 Return Values : New String after removing spaces.
 *************************************************************************/
char *cm_trim_string (char * string_p)
{
  char *pInBuffer = string_p, *pOutBuff = string_p;
  int i = 0, count_i = 0;

  if (string_p)
  {
    for (pInBuffer = string_p; *pInBuffer && isspace (*pInBuffer); ++pInBuffer)
      ;
    if (string_p != pInBuffer)
      memmove (string_p, pInBuffer, pInBuffer - string_p);

    while (*pInBuffer)
    {
      if (isspace (*pInBuffer) && count_i)
        pInBuffer++;
      else
      {
        if (!isspace (*pInBuffer))
          count_i = 0;
        else
        {
          *pInBuffer = ' ';
          count_i = 1;
        }
        pOutBuff[i++] = *pInBuffer++;
      }
    }
    pOutBuff[i] = '\0';

    while (--i >= 0)
    {
      if (!isspace (pOutBuff[i]))
        break;
    }
    pOutBuff[++i] = '\0';
  }
  return string_p;
}
/****************************************************************************
 * Function Name:cm_str_tok
 * 
 * Description -A strtok which returns empty strings, too
 * input: The string to be searched
 *        The characters to search for
 * output:1.)Null if not match found
 *        2.)Pointer pointing to the matching position in
 ****************************************************************************/

char* cm_str_tok(char *s, char *ct)
{
  char *sbegin=NULL;
  char *send=NULL;
  static char *ssave = NULL;

  sbegin  = s ? s : ssave;
  if (!sbegin)
  {
     return NULL;
  }
  if (*sbegin == '\0')
  {
    ssave = NULL;
    return NULL;
  }
  send =cm_strp_brk(sbegin, ct);
  if (send && *send != '\0')
  {
    *send++ = '\0';
  }
  ssave = send;
  return sbegin;
}
/**********************************************************************************
 * Function Name: ucmStrpbrk                                                      *
 * Description - Find the first occurrence of a set of characters              *
 * input:@cs: The string to be searched                        *      
 *       @ct: The characters to search for                                     *  
 * output:1.)Null if not match found                                              *
 *        2.)Pointer pointing to the matching position in @cs                     *
 *********************************************************************************/
char* cm_strp_brk(char* cs,char* ct)
{
  char *sc1=NULL;
  char *sc2=NULL;
  for( sc1 = cs; *sc1 != '\0'; ++sc1) 
  {
    for( sc2 = ct; *sc2 != '\0'; ++sc2)
    {
      if (*sc1 == *sc2)
      {    
        return (char *) sc1;
    }
    }
  }
  return NULL;
}
#endif /*CM_PAM_SUPPORT*/
