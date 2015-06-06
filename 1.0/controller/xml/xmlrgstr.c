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
 *  File         : xmlrgstr.c                                                *
 *                                                                           *
 *  Description  : XML handler for iGateway handler.                         *
 *                                                                           *
 *  Version      Date        Author         Change Description               *
 *  -------    --------      ------        ----------------------            *
 *   1.0       14/06/05       sbala         Initial implementation           *
 ****************************************************************************/
#ifdef OF_XML_SUPPORT

/***********************************************************
 * * * *  I n c l u d e    H e a d e r    f i l e s  * * * *
 ***********************************************************/
 #include "cntrlxml.h"
// #include "pspdummy.h"
 //#include "pspcmn.h"

#ifdef PSP_XML_TEST_SUPPORT
 void PSPXmlNTGRPHttpInit(void);
#endif /* PSP_XML_TEST_SUPPORT */
#if 0
#ifdef PSP_XML_TEST_SUPPORT
 void PSPXmlNetworkGroupHttpInit(void);
#endif /* PSP_XML_TEST_SUPPORT */
#endif


/**************************************************************************
 * Function Name	: XMLMain_RegisterAll                                 *
 * Description		: Registers all XML callback functions.               *
 * Input			: None                                                *
 * Output		: None                                                    *
 * Return value	: None                                                    *
 **************************************************************************/

void XMLMain_RegisterAll( void )
{
	#ifdef PSP_XML_TEST_SUPPORT
	   /*PSPXmlNetworkGroupHttpInit();*/
	  /* PSPXmlNTGRPHttpInit();*/    /*TODO: move to mgmt.*/
	#endif /* PSP_XML_TEST_SUPPORT */
}

#endif /*PSP_XML_SUPPORT */


