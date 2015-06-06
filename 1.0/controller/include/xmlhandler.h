#ifdef PSP_XML_SUPPORT
#ifndef __XMLHANDLER_H__
#define __XMLHANDLER_H__

#define MAX_MACADDR_LEN 30
#define MAX_NAME_LEN	30
#define NAME_MAX_LEN 30



/**************************************
 * * * *  Include Header files  * * * *
 **************************************/

#include "ixml.h"
#include "sll.h"
#include "xmlstring.h"

#define MODULE_NAME_MAX_LEN  32
#define BROWSER_TAG "Browser"
#define XMLBUFFERFREE free
/****************************************************
 * * * *  T y p e    D e c l a r a t i o n s  * * * *
 ****************************************************/
/*
uint32_t uiMaxDatapaths_g;
uint32_t uiMaxExpVendors_g;
uint32_t uiMaxTables_g;
uint32_t uiNoofmsgpools_g;
uint32_t uiStaticFds_g;
uint32_t uiTrnprtRecbuf_g;
*/

typedef uint32_t TRANSACTION_ID;

typedef struct MODULE_s MODULE_t;
typedef struct TransactionSet_s TransactionSet_t;

typedef void* DOM;
typedef char* XML_STR;


/*New Transaction related Structures*/

struct MODULE_s
{
  unsigned char ModuleName[MODULE_NAME_MAX_LEN];
  void  *ModTrID;
  uint16_t  uiLen;
  MODULE_t  *pNext;
};

struct TransactionSet_s
{
  TRANSACTION_ID MasterTrId;
  MODULE_t *pModuleListHead;
  MODULE_t *pModuleListTail;
  TransactionSet_t *pNext;
};

typedef enum CNTRLXMLRegFunctions
{
 CNTRLXML_MOD_START,
 CNTRLXML_MOD_CALLBAK_HNDLR,
 CNTRLXML_MOD_READYToCOMMIT,
 CNTRLXML_MOD_ABORT,
 CNTRLXML_MOD_COMMIT
}CNTRLXMLRegFunctions_t;


typedef struct XMLCallBakFuns_s XMLCallBakFuns_t;
struct XMLCallBakFuns_s
{
	XML_MODULE_START modStartCBFun;
	XML_CALLBACK_HANDLER xmlCBFun;
	XML_MOD_TRANS_HNDLR pReadyToCommitFun;
	XML_MOD_TRANS_HNDLR pCommitFun;
	XML_MOD_TRANS_HNDLR pAbortFun;
};


/********************************************************
 * * * *  F u n c t i o n    P r o t o t y p e s  * * * *
 ********************************************************/

int32_t XmlRegModuleInit(void);


/***********************************************************************
 * Function Name : CNTRLXMLCallRegXmlURL
 * Description   : This function checks if regUrl exists and
 				   calls corresponding registed function by passing
				   the regUrl and httpRequest request.
 * Input         : regUrl - File Name or URL
 * 				   httpRequest - HttpRequest object
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
char CNTRLXMLCallRegXmlURL(char *regUrl,HttpRequest *httpRequest);

/***********************************************************************
 * Function Name : CNTRLXMLGetRegHandler
 * Description   : This function retrives registered callback functions
 				   for the given Module name.
 * Input         : pInModule	- Module name
 * Output		 : pOutFunLst	- Registered function list
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
char CNTRLXMLGetRegHandler(char *pInModule, void **pOutFunLst);

/**********************************************************************
 * Function Name: PSPInitXmlModsetTable
 * Desription	: This functions Initializes the HashTable for Modset
 * Input	: None
 * Output	: xmlHashTableptr
 * Return Value	: None
 ***********************************************************************/
int32_t  PSPInitXmlModsetTable(void);

/***********************************************************************
 * Function Name: CNTRLXMLRegisterModsets
 * Desription	: This functions Register Modset entry to Hash Table
 * Input	: pModsetName - Modset Name
 * 		  funptr      - Call back function pointer
 * Output	: Success or Failure
 * Return Value	: None
 ***********************************************************************/
int32_t CNTRLXMLRegisterModsets(char *pModsetName, void *funptr);

/***********************************************************************
 * Function Name	: CNTRLXMLHandler
 * Description		: This function is the main XML handler function to
 *				   process the XML request and response.
 * Input			: XML request buffer
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_FAILURE
 ***********************************************************************/
int32_t CNTRLXMLHandler(char *pInXMLBuffer, char **pOutXMLStr);


/****************************************
 *  Desription   : This functions Initializes the HashTable for Modset
 *  Input        : None
 *  Output       : xmlHashTableptr
 *  Return Value : None
 *  ***************************************/
int32_t InitXmlModHashTable(void);


/******************************************
 *   Function Name: Xml_Register_Modset
 *   Desription   : This functions Register Modset entry to Hash Table
 *   Input        : pModsetName - Modset Name
 *                   funptr      - Call back function pointer
 *   Output       : Success or Failure
 *   Return Value : None
 ********************************************/
int32_t Xml_Register_Modset(char *pModsetName, void *funptr);
void PSPXmlHttpInit(void);

void CNTRLXMLGenerateResultString(char *xmlFile,
								   char *url,
								   char **outStr);
/***********************************************************************
 * Function Name : CNTRLXMLGETReqHandler
 * Description   : This function calls the registered callback functions
 				   against the XML file URL
 * Input         : regUrl - XML file URL
 * 				   pAppData - optional parameter for any application data
 * 				   			  can be NULL.
 *				   pOutXMLStr - Response XML buffer
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
char CNTRLXMLGETReqHandler(char *pInXMLFile,void *pAppData, char **pOutXMLStr);

#endif /* __XMLHANDLER_H__ */
#endif /* PSP_XML_SUPPORT */

