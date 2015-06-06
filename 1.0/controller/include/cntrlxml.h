#ifdef OF_XML_SUPPORT
#ifndef __CNTRLXML_H__
#define __CNTRLXML_H__
/**************************************
 * * * *  Include Header files  * * * *
 **************************************/

#include "stdint.h"
#include "ixml.h"
#include "ixmlmembuf.h"
#include "ixmlparser.h"


uint32_t uiCntrlMaxDatapaths_g;
uint32_t uiCntrlMaxExpVendors_g;
uint32_t uiCntrlMaxTables_g;
uint32_t uiCntrlNoofmsgpools_g;
uint32_t uiCntrlStaticFds_g;
uint32_t uiCntrlTrnprtRecbuf_g;

typedef enum CNTRLXMLErrcodes
{
	CNTRLXML_SUCCESS,
	CNTRLXML_BADPARAM,
	CNTRLXML_NOTFOUND,
	CNTRLXML_FAILURE,
	CNTRLXML_NOCMD,
	CNTRLXML_INVALIDCMD,
	CNTRLXML_NOATTRIB,
    	CNTRLXML_NORECORDS,
    	CNTRLXML_LOAD_SCHEMA_FAILURE,
    	CNTRLXML_XML_SCHEMA_VAL_FAILURE,
	CNTRLXML_MAX_ERR_NUM
}CNTRLXMLErrcodes_t;

#define PSP_MAX_XML_ATTRIB_VALUE_LEN 255
typedef struct
{
  unsigned char AttribName[48];
  unsigned char AttribValue[PSP_MAX_XML_ATTRIB_VALUE_LEN+1];
}XMLAttribs;
#define XSLREF "xslref"
#define CNTRLXML_ERR_MESS_MAX_LEN 50
#define CNTRLXML_MAX_XSL_FILENAME_LEN 30

#define CNTRLXML_ACTIONTAG	"action"
#define CNTRLXML_INCLUDETAG "INCLUDE"
#define CNTRLXML_XMLREF_ATTRIBUTE "xmlref"

/*************************************
 * * * *  Function Prototypes  * * * *
 *************************************/

#ifdef PSP_XML_INTERNATIONALIZATION_SUPPORT
int8_t PSPXmlSetLanguage(char *pLangName);
#endif /*PSP_XML_INTERNATIONALIZATION_SUPPORT*/

char* PSPXmlGetLanguage( void );

/***********************************************************************
 * Function Name : CNTRLXMLParseXML
 * Description   : This function parses an XML string
 				   and populates a DOM tree	(pOutOpaqueXMLDoc)
 * Input         : pInXMLBuff - XML String
 * Output		   pOutOpaqueXMLDoc - DOM tree
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLParseXML(unsigned char *pInXMLBuff, void **pOutOpaqueXMLDoc);

/***********************************************************************
 * Function Name : CNTRLXMLLoadXML
 * Description   : This function parses an XML file
 				   and populates a DOM tree	(pOutOpaqueXMLDoc)
 * Input         : pInXMLFile - XML String
 * Output		   pOutOpaqueXMLDoc - DOM tree
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLLoadXML(unsigned char *pInXMLFile, void **pOutOpaqueXMLDoc);

/***********************************************************************
 * Function Name : CNTRLXMLHasIncludeTags
 * Description   : This function checks for existance of  INCLUDE tags
 * 				   from given XML DOM tree.
 * Input         : pInOpaqueDoc - DOM tree
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int32_t CNTRLXMLHasIncludeTags(void *pInOpaqueXMLDoc);

/***********************************************************************
 * Function Name : CNTRLXMLProcessIncludeTags
 * Description   : This function process the XML DOM tree for INCLUDE tags
 *				   and converts it to String buffer(pInXMLBuff)
 * Input         : pInOpaqueDoc - DOM tree
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int32_t CNTRLXMLProcessIncludeTags(void **pInOpaqueDoc);

/***********************************************************************
 * Function Name : CNTRLXMLtoBuffer
 * Description   : This function parses an XML DOM tree
 				   and converts it to String buffer(pInXMLBuff)
 * Input         : pInOpaqueXMLDoc - DOM tree
 * Output		   pInXMLBuff - XML String
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t CNTRLXMLtoBuffer(void *pInOpaqueXMLDoc, char **pInXMLBuff);

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
/*  
int8_t CNTRLXMLCallRegXmlURL(char *regUrl,HttpRequest *httpRequest);
*/

/***********************************************************************
 * Function Name : CNTRLXMLGetRegHandler
 * Description   : This function retrives registered callback functions
 				   for the given Module name.
 * Input         : pInModule	- Module name
 * Output		 : pOutFunLst	- Registered function list
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetRegHandler(char *pInModule, void **pOutFunLst);

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
 * Function Name : CNTRLXMLGetFirstChildByTagName
 * Description   : This function retrieves the first Record
 				   with name equal to the passed tag name.
 * Input         : pInTagName - tag name
 				   pInOpaqueNode - Input Element
 * Output        : pOutOpaque - Out Node
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetFirstChildByTagName(unsigned char *pInTagName,
								   	void *pInOpaqueNode,
								    void **pOutOpaqueNode);

/***********************************************************************
 * Function Name : CNTRLXMLGetNextSiblingByTagName
 * Description   : This function retrieves the Next sibling element
 *                 with name equal to passed tagname.
 * Input         : pInOpaqueNode - current Node.
 * Output        : pOutOpaqueElement - new Node list element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetNextSiblingByTagName(unsigned char *pInTagName,
									 void *pInOpaqueNode,
						 			 void **pOutOpaqueElement);

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildElement
 * Description   : This function retrieves the first Child element of
 				   the given element(pInOpaqueElement).
 * Input         : pInOpaqueElement - Parent Element
 * Output        : pOutOpaqueElement - First Child element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetFirstChildElement(void *pInOpaqueElement,
								  void **pOutOpaqueElement);

/***********************************************************************
 * Function Name : CNTRLXMLGetNextSibling
 * Description   : This function returns the Next Sibling
 * Input         : pInOpaqueElement - current child element
 * Output        : pOutOpaqueElement - next sibling element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetNextSibling(void *pInOpaqueElement,
					        void **pOutOpaqueElement);

/***********************************************************************
 * Function Name : CNTRLXMLGetTagElementFirstAttrib
 * Description   : This function retrieves the first Attribute of the
				   passed element.
 * Input         : pInOpaqueElement - Node list element
 * Output        : pOutOpaqueAttrib - Attribute Node
				   pOutAttribName - Attribute Name
				   pOutAttribValue - Attribute Value
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetTagElementFirstAttrib(void *pInOpaqueElement,
	 							   	  void **pOutOpaqueAttrib,
	 							   	  unsigned char *pOutAttribName,
								   	  unsigned char **pOutAttribValue);

/***********************************************************************
 * Function Name : CNTRLXMLGetTagElementNextAttrib
 * Description   : This function retrieves the next Attribute of the
 				   passed element.
 * Input         : pInOpaqueAttrib - Current Attribute
 * Output        : pOutOpaqueAttrib - Next Attribute
 				   pOutAttribName - Next Attribute Name
 				   pOutAttribValue - Next Attribute Value
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetTagElementNextAttrib(void *pInOpaqueAttrib,
								  	 void **pOutOpaqueAttrib,
								  	 unsigned char *pOutAttribName,
								  	 unsigned char *pOutAttribValue);

/***********************************************************************
 * Function Name : CNTRLXMLGetTotNoOfElements
 * Description   : This function retrieves the total number of elements
 				   in the given element list.
 * Input         : pInOpaqueElement - Element List
 * Output		   pOutCount - Total count of elements
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetTotNoOfElements(void *pInOpaqueElement, int32_t *pOutCount);

/***********************************************************************
 * Function Name : CNTRLXMLGetTotNoOfAttribs
 * Description   : This function retrieves the total number of
 				   attriutes in the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutCount - Total count of attributes
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetTotNoOfAttribs(void *pInOpaqueElement, int32_t *pOutCount);

/***********************************************************************
 * Function Name : CNTRLXMLGetNodeValue
 * Description   : This function retrieves the value of
 				   the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutValue - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLGetNodeValue(void *pInOpaqueElement, unsigned char *pOutValue);

/***********************************************************************
 * Function Name : CNTRLXMLGetNodeValueByLength
 * Description   : This function retrieves the value of
 				   the given element.
 * Input         : pInOpaqueElement - Given Element
 * 				   len - String length
 * Output		   pOutValue - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/
int8_t CNTRLXMLGetNodeValueByLength(void *pInOpaqueElement, 
								  unsigned char *pOutValue,
								  uint32_t ulen);
/***********************************************************************
 * Function Name : CNTRLXMLNCopyNodeValue 
 * Description   : This function retrieves the 'len' Bytes  value of
 				   the given element .
 * Input         : pInOpaqueElement - Given Element
 * 					 pOutValue - Output string
 * 				   len - String length
 * Output		   pOutValue - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/
int8_t
CNTRLXMLNCopyNodeValue (void * pInOpaqueElement,
                      unsigned char * pOutValue, uint32_t ulen);
/***********************************************************************
 * Function Name : CNTRLXMLGetNodeName
 * Description   : This function retrieves the name of
 *                 the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutName - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 * **********************************************************************/
int8_t CNTRLXMLGetNodeName(void *pInOpaqueElement, unsigned char *pOutName);

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLDoc
 * Description   : This function frees the XML Doc returned from
 *                 the function CNTRLXMLParseXML.
 * Input         : pInOpaqueDoc - Given XML Doc
 * Return value  : Void
 ***********************************************************************/
void CNTRLXMLFreeXMLDoc(void **pInOpaqueDoc);

/***********************************************************************
 * Function Name	: CNTRLXMLFreeXMLNode
 * Description		: This function frees the XML Node returned from
 *				   the function CNTRLXMLParseXML.
 * Input			: pInOpaqueNode - Given XML Doc
 * Return value  : Void
 ***********************************************************************/
void CNTRLXMLFreeXMLNode(void **pInOpaqueNode);

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLElementList
 * Description   : This function frees the Element List returned
 *                 from the function CNTRLXMLGetFirstElementByTagName.
 * Input         : pInOpaqueElement - Given Element List
 * Return value  : void
 ***********************************************************************/
void CNTRLXMLFreeXMLElementList(void **pInOpaqueElement);

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLAttribList
 * Description   : This function frees the Attribute List returned
 *                 from the function CNTRLXMLGetTagElementFirstAttrib.
 * Input         : pInOpaqueAttrib - Given Attribute List
 * Output          pOutName - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
void CNTRLXMLFreeXMLAttribList(void **pInOpaqueAttrib);

/***********************************************************************
 * Function Name : CNTRLXMLGetNextCommand
 * Description   : This function retrieves the next Command in the
 *                 passed DOM Tree. If pInOpaqueDoc is NULL, it returns
 *                 the first command. Otherwise it returns the next
 *                 with respect to the given pInOpaqueElement which
 *                 represents the current command.
 * Input         : pInOpaqueDoc - DOM tree
 *				   pInOpaqueElement - current Element
 * Output        : pOutOpaqueElement - next Element
 *                 pOutCmdName - Command Name
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetNextCommand(void *pInOpaqueDoc,
							void *pInOpaqueElement,
							void **pOutOpaqueElement,
							unsigned char *pOutCmdName);

/***********************************************************************
 * Function Name : CNTRLXMLSetElementValue
 * Description   : This function sets the value of the given
 *                 element.
 * Input         : pInElement - current Element
 *				   pInValue   - value to be set```
 * Output        : none
 *
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t CNTRLXMLSetElementValue(void *pInElement, char *pInValue);

/***********************************************************************
 * Function Name : CNTRLXMLSetXSLFile
 * Description   : This function updates the XML declaration with the
 *                 passed pInXSLFile file name.
 * Input         : pInXSLFile - Given XSL file name
 * Output          pOutXMLDecl - updated XML header string
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t CNTRLXMLSetXSLFile(char *pInXSLFile, char **pOutXMLDecl);

/***********************************************************************
* Function Name : CNTRLXMLFillAttributeValues
* Description   : This function sets the attribute values for the specified
*                 **pOutElement with the passed ValArray (list of values).
* Input         : pOutElement - Element for which attribute values needs to be set
*		  		  ValArray - list of attribute values
*		  		  recSize  - record size
* Output          pOutElement - Element populated with attributes
* Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
***********************************************************************/
int8_t CNTRLXMLFillAttributeValues(void **pOutElement, char *ValArray[],int32_t recSize);

/***********************************************************************
* Function Name : CNTRLXMLFillChildElementValues
* Description   : This function sets the child element values for the
*                 specified **pOutElement with the passed ValArray (list of values).
* Input         : pOutElement - Element for which child node values needs to be set
*		  		  ValArray - list of attribute values
*		  		  recSize  - record size
* Output          pOutElement - Element populated with attributes
* Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
***********************************************************************/
int8_t CNTRLXMLFillChildElementValues(void **pOutElement, char *ValArray[],int32_t recSize);

/***********************************************************************
 * Function Name : CNTRLXMLCloneElementNode
 * Description   : This function clones the given element(pInOpaqueElement).
 * Input         : pInOpaqueElement - Current element
 * Output        : pOutNewElement   - Cloned element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLCloneElementNode(void *pInOpaqueElement, void **pOutNewElement);

/***********************************************************************
 * Function Name : CNTRLXMLAppendChildNode
 * Description   : This function appends given element(pOpaqueNewElem)
 * 				   to pParentOpaqueElem element
 * Input         : pParentOpaqueElem - Current Node pointer
 * 				   pOpaqueNewElem  - New element node to be appended
 * Output        : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int8_t CNTRLXMLAppendChildNode(void **pParentOpaqueElem, void *pOpaqueNewElem);

/***********************************************************************
 * Function Name : CNTRLXMLAddSoapEnvelope
 * Description   : This function adds the SOAP envelope from a given file
 * 				   (pInenvFile) and returns new document (pOutXMLDoc)
 * 				   with the envelope. This new document needs to be freed.
 * Input         : rootNode - Document root node
 * 				   pInenvFile - XML file with SOAP envelope
 * Output        : pOutXMLDoc - XML document with SOAP envelope
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int32_t CNTRLXMLAddSoapEnvelope(void *rootNode, char *pInenvFile, void **pOutXMLDoc);
/***********************************************************************
 * Function Name : CNTRLXMLGetRootNode
 * Description   : This function gets the root node of a given XML Document.
 * Input         : pInOpaqueDoc   - Document pointer
 * Output        : pOutOpaqueNode - Root node element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetRootNode(void *pInOpaqueDoc, void **pOutOpaqueNode);

/***********************************************************************
 * Function Name	: CNTRLXMLGetFirstChildElement
 * Description		: This function retrieves the first parent element of
 				   the given element(pInOpaqueElement).
 * Input			: pInOpaqueElement - Current Element/Node pointer
 * Output		: pOutOpaqueElement - Parent element
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t CNTRLXMLGetParentElement(void *pInOpaqueElement,
							      void **pOutOpaqueElement);
/***********************************************************************
 * Function Name : CNTRLXMLGetAttributeValue
 * Description   : This function retrieves the value of the given
 *                 attribute.
 * Input         : pInAttribName - Attribute Name
 *				   pInOpaqueRec - current Element
 * Output        : pOutOpaqueElement - next Element
 *                 pOutAtribValue - Attribute Value
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t CNTRLXMLGetAttributeValue(unsigned char *pInAttribName,
							   void *pInOpaqueRec,
							   unsigned char *pOutAtrribValue);

/**************************************************************************
 * Function Name : XML_GetDocPath
 * Description   : This function is used to get the document path
 *			    configured for the http server.
 * Input         	  : None.
 * Output          : Document path.
 * Return value : Configured document path.
 **************************************************************************/
unsigned char *XML_GetDocPath( void );

#ifdef PSP_XML_MULTIMODSET_SUPPORT

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

#endif

#ifdef PSP_XML_SCHEMA_VAL_SUPPORT
/***********************************************************************
 * Function Name 	: CNTRLXMLLoadSchema
 * Description   	: This function loads an XML Schema
 * Input         		: pInSchema - XML Schema to load
 * Output 	       : pOutSchemaCtxt - output Schema context
 * Return value  	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   		  CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t CNTRLXMLLoadSchema(unsigned char *pInSchema, void **pOutSchemaCtxt);

/***********************************************************************
 * Function Name 	: CNTRLXMLValidateXML
 * Description   	: This function validates a XML Doc using Schema
 * Input         		: pInSchemaCtxt - XML Schema to load
 *				  pInXMLDoc - XML Doc to be validated
 * Output 	       : None
 * Return value  	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   		  CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t CNTRLXMLValidateXML(void *pInSchemaCtxt, void *pInXMLDoc);

#ifdef PSP_XML_MULTIMODSET_SUPPORT
/***********************************************************************
 *  Function Name        : CNTRLXMLValidateXMLElement
 *  Description          : This function validates a XML Element using Schema
 *  Input                : pInSchemaCtxt - XML Schema to load
 *                                 pInXMLElem - XML Element to be validated
 *  Output               : None
 *  Return value         : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
 *                                         CNTRLXML_FAILURE.
 ***********************************************************************
*/
int8_t CNTRLXMLValidateXMLElement(void *pInSchemaCtxt, void *pInXMLElem);
#endif /* PSP_XML_MULTIMODSET_SUPPORT */
#endif /* PSP_XML_SCHEMA_VAL_SUPPORT */

#ifdef PSP_XPATH_SUPPORT

/***********************************************************************
 * Function Name : PSPNewXPathContext
 * Description   : This function creates a new Context for XPath with the
 *                       passed DOM Tree.
 * Input         : pInOpaqueDoc - DOM tree
 *	                pOutOpaqueContext - XPath Context
 * Output        : pOutOpaqueContext - New XPath Context
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t PSPNewXPathContext(void *pInOpaqueDoc, void **pOutOpaqueContext);

/***********************************************************************
 * Function Name : PSPXPathExpressionEX
 * Description   : This function evaluates XPath expression/location
 * 				   pInXPathExpr and executes the query with the passed
 * 				   XPath Context pInOpaqueContext.
 * Input         : pInXPathExpr      - XPath Expression
 *	               pInOpaqueContext  - XPath Context
 * Output        : pOutOpaqueResult  - XPath Query result
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t PSPXPathExpressionEX(unsigned char *pInXPathExpr,
		void *pInOpaqueContext, void **pOutOpaqueResult);

/***********************************************************************
 * Function Name : PSPXPathResCount
 * Description   : This function evaluates XPath ResultSet and returns number of nodes
				   present
 * Input         : pInResultSet      - XPath ResultSet
 * Output        : None
 * Return value  : count - Number of Result Nodes in XPath ResultSet
 ***********************************************************************/
int32_t PSPXPathResCount(void *pInResultSet);

/***********************************************************************
 * Function Name : PSPXPathGetItemAt
 * Description   : This function evaluates XPath ResultSet and returns IXML_Node
				   pointer at given index.
 * Input         : pInResultSet      - XPath ResultSet
 *				 : index 			 - Required IXML_Node number in XPath ResultSet
 * Output        : None
 * Return value  : item - IXML_Node in XPath Resultset at given position
 ***********************************************************************/
void * PSPXPathGetItemAt(void *pInResultSet, int32_t index);

/***********************************************************************
 * Function Name : PSPXPathFreeResultSet
 * Description   : This function Frees XPath ResultSet
 * Input         : pInResultSet      - XPath ResultSet
 * Output        : None
 * Return value  : None
 ***********************************************************************/
void PSPXPathFreeResultSet(void *pInResultSet);

/***********************************************************************
 * Function Name : PSPXPathFreeContext
 * Description   : This function Frees XPath Context Pointer
 * Input         : pInXPathContext      - XPath Context
 * Output        : None
 * Return value  : None
 ***********************************************************************/
void PSPXPathFreeContext(void *pInXPathContext);


#endif /* PSP_XPATH_SUPPORT */

void XMLMain_RegisterAll( void );

/***********************************************************************
 * Function Name : CNTRLXMLRemoveNode
 * Description   : This function removes the specified node.
 * Input         : pInOpaqueNode   - Node pointer to be removed
 * Output        : pOutDelNode - Node that was removed needs to be freed.
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE.
 ***********************************************************************/
int32_t CNTRLXMLRemoveNode(void *pInOpaqueNode, void **pOutDelNode);

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildValueByTagName
 * Description   : This function retrieves the value of
 				   the child of the given element.
 * Input         : pTagName - Element tag name.
 *				   pInOpaqueElement - Given Element
 * 				   ulOutBufLength - String length
 * Output		   pOutBuf - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/

int32_t CNTRLXMLGetFirstChildValueByTagName(char* pTagName, 
                                      void* pInOpaqueElement, 
                                      unsigned char* pOutBuf, 
                                      uint32_t ulOutBufLength);

/***********************************************************************
 * Function Name : CNTRLXMLSetFirstChildValueByTagName
 * Description   : This function sets the value of
 				   the child of the given element.
 * Input         : pTagName - Element tag name.
 *				   pInOpaqueElement - Given Element
 * Output		   pOutBuf - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int32_t CNTRLXMLSetFirstChildValueByTagName(char* pTagName, 
                                      void* pInOpaqueElement, 
                                      char* pInBuf);

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildValue
 * Description   : This function retrieves the value of
 				   the child of the given element.
 * Input         : pInOpaqueElement - Given Element
 * 				   ulOutBufLength - String length
 * Output		   pOutBuf - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/

int32_t CNTRLXMLGetFirstChildValue(void* pInOpaqueElement, 
                                      unsigned char* pOutBuf, 
                                      uint32_t ulOutBufLength);

/***********************************************************************
 * Function Name : CNTRLXMLSetNodeName
 * Description   : This function sets the name of
 *                 the given element.
 * Input         : pInOpaqueElement - Given Element
 *                 pInName - name of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 * **********************************************************************/
int8_t
CNTRLXMLSetNodeName (void * pInOpaqueElement, unsigned char * pInName);

/***************************************************************************
* * * * * * * * * * U T I L I T I E S * * * * * * * * * * * * * * * * * * *
****************************************************************************/
int32_t
CNTRLXMLGetNodebyTagName (void * pInNode,
                        char * pKeyTagName, void ** pOutSearchNode);

int32_t
CNTRLXMLGetTagValue (char * pInTag, void * pCurrentCommand,
                   char * pOutValue);
int32_t
CNTRLXMLLoadXMLDOM (char * pXmlFile, void ** pXmlDOM);

int8_t
PSPConfigLoadXML (unsigned char * pInXMLFile, void ** pOutOpaqueXMLDoc);

int32_t
PSPConfigLoadXMLDOM (char * pXmlFile, void ** pXmlDOM);

int32_t
CNTRLXMLSendXMLDOM (void * pXmlDOM);

int32_t
CNTRLXMLPopulateXMLDOM (void * pXMLDoc,
                      char * pKeyTagName,
                      XMLAttribs Params[], uint32_t uiCount);

int32_t
CNTRLXMLPopulateXMLValues (void * pSearchTagRecord,
                       XMLAttribs Params[], uint32_t uiCount);

int32_t CNTRLXMLCreateInXMLDOM(char  *pFileName,
                               void  **pXmlDOM,
                               void   *pData, 
                               uint32_t  ulMaxKeyAttribs);

int32_t
CNTRLXMLCreateErrorXMLDOM (char * pModTag, int32_t iErrCode,
                         char * pErrMsg, void ** ppOutXmlDom);

int32_t
CNTRLXMLCreateSuccessXMLDOM (char * pModTag, int32_t iSuccessCode,
                           char * pSuccessMsg, void ** ppOutXmlDom);

int32_t
CNTRLXMLGetTagValues (void * pInXMLReq, XMLAttribs XMLParams[],
		                    uint32_t ulCnt);

int32_t
CNTRLXMLAddErrorMessageToXMLDOM (char * pErrorMsg, void  *pXMLDoc);

void  PSPTestAppXmlResp(void *pInOpaqueElement,int32_t iRet,
                          char *pErrMsg);

int32_t execute_cntrl_resource_conf_extract(void *pDOMDoc);

#endif /* __CNTRLXML_H__ */
#endif /* PSP_XML_SUPPORT */
