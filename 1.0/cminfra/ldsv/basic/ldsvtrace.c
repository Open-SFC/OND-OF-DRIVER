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
 * File name:   ldsvtrace.c
 * Author:      Freescale Semiconductor    
 * Date:        06/12/2013
 * Description: This file contains debug traces for LDSV.
*/

#ifdef CM_LDSV_SUPPORT

#include "ldsvinc.h"
//#include "jeincld.h"
#include "ldsvldef.h"
//#include "jelif.h"
#include "stdarg.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef OF_CM_SWITCH_CBK_SUPPORT 
#define  CM_LDSV_LOG_FILE "/psp/debug/ldsvdebug"
#endif

#ifdef OF_CM_CNTRL_SUPPORT
#define  CM_LDSV_LOG_FILE "/usr/local/ond_of_driver/1.0/debug/ldsvdebug"
#endif
/******************************************************************************
 * * * * * * * * * * * * * * * GLOBAL VARIABLES* * * * * * * * * * * * * * * * 
 *****************************************************************************/
#ifdef CM_MGMT_CARD_SUPPORT
#define CM_LDSV_MAX_DEBUG_FILES 10
#define CM_LDSV_LOG_FILE_MAX_SIZE  2500000
#else
#define CM_LDSV_MAX_DEBUG_FILES 5
#define CM_LDSV_LOG_FILE_MAX_SIZE  500000
#endif
#define CM_LDSV_MAX_TRACE_STRING_LEN 256

char aLDSVCurFileName_g[100] = CM_LDSV_LOG_FILE;
uint32_t ulLDSVFileNo_g = 1;
 int32_t cm_ldsv_trace_print_message (unsigned char * pTraceLoc,
      char * pFormatString, va_list ap);
/******************************************************************************
 * * * * * * * * * * * * * * FUNCTION DEFINITION * * * * * * * * * * * * * * * 
 *****************************************************************************/
/**************************************************************************
 *  Function Name :ucmConfigVersionInit
 *  Input Args    : none
 *  Output Args   : none
 *  Description   : This method is used to Initialize the global version list
 *  Return Values : Successful Version list initialization returns OF_SUCCESS,
 *          othewise returns OF_FAILURE.
 ***************************************************************************/

void cm_ldsv_trace (int32_t iModuleId, unsigned char ucLevel, char * pFunc,
      int32_t ulLine, char * pFormatString, ...)
{
   va_list ap;
   char aTraceLoc[100];

   snprintf (aTraceLoc, 100, " %35s(%4d): ", pFunc, ulLine);
   va_start (ap, pFormatString);
   cm_ldsv_trace_print_message (aTraceLoc, pFormatString, ap);
   va_end (ap);
}

 int32_t cm_ldsv_trace_print_message (unsigned char * pTraceLoc,
      char * pFormatString, va_list ap)
{
   char aLrgBuf[CM_LDSV_MAX_TRACE_STRING_LEN];
   char aCmd[100] = {};
   char aNewFileName[100] = {};
   char aRemoveFileName[100] = {};
   uint32_t ulTraceLength;
   FILE *fp;
   char aTimeStamp[100];
   struct stat file_status;
   time_t ulTime;
   struct tm                *pLocaltime;
   int32_t iRet,iBytesPrinted;
   int32_t iRMFileNo;

	 ulTraceLength = CM_LDSV_MAX_TRACE_STRING_LEN;
	 iBytesPrinted=vsnprintf (aLrgBuf, ulTraceLength, pFormatString, ap);
	 if( iBytesPrinted >= ulTraceLength)
			aLrgBuf[ulTraceLength - 1] = '\0';
	 else
			aLrgBuf[iBytesPrinted] = '\0';

   
   ulTime=time(NULL);
   pLocaltime=localtime(&ulTime);
#if 0
   snprintf (aTimeStamp, 100, "\n%04d-%02d-%02d %02d:%02d:%02d ",
         pLocaltime->tm_year+1900, pLocaltime->tm_mon+1, pLocaltime->tm_mday,
         pLocaltime->tm_hour, pLocaltime->tm_min, pLocaltime->tm_sec);
#endif
   strftime (aTimeStamp, 100, "\n%d %b %y %H:%M:%S ", pLocaltime);
   fp = fopen (aLDSVCurFileName_g, "a");
   if (!fp)
   {
      perror ("fopen");
      return;
   }


#if 0
   printf ("%a", aTimeStamp);
   printf ("%s", pTraceLoc);
   printf ("%s", aLrgBuf);
#endif

   fprintf (fp, "%s", aTimeStamp);
   fprintf (fp, "%s", pTraceLoc);
   fprintf (fp, "%s", aLrgBuf);

   if (stat (aLDSVCurFileName_g, &file_status) != OF_SUCCESS)
   {
      perror ("stat failed");
      fclose (fp);
      return;
   }
   if (file_status.st_size > CM_LDSV_LOG_FILE_MAX_SIZE)
   {
      sprintf (aNewFileName, "%s.%d", CM_LDSV_LOG_FILE, ulLDSVFileNo_g);
      sprintf (aCmd, "mv %s %s", aLDSVCurFileName_g, aNewFileName);
      iRet=system(aCmd);
      memset(aCmd,0, 100);
      sprintf (aCmd, "gzip %s", aNewFileName);
      iRet=system(aCmd);

      iRMFileNo=ulLDSVFileNo_g - CM_LDSV_MAX_DEBUG_FILES;
      if(iRMFileNo > 0) 
      {
         sprintf (aRemoveFileName, "%s.%d", CM_LDSV_LOG_FILE,iRMFileNo);
         sprintf (aCmd, "rm -f %s", aRemoveFileName);
         iRet=system(aCmd);
         memset(aRemoveFileName,0,100);
         sprintf (aRemoveFileName, "%s.%d.gz", CM_LDSV_LOG_FILE, iRMFileNo);
         sprintf (aCmd, "rm -f %s", aRemoveFileName);
         iRet=system(aCmd);
      }
      ulLDSVFileNo_g++;
   }

   fclose(fp);
   return 0;
}

#endif
