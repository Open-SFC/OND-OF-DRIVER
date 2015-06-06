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
 * File name: ucmfile.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   01/23/2013
 * Description: This file contains wrapper functions that in turn
 *              calls underlying file-system specific api's.
*/

#ifdef CM_SUPPORT

#include "cmincld.h"
#include "cmfile.h"
#include "fcntl.h"

int32_t cm_file_is_file_system_exist (uint32_t uiStartAddr, uint32_t length)
{
  return OF_SUCCESS;
}

/***********************************************************************
 * Function Name : cm_file_open                                         *
 * Description   : Issues underlying File System specific open call    *
 * Input         : lFsId - File id
 *                 file_name_p - File name
 *                 lMode - Mode of open
 * Output        :                                                     *
 * Return value  :
 ***********************************************************************/

int32_t cm_file_open (int32_t lFsId, unsigned char * file_name_p, int32_t lMode)
{
  return open (file_name_p, lMode, S_IRWXU);
}

/***********************************************************************
 * Function Name : cm_file_close                                        *
 * Description   : Issues underlying File System specific close call   *
 * Input         :
 * Output        :
 * Return value  :
 ***********************************************************************/

int32_t cm_file_close (int32_t lFsId, int32_t Fd)
{
  return close (Fd);
}

/***********************************************************************
 * Function Name : cm_file_create                                        *
 * Description   : Issues underlying File System specific create call  *
 * Input         : File descriptor, pointer to the File name           *
 * Output        : none                                                 *
 * Return value  : 
 ***********************************************************************/

int32_t cm_file_create (int32_t lFsId, unsigned char * file_name_p)
{
  return creat (file_name_p, S_IRWXU);
}

/***********************************************************************
 * Function Name : cm_file_read                                         *
 * Description   : Issues underlying File system specific read call    *
 * Input         : File descriptor, pointer to the read buffer and     *
 *                 size                                                *
 * Output        : Reads the data in the buffer                        *
 * Return value  :
 ***********************************************************************/

int32_t cm_file_read (int32_t lFsId, int32_t lFd, unsigned char * pBuf,
                     uint32_t ulCount)
{
  return (read (lFd, pBuf, ulCount));
}

/***********************************************************************
 * Function Name : cm_file_write                                        *
 * Description   : Issues underlying File System specific write call   *
 * Input         : File descriptor, pointer to the write buffer and    *
 *                 size                                                *
 * Output        :                                                     *
 * Return value  :                          *
 ***********************************************************************/

int32_t cm_file_write (int32_t lFsId, int32_t lFd, unsigned char * pBuf,
                      uint32_t ulCount)
{
  return write (lFd, pBuf, ulCount);
}

/***********************************************************************
 * Function Name : cm_file_seek                                         *
 * Description   : Issues underlying File System specific lseek call   *
 * Input         :                                                     *
 * Output        :                                                     *
 * Return value  :
 ***********************************************************************/

int32_t cm_file_seek (int32_t FsId, int32_t lFd, int32_t lPosition,
                     int32_t lOffset)
{
  return lseek (lFd, lOffset, lPosition);
}

/***********************************************************************
 * Function Name : cm_file_tell                                         *
 * Description   : Issues underlying File System specific lseek call   *
 * Input         :
 * Output        :
 * Return value  :
 ***********************************************************************/

int32_t cm_file_tell (int32_t lFsId, int32_t lFd)
{
  return lseek (lFd, 0, UCMFILE_SEEK_CUR);
}

/***********************************************************************
 * Function Name : cm_file_verify_checksum                               *
 * Description   :                                                     *
 * Input         :
 * Output        :
 * Return value  :
 ***********************************************************************/

int32_t cm_file_verify_checksum (int32_t lFsId, unsigned char * file_name_p)
{
  volatile int32_t lTempFd, lRetVal;

  /* If File Exists Return Success */

  lTempFd = cm_file_open (lFsId, file_name_p, UCMFILE_MODE_READ);
  if (lTempFd < 0)
    lRetVal = OF_FAILURE;
  else
    lRetVal = OF_SUCCESS;

  cm_file_close (lFsId, lTempFd);
  return lRetVal;
}

/***********************************************************************
 * Function Name : cm_file_format_file_system                             *
 * Description   : It is a dummy function                              *
 * Input         :
 * Output        :                                                     *
 * Return value  : OF_SUCCESS
 ***********************************************************************/

int32_t cm_file_format_file_system (int32_t lFsId, uint32_t ulMaxFiles,
                                 int32_t lMode)
{
  return OF_SUCCESS;
}

/***********************************************************************
 * Function Name : cm_file_remove                                       *
 * Description   : Issues underlying File System specific remove call  *
 * Input         : File descriptor, pointer to the File name           *
 * Output        :
 * Return value  :
 ***********************************************************************/

int32_t cm_file_remove (int32_t lFsId, unsigned char * file_name_p)
{
  return remove (file_name_p);
}

/***********************************************************************
 * Function Name : cm_file_is_format_needed                               *
 * Description   : It is a dummy function                              *
 * Input         :
 * Output        :
 * Return value  : OF_SUCCESS
 ***********************************************************************/

int32_t cm_file_is_format_needed (int32_t ulFsId)
{
  return 0;
}

/***********************************************************************
 * Function Name : cm_file_flush                                        *
 * Description   : It is a dummy function                              *
 * Input         :
 * Output        :
 * Return value  : OF_SUCCESS
 ***********************************************************************/

int32_t cm_file_flush (int32_t FsId, int32_t Fd)
{
  return OF_SUCCESS;
}

/***********************************************************************
 * Function Name : cm_file_rename                                       *
 * Description   : Issues underlying File System specific rename call  *
 * Input         : Address to Old file name,and address to new file
 *                 name.
 * Output        :                                                     *
 * Return value  : OF_SUCCESS or OF_FAILURE
 ***********************************************************************/

int32_t cm_file_rename (unsigned char * pOldFileName, unsigned char * pNewFileName)
{
  if (rename (pOldFileName, pNewFileName) == 0)
  {
    return OF_SUCCESS;
  }
  else
  {
    return OF_FAILURE;
  }
}

#endif
