/*****************************************************************************
*                                                                            *
*                           Copyright 1998 - 2002,                           *
*                      Intoto, Inc, SANTA CLARA, CA, USA                     *
*                             ALL RIGHTS RESERVED                            *
*                                                                            *
*   Permission is hereby granted to licensees of Intoto, Inc. products to    *
*   use or abstract this computer program for the sole purpose of            *
*   implementing a product based on Intoto, Inc. products. No other rights   *
*   to reproduce, use, or disseminate this computer program, whether in      *
*   part or in  whole, are granted.                                          *
*                                                                            *
*   Intoto, Inc. makes no representation or warranties with respect to the   *
*   performance of this computer program, and specifically disclaims any     *
*   responsibility for any damages, special or consequential, connected      *
*   with the use of this program.                                            *
*                                                                            *
*****************************************************************************/

/*****************************************************************************
 *  File         : xmlfutil.c                                                *
 *                                                                           *
 *  Description  : XML File handler for iGateway handler.                    *
 *                                                                           *
 *  Version      Date        Author         Change Description               *
 *  -------    --------      ------        ----------------------            *
 *   1.0       03/09/05                Initial implementation                *
 ****************************************************************************/

#ifdef OF_XML_SUPPORT

/***********************************************************
 * * * *  I n c l u d e    H e a d e r    f i l e s  * * * *
 ***********************************************************/
/*#include "pspdummy.h"*/
//#include <psp/psp.h>
#include <cntrlxml.h>
#include <stdio.h>
#include "xmlfutil.h"
#include "ixmlparser.h"
#include "oftype.h"
#include "cntlr_error.h"

/*********************************************************
*   CNTRLXMLReadFile
*       read a xml file or buffer contents into xml parser.
*       Internal to parser only.
*
**********************************************************/

int32_t CNTRLXMLReadFile(char *xmlFileName, char **dataBuffer)
{
    int fileSize = 0;
    int bytesRead = 0;
    FILE *xmlFilePtr = NULL;


    printf("%s:%d xml file = %s \r\n",__FUNCTION__,__LINE__,xmlFileName);

    xmlFilePtr = fopen( xmlFileName, "rb" );
    if( xmlFilePtr == NULL ) {
            perror("fopen: error - \r\n");
            printf("%s:%d \r\n",__FUNCTION__,__LINE__);
        return IXML_NO_SUCH_FILE;
    } else {
        fseek( xmlFilePtr, 0, SEEK_END );
        fileSize = ftell( xmlFilePtr );
        if( fileSize == 0 ) {
            printf("%s:%d \r\n",__FUNCTION__,__LINE__);
            fclose( xmlFilePtr );
            return IXML_SYNTAX_ERR;
        }

        *dataBuffer = ( char * )malloc( fileSize + 1 );
        if( *dataBuffer == NULL ) {
            fclose( xmlFilePtr );
            printf("%s:%d \r\n",__FUNCTION__,__LINE__);
            return IXML_INSUFFICIENT_MEMORY;
        }

        fseek( xmlFilePtr, 0, SEEK_SET );
        bytesRead =
           fread( *dataBuffer, 1, fileSize, xmlFilePtr );
        (*dataBuffer)[bytesRead] = '\0';    /* append null*/
        fclose( xmlFilePtr );
    }
	return PSP_SUCCESS;
}

#endif /*PSP_XML_SUPPORT*/

