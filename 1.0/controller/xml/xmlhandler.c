
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
 *  File         : xmlhandler.c                                              *
 *                                                                           *
 *  Description  : XML handler for iGateway handler.                         *
 *                                                                           *
 *  Version      Date        Author         Change Description               *
 *  -------    --------      ------        ----------------------            *
 *   1.0       14/06/05       sbala         Initial implementation           *
 ****************************************************************************/
#if defined(PSP_HTTPD_SUPPORT)
#ifdef OF_XML_SUPPORT

/***********************************************************
 * * * *  I n c l u d e    H e a d e r    f i l e s  * * * *
 ***********************************************************/
//#include <psp/psp.h>
#include <cntrlxml.h>
#include "hash.h"
#include "xmlhandler.h"
#include "xmlrec.h"
#ifdef PSP_XPATH_SUPPORT
#include "pspxpath.h"
#endif
/****************************************************
 * * * *  M a c r o    D e f i n i t i o n s  * * * *
 ****************************************************/

#define TRANSACTION_TAG "Transaction"
#define ID_ATTRIB "id"

#define NAME_MAX_LEN	30
#define VALUE_MAX_LEN	512

#define MAX_HASH_SIZE 10
#define NO_OF_FILE_ATTRIBS  4

#define SREDIRECT_URL "<RedirectURL>testtabl.xml</RedirectURL>"
#define FREDIRECT_URL "<RedirectURL>testmod.xml</RedirectURL>"
#define RESULT_MAX_LEN 2048

/*#define SUCCESS_ATTRIB_INX		0
#define FAILURE_ATTRIB_INX		1*/
#define SREDIRECT_ATTRIB_INX	0
#define FREDIRECT_ATTRIB_INX	1
#define SUCCESS_XSLREF_INX		2
#define FAILURE_XSLREF_INX		3

/****************************************************
 * * * *  T y p e    D e c l a r a t i o n s  * * * *
 ****************************************************/

typedef t_int32 (*XMLTEMPLATE_CALLBACK_HANDLER) (t_char8 * fileName,
                                                 t_void * pData);

typedef struct
{
  t_uchar8 AttribName[NAME_MAX_LEN];
  t_uchar8 AttribValue[VALUE_MAX_LEN];
} Attribs;

/********************************************************
 * * * *  F u n c t i o n    P r o t o t y p e s  * * * *
 ********************************************************/

TR_PRIVATE t_int32 CNTRLXMLReadytoCommit (TransactionSet_t * pTrSet);
TR_PRIVATE t_int32 CNTRLXMLCommit (TransactionSet_t * pTrSet);
TR_PRIVATE t_int32 CNTRLXMLAbortTransactions (TransactionSet_t * pTrSet);
TR_PRIVATE t_int32 CNTRLXMLParseModsets (t_void * docPtr);
TR_PRIVATE t_void CNTRLXMLFreeTrans (TransactionSet_t * pTrSet);
TR_PRIVATE t_int32 CNTRLXMLAddToTrList (TransactionSet_t * pTrSet,
                                      PSPTransactionId_t * pFullTr,
                                      t_char8 * pModName);

TR_PRIVATE t_uint32 CNTRLXMLGenerateTrans (TransactionSet_t ** pTrSet);
TR_PRIVATE t_void CNTRLXMLProcessBrowserTag (t_void * pCurrentCommand);
#ifdef PSP_XPATH_SUPPORT
TR_PRIVATE t_int32 XPathPopulateValues (t_void * doc,
                                        t_char8 * ValArray[],
                                        t_uchar8 * uXPathExpr,
                                        t_int32 recSize,
                                        IXML_NODE_TYPE nodeType,
                                        t_void * xpathCtxt);
TR_PRIVATE t_int32 PSPCreateXPathExpression (t_uchar8 * pIdRef,
                                             t_uchar8 * pOutXPathExpr);
#endif
/************************************************************
 * * * *  V a r i a b l e    D e c l a r a t i o n s  * * * *
 ************************************************************/

xmlHashTablePtr hTable = NULL;
xmlHashTablePtr pModSetTable = NULL;
xmlHashTablePtr pIdRefTable = NULL;

t_char8 bTransFlag = FALSE;

TransactionSet_t *pTransList = NULL;
t_uint32 MasterTransID = 0;     /*Used for generate unique Transaction ID */
DOM pXmlResult = NULL;          /*XML result structure */
t_char8 *pXMLResultStr = NULL;  /*XML result string */
DOM pXmlError = NULL;           /*XML error structure */
                                                                                                                                                                                                                                                                                                                                                                                                                                         /*t_char8 pXMLErrorStr[RESULT_MAX_LEN]; *//*XML Error string */
t_char8 *pXMLErrorStr = NULL;
t_char8 *pBrowserTag = NULL;    /*UI Specific browser tag */

t_char8 xmlResDeclare[] =
  "<?xml version=\"1.0\"encoding=\"UTF-8\"?>\n<?xml-stylesheet type=\"text/xsl\"href=\"success_en.xsl\"?><result><Transaction status=\'ok\'>";
t_char8 xmlResEndDecl[] = "</Transaction></result>";

t_char8 resultTagBegin[] = "<result>";
t_char8 resultTagEnd[] = "</result>";
t_char8 startTransTag[] = "<Transaction status=\'ok\'>";
t_char8 failureTransTag[] = "<Transaction status=\'error\'>";
t_char8 endTransTag[] = "</Transaction>";
t_char8 redirectURLTag[] = "<RedirectURL>";
t_char8 redirectURLEndTag[] = "</RedirectURL>";

/***********************************************************************
 * Function Name : CNTRLXMLGETReqHandler
 * Description   : This function calls the registered callback functions
 				   against the XML file URL
 * Input         : regUrl - XML file URL
 *				   pOutXMLStr - Response XML buffer
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
t_char8
CNTRLXMLGETReqHandler (t_char8 * pInXMLFile, t_void * pAppData,
                     t_char8 ** pOutXMLStr)
{

  XMLTEMPLATE_CALLBACK_HANDLER pCbfun;
  t_int32 retStatus = CNTRLXML_SUCCESS;

  if (!pInXMLFile)
  {
    XML_Error ("pInXMLFile is NULL");
    return CNTRLXML_BADPARAM;
  }

  XML_DebugArg ("Look up for tag = ", pInXMLFile);
  pCbfun = xmlHashLookup (hTable, pInXMLFile);

  if (pCbfun != NULL)
  {
    XML_DebugArg ("Found function for tag =", pInXMLFile);
    retStatus = (*pCbfun) (pInXMLFile, pAppData);
    if (retStatus != CNTRLXML_SUCCESS)
    {
      XML_Error ("GET Request CbkFunction Failed");
      if(pXMLErrorStr)
      {
        *pOutXMLStr = xmlStrdup ((const xmlChar *) pXMLErrorStr);

        XML_DebugArg ("*pOutXMLStr=", *pOutXMLStr);
        XMLBUFFERFREE (pXMLErrorStr);

        pXMLErrorStr = NULL;

        return CNTRLXML_SUCCESS;
      }
      *pOutXMLStr = NULL;
      return CNTRLXML_FAILURE;
    }

    /**pOutXMLStr = (t_char8*) T_malloc(T_strlen(pXMLResultStr)+1);
    of_strcpy(*pOutXMLStr, pXMLResultStr);*/
    *pOutXMLStr = xmlStrdup ((const xmlChar *) pXMLResultStr);

    XML_DebugArg ("*pOutXMLStr=", *pOutXMLStr);
    XMLBUFFERFREE (pXMLResultStr);

    pXMLResultStr = NULL;

    return CNTRLXML_SUCCESS;

  }
  return CNTRLXML_FAILURE;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetRegHandler
 * Description   : This function retrives registered callback functions
 				   for the given Module name.
 * Input         : pInModule	- Module name
 * Output		 : pOutFunLst	- Registered function list
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
t_char8
CNTRLXMLGetRegHandler (t_char8 * pInModule, t_void ** pOutFunLst)
{

  XML_DebugArg ("Look up for tag = ", pInModule);
  *pOutFunLst = (t_void *) xmlHashLookup (pModSetTable, pInModule);

  if (*pOutFunLst == NULL)
  {
    XML_DebugArg ("Could not find function for tag", pInModule);
    return CNTRLXML_FAILURE;
  }
  XML_DebugArg ("Found function for tag = %s", pInModule);

  return CNTRLXML_SUCCESS;

}

/************************************************************************
* Function Name : XmlRegModuleInit                                      *
* Description   : initiates modules to register with XML                *
* Input         : nil                                                   *
* Output        : null                                                  *
* Return value  : T_SUCCESS or T_FAILURE.                               *
************************************************************************/
t_int32
XmlRegModuleInit (t_void)
{

  if (InitXmlModHashTable () != T_SUCCESS)
  {
    return T_FAILURE;
  }
  return T_SUCCESS;
}

/***********************************
 * Function Name: InitXmlModHashTable
 * Desription	: This functions Initializes the HashTable for Modset
 * Input	: None
 * Output	: xmlHashTableptr
 * Return Value	: None
 ***************************************/
t_int32
InitXmlModHashTable (t_void)
{
  hTable = xmlHashCreate (MAX_HASH_SIZE);
  if (hTable == NULL)
  {
    XML_Debug ("A xml Modset Table is not created");
    return -1;
  }
  return 0;
}

/***********************************
 * Function Name: Xml_Register_Modset
 * Desription	: This functions Register Modset entry to Hash Table
 * Input	: pModsetName - Modset Name
 * 		  funptr      - Call back function pointer
 * Output	: Success or Failure
 * Return Value	: None
 ***************************************/
t_int32
Xml_Register_Modset (t_char8 * pModsetName, t_void * funptr)
{
  int val;
  XML_DebugArg ("pModsetName =", pModsetName);
  if (hTable == NULL)
  {
    XML_Debug ("Xml Module Hash Table does not exist");
    return -1;
  }

  val = xmlHashAddEntry (hTable, pModsetName, funptr);
  if (val != 0)
  {
    XML_DebugArg (pModsetName, "is already exist in Hash");
    return -1;
  }
  return 0;
}

/***********************************************************************
 * Function Name: CNTRLXMLRegisterModsets
 * Desription	: This functions Register Modset entry to Hash Table
 * Input	: pModsetName - Modset Name
 * 		  funptr      - Call back function pointer
 * Output	: Success or Failure
 * Return Value	: None
 ***********************************************************************/
t_int32
CNTRLXMLRegisterModsets (t_char8 * pModsetName, t_void * strPtr)
{
  int val;
  XML_DebugArg ("pModsetName = ", pModsetName);
  if (pModSetTable == NULL)
  {
    XML_Error ("Xml Module Hash Table does not exist");
    return CNTRLXML_FAILURE;
  }
  val = xmlHashAddEntry (pModSetTable, pModsetName, strPtr);

  if (val != 0)
  {
    XML_DebugArg (pModsetName, "is already exist in Hash");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/**********************************************************************
 * Function Name: PSPInitXmlModsetTable
 * Desription	: This functions Initializes the HashTable for Modset
 * Input	: None
 * Output	: xmlHashTableptr
 * Return Value	: None
 ***********************************************************************/
t_int32
PSPInitXmlModsetTable (t_void)
{
  pModSetTable = xmlHashCreate (MAX_HASH_SIZE);
  if (pModSetTable == NULL)
  {
    XML_Error ("xml Modset Table is not created");
    return CNTRLXML_FAILURE;
  }
  /*    XMLMain_RegisterAll(); *//*This will be called from xmlhtphdlr.c. */
  return CNTRLXML_SUCCESS;
}

#if 0
/***********************************************************************
 * Function Name	: PSPReturnXMLResult
 * Description		: This function appends given element(xmlNode)
 * 				      to the Result XML
 * Input			: pTrSet - Transaction Set
 * 				      xmlNode  - Result XML element to be appended.
 *				      Calling function should free"xmlNode".
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
t_int32
PSPReturnXMLResult (PSPTransactionId_t * pTrSet, DOM xmlNode)
{
  t_char8 xmlDeclare[] = "<?xml version=\"1.0\"encoding=\"UTF-8\"?>";
  t_char8 xmlFile[] =
    "<result><Transaction status=\'ok\'></Transaction></result>";
  t_int32 retVal = 0;
  t_char8 *tempBuff = NULL;

  if (!xmlNode)
  {
    return CNTRLXML_BADPARAM;
  }

  if (IsValidTransaction (pTrSet))
  {
    if (pXmlResult == NULL)
    {
      /*
       *      Create error XML DOM structure.
       */
      retVal = CNTRLXMLParseXML (xmlFile, &pXmlResult);
    }

    if (!pTransList)            /*If no transactions in progress */
    {
      /*
       *      Convert the XML structure into string buffer
       */
      retVal = CNTRLXMLtoBuffer (pXmlResult, &tempBuff);

      if (retVal == CNTRLXML_SUCCESS)
      {
        pXMLResultStr = (t_char8 *) T_malloc (T_strlen (xmlDeclare)
                                              + T_strlen (tempBuff) + 1);
        T_strcat (pXMLResultStr, xmlDeclare);
        T_strcat (pXMLResultStr, tempBuff);
        free (tempBuff);
      }
    }
    else
    {
      /*
       *      Append xmlNode structure to pXmlError
       */
      t_void *rootNode = NULL;
      t_void *pCurrentCommand = NULL;

      CNTRLXMLGetRootNode (pXmlResult, &rootNode);

      retVal = CNTRLXMLGetFirstChildElement (rootNode, &pCurrentCommand);
      if (retVal != CNTRLXML_SUCCESS)
      {
#ifdef XMLDEBUG
        XML_Debug ("Could not append the child.", __FUNCTION__);
#endif
        return CNTRLXML_FAILURE;
      }
      retVal = CNTRLXMLAppendChildNode (&rootNode, xmlNode);

      if (retVal != CNTRLXML_SUCCESS)
      {
#ifdef XMLDEBUG
        XML_Debug ("Could not append the child.", __FUNCTION__);
#endif
        return CNTRLXML_FAILURE;
      }
    }
  }
  else
  {
    /*
     *      Convert the XML structure into string buffer
     */
    retVal = CNTRLXMLtoBuffer (pXmlResult, &pXMLResultStr);
    if (retVal != CNTRLXML_SUCCESS)
    {
#ifdef XMLDEBUG
      XML_Debug ("Could not append the Result XML.", __FUNCTION__);
#endif
      return CNTRLXML_FAILURE;
    }
  }
  return CNTRLXML_SUCCESS;

}
#endif

t_int32
PSPReturnXMLResult (PSPTransactionId_t * pTrSet, DOM XMLNode)
{
  t_int32 iErrCode;
  t_char8 *pXmlBuffer = NULL;

  if (!XMLNode)
  {
    return CNTRLXML_BADPARAM;
  }

  if ((iErrCode = CNTRLXMLtoBuffer (XMLNode, &pXmlBuffer)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("Failed to load result DOM.");
    return CNTRLXML_FAILURE;
  }

  if ((iErrCode =
       PSPReturnXMLResultString (pTrSet, XMLNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("PSPReturnXMLResultString Failed");
  }
  XML_DebugArg ("pXmlBuffer=", pXmlBuffer);
  XMLBUFFERFREE (pXmlBuffer);
  return CNTRLXML_SUCCESS;

}

t_int32
PSPReturnXMLError (PSPTransactionId_t * pTrSet, DOM XMLNode)
{
  t_int32 iErrCode;
  t_char8 *pXmlBuffer = NULL;

  if (!XMLNode)
  {
    return CNTRLXML_BADPARAM;
  }

  if ((iErrCode = CNTRLXMLtoBuffer (XMLNode, &pXmlBuffer)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("Failed to load result DOM.");
    return CNTRLXML_FAILURE;
  }

  if ((iErrCode =
       PSPReturnXMLErrorString (pTrSet, pXmlBuffer)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("PSPReturnXMLErrorString Failed");
  }
  XML_DebugArg ("pXmlBuffer=", pXmlBuffer);
  XMLBUFFERFREE (pXmlBuffer);
  return CNTRLXML_SUCCESS;

}

/***********************************************************************
 * Function Name	: PSPReturnXMLResultString
 * Description		: This function appends given result XML string(xmlStr)
 * 				   to the Result XML buffer.
 * Input			: pTrSet - Transaction Set
 * 				  xmlStr  - Result XML string to be appended
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
t_int32
PSPReturnXMLResultString (PSPTransactionId_t * pTrSet, XML_STR pInXMLStr)
{
  t_char8 xmlDeclare[] =
    "<?xml version=\"1.0\"encoding=\"UTF-8\"?>\n<result><Transaction status=\'ok\'>";
  t_char8 xmlEndDecl[] = "</Transaction></result>";

  if (!pInXMLStr)
  {
    XML_Error ("BAD Input Parameters");
    return CNTRLXML_BADPARAM;
  }

  if (IsValidTransaction (pTrSet))
  {
    /*
     *      Transaction based XML processing.
     */
    if (pXMLResultStr == NULL)
    {
      pXMLResultStr = xmlStrdup ((const xmlChar *) xmlDeclare);
    }

    if (!pTransList)            /*If no transaction in progress */
    {
      pXMLResultStr = xmlStrcat (pXMLResultStr, (const xmlChar *) pInXMLStr);
      pXMLResultStr = xmlStrcat (pXMLResultStr, (const xmlChar *) xmlEndDecl);
    }
    else
    {
      pXMLResultStr = xmlStrcat (pXMLResultStr, (const xmlChar *) pInXMLStr);
    }
  }
  else
  {
    /*
     *      Non-Transaction XML processing.
     */
    XML_Debug ("Non-Transaction based XML processing");
/*    XML_DebugArg ("pInXMLStr =", pInXMLStr);*/

    if (pXMLResultStr == NULL ||
        T_strnicmp (pInXMLStr, "<html>", 6) == 0)
    {
      pXMLResultStr = xmlStrdup ((const xmlChar *) pInXMLStr);
    }
    else
    {
      if (T_strnicmp (pXMLResultStr, "<html>", 6) != 0)
        pXMLResultStr = xmlStrcat (pXMLResultStr, (const xmlChar *) pInXMLStr);
    }
  }

  return CNTRLXML_SUCCESS;

}

/***********************************************************************
 * Function Name	: PSPReturnXMLErrorString
 * Description		: This function appends given error XML string(xmlStr)
 * 				   to the Result XML buffer.
 * Input			: pTrSet - Transaction Set
 * 				  xmlStr  - Error XML string to be appended
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
t_int32
PSPReturnXMLErrorString (PSPTransactionId_t * pTrSet, XML_STR pInXMLStr)
{
  t_char8 xmlDeclare[] =
    "<?xml version=\"1.0\"encoding=\"UTF-8\"?>\n<result><Transaction status=\'error\'>";
  t_char8 xmlEndDecl[] = "</Transaction></result>";

  if (!pInXMLStr)
  {
    XML_Error ("Bad Input Parameters");
    return CNTRLXML_BADPARAM;
  }

  if (IsValidTransaction (pTrSet))
  {
    /*
     *      Transaction based XML processing.
     */
    XML_Debug ("Transaction based XML processing.");
    /*  XML_Debug("Master tr_id = %d", (int)pTrSet->MasterTrId); */
    if (pXMLErrorStr == NULL)
    {

      /*of_strcpy(pXMLErrorStr, xmlDeclare); */
      pXMLErrorStr = xmlStrdup ((const xmlChar *) xmlDeclare);
    }

    if (!pTransList)            /*If no transaction in progress */
    {
      /*T_strcat(pXMLErrorStr, pInXMLStr);
         T_strcat(pXMLErrorStr, xmlEndDecl); */
      pXMLErrorStr = xmlStrcat (pXMLErrorStr, pInXMLStr);
      pXMLErrorStr = xmlStrcat (pXMLErrorStr, xmlEndDecl);
    }
    else
    {
      /*T_strcat(pXMLErrorStr, pInXMLStr); */
      pXMLErrorStr = xmlStrcat (pXMLErrorStr, pInXMLStr);
    }
  }
  else
  {
    /*
     *      Non-Transaction XML processing.
     */
    XML_Debug ("Non-Transaction XML processing.");

    if (pXMLErrorStr == NULL)
    {
      pXMLErrorStr = xmlStrdup ((const xmlChar *) pInXMLStr);
    }
    else
    {
      /*pXMLErrorStr = (XML_STR) T_malloc(T_strlen(pInXMLStr) + 1); */
      /*of_strcpy(pXMLErrorStr, pInXMLStr); */
      pXMLErrorStr = xmlStrcat (pXMLErrorStr, pInXMLStr);

      /*free(pInXMLStr); */
    }

  }

  XML_DebugArg ("pXMLErrorStr = ", pXMLErrorStr);
  return CNTRLXML_SUCCESS;

}

TR_PRIVATE t_uint32
CNTRLXMLGenerateTrans (TransactionSet_t ** pTrSet)
{
  TransactionSet_t *pTemptr, *pPrevRec = NULL, *pCurrRec;

  XML_Debug ("Entered ....");

  if (!pTrSet)
  {
    XML_Debug ("Bad Parameter...");
    return CNTRLXML_FAILURE;
  }

  /*
   *    Create the Transaction Set.
   */
  pTemptr = (TransactionSet_t *) T_calloc (1, sizeof (TransactionSet_t));
  if (!pTemptr)
  {
    XML_Debug ("Could not create Transaction list node...");
    return CNTRLXML_FAILURE;
  }

  /* Append to the list */
  if (pTransList == NULL)
  {
    pTransList = pTemptr;
    pTransList->pNext = NULL;
  }
  else
  {
    pCurrRec = pTransList;
    while (pCurrRec != NULL)
    {
      pPrevRec = pCurrRec;
      pCurrRec = pCurrRec->pNext;
    }
    pPrevRec->pNext = pTemptr;
    pTemptr->pNext = NULL;
  }

  if (MasterTransID == 0)
  {
  XMLTransInit (MasterTransID)}
  else
  {
  GenerateXMLTransID (MasterTransID)}
  pTemptr->MasterTrId = MasterTransID;

#ifdef XMLDEBUG
  printf ("Generated Master TransID is %ld***\r", pTemptr->MasterTrId);
#endif
  /* populate output */
  *pTrSet = pTemptr;

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLAddToTrList
 * Description		: This function adds the transaction information to
 * 					  the Transaction list with the given transaction ID
 * 					  and Module name.
 * Input			: fullTr		- Full transaction ID
 *				      modName	- Module
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
TR_PRIVATE t_int32
CNTRLXMLAddToTrList (TransactionSet_t * pTrSet,
                   PSPTransactionId_t * pFullTr, t_char8 * pModName)
{
  TransactionSet_t *pTravers;
  MODULE_t *pModuleNode;

  if (!pTrSet || !pFullTr || !pModName)
  {
    XML_Debug ("Invalid parameters.");
    return CNTRLXML_BADPARAM;
  }

  /*
   *      Look in the Transaction Set List
   */

  pTravers = pTransList;

  while (pTravers)
  {
    if (pTravers == pTrSet)
    {
      break;
    }
    pTravers = pTravers->pNext;
  }

  if (!pTravers)
  {
    XML_Debug ("Could not find Transaction list node...");
    return CNTRLXML_FAILURE;
  }
  /*
   *      Populate the Transaction Set contents.
   */

  /* Allocate a Module Node */
  pModuleNode = (MODULE_t *) T_calloc (1, sizeof (MODULE_t));

  if (!pModuleNode)
  {
    return CNTRLXML_FAILURE;
  }

  /* populate the node */
  of_strcpy (pModuleNode->ModuleName, pModName);

  /* Allocate memory for Module Tr ID */
  pModuleNode->ModTrID = T_malloc (pFullTr->ulLength);

  if (!pModuleNode->ModTrID)
  {

    free (pModuleNode);
    return CNTRLXML_FAILURE;
  }

  T_memcpy (pModuleNode->ModTrID, pFullTr->pModID, pFullTr->ulLength);
  pModuleNode->uiLen = pFullTr->ulLength;

  if (pTravers->pModuleListHead == NULL)
  {
    pTravers->pModuleListHead = pModuleNode;
    pTravers->pModuleListTail = pModuleNode;
  }
  else
  {
    pTravers->pModuleListTail->pNext = pModuleNode;
    pTravers->pModuleListTail = pModuleNode;
  }

  XML_DebugArg ("Added to Transactionset_t for the Module", pModName);
  XML_DebugArg ("pModuleNode->ModuleName=", pModName);
  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLFreeTrans
 * Description		: This function frees the given node in the Transaction
 *                    list (pTransList).
 * Input			: pointer to the node to be freed
 * Output		: none
 * Return value	: none
 ***********************************************************************/
TR_PRIVATE t_void
CNTRLXMLFreeTrans (TransactionSet_t * pTrSet)
{
  MODULE_t *pModule, *pPrevModule;
  TransactionSet_t *pTravers, *pPrevTrSet;

  /* Validate Input */
  if (!pTrSet)
  {
    return;
  }

  pModule = pTrSet->pModuleListHead;

  while (pModule)
  {
    /* Free the Module Transaction Id */
    free (pModule->ModTrID);

    pPrevModule = pModule;
    pModule = pModule->pNext;

    /* Free the Module Node */
    free (pPrevModule);
  }

  /* Now De-link the Transaction Node from the List */
  pTravers = pTransList;
  pPrevTrSet = pTravers;

  while (pTravers)
  {
    if (pTravers == pTrSet)
    {
      break;
    }

    pPrevTrSet = pTravers;
    pTravers = pTravers->pNext;
  }

  if (!pTravers)
  {
    /* Should not happen */
    return;
  }

  /* De-link from the list */
  if (pTravers == pTransList)
  {
    pTransList = NULL;
  }
  else
  {
    pPrevTrSet->pNext = pTravers->pNext;
  }

  free (pTravers);

  return;
}

#ifdef PSP_XPATH_SUPPORT
TR_PRIVATE t_int32
XPathPopulateValues (t_void * doc,
                     t_char8 * ValArray[],
                     t_uchar8 * uXPathExpr,
                     t_int32 recSize,
                     IXML_NODE_TYPE nodeType, t_void * xpathCtxt)
{
  int iErrCode = 0;
  int resCount = 0, i = 0;
  t_void *resultSet = NULL;

  XML_Debug ("Entered XPathPopulateValue ");

  iErrCode = PSPXPathExpressionEX (uXPathExpr, xpathCtxt, &resultSet);

  if (iErrCode != 0)
  {
    XML_Debug ("PSPXPathExpressionEX failed");
  }

  resCount = PSPXPathResCount (resultSet);

  /*if (resCount > 0 ) */
  for (i = 0; i < resCount; i++)
  {
    t_void *item = NULL;

    /*item = PSPXPathGetItemAt(resultSet, resCount -1 ); */
    item = PSPXPathGetItemAt (resultSet, i);

    if (item)
    {
      XML_DebugArg ("item->name =", ((IXML_Node *) item)->nodeName);
      XML_DebugArg ("FOUND RECORD =", uXPathExpr);

      if (nodeType == eATTRIBUTE)
      {
        XML_Debug ("SET ATTRIBUTES ");
        CNTRLXMLFillAttributeValues (&item, ValArray, recSize);
      }
      else if (nodeType == eELEMENT)
      {
        XML_Debug ("SET ELEMENT VALUE");
        iErrCode = CNTRLXMLSetElementValue (item, ValArray[0]);
      }

    }                           /*if */

  }                             /*if (resCount > 0) */

  PSPXPathFreeResultSet (resultSet);

  return T_SUCCESS;
}

TR_PRIVATE t_int32
PSPCreateXPathExpression (t_uchar8 * pIdRef, t_uchar8 * pOutXPathExpr)
{
  if (!pOutXPathExpr || !pIdRef)
  {
    XML_Debug ("Invalid parameters");
    return CNTRLXML_FAILURE;
  }
#if 0
  XML_Debug (pOutXPathExpr, "//*[@idref=\'%s\']", pIdRef);
#endif
  XML_DebugArg ("pOutXPathExpr =", pOutXPathExpr);

  return CNTRLXML_SUCCESS;
}
#endif /*PSP_XPATH_SUPPORT */
TR_PRIVATE t_void
CNTRLXMLProcessBrowserTag (t_void * pCurrentCommand)
{
  t_uchar8 ClmCnt = 0;
  t_int32 iErrCode = 0;
  t_char8 *declStr = NULL;
  t_void *pNextCommand = NULL;
  Attribs FileAttribs[NO_OF_FILE_ATTRIBS] = {
    {"sredirect", ""}
    ,
    {"fredirect", ""}
    ,
    {"successxsl", ""}
    ,
    {"failurexsl", ""}
  };

  iErrCode = CNTRLXMLGetFirstChildByTagName (BROWSER_TAG,
                                           pCurrentCommand, &pNextCommand);
  if (pNextCommand)
  {
    pCurrentCommand = pNextCommand;

    for (ClmCnt = 0; ClmCnt < NO_OF_FILE_ATTRIBS; ClmCnt++)
    {
      iErrCode = CNTRLXMLGetAttributeValue (FileAttribs[ClmCnt].AttribName,
                                          pCurrentCommand,
                                          FileAttribs[ClmCnt].AttribValue);

      if ((iErrCode != CNTRLXML_SUCCESS) && (iErrCode != CNTRLXML_NOTFOUND))
      {
        XML_Debug ("UNABLE TO FIND Browser attributes");
        return;
      }
      XML_DebugArg ("AttribName = ", FileAttribs[ClmCnt].AttribName);
      XML_DebugArg ("AttribValue = %s", FileAttribs[ClmCnt].AttribValue);
    }                           /*for */
  }                             /*if */

  iErrCode = CNTRLXMLtoBuffer (pNextCommand, &pBrowserTag);

  if (iErrCode != CNTRLXML_SUCCESS)
  {
    XML_Debug ("UNABLE TO FIND Browser attributes...");
  }
  XML_DebugArg ("pBrowserTag =", pBrowserTag);

  CNTRLXMLSetXSLFile (FileAttribs[SUCCESS_XSLREF_INX].AttribValue, &declStr);

  PSPReturnXMLResultString (NULL, declStr);
  XMLBUFFERFREE (declStr);
  declStr = NULL;

  PSPReturnXMLResultString (NULL, resultTagBegin);

  CNTRLXMLSetXSLFile (FileAttribs[FAILURE_XSLREF_INX].AttribValue, &declStr);

  PSPReturnXMLErrorString (NULL, declStr);
  XMLBUFFERFREE (declStr);
  declStr = NULL;
  PSPReturnXMLErrorString (NULL, resultTagBegin);
}

TR_PRIVATE t_int32
CNTRLXMLParseModsets (t_void * docPtr)
{
  t_int32 iErrCode = 0;
  t_char8 CommandName[NAME_MAX_LEN];
  t_void *pCurrentCommand = NULL, *pNextCommand = NULL, *nodePtr = NULL;
  t_void *pRootNode = NULL;
  t_char8 *pID = NULL;
  TransactionSet_t *pTrSet = NULL;
  PSPTransactionId_t ModFullTrID;
  /*t_char8 *outStr=NULL; */
  t_bool8 CommitSuccess = FALSE;
#ifdef PSP_XPATH_SUPPORT
  t_void *xpCtxt = NULL;
  t_int32 tempFlag = 1;
#endif

  XML_Debug ("Entered.");
#ifdef PSP_XPATH_SUPPORT
  iErrCode = PSPNewXPathContext (docPtr, &xpCtxt);
  if (iErrCode != CNTRLXML_SUCCESS || xpCtxt == NULL)
  {
#ifdef XMLDEBUG
    XML_Debug (": Could not create XPath Context.");
#endif
    return CNTRLXML_FAILURE;
  }
#endif /*PSP_XPATH_SUPPORT */
  /*
   * Get the First Command for this Module. e.g  <MODULE_Method1>
   *  or <Transaction>
   */
  iErrCode = CNTRLXMLGetNextCommand (docPtr, NULL, &pNextCommand, CommandName);
  if (iErrCode != CNTRLXML_SUCCESS)
  {
    XML_Debug ("Could not get CNTRLXMLGetNextCommand...");
    return CNTRLXML_FAILURE;
  }
  XML_DebugArg ("CommandName =", CommandName);

  CNTRLXMLGetRootNode (docPtr, &pRootNode);

  /*
   * Process BROWSER_TAG
   */
  CNTRLXMLProcessBrowserTag (pRootNode);

  pCurrentCommand = pNextCommand;
  pNextCommand = NULL;
  CNTRLXMLGetNextCommand (NULL, pCurrentCommand, &pNextCommand, CommandName);
  XML_DebugArg ("CommandName1 = ", CommandName);

  while (pNextCommand)
  {
    t_char8 *pModName = NULL;
    t_char8 *pMethodName = NULL;
    t_void *funPtrs = NULL;
    t_void *pIDAttrib = NULL;
    XML_CALLBACK_HANDLER pXmlCbfun;
    XML_MODULE_START pModStartFun;
    t_uchar8 *pIdRef = NULL;
#ifdef PSP_XPATH_SUPPORT
    t_uchar8 XPathExpr[NAME_MAX_LEN];
#endif /*PSP_XPATH_SUPPORT */
    pCurrentCommand = pNextCommand;
    pNextCommand = NULL;

    if (T_strcmp (CommandName, TRANSACTION_TAG) == 0)
    {
      bTransFlag = TRUE;
      nodePtr = pCurrentCommand;
      pNextCommand = NULL;

      /*
       *  Generate Global Transaction ID
       */
      CNTRLXMLGenerateTrans (&pTrSet);
      PSPReturnXMLResultString (NULL, startTransTag);
      PSPReturnXMLErrorString (NULL, failureTransTag);
      bTransFlag = TRUE;
      if (!pTrSet)
      {
        return CNTRLXML_FAILURE;
      }

      iErrCode = CNTRLXMLGetFirstChildElement (nodePtr, &pNextCommand);
      if (!pNextCommand)
        XML_Debug ("NextCommand is Null CNTRLXMLGetFirstChildElement");
      pCurrentCommand = pNextCommand;
      pNextCommand = NULL;
    }

    iErrCode = CNTRLXMLGetNodeName (pCurrentCommand, CommandName);

    if (iErrCode != 0)
    {
      XML_Debug ("Failure is returned from  CNTRLXMLGetNodeName ");
      return CNTRLXML_FAILURE;
    }

    pModName = strtok (CommandName, "_");

    XML_DebugArg ("pModName = %s", pModName);
    /*
     *  Check if the MODULE is registered and get the function pointers.
     */

    iErrCode = CNTRLXMLGetRegHandler (pModName, &funPtrs);

    if (iErrCode != CNTRLXML_SUCCESS || !funPtrs)
    {
      XML_DebugArg (pModName, "is not registred");
      return CNTRLXML_FAILURE;
    }

    if (IsValidTransaction (pTrSet))
    {
      pMethodName = strtok (NULL, "_");
      XML_DebugArg ("pMethodName = ", pMethodName);

      if (T_strcmp (pMethodName, "Start") == 0)
      {
        ModFullTrID.ulGlobalID = pTrSet->MasterTrId;
        ModFullTrID.pModID = NULL;
        ModFullTrID.ulLength = 0;
/*
#ifdef XMLDEBUG
        XML_Debug ("FullTr.ulGlobalID= %ld  for  pModName = %s .",
                   FullTr.ulGlobalID, pModName);
#endif
                                                        *//*
         *  Call Start Transaction method
         */
        pModStartFun = ((XMLCallBakFuns_t *) funPtrs)->modStartCBFun;
        iErrCode = (*pModStartFun) (&ModFullTrID, pCurrentCommand);

        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Start returned Failure.");
          return CNTRLXML_FAILURE;
        }

        /* The Call to Start would have filled the length
           of the Module Transaction Id. We need to malloc
           the size and call Start again. */

        ModFullTrID.pModID = T_calloc (1, ModFullTrID.ulLength);
        if (!ModFullTrID.pModID)
        {
          XML_Debug ("ModFullTrID.ModID memory allocation Failure.");
          return CNTRLXML_FAILURE;
        }

        iErrCode = (*pModStartFun) (&ModFullTrID, pCurrentCommand);

        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Start returned Failure.");
          return CNTRLXML_FAILURE;
        }

        /*
         *  Add the module to the Transaction list.
         */
        pModName = strtok (CommandName, "_");
        XML_Debug ("Callback Handler  returned  Success...");

        iErrCode = CNTRLXMLAddToTrList (pTrSet, &ModFullTrID, pModName);
        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_DebugArg (pModName, "Could not added to the transaction list");
          return CNTRLXML_FAILURE;
        }

        /*
         *  Get next command
         */
        CNTRLXMLGetNextCommand (NULL, pCurrentCommand,
                              &pNextCommand, CommandName);
        pCurrentCommand = pNextCommand;
        pNextCommand = NULL;
      }
    }
    /*
     *  Call XMLCallbackHandler
     */
    XML_Debug ("Call XML Callback handler.");

    pXmlCbfun =
      (XML_CALLBACK_HANDLER) ((XMLCallBakFuns_t *) funPtrs)->xmlCBFun;

    if (pXmlCbfun == NULL)
    {
      XML_Debug ("XML Callback handler is not registered.");
      return CNTRLXML_FAILURE;
    }
    iErrCode = CNTRLXMLGetTagElementFirstAttrib (pCurrentCommand,
                                               &pIDAttrib, ID_ATTRIB,
                                               &pIdRef);
    if (iErrCode != CNTRLXML_SUCCESS)
    {
      XML_Debug ("IDRef not found.");
    }
    if (bTransFlag)
      iErrCode = (*pXmlCbfun) (&ModFullTrID, pCurrentCommand, &pID);
    else
      iErrCode = (*pXmlCbfun) (NULL, pCurrentCommand, &pID);
#if 0
    if (tempFlag == 1)
    {
      pID = xmlStrdup ((const xmlChar *) "RecID1");     /*TO BE REMOVED */
    }
#endif
#ifdef PSP_XPATH_SUPPORT
    if (pIdRef != NULL && pID != NULL)
    {
      t_char8 *xmlstr = NULL;
      /*
       * Populate idref and replace all such references.
       */

      XML_DebugArg ("BEFORE:: pIdRef = %s", pIdRef);

      if (PSPCreateXPathExpression (pIdRef, XPathExpr) == CNTRLXML_SUCCESS)
      {
        t_char8 *ValArray[] = { pID };
        iErrCode =
          XPathPopulateValues (docPtr, ValArray, XPathExpr, 1, eELEMENT,
                               xpCtxt);
        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Failed to process idref...");
        }
      }

      CNTRLXMLtoBuffer (docPtr, &xmlstr);
      XML_DebugArg ("PROCESS XPATH: xmlstr = %s", xmlstr);
      free (pID);
      free (pIdRef);
      pIdRef = NULL;
      pID = NULL;
      tempFlag = 0;
    }

#endif /*PSP_XPATH_SUPPORT */
    if (iErrCode != CNTRLXML_SUCCESS)
    {
      XML_Debug ("Function returned failure...");

      if (IsValidTransaction (pTrSet))
      {
        /*
         *  Call Abort process for each module.
         */
        iErrCode = CNTRLXMLAbortTransactions (pTrSet);
        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Failed to Abort transactions...");
        }
      }
      return CNTRLXML_FAILURE;
    }

    CNTRLXMLGetNextCommand (NULL, pCurrentCommand, &pNextCommand, CommandName);

    if (IsValidTransaction (pTrSet) && pNextCommand == NULL)
    {
      /* We have reached the end this Group Transaction Set.
         Call Ready to Commit for all the modules in this
         transaction set */

      iErrCode = CNTRLXMLReadytoCommit (pTrSet);
      if (iErrCode != CNTRLXML_SUCCESS)
      {
        /* Abort transaction */
        iErrCode = CNTRLXMLAbortTransactions (pTrSet);
        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Failed to Abort transactions...");
        }
        return CNTRLXML_FAILURE;
      }

      /* Call Commit for all the modules in this
         transaction set */
      iErrCode = CNTRLXMLCommit (pTrSet);
      if (iErrCode != CNTRLXML_SUCCESS)
      {
        /* Abort transaction */
        iErrCode = CNTRLXMLAbortTransactions (pTrSet);
        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("Failed to Abort transactions...");
        }
        return CNTRLXML_FAILURE;
      }
      CommitSuccess = TRUE;
      CNTRLXMLFreeTrans (pTrSet); /*Free the Transaction List */
      PSPReturnXMLResultString (NULL, endTransTag);

      /* Reset */
      pTrSet = NULL;

      /* Look for the next Group Transaction Set */
      CNTRLXMLGetParentElement (pCurrentCommand, &pNextCommand);
      pCurrentCommand = pNextCommand;
      pNextCommand = NULL;
      CNTRLXMLGetNextCommand (NULL, pCurrentCommand, &pNextCommand,
                            CommandName);
    }
  }                             /*while */

#ifdef PSP_XPATH_SUPPORT
  if (xpCtxt)
  {
    PSPXPathFreeContext (xpCtxt);
  }
#endif
  if (CommitSuccess == TRUE)
  {

    /*outStr = (t_char8*) T_malloc(T_strlen(xmlResDeclare)
       + T_strlen(SREDIRECT_URL)
       + T_strlen(xmlResEndDecl));

       CNTRLXMLGenerateResultString("success.xml", outStr);
       PSPReturnXMLResultString(NULL, outStr);
       free(outStr); */

    /*CNTRLXMLGenerateResultString(
       (t_char8*)FileAttribs[SUCCESS_ATTRIB_INX].AttribValue,
       (t_char8*)FileAttribs[SREDIRECT_ATTRIB_INX].AttribValue, &outStr);
       PSPReturnXMLResultString(NULL, outStr);
       free(outStr); */

  }
  XML_Debug ("No More Next Command Found ");

  if (pBrowserTag)
  {
    PSPReturnXMLResultString (NULL, pBrowserTag);
    XMLBUFFERFREE (pBrowserTag);
  }
  PSPReturnXMLResultString (NULL, resultTagEnd);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLCommit
 * Description		: This function calls Transaction Commit for all the Modules.
 * Input			: none
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_FAILURE
 ***********************************************************************/
TR_PRIVATE t_int32
CNTRLXMLCommit (TransactionSet_t * pTrSet)
{
  MODULE_t *pModule;
  t_int32 iErrCode = 0;
  t_void *funPtrs = NULL;
  XML_MOD_TRANS_HNDLR pComitFun;
  PSPTransactionId_t FullTr;
  /*t_char8 *outStr=NULL; */

  XML_Debug ("Entered");

  /* Validate Input */
  if (!pTrSet)
  {
    return CNTRLXML_FAILURE;
  }

  pModule = pTrSet->pModuleListHead;

  XML_DebugArg ("pModule->ModuleName =", pModule->ModuleName);
  while (pModule)
  {
    CNTRLXMLGetRegHandler (pModule->ModuleName, &funPtrs);

    /*
     *  Call Module's Transaction commit method.
     */

    pComitFun = ((XMLCallBakFuns_t *) funPtrs)->pCommitFun;

    FullTr.ulGlobalID = pTrSet->MasterTrId;
    FullTr.pModID = pModule->ModTrID;
    FullTr.ulLength = pModule->uiLen;

    iErrCode = (*pComitFun) (&FullTr);

    if (iErrCode != CNTRLXML_SUCCESS)
    {
      return CNTRLXML_FAILURE;
    }
    pModule = pModule->pNext;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLReadytoCommit
 * Description		: This function calls Transaction Ready to Commit
                      for all the Modules in the given Transaction Set.
 * Input			: none
 * Output		    : none
 * Return value	    : CNTRLXML_SUCCESS, CNTRLXML_FAILURE
 ***********************************************************************/
TR_PRIVATE t_int32
CNTRLXMLReadytoCommit (TransactionSet_t * pTrSet)
{
  MODULE_t *pModule;
  t_int32 iErrCode = 0;
  t_void *funPtrs = NULL;
  XML_MOD_TRANS_HNDLR pReadToCommitFun;
  PSPTransactionId_t FullTr;

  XML_Debug ("Entered**\r");
  /* Validate Input */
  if (!pTrSet)
  {
    return CNTRLXML_FAILURE;
  }

  pModule = pTrSet->pModuleListHead;

  while (pModule)
  {
    CNTRLXMLGetRegHandler (pModule->ModuleName, &funPtrs);

    /*
     *  Call Module's Transaction ready to commit method.
     */

    pReadToCommitFun = ((XMLCallBakFuns_t *) funPtrs)->pReadyToCommitFun;

    FullTr.ulGlobalID = pTrSet->MasterTrId;
    FullTr.pModID = pModule->ModTrID;
    FullTr.ulLength = pModule->uiLen;

    iErrCode = (*pReadToCommitFun) (&FullTr);

    if (iErrCode != CNTRLXML_SUCCESS)
    {
      return CNTRLXML_FAILURE;
    }
    pModule = pModule->pNext;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLAbortTransactions
 * Description		: This function aborts all the Transactions
 * Input			: none
 * Output		: none
 * Return value	: none
 ***********************************************************************/
TR_PRIVATE t_int32
CNTRLXMLAbortTransactions (TransactionSet_t * pTrSet)
{
  MODULE_t *pModule;
  MODULE_t *pTempModule = NULL, *pTemp = NULL, *pTempHead =  NULL;
  t_int32 iErrCode = 0;
  t_void *funPtrs = NULL;
  XML_MOD_TRANS_HNDLR pModTrAbortFun;
  PSPTransactionId_t FullTr;

  XML_Debug ("Entered");
  /* Validate Input */
  if (!pTrSet)
  {
    return CNTRLXML_FAILURE;
  }
 
  /* Reversing the list to call abort functions in the reverse order 
     in which they are started - BUG:54230 */ 
  pTempModule = pTrSet->pModuleListHead;
  while(pTempModule)
  {
    if(pTempHead == NULL)
    { 
      pTempHead = pTempModule;
      pTempModule = pTempModule->pNext;
      pTempHead->pNext = NULL;
    }
    else
    {
      pTemp = pTempModule;
      pTempModule = pTempModule->pNext;
      pTemp->pNext = pTempHead;
      pTempHead = pTemp;
    }
  }

  /* pModule = pTrSet->pModuleListHead; */
  pModule = pTempHead;
  while (pModule)
  {
    iErrCode = CNTRLXMLGetRegHandler (pModule->ModuleName, &funPtrs);

    /*
     *  Call Module's Transaction ready to commit method.
     */

    pModTrAbortFun = ((XMLCallBakFuns_t *) funPtrs)->pAbortFun;

    FullTr.ulGlobalID = pTrSet->MasterTrId;
    FullTr.pModID = pModule->ModTrID;
    FullTr.ulLength = pModule->uiLen;

    XML_DebugArg ("Abort Transactions for the Module", pModule->ModuleName);
    iErrCode = (*pModTrAbortFun) (&FullTr);

    if (iErrCode != CNTRLXML_SUCCESS)
    {
      CNTRLXMLFreeTrans (pTrSet); /*Free the Transaction List */
      PSPReturnXMLErrorString (NULL, endTransTag);
      bTransFlag = FALSE;
      return CNTRLXML_FAILURE;
    }
    pModule = pModule->pNext;
  }

  CNTRLXMLFreeTrans (pTrSet);     /*Free the Transaction List */

  PSPReturnXMLErrorString (NULL, endTransTag);
  bTransFlag = FALSE;
  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLHandler
 * Description		: This function is the main XML handler function to
 *				   process the XML request and response.
 * Input			: XML request buffer
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_FAILURE
 ***********************************************************************/
t_int32
CNTRLXMLHandler (t_char8 * pInXMLBuffer, t_char8 ** pOutXMLStr)
{
  t_uchar8 iErrCode = 0;
  t_void *pXmlDoc = NULL;

  /*
   * Load XML DOM tree
   */
  XML_Debug ("Entered");

  if ((iErrCode = CNTRLXMLParseXML (pInXMLBuffer, &pXmlDoc)) != CNTRLXML_SUCCESS)
  {
    XML_Debug ("CNTRLXMLParseXML Failed");
    return CNTRLXML_FAILURE;
  }

  /*
   *  Parse the XML DOM tree for,
   *  - Transaction support tags
   *  - Module action tags
   */
  bTransFlag = FALSE;
  iErrCode = CNTRLXMLParseModsets (pXmlDoc);

  if (iErrCode != CNTRLXML_SUCCESS)
  {
    XML_Debug ("CNTRLXMLParseModsets Failed");
    if (pBrowserTag)
    {
      if (bTransFlag)
      {
        PSPReturnXMLErrorString (NULL, endTransTag);
      }
      PSPReturnXMLErrorString (NULL, pBrowserTag);
      PSPReturnXMLErrorString (NULL, resultTagEnd);
      XMLBUFFERFREE (pBrowserTag);
    }
    if (pXMLErrorStr)
    {
      *pOutXMLStr = xmlStrdup ((const xmlChar *) pXMLErrorStr);
      free (pXMLErrorStr);
      pXMLErrorStr = NULL;
                  /**pOutXMLStr = (t_char8*) T_malloc(T_strlen(pXMLErrorStr));
		  of_strcpy(*pOutXMLStr, pXMLErrorStr);*/
      /*free(pXMLErrorStr);
         pXMLErrorStr = NULL; */
    }
    else
    {
      of_strcpy (*pOutXMLStr, "No Data");
    }
    if (pXMLResultStr)
    {
      free (pXMLResultStr);
      pXMLResultStr = NULL;
    }

    return CNTRLXML_FAILURE;
  }
        /**pOutXMLStr = (t_char8*) T_malloc(T_strlen(pXMLResultStr));
	of_strcpy(*pOutXMLStr, pXMLResultStr);*/
  *pOutXMLStr = xmlStrdup ((const xmlChar *) pXMLResultStr);
  XML_DebugArg ("pXMLResultStr=", pXMLResultStr);
  XMLBUFFERFREE (pXMLResultStr);
  XML_Debug ("After freeing pXMLResultStr");

  pXMLResultStr = NULL;

  if (pXMLErrorStr)
  {
    XMLBUFFERFREE (pXMLErrorStr);
    pXMLErrorStr = NULL;
  }
  return CNTRLXML_SUCCESS;

}

t_void
CNTRLXMLGenerateResultString (t_char8 * pInXMLFile,
                            t_char8 * url, t_char8 ** ppOutXMLResultStr)
{

  *ppOutXMLResultStr = xmlStrdup ((const xmlChar *) redirectURLTag);
  *ppOutXMLResultStr = xmlStrcat (*ppOutXMLResultStr, url);
  *ppOutXMLResultStr = xmlStrcat (*ppOutXMLResultStr, redirectURLEndTag);

  XML_DebugArg ("XML-STR = %s", *ppOutXMLResultStr);
}

#endif /* PSP_XML_SUPPORT */
#endif /* # PSP_HTTPD_SUPPORT && PSP_HTTPD_CBK_SUPPORT */
