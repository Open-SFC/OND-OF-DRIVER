#ifdef OF_XML_SUPPORT

/***********************************************************
 * * * *  I n c l u d e    H e a d e r    f i l e s  * * * *
 ***********************************************************/
//#include <psp/psp.h>
#include <cntrlxml.h>
/*#include <psp/xmlhandler.h> */
#include <xmlrec.h>
//#include <hash.h> 
//#include <mchash.h> 
#include <oftype.h> 
#include <cntlr_error.h> 
#include <stdint.h> 
#ifdef PSP_XML_SCHEMA_VAL_SUPPORT
#include <xmlstring.h>
#include <xmlschemas.h>
#endif /* PSP_XML_SCHEMA_VAL_SUPPORT */

#define xmlChar char

#define MAX_HASH_SIZE 10
#define XML_ATTRIB_VALUE_MAX_LEN   256 
extern void ixmlDocument_setOwnerDocument (IN IXML_Document * doc,
                                           IN IXML_Node * nodeptr);
char *PSPXmlLanguage = NULL;
char xmlDeclare[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"";
char endXmlDeclare[] = "\"?>\n";

#ifdef PSP_XML_INTERNATIONALIZATION_SUPPORT
int32_t
PSPXmlSetLanguage (char * pLangName)
{
  /*
   * Following value 'PSPXmlLanguage'
   * should be stored permanantly,
   * so that it can be retrieved even
   * after the server reboot.
   */
  if (!pLangName)
  {
    return CNTRLXML_FAILURE;
  }
  PSPXmlLanguage = strdup (pLangName);
  XML_DebugArg ("PSPXmlLanguage =", PSPXmlLanguage);

  return CNTRLXML_SUCCESS;
}
#endif /*PSP_XML_INTERNATIONALIZATION_SUPPORT */

char *
PSPXmlGetLanguage (void)
{
  char *pLangName = NULL;
  if (PSPXmlLanguage)
  {
    pLangName = strdup (PSPXmlLanguage);
  }
  else
  {
    pLangName = strdup ("en");
  }

  XML_DebugArg ("pLangName = ", pLangName);
  return pLangName;
}

/**************************************************************************
 * Function Name : XML_GetDocPath
 * Description   : This function is used to get the document path
 *			    configured for the http server.
 * Input         	  : None.
 * Output          : Document path.
 * Return value : Configured document path.
 **************************************************************************/
unsigned char *
XML_GetDocPath (void)
{
  static unsigned char ucDocPath[] = XML_DOC_PATH;
  return ucDocPath;
}                               /* XML_GetDocPath() */


/**************************************************************************
 *  * Function Name : Config_GetDocPath
 *  * Description   : This function is used to get the document path
 *  *                          configured for the http server.
 *  * Input                  : None.
 *  * Output          : Document path.
 *  * Return value : Configured document path.
 *  **************************************************************************/
unsigned char *
Config_GetDocPath (void)
{
  static unsigned char cxDocPath[] = Config_DOC_PATH;
  return cxDocPath;
}                               

/***********************************************************************
 * Function Name : CNTRLXMLParseXML
 * Description   : This function parses an XML string
 				   and populates a DOM tree	(pOutOpaqueXMLDoc)
 * Input         : pInXMLBuff - XML String
 * Output		   pOutOpaqueXMLDoc - DOM tree
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLParseXML (unsigned char * pInXMLBuff, void ** pOutOpaqueXMLDoc)
{
  char iErrCode;

  if (!pInXMLBuff || (strlen ((char *)pInXMLBuff) == 0))
  {
    return CNTRLXML_BADPARAM;
  }
  if ((iErrCode =
       ixmlParseBufferEx ((char *)pInXMLBuff,
                          (IXML_Document **) pOutOpaqueXMLDoc)) !=
      IXML_SUCCESS)
  {
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLLoadXML
 * Description   : This function parses an XML file
 				   and populates a DOM tree	(pOutOpaqueXMLDoc)
 * Input         : pInXMLFile - XML String
 * Output		   pOutOpaqueXMLDoc - DOM tree
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLLoadXML (unsigned char * pInXMLFile, void ** pOutOpaqueXMLDoc)
{
  char ucFileName[XML_MAX_NAMELEN + 1];

  if (!pInXMLFile || (strlen ((char *)pInXMLFile) == 0))
  {
    XML_ErrorArg ("Bad Arguments ", ucFileName);
    return CNTRLXML_BADPARAM;
  }
  /*
   * Construct absolute path.
   */
  //sprintf (ucFileName, "%s%s", XML_GetDocPath (), pInXMLFile);
  sprintf (ucFileName, "%s",pInXMLFile);
  XML_DebugArg ("ucFileName = \r\n", ucFileName);

  *pOutOpaqueXMLDoc =
    (IXML_Document *) ixmlLoadDocument ((char *) ucFileName);

  if (!*pOutOpaqueXMLDoc)
  {
    XML_ErrorArg ("Unable to Load File ", ucFileName);
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 *  * Function Name : PSPConfigLoadXML
 *  * Description   : This function parses an XML file
 *  *  and populates a DOM tree     (pOutOpaqueXMLDoc)
 *  * Input         : pInXMLFile - XML String
 *  * Output                  pOutOpaqueXMLDoc - DOM tree
 *  * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ************************************************************************/

int8_t
PSPConfigLoadXML (unsigned char * pInXMLFile, void ** pOutOpaqueXMLDoc)
{
  char ucFileName[XML_MAX_NAMELEN + 1];

  if (!pInXMLFile || (strlen ((char *)pInXMLFile) == 0))
  {
    return CNTRLXML_BADPARAM;
  }
  /*
 *    * Construct absolute path.
 *       */
  sprintf (ucFileName, "%s%s", Config_GetDocPath (), pInXMLFile);
  XML_DebugArg ("ucFileName =", ucFileName);

  *pOutOpaqueXMLDoc =
    (IXML_Document *) ixmlLoadDocument ((char *) ucFileName);

  if (!*pOutOpaqueXMLDoc)
  {
    XML_ErrorArg ("Unable to Load File ", ucFileName);
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}


/***********************************************************************
 * Function Name : CNTRLXMLHasIncludeTags
 * Description   : This function checks for existance of  INCLUDE tags
 * 				   from given XML DOM tree.
 * Input         : pInOpaqueDoc - DOM tree
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int32_t
CNTRLXMLHasIncludeTags (void * pInOpaqueXMLDoc)
{
  IXML_NodeList *pNodeList = NULL;

  XML_Debug ("Entered");
  if (!pInOpaqueXMLDoc)
  {
    return CNTRLXML_BADPARAM;
  }

  pNodeList = ixmlDocument_getElementsByTagName ((IXML_Document *)
                                                 pInOpaqueXMLDoc,
                                                 CNTRLXML_INCLUDETAG);

  if (pNodeList == NULL)
  {
    return CNTRLXML_FAILURE;
  }
  ixmlNodeList_free (pNodeList);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLProcessIncludeTags
 * Description   : This function process the XML DOM tree for INCLUDE tags
 * Input         : pInOpaqueDoc - DOM tree
 * Output		 : none
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/
int32_t
CNTRLXMLProcessIncludeTags (void ** pInOpaqueDoc)
{
  void *pRootNode = NULL;
  IXML_NodeList *pNodeList = NULL;
  int32_t i = 0, n = 0, iErrCode = 0;
  IXML_Node *ptmpNode = NULL;
  void *pOutXMLDoc = NULL;

  if (!*pInOpaqueDoc)
    return CNTRLXML_BADPARAM;

  CNTRLXMLGetRootNode (*pInOpaqueDoc, &pRootNode);
  XML_DebugArg ("ROOT NODE = ", ((IXML_Node *) pRootNode)->nodeName);
  pNodeList =
    ixmlDocument_getElementsByTagName ((IXML_Document *) * pInOpaqueDoc,
                                       CNTRLXML_INCLUDETAG);
  if (pNodeList != NULL)
  {
    XML_Debug (" Found INCLUDE tags");

    n = ixmlNodeList_length (pNodeList);
    for (i = 0; i < n; i++)     /* process all records */
    {
      unsigned char *value = NULL;
      void *pAttribute = NULL;
      void *pRootNode = NULL;

      ptmpNode = ixmlNodeList_item (pNodeList, i);

      pAttribute = (void *) ixmlNode_getAttributeByName (ptmpNode,
                                                           CNTRLXML_XMLREF_ATTRIBUTE);
      if (pAttribute)
      {
        IXML_Node *pRetNode = NULL;
        IXML_Node *repNode = NULL;

        value = (unsigned char *) ixmlNode_getNodeValue ((IXML_Node *) pAttribute);

        XML_DebugArg ("xmlref=", value);

        iErrCode = CNTRLXMLLoadXML (value, &pOutXMLDoc);

        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Error ("UNABLE to load XML document");
          return CNTRLXML_FAILURE;
        }

        CNTRLXMLGetRootNode (pOutXMLDoc, &pRootNode);

        iErrCode = ixmlDocument_importNode (ptmpNode->ownerDocument,
                                            (IXML_Node *) pRootNode, TRUE,
                                            &pRetNode);

        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Error ("UNABLE to import document");
          if (pRetNode)
          {
            ixmlNode_free (pRetNode);
          }
          return CNTRLXML_FAILURE;
        }
        pRetNode->parentNode = ptmpNode->parentNode;

        iErrCode = ixmlNode_replaceChild (ptmpNode->parentNode, pRetNode,
                                          ptmpNode, &repNode);

        if (iErrCode != CNTRLXML_SUCCESS)
        {
          XML_Debug ("UNABLE to replace INCLUDE tag");
          if (repNode)
          {
            ixmlNode_free (repNode);
          }
          return CNTRLXML_FAILURE;
        }
        if (repNode)
        {
          ixmlNode_free (repNode);
        }

        CNTRLXMLFreeXMLDoc (&pOutXMLDoc);

      }

    }                           /*end for */
  }
  if (pNodeList)
  {
    ixmlNodeList_free (pNodeList);
  }

  if (CNTRLXMLHasIncludeTags (*pInOpaqueDoc) == CNTRLXML_SUCCESS)
  {
    iErrCode = CNTRLXMLProcessIncludeTags (pInOpaqueDoc);

    if (iErrCode != CNTRLXML_SUCCESS)
    {
      XML_Debug ("Failed to process INCLUDE tags");
    }
  }
  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLtoBuffer
 * Description   : This function parses an XML DOM tree
 *				   and converts it to String buffer(pInXMLBuff)
 * Input         : pInOpaqueXMLDoc - DOM tree
 * Output		   pInXMLBuff - XML String
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_FAILURE, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLtoBuffer (void * pInOpaqueXMLDoc, char ** pInXMLBuff)
{
  if (!pInOpaqueXMLDoc)
  {
    return CNTRLXML_BADPARAM;
  }

  *pInXMLBuff = (char *) ixmlNodetoString ((IXML_Node *) pInOpaqueXMLDoc);

  if (!*pInXMLBuff)
  {
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildByTagName
 * Description   : This function retrieves the first child element
 				   with name equal to the passed tag name.
 * Input         : pInTagName - tag name
 				   pInOpaqueNode - Input Element
 * Output        : pOutOpaqueNode - Out Node
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetFirstChildByTagName (unsigned char * pInTagName,
                              void * pInOpaqueNode,
                              void ** pOutOpaqueNode)
{
  IXML_Node *pNode = NULL;

  if (!pInTagName || (strlen ((char *)pInTagName) == 0) || pInOpaqueNode == NULL)
  {
    XML_Error ("CNTRLXML_BADPARAM");
    return CNTRLXML_BADPARAM;
  }

  pNode = ixmlNode_getFirstChild ((IXML_Node *) pInOpaqueNode);
  while (pNode)
  {
    if (ixmlNode_getNodeType (pNode) == eELEMENT_NODE)
    {
#ifdef PSP_CORE_DEBUG
      printf("\r\n %s:%d  In Name=**%s** , TagName = **%s**  \r\n",__FUNCTION__,__LINE__,pInTagName,ixmlNode_getNodeName (pNode));
#endif
      if (!strcmp ((char *)pInTagName, ixmlNode_getNodeName (pNode)))
      {
        /* found a match */
        break;
      }
    }
    /* get the next sibling node */
    pNode = ixmlNode_getNextSibling (pNode);
  }

  *pOutOpaqueNode = pNode;

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetNextSiblingByTagName
 * Description   : This function retrieves the Next sibling element
 *                 with name equal to passed tagname.
 * Input         : pInOpaqueNode - current Node.
 * Output        : pOutOpaqueNode - new Node element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetNextSiblingByTagName (unsigned char * pInTagName,
                               void * pInOpaqueNode,
                               void ** pOutOpaqueNode)
{
  IXML_Node *pNode = NULL;

  if (!pInTagName || (strlen ((char *)pInTagName) == 0) || pInOpaqueNode == NULL)
  {
    XML_Error ("CNTRLXML_BADPARAM");
    return CNTRLXML_BADPARAM;
  }


  /* We need the next element. pInOpaqueNode has the current element */
  pNode = ixmlNode_getNextSibling ((IXML_Node *) pInOpaqueNode);


  while (pNode)
  {
    if (ixmlNode_getNodeType (pNode) == eELEMENT_NODE)
    {
      if (!strcmp ((char *)pInTagName, ixmlNode_getNodeName (pNode)))
      {
        /* found a match */
        break;
      }
    }

    /* get the next sibling node */
    pNode = ixmlNode_getNextSibling (pNode);
  }

  *pOutOpaqueNode = pNode;

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLRemoveNode
 * Description   : This function removes the specified node.
 * Input         : pInOpaqueNode   - Node pointer to be removed
 * Output        : pOutDelNode - Node that was removed needs to be freed.
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE.
 ***********************************************************************/
int32_t
CNTRLXMLRemoveNode (void * pInOpaqueNode, void ** pOutDelNode)
{
  int32_t iErrCode = 0;
  if (!pInOpaqueNode)
    return CNTRLXML_BADPARAM;

  iErrCode = ixmlNode_removeChild (((IXML_Node *) pInOpaqueNode)->parentNode,
                                   (IXML_Node *) pInOpaqueNode,
                                   (IXML_Node **) pOutDelNode);
  if (iErrCode != CNTRLXML_SUCCESS)
  {
    return CNTRLXML_FAILURE;
  }
  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetRootNode
 * Description   : This function gets the root node of a given XML Document.
 * Input         : pInOpaqueDoc   - Document pointer
 * Output        : pOutOpaqueNode - Root node element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetRootNode (void * pInOpaqueDoc, void ** pOutOpaqueNode)
{
  if (!pInOpaqueDoc)
    return CNTRLXML_BADPARAM;

  *pOutOpaqueNode =
    (void *) (((IXML_Document *) pInOpaqueDoc)->n.firstChild);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLCloneElementNode
 * Description   : This function clones the given element(pInOpaqueElement).
 * Input         : pInOpaqueElement - Current element
 * Output        : pOutNewElement   - Cloned element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLCloneElementNode (void * pInOpaqueElement, void ** pOutNewElement)
{
  char rc = 0;

  if (!pInOpaqueElement)
  {
    return CNTRLXML_BADPARAM;
  }
  *pOutNewElement =
    (void *) ixmlNode_cloneNode ((IXML_Node *) pInOpaqueElement, TRUE);
  if (*pOutNewElement)
  {

    ixmlDocument_setOwnerDocument (ixmlNode_getOwnerDocument
                                   ((IXML_Node *) pInOpaqueElement),
                                   (IXML_Node *) * pOutNewElement);

    rc = ixmlNode_appendChild (((IXML_Node *) pInOpaqueElement)->parentNode,
                               (IXML_Node *) * pOutNewElement);
    if (rc != 0)
    {
      XML_Debug ("UNABLE TO APPEND CLONED ELEMENT");
      return CNTRLXML_FAILURE;
    }
  }
  else
  {
    XML_Debug ("UNABLE TO CLONE ");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLAppendChildNode
 * Description		: This function appends given element(pOpaqueNewElem)
 * 				   to pParentOpaqueElem element
 * Input			: pParentOpaqueElem - Current Node pointer
 * 				  pOpaqueNewElem  - New element node to be appended
 *				  Calling function should free "pOpaqueNewElem".
 * Output		: none
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int8_t
CNTRLXMLAppendChildNode (void ** pParentOpaqueElem, void * pOpaqueNewElem)
{
  int32_t iErrCode = 0;
  void *pOutNewElement = NULL;

  if (!*pParentOpaqueElem || !pOpaqueNewElem)
  {
    return CNTRLXML_BADPARAM;
  }

  pOutNewElement =
    (void *) ixmlNode_cloneNode ((IXML_Node *) pOpaqueNewElem, TRUE);

  if (pOutNewElement)
  {
    ((IXML_Node *) pOutNewElement)->ownerDocument =
      ixmlNode_getOwnerDocument ((IXML_Node *) * pParentOpaqueElem);

    iErrCode = ixmlNode_appendChild ((IXML_Node *) * pParentOpaqueElem,
                                     (IXML_Node *) pOutNewElement);
    if (iErrCode != CNTRLXML_SUCCESS)
    {
      XML_Error ("Unable to append  the Child Node");
      return CNTRLXML_FAILURE;
    }
  }
  else
  {
    XML_Debug ("Unable to Clone the Child Node");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;

}

/***********************************************************************
 * Function Name : CNTRLXMLAddSoapEnvelope
 * Description   : This function adds the SOAP envelope from a given file
 * 				   (pInenvFile) and returns new document (pOutXMLDoc)
 * 				   with the envelope. This new document needs to be freed.
 * Input         : pRootNode - Document root node
 * 				   pInenvFile - XML file with SOAP envelope
 * Output        : pOutXMLDoc - XML document with SOAP envelope
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int32_t
CNTRLXMLAddSoapEnvelope (void * pRootNode, char * pInenvFile,
                       void ** pOutXMLDoc)
{
  int32_t iErrCode = 0;
  void *pEnvRootNode = NULL, *pCurrentCommand = NULL;
  IXML_Node *pRetNode = NULL;

  if (!pRootNode || !pInenvFile)
  {
    return CNTRLXML_BADPARAM;
  }
  if ((iErrCode = CNTRLXMLLoadXML ((unsigned char *)pInenvFile, 
             pOutXMLDoc)) != CNTRLXML_SUCCESS)
  {
    return CNTRLXML_FAILURE;
  }

  CNTRLXMLGetRootNode (*pOutXMLDoc, &pEnvRootNode);

  if ((iErrCode =
       CNTRLXMLGetFirstChildElement (pEnvRootNode,
                                   &pCurrentCommand)) != CNTRLXML_SUCCESS)
  {
    XML_Debug ("UNABLE to Get first child");
  }
  if ((iErrCode = ixmlDocument_importNode (((IXML_Node *) pCurrentCommand)->
                                           ownerDocument,
                                           (IXML_Node *) pRootNode, TRUE,
                                           &pRetNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("UNABLE to import document");
    if (pRetNode)
    {
      ixmlNode_free (pRetNode);
    }
    return CNTRLXML_FAILURE;
  }
  if ((iErrCode =
       ixmlNode_appendChild ((IXML_Node *) pCurrentCommand,
                             pRetNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("UNABLE to append the document");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildElement
 * Description   : This function retrieves the first Child element of
 				   the given element(pInOpaqueElement).
 * Input         : pInOpaqueElement - Parent Element
 * Output        : pOutOpaqueElement - First Child element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetFirstChildElement (void * pInOpaqueElement,
                            void ** pOutOpaqueElement)
{
  if (!pInOpaqueElement)
  {
    return CNTRLXML_BADPARAM;
  }

  *pOutOpaqueElement =
    (void *) ixmlNode_getFirstChild ((IXML_Node *) pInOpaqueElement);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name	: CNTRLXMLGetParentElement
 * Description		: This function retrieves the first parent element of
 				   the given element(pInOpaqueElement).
 * Input			: pInOpaqueElement - Current Element/Node pointer
 * Output		: pOutOpaqueElement - Parent element
 * Return value	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetParentElement (void * pInOpaqueElement,
                        void ** pOutOpaqueElement)
{
  if (!pInOpaqueElement)
  {
    return CNTRLXML_BADPARAM;
  }

  *pOutOpaqueElement =
    (void *) ixmlNode_getParentNode ((IXML_Node *) pInOpaqueElement);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetNextSibling
 * Description   : This function returns the Next Sibling
 * Input         : pInOpaqueElement - current child element
 * Output        : pOutOpaqueElement - next sibling element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM.
 ***********************************************************************/
int8_t
CNTRLXMLGetNextSibling (void * pInOpaqueElement, void ** pOutOpaqueElement)
{
  if (!pInOpaqueElement)
  {
    return CNTRLXML_BADPARAM;
  }

  *pOutOpaqueElement =
    (void *) ixmlNode_getNextSibling ((IXML_Node *) pInOpaqueElement);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetTagElementFirstAttrib
 * Description   : This function retrieves the first Attribute of the
				   passed element.
 * Input         : pInOpaqueElement - Node list element
 * Output        : pOutOpaqueAttrib - Attribute Node
				   pOutAttribName - Attribute Name
				   pOutAttribValue - Attribute Value (To be freed by the caller)
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int8_t
CNTRLXMLGetTagElementFirstAttrib (void * pInOpaqueElement,
                                void ** pOutOpaqueAttrib,
                                unsigned char * pOutAttribName,
                                unsigned char ** pOutAttribValue)
{

  if (!pInOpaqueElement)
  {
    return CNTRLXML_BADPARAM;
  }

  if (pOutAttribName != NULL)
  {
    if (strlen ((char *)pOutAttribName) != 0)
    {
      *pOutOpaqueAttrib =
        (void *) ixmlNode_getAttributeByName ((IXML_Node *)
                                                pInOpaqueElement,
                                                (char *)pOutAttribName);
      if (!*pOutOpaqueAttrib)
      {
        return CNTRLXML_FAILURE;
      }
      *pOutAttribValue =
        (unsigned char *)
        xmlStrdup (ixmlNode_getNodeValue ((IXML_Node *) * pOutOpaqueAttrib));
    }
  }
  else
  {
    if (((IXML_Node *) pInOpaqueElement)->firstAttr)
    {
      *pOutOpaqueAttrib = ((IXML_Node *) pInOpaqueElement)->firstAttr;
      *pOutAttribValue =
        (unsigned char *)
        xmlStrdup (ixmlNode_getNodeValue ((IXML_Node *) * pOutOpaqueAttrib));
    }
    else
    {
      return CNTRLXML_FAILURE;
    }
  }

  return CNTRLXML_SUCCESS;
}

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
int8_t
CNTRLXMLGetTagElementNextAttrib (void * pInOpaqueAttrib,
                               void ** pOutOpaqueAttrib,
                               unsigned char * pOutAttribName,
                               unsigned char * pOutAttribValue)
{
  IXML_Node *pAttribNode = NULL;

  if (!pOutAttribName || (strlen ((char *)pOutAttribName) == 0) ||
      !pOutAttribValue || (strlen ((char *)pOutAttribValue) == 0) ||
      !*pOutOpaqueAttrib || !pInOpaqueAttrib)
  {
    return CNTRLXML_BADPARAM;
  }

  pAttribNode = ((IXML_Node *) pInOpaqueAttrib)->nextSibling;

  if (pAttribNode != NULL)
  {
    strcpy ((char *)pOutAttribName,
              (char *) ixmlNode_getNodeName (pAttribNode));
    strcpy ((char *)pOutAttribValue,
              (char *) ixmlNode_getNodeValue (pAttribNode));
  }

  /* Update the current Attribute Node */
  *pOutOpaqueAttrib = (void *) pAttribNode;

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetTotNoOfElements
 * Description   : This function retrieves the total number of elements
 				   in the given element list.
 * Input         : pInOpaqueElement - Element List
 * Output		   pOutCount - Total count of elements
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLGetTotNoOfElements (void * pInOpaqueElement, int32_t * pOutCount)
{
  if (!pInOpaqueElement || !pOutCount)
  {
    return CNTRLXML_BADPARAM;
  }

  *pOutCount =
    (int32_t) ixmlNodeList_length ((IXML_NodeList *) pInOpaqueElement);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetTotNoOfAttribs
 * Description   : This function retrieves the total number of
 				   attriutes in the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutCount - Total count of attributes
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLGetTotNoOfAttribs (void * pInOpaqueElement, int32_t * pOutCount)
{
  if (!pInOpaqueElement || !pOutCount)
  {
    return CNTRLXML_BADPARAM;
  }

  *pOutCount =
    (int32_t) ixmlNamedNodeMap_getLength ((IXML_NamedNodeMap *)
                                          pInOpaqueElement);

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLGetNodeValue
 * Description   : This function retrieves the value of
 				   the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutValue - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/

int8_t
CNTRLXMLGetNodeValue (void * pInOpaqueElement, unsigned char * pOutValue)
{
  IXML_Node *pChild = NULL;

  if (!pInOpaqueElement || !pOutValue)
  {
    return CNTRLXML_BADPARAM;
  }

  pChild = ixmlNode_getFirstChild ((IXML_Node *) pInOpaqueElement);

  if ((pChild != 0) && (ixmlNode_getNodeType (pChild) == eTEXT_NODE))
  {
    strcpy ((char *)pOutValue, 
     (char *) ixmlNode_getNodeValue (pChild));
    return CNTRLXML_SUCCESS;
  }
  else
  {
    if (ixmlNode_getNodeValue ((IXML_Node *) pInOpaqueElement) != NULL)
    {
      strcpy ((char *)pOutValue,
                (char *) ixmlNode_getNodeValue ((IXML_Node *)
                                                    pInOpaqueElement));

      return CNTRLXML_SUCCESS;
    }

    return CNTRLXML_NOTFOUND;
  }
}

/***********************************************************************
 * Function Name : CNTRLXMLGetNodeValueByLength
 * Description   : This function retrieves the value of
 				   the given element.
 * Input         : pInOpaqueElement - Given Element
 * 				   len - String length
 * Output		   pOutValue - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/
int8_t
CNTRLXMLGetNodeValueByLength (void * pInOpaqueElement,
                            unsigned char * pOutValue, uint32_t ulen)
{
  int32_t iNodeValLen = 0;

  if (!pInOpaqueElement || !pOutValue || ulen == 0)
  {
    return CNTRLXML_BADPARAM;
  }
  iNodeValLen =
    xmlStrlen (ixmlNode_getNodeValueEx ((IXML_Node *) pInOpaqueElement));
  if (iNodeValLen == 0)
  {
    return CNTRLXML_NOTFOUND;
  }
 
#ifdef PSP_CORE_DEBUG
 pName = ixmlNode_getNodeName (pInOpaqueElement);
 XML_DebugArg ("pNodeName=\r\n", pName); 
#endif

  if (iNodeValLen > ulen)
  {
    return CNTRLXML_BADPARAM;
  }

  strncpy ((char *)pOutValue,
             (char *) ixmlNode_getNodeValueEx ((IXML_Node *)
                                                   pInOpaqueElement), iNodeValLen);
                                                   /* pInOpaqueElement), ulen); */
  pOutValue[iNodeValLen] = '\0';
  //strcpy (pOutValue, (unsigned char *) ixmlNode_getNodeValueEx ((IXML_Node *) pInOpaqueElement));

  return CNTRLXML_SUCCESS;
}

int8_t
CNTRLXMLNCopyNodeValue (void * pInOpaqueElement,
                      unsigned char * pOutValue, uint32_t ulen)
{
  int32_t iNodeValLen = 0;

  if (!pInOpaqueElement || !pOutValue || ulen == 0)
  {
    return CNTRLXML_BADPARAM;
  }
  iNodeValLen =
    xmlStrlen (ixmlNode_getNodeValueEx ((IXML_Node *) pInOpaqueElement));
  if (iNodeValLen == 0)
  {
    return CNTRLXML_NOTFOUND;
  }

  if (iNodeValLen > ulen)
  {
  strncpy ((char *)pOutValue,
             (char *) ixmlNode_getNodeValueEx ((IXML_Node *)
                                                   pInOpaqueElement), ulen - 1);
  *(pOutValue + ulen - 1) = '\0';
  }
  else
  strncpy ((char *)pOutValue,
             (char *) ixmlNode_getNodeValueEx ((IXML_Node *)
                                                   pInOpaqueElement), ulen);

  return CNTRLXML_SUCCESS;
}
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

int32_t
CNTRLXMLGetFirstChildValueByTagName (char * pTagName,
                                   void * pInOpaqueElement,
                                   unsigned char * pOutBuf,
                                   uint32_t ulOutBufLength)
{
  void *pNodePtr = NULL;
  int32_t iErrCode = 0;

  if (!pTagName || !pInOpaqueElement || ulOutBufLength == 0)
  {
    XML_Error ("Invalid parameters");
    return CNTRLXML_BADPARAM;
  }
  iErrCode =
    CNTRLXMLGetFirstChildByTagName ((unsigned char *)pTagName, pInOpaqueElement, 
            &pNodePtr);
  if (iErrCode != CNTRLXML_SUCCESS || pNodePtr == NULL)
  {
    XML_DebugArg ("Could not find the child with tag = ", pTagName);
    return iErrCode;
  }

#ifdef PSP_CORE_DEBUG
 pName = ixmlNode_getNodeName (pNodePtr);
 XML_DebugArg ("pNodeName=", pName); 
#endif

  if (CNTRLXMLGetFirstChildValue (pNodePtr, pOutBuf, ulOutBufLength)
      != CNTRLXML_SUCCESS)
  {
    XML_Debug ("Could not find the value of first child");
    return CNTRLXML_NOTFOUND;
  }

  return CNTRLXML_SUCCESS;

}

/***********************************************************************
 * Function Name : CNTRLXMLGetFirstChildValue
 * Description   : This function retrieves the value of
 				   the child of the given element.
 * Input         : pInOpaqueElement - Given Element
 * 				   ulOutBufLength - String length
 * Output		   pOutBuf - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_NOTFOUND
 ***********************************************************************/

int32_t
CNTRLXMLGetFirstChildValue (void * pInOpaqueElement,
                          unsigned char * pOutBuf, uint32_t ulOutBufLength)
{
  void *pNodePtr = NULL;
  int32_t iErrCode = 0;

  if (!pInOpaqueElement || ulOutBufLength == 0)
  {
    XML_Error ("Invalid parameters");
    return CNTRLXML_BADPARAM;
  }
  iErrCode = CNTRLXMLGetFirstChildElement (pInOpaqueElement, &pNodePtr);

  if (iErrCode != CNTRLXML_SUCCESS || pNodePtr == NULL)
  {
    XML_Error ("Could not find the child");
    return iErrCode;
  }

#ifdef PSP_CORE_DEBUG
 pName = ixmlNode_getNodeName (pNodePtr);
 XML_DebugArg ("pNodeName=\r\n", pName); 
#endif

  if (CNTRLXMLGetNodeValueByLength (pNodePtr, pOutBuf, ulOutBufLength)
      != CNTRLXML_SUCCESS)
  {
    XML_Error ("Could not find the value of first child");
    return CNTRLXML_NOTFOUND;
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLSetFirstChildValueByTagName
 * Description   : This function sets the value of
 				   the child of the given element.
 * Input         : pTagName - Element tag name.
 *				   pInOpaqueElement - Given Element
 * Output		   pOutBuf - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM, CNTRLXML_FAILURE
 ***********************************************************************/
int32_t
CNTRLXMLSetFirstChildValueByTagName (char * pTagName,
                                   void * pInOpaqueElement,
                                   char * pInBuf)
{
  void *pChildNode = NULL;
  void *pTextNode = NULL;
  int32_t iErrCode = 0;

  if (!pTagName || !pInOpaqueElement || !pInBuf)
  {
    return CNTRLXML_BADPARAM;
  }
  iErrCode =
    CNTRLXMLGetFirstChildByTagName ((unsigned char *)pTagName, pInOpaqueElement, 
          &pChildNode);
  if (iErrCode != CNTRLXML_SUCCESS || pChildNode == NULL)
  {
    return iErrCode;
  }

  iErrCode = CNTRLXMLGetFirstChildElement (pChildNode, &pTextNode);

  if (iErrCode != CNTRLXML_SUCCESS || pChildNode == NULL)
  {
    XML_Error ("Could not find the child");
    return iErrCode;
  }

  if (CNTRLXMLSetElementValue (pTextNode, pInBuf) != CNTRLXML_SUCCESS)
  {
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;

}

/***********************************************************************
 * Function Name : CNTRLXMLGetNodeName
 * Description   : This function retrieves the name of
 *                 the given element.
 * Input         : pInOpaqueElement - Given Element
 * Output		   pOutName - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 * **********************************************************************/
int8_t
CNTRLXMLGetNodeName (void * pInOpaqueElement, unsigned char * pOutName)
{
  if (!pInOpaqueElement || !pOutName)
  {
    return CNTRLXML_BADPARAM;
  }
  strcpy ((char *)pOutName,
            (char *) ixmlNode_getNodeName ((IXML_Node *)
                                               pInOpaqueElement));

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLDoc
 * Description   : This function frees the XML Doc returned from
 *                 the function CNTRLXMLParseXML.
 * Input         : pInOpaqueDoc - Given XML Doc
 * Return value  : Void
 ***********************************************************************/
void
CNTRLXMLFreeXMLDoc (void ** pInOpaqueDoc)
{
  if (*pInOpaqueDoc != NULL)
  {
    ixmlDocument_free ((IXML_Document *) (*pInOpaqueDoc));
  }
  return;
}

/***********************************************************************
 * Function Name	: CNTRLXMLFreeXMLNode
 * Description		: This function frees the XML Node returned from
 *				   the function CNTRLXMLParseXML.
 * Input			: pInOpaqueNode - Given XML Doc
 * Return value  : Void
 ***********************************************************************/
void
CNTRLXMLFreeXMLNode (void ** pInOpaqueNode)
{
  if (*pInOpaqueNode != NULL)
  {
    ixmlNode_free ((IXML_Node *) (*pInOpaqueNode));
  }
  return;
}

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLElementList
 * Description   : This function frees the Element List returned
 *                 from the function CNTRLXMLGetElementsByTagName.
 * Input         : pInOpaqueElement - Given Element List
 * Return value  : void
 ***********************************************************************/
void
CNTRLXMLFreeXMLElementList (void ** pInOpaqueElement)
{
  if (*pInOpaqueElement != NULL)
  {
    ixmlNodeList_free ((IXML_NodeList *) (*pInOpaqueElement));
  }
  return;
}

/***********************************************************************
 * Function Name : CNTRLXMLFreeXMLAttribList
 * Description   : This function frees the Attribute List returned
 *                 from the function CNTRLXMLGetTagElementFirstAttrib.
 * Input         : pInOpaqueAttrib - Given Attribute List
 * Output          pOutName - Value of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
void
CNTRLXMLFreeXMLAttribList (void ** pInOpaqueAttrib)
{
  if (*pInOpaqueAttrib != NULL)
  {
    ixmlNamedNodeMap_free ((IXML_NamedNodeMap *) (*pInOpaqueAttrib));
  }
  return;
}

/***********************************************************************
 * Function Name : CNTRLXMLSetXSLFile
 * Description   : This function updates the XML declaration with the
 *                 passed pInXSLFile file name.
 * Input         : pInXSLFile - Given XSL file name
 * Output          pOutXMLDecl - updated XML header string
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 ***********************************************************************/
int8_t
CNTRLXMLSetXSLFile (char * pInXSLFile, char ** pOutXMLDecl)
{
  unsigned char ucFileName[XML_MAX_NAMELEN + 1];
  char *pLangName = NULL;

  if (!pInXSLFile)
  {
    return CNTRLXML_BADPARAM;
  }
  /*
   * Populate the file name.
   *
   */
  pLangName = PSPXmlGetLanguage ();
  if (pLangName)
  {
    sprintf ((char *)ucFileName, "%s_%s.xsl", pInXSLFile, pLangName);
    free (pLangName);
  }
  else
  {
    return CNTRLXML_FAILURE;
  }
  XML_DebugArg ("ucFileName = ", ucFileName);

  *pOutXMLDecl = xmlStrdup ((const xmlChar *) xmlDeclare);
  *pOutXMLDecl = xmlStrcat (*pOutXMLDecl, (const xmlChar *) ucFileName);
  *pOutXMLDecl = xmlStrcat (*pOutXMLDecl, (const xmlChar *) endXmlDeclare);

  return CNTRLXML_SUCCESS;
}

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
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   CNTRLXML_FAILURE, CNTRLXML_NOTFOUND.
 ***********************************************************************/
int8_t
CNTRLXMLGetNextCommand (void * pInOpaqueDoc,
                      void * pInOpaqueElement,
                      void ** pOutOpaqueElement, unsigned char * pOutCmdName)
{
  IXML_NodeList *pNodeList = NULL;
  IXML_Node *pNode = NULL;

  if (!pOutCmdName || !pOutOpaqueElement)       /*To be fixed */
  {
    XML_Debug ("Bad Parameters");
    return CNTRLXML_BADPARAM;
  }

  if (pInOpaqueDoc)
  {
    /* We need the first command */
    /* The function below returns void. So no Err check needed. */
    ixmlNode_getElementsByTagName ((IXML_Node *) pInOpaqueDoc,
                                   CNTRLXML_ACTIONTAG, &pNodeList);
    if (pNodeList)
    {
      /* Validate pNodeList */
      if (!pNodeList->nodeItem)
      {
        XML_Error ("Return2");
        ixmlNodeList_free (pNodeList);
        return CNTRLXML_FAILURE;
      }

      /* Get the first Command */
      pNode = ixmlNode_getFirstChild (pNodeList->nodeItem);

      ixmlNodeList_free (pNodeList);
    }
    else
    {
      /* Action Tag Not Found */
      XML_Debug ("Return3");
      *pOutOpaqueElement = NULL;
      return CNTRLXML_SUCCESS;
    }
  }
  else
  {
    /* We need the next command */
    if (!pInOpaqueElement)
    {
      XML_Debug ("Return4");
      return CNTRLXML_BADPARAM;
    }

    pNode = ixmlNode_getNextSibling ((IXML_Node *) pInOpaqueElement);
    if (!pNode)
    {
      XML_Debug ("Return4");
      *pOutOpaqueElement = NULL;
      return CNTRLXML_FAILURE;
    }
  }

  /* Update Output parms */
  *pOutOpaqueElement = (void *) pNode;
  XML_DebugArg ("pNode->nodelName = ", pNode->nodeName);
  /*printf("pNode->firstAttr->nodeName = %s",pNode->firstAttr->nodeName);
     printf("pNode->firstAttr->nodeValue = %s",pNode->firstAttr->nodeValue);
     printf("pNode->firstAttr->nextSibling->nodeName = %s",pNode->firstAttr->nextSibling->nodeName);
     printf("pNode->firstAttr->nextSibling->nodeValue = %s",pNode->firstAttr->nextSibling->nodeValue); */

  if (!pNode)
  {
    XML_Debug ("Return5");
  }
  else
  {
    strcpy ((char *)pOutCmdName, (char *) (pNode->nodeName));
    XML_DebugArg ("*pOutCmdName= ", pOutCmdName);
  }

  return CNTRLXML_SUCCESS;
}

/***********************************************************************
 * Function Name : CNTRLXMLSetElementValue
 * Description   : This function sets the value of the given
 *                 element.
 * Input         : pInElement - current Element
 *				   pInValue   - value to be set
 * Output        : none
 *
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   CNTRLXML_FAILURE.
 ***********************************************************************/
int8_t
CNTRLXMLSetElementValue (void * pInElement, char * pInValue)
{

  int iErrCode = 0;
  IXML_Node *pChildNode = NULL;

  if (!pInElement || !pInValue)
    return CNTRLXML_BADPARAM;

  pChildNode = ixmlNode_getFirstChild ((IXML_Node *) pInElement);
  if (pChildNode)
  {
    iErrCode = ixmlNode_getNodeType (pChildNode);

    if ((pChildNode != 0)
        && (ixmlNode_getNodeType (pChildNode) == eTEXT_NODE))
    {
      iErrCode = ixmlNode_setNodeValue (pChildNode, pInValue);
    }
  }
  else
  {
    pChildNode =
      ixmlDocument_createTextNode (((IXML_Node *) pInElement)->ownerDocument,
                                   pInValue);
    if (pChildNode)
    {
      iErrCode =
        ixmlNode_appendChild (((IXML_Node *) pInElement), pChildNode);
    }
    else
    {
      XML_Error ("could not create or set pChildNode value");
      return CNTRLXML_FAILURE;
    }
  }

  return iErrCode;
}

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
int8_t
CNTRLXMLFillAttributeValues (void ** pOutElement, char * ValArray[],
                           int32_t recSize)
{
  IXML_Node *prop;
  IXML_NamedNodeMap *pAttribs = NULL;
  int ulAttrCount = 0;
  int i = 0, iErrCode = 0;

  if (*pOutElement == NULL)
    return CNTRLXML_BADPARAM;

  pAttribs = ixmlNode_getAttributes ((IXML_Node *) * pOutElement);
  if (!pAttribs)
  {
    return CNTRLXML_FAILURE;
  }

  ulAttrCount = ixmlNamedNodeMap_getLength (pAttribs);

  for (i = 0; i < ulAttrCount; i++)
  {
    if (i == recSize)
      break;

    prop = ixmlNamedNodeMap_item (pAttribs, i);

    iErrCode = ixmlElement_setAttribute ((IXML_Element *) * pOutElement,
                                         (char *)
                                         ixmlNode_getNodeName (prop),
                                         ValArray[i]);
    if (iErrCode != 0)
    {
      XML_Debug ("Unable to set the attribute value");
      ixmlNamedNodeMap_free (pAttribs);
      return CNTRLXML_FAILURE;
    }
  }                             /* for */

  ixmlNamedNodeMap_free (pAttribs);
  return CNTRLXML_SUCCESS;

}

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
int8_t
CNTRLXMLFillChildElementValues (void ** pOutElement, char * ValArray[],
                              int32_t recSize)
{
  IXML_Node *pChildNode = NULL;
  int32_t iErrCode = 0, i = 0;
  IXML_Document *ownerDoc = NULL;

  if (*pOutElement == NULL)
  {
    return CNTRLXML_BADPARAM;
  }

  if (ixmlNode_hasChildNodes ((IXML_Node *) * pOutElement))
  {
    ownerDoc = ixmlNode_getOwnerDocument ((IXML_Node *) * pOutElement);
    if (!ownerDoc)
    {
      ownerDoc =
        ixmlNode_getOwnerDocument (((IXML_Node *) * pOutElement)->parentNode);
    }
    pChildNode = ((IXML_Node *) * pOutElement)->firstChild;

    while (pChildNode && i < recSize)
    {
      if (!ixmlNode_getOwnerDocument ((IXML_Node *) pChildNode))
      {
        ((IXML_Node *) pChildNode)->ownerDocument = ownerDoc;
      }
      if (ixmlNode_hasChildNodes (pChildNode)
          && (ixmlNode_getNodeType (pChildNode->firstChild) != eTEXT_NODE))
      {
        int32_t subRecSize = 0;

        subRecSize = recSize - i;

        iErrCode =
          CNTRLXMLFillChildElementValues ((void **) & pChildNode,
                                        &ValArray[i], subRecSize);
        if (iErrCode != 0)
        {
          XML_Error ("Recursion ERROR: Unable to set NODE value ");
          return CNTRLXML_FAILURE;
        }
      }
      else
      {
        iErrCode = CNTRLXMLSetElementValue (pChildNode, ValArray[i]);
        if (iErrCode != 0)
        {
          XML_Error (" Unable to set NODE value ");
          return CNTRLXML_FAILURE;
        }
      }                         /*else */

      if (iErrCode != 0)
      {
        XML_Error ("Unable to set NODE value ");
        return CNTRLXML_FAILURE;
      }
      pChildNode = pChildNode->nextSibling;
      i++;
    }                           /*while */

  }
  return CNTRLXML_SUCCESS;
}

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
int8_t
CNTRLXMLGetAttributeValue (unsigned char * pInAttribName,
                         void * pInOpaqueRec, unsigned char * pOutAtrribValue)
{
  IXML_Node *ptmpNode = NULL;
  IXML_Node *pAttribNode = NULL;
  unsigned char *pName;
  unsigned char *pAttrValue = NULL;

  /*pOutAtrribValue ||    !pInOpaqueRec) */
  if (!pInAttribName || (strlen ((char *)pInAttribName) == 0) 
          || !pInOpaqueRec)
  {
    return CNTRLXML_BADPARAM;
  }

  ptmpNode = (IXML_Node *) pInOpaqueRec;
  if (!ptmpNode)
  {
    return CNTRLXML_FAILURE;
  }

  pAttribNode = ptmpNode->firstAttr;
  while (pAttribNode)
  {
    pName = (unsigned char *) ixmlNode_getNodeName (pAttribNode);

    if (!strcmp ((char *)pInAttribName, (char *)pName))
    {
      pAttrValue = (unsigned char *)ixmlNode_getNodeValue (pAttribNode);
      if (pAttrValue)
      {                         /* Found a match */
        strcpy ((char *)pOutAtrribValue, (char *)pAttrValue);
        return CNTRLXML_SUCCESS;
      }
      else
      {
        return CNTRLXML_NOTFOUND;
      }

      /* XML_DebugArg("pOutAtrribValue=",pOutAtrribValue); */
    }
    /* Get Next sibling */
    pAttribNode = pAttribNode->nextSibling;
  }

  return CNTRLXML_NOTFOUND;
}

/***********************************************************************
 * Function Name : CNTRLXMLSetNodeName
 * Description   : This function sets the name of
 *                 the given element.
 * Input         : pInOpaqueElement - Given Element
 *                 pInName - name of the element
 * Return value  : CNTRLXML_SUCCESS, CNTRLXML_BADPARAM
 * **********************************************************************/
int8_t
CNTRLXMLSetNodeName (void * pInOpaqueElement, unsigned char * pInName)
{
  if (!pInOpaqueElement || !pInName)
  {
    return CNTRLXML_BADPARAM;
  }
  return ixmlNode_setNodeName ((IXML_Node *) pInOpaqueElement, 
          (char *)pInName);
}

#ifdef PSP_XML_SCHEMA_VAL_SUPPORT
/***********************************************************************
 * Function Name 	: CNTRLXMLLoadSchema
 * Description   	: This function loads an XML Schema
 * Input         		: pInSchema - XML Schema to load
 * Output 	       : pOutSchemaCtxt - output Schema context
 * Return value  	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   		  CNTRLXML_FAILURE.
 ***********************************************************************/
int32_t
CNTRLXMLLoadSchema (unsigned char * pInSchema, void ** pOutSchemaCtxt)
{
  xmlSchemaParserCtxtPtr ctxt = NULL;
  unsigned char ucFileName[XML_MAX_NAMELEN + 1];

  XML_DebugArg ("pInSchema =", pInSchema);
  if (!pInSchema || (strlen (pInSchema) == 0) || !(pOutSchemaCtxt))   /*To be fixed:: pOutSchemaCtxt can be NULL */
  {
    XML_Error ("Bad Param");
    return CNTRLXML_BADPARAM;
  }

          /**
	   * Construct absolute path.
	   */
  sprintf (ucFileName, "%s%s", XML_GetDocPath (), pInSchema);

  XML_DebugArg ("ucFileName =", ucFileName);

  ctxt = xmlSchemaNewParserCtxt (ucFileName);

  xmlSchemaSetParserErrors (ctxt,
                            (xmlSchemaValidityErrorFunc) fprintf,
                            (xmlSchemaValidityWarningFunc) fprintf, stderr);

  *pOutSchemaCtxt = (xmlSchemaPtr) xmlSchemaParse (ctxt);

  xmlSchemaFreeParserCtxt (ctxt);

  if (*pOutSchemaCtxt == NULL)
  {
    XML_Error ("Unable to Load");
    return CNTRLXML_FAILURE;
  }
  else
  {
    return CNTRLXML_SUCCESS;
  }
}

/***********************************************************************
 * Function Name 	: CNTRLXMLValidateXML
 * Description   	: This function validates a XML Doc using Schema
 * Input         		: pInSchemaCtxt - XML Schema to load
 *				  pInXMLDoc - XML Doc to be validated
 * Output 	       : None
 * Return value  	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   		  CNTRLXML_FAILURE.
 ***********************************************************************/
int32_t
CNTRLXMLValidateXML (void * pInSchemaCtxt, void * pInXMLDoc)
{
  xmlSchemaValidCtxtPtr ctxt;
  char iRetVal;

  if (!pInSchemaCtxt || !pInXMLDoc)
  {
    XML_Error ("Bad Param");
    return CNTRLXML_BADPARAM;
  }

  ctxt = xmlSchemaNewValidCtxt ((xmlSchemaPtr) pInSchemaCtxt);

  if (ctxt)
    XML_Debug ("xmlSchemaNewValidCtxt returns success");
  else
    XML_Debug ("xmlSchemaNewValidCtxt returns failure");

  xmlSchemaSetValidErrors (ctxt,
                           (xmlSchemaValidityErrorFunc) fprintf,
                           (xmlSchemaValidityWarningFunc) fprintf, stderr);

  iRetVal = (char) xmlSchemaValidateDoc (ctxt, (IXML_Document *) pInXMLDoc);

  if (iRetVal == 0)
  {
    XML_Debug ("xmlSchemaValidateDoc returns Success");
    return CNTRLXML_SUCCESS;
  }
  else
  {
    XML_Error ("xmlSchemaValidateDoc returns Failed");
    return CNTRLXML_FAILURE;
  }

}

#ifdef PSP_XML_MULTIMODSET_SUPPORT
/***********************************************************************
 * Function Name 	: CNTRLXMLValidateXMLElement
 * Description   	: This function validates a XML Element using Schema
 * Input         		: pInSchemaCtxt - XML Schema to load
 *				  pInXMLElem - XML Element to be validated
 * Output 	       : None
 * Return value  	: CNTRLXML_SUCCESS, CNTRLXML_BADPARAM,
                   		  CNTRLXML_FAILURE.
 ***********************************************************************/
int32_t
CNTRLXMLValidateXMLElement (void * pInSchemaCtxt, void * pInXMLElem)
{
  xmlSchemaValidCtxtPtr ctxt;
  char iRetVal;

  if (!pInSchemaCtxt || !pInXMLElem)
  {
    XML_Error ("Bad Param");
    return CNTRLXML_BADPARAM;
  }

  ctxt = xmlSchemaNewValidCtxt ((xmlSchemaPtr) pInSchemaCtxt);
  if (ctxt)
    XML_Debug ("xmlSchemaNewValidCtxt returns success");
  else
    XML_Debug ("xmlSchemaNewValidCtxt returns failure");
  xmlSchemaSetValidErrors (ctxt,
                           (xmlSchemaValidityErrorFunc) fprintf,
                           (xmlSchemaValidityWarningFunc) fprintf, stderr);

  iRetVal =
    (char) xmlSchemaValidateOneElement (ctxt, (IXML_Node *) pInXMLElem);

  if (iRetVal == 0)
  {
    XML_Debug ("xmlSchemaValidateOneElement returns success");
    return CNTRLXML_SUCCESS;
  }
  else
  {
    XML_Error ("xmlSchemaValidateOneElement returns Failure");
    return CNTRLXML_FAILURE;
  }

}

#endif /* PSP_XML_MULTIMODSET_SUPPORT */
#endif /* PSP_XML_SCHEMA_VAL_SUPPORT */
/***************************************************************************
* * * * * * * * * * U T I L I T I E S * * * * * * * * * * * * * * * * * * *
****************************************************************************/

int32_t
CNTRLXMLGetTagValue (char * pInTag, void * pCurrentCommand,
                   char * pOutValue)
{

  void *pChildRecord = NULL;
  unsigned char iErrCode = 0;

  XML_Debug ("Entered ");

  pChildRecord = NULL;
  if ((iErrCode =
       CNTRLXMLGetFirstChildByTagName ((unsigned char *)pInTag, pCurrentCommand,
                                     &pChildRecord)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("Tag NODE  Not found");
    return CNTRLXML_FAILURE;
  }
  else
  {
    iErrCode = CNTRLXMLGetNodeValue (pChildRecord, (unsigned char *)pOutValue);
    if (iErrCode == CNTRLXML_SUCCESS)
    {
      XML_DebugArg ("Node Value=", pOutValue);
    }
  }

  return CNTRLXML_SUCCESS;
}

int32_t
CNTRLXMLLoadXMLDOM (char * pXmlFile, void ** pXmlDOM)
{
  int32_t iRetVal = 0;

  XML_Debug ("Entered");

  /*Load XML template file */
  if ((iRetVal = CNTRLXMLLoadXML ((unsigned char *)pXmlFile, 
            pXmlDOM)) != CNTRLXML_SUCCESS)
  {
    XML_ErrorArg ("CNTRLXMLLoadXML Failed for file =", pXmlFile);
    return iRetVal;
  }

  /* Process Include Tags in XML Template */
  if ((iRetVal = CNTRLXMLProcessIncludeTags (pXmlDOM)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLProcessIncludeTags returs Failure");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

int32_t
PSPConfigLoadXMLDOM (char * pXmlFile, void ** pXmlDOM)
{  
  int32_t iRetVal = 0;
   
  XML_Debug ("Entered");
   
  /*Load XML template file */
  if ((iRetVal = PSPConfigLoadXML ((unsigned char *)pXmlFile, 
             pXmlDOM)) != CNTRLXML_SUCCESS) 
  {
    XML_ErrorArg ("PSPConfigLoadXML Failed for file =", pXmlFile);
    return iRetVal;
  }
   
  /* Process Include Tags in XML Template */
  if ((iRetVal = CNTRLXMLProcessIncludeTags (pXmlDOM)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLProcessIncludeTags returs Failure");
    return CNTRLXML_FAILURE;
  }
  
  return CNTRLXML_SUCCESS;
}   


int32_t
CNTRLXMLGetNodebyTagName (void * pInNode,
                        char * pKeyTagName, void ** pOutSearchNode)
{
  IXML_Node *pCurrNode = pInNode;
  IXML_Node *pChildNode = NULL;
  IXML_Node *pSiblingNode = NULL;
  const char *pNodeName = NULL;

  if (!pCurrNode)
  {
    XML_Error ("BAD Parameters");
    return CNTRLXML_FAILURE;
  }
  if (*pOutSearchNode)
  {
    XML_Debug ("pOutSearchNode already filled");
    return CNTRLXML_SUCCESS;
  }

  if (pCurrNode)
  {
    pNodeName = ixmlNode_getNodeName (pCurrNode);
/*  XML_DebugArg ("pNodeName=", pNodeName);*/
    if (!xmlStrcmp (pNodeName, (xmlChar *) pKeyTagName))
    {
     if ( ixmlNode_getNodeType(pCurrNode) == eELEMENT_NODE )
     {
       XML_Debug ("Search Tag Found");
      *pOutSearchNode = pCurrNode;
      return CNTRLXML_SUCCESS;
     }
    }
    pSiblingNode = pCurrNode->nextSibling;
    pChildNode = pCurrNode->firstChild;
  
  
   if (pChildNode)
      CNTRLXMLGetNodebyTagName (pChildNode, pKeyTagName, pOutSearchNode);
    if (pSiblingNode)
      CNTRLXMLGetNodebyTagName (pSiblingNode, pKeyTagName, pOutSearchNode);
  }
  return CNTRLXML_SUCCESS;
}

int32_t
CNTRLXMLGetTagValues (void * pInXMLReq, XMLAttribs XMLParams[],
                    uint32_t ulCnt)
{

  uint32_t ulIndex = 0;
  int32_t iErrCode;
  void *pSearchTagNode = pInXMLReq;
  void *pChildRecord = NULL;

  XML_Debug ("Entered ");
  if (!pSearchTagNode)
  {
    XML_Error ("pSearchTagNode is NULL");
    return CNTRLXML_FAILURE;
  }

  for (ulIndex = 0; ulIndex < ulCnt; ulIndex++)
  {
    iErrCode =
      CNTRLXMLGetFirstChildByTagName (XMLParams[ulIndex].AttribName,
                                    pSearchTagNode, &pChildRecord);
    if (iErrCode == CNTRLXML_SUCCESS)
    {

      memset (XMLParams[ulIndex].AttribValue, 0,
                sizeof (XMLParams[ulIndex].AttribValue));
      iErrCode =
        CNTRLXMLGetNodeValue (pChildRecord, XMLParams[ulIndex].AttribValue);
    }
    XML_DebugArg ("AttribName = ", XMLParams[ulIndex].AttribName);
    XML_DebugArg ("AttribValue = ", XMLParams[ulIndex].AttribValue);

    pChildRecord = NULL;
  }                             /* for loop */
  return CNTRLXML_SUCCESS;
}

#ifdef PSP_XML_NOCODE

int32_t
CNTRLXMLAddErrorMessageToXMLDOM (char * pErrorMsg, void  *pXMLDoc)
{
  int32_t iErrCode = 0;
  void *pRootNode = NULL;
  void *pErrorXMLDoc = NULL;
  void *pErrorRootNode = NULL;
  char pErrorXMLBuffer[] =
    "<Error><Message/></Error>";
  XMLAttribs ErrorAttribs[1] = {
    {
     "Message", ""}
  };

  XML_Debug ("Entered");

  if ((iErrCode=CNTRLXMLGetRootNode (pXMLDoc, &pRootNode)) != CNTRLXML_SUCCESS)
  {
	XML_Error("CNTRLXMLGetRootNode Failed");
	return CNTRLXML_FAILURE;
  }

  if (pErrorMsg)
  {
    strcpy (ErrorAttribs[0].AttribValue, pErrorMsg);
  }
  else
  {
    strcpy (ErrorAttribs[0].AttribValue, "Action Error");
  }

  if ((iErrCode =
       CNTRLXMLParseXML (pErrorXMLBuffer, &pErrorXMLDoc)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLParseXML Failed");
    return CNTRLXML_FAILURE;
  }
 
  if((iErrCode=  CNTRLXMLPopulateXMLDOM (pErrorXMLDoc, NULL, ErrorAttribs,1)) != CNTRLXML_SUCCESS)
  {
      XML_Error("CNTRLXMLPopulateXMLDom failed");
      return CNTRLXML_FAILURE;
  }
 
  if((CNTRLXMLGetRootNode (pErrorXMLDoc, &pErrorRootNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error("CNTRLXMLGetRootNode Failed");
    return CNTRLXML_FAILURE;
  }

  if ((iErrCode=CNTRLXMLAppendChildNode (&pRootNode, pErrorRootNode)) != CNTRLXML_SUCCESS)
  {
     XML_Error("CNTRLXMLAppendChildNode Failed");
     return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}


int32_t
IGXMLSendXMLDOM (void * pXmlDOM)
{
  void *pRootNode = NULL;
  char aXSLFile[CNTRLXML_MAX_XSL_FILENAME_LEN];
  char *pXSLString = NULL, *pXmlString = NULL;
  int32_t iRetVal = 0;

  if (!pXmlDOM)
  {
    XML_Error ("Invalid Input");
    return CNTRLXML_FAILURE;
  }

  /*Get the Root Node of the XML DOM */
  if ((iRetVal = CNTRLXMLGetRootNode (pXmlDOM, &pRootNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLGetRootNode Failed");
    return iRetVal;
  }

  /*Get the XSL File Name */
  if ((iRetVal =
       CNTRLXMLGetAttributeValue (XSLREF, pRootNode,
                                aXSLFile)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLGetAttributeValue for xslref Failed");
    return iRetVal;
  }

  /*Convert XML_DOM to XML_STRING */
  if ((iRetVal = CNTRLXMLtoBuffer (pRootNode, &pXmlString)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLtoBuffer Failed ");
    return iRetVal;
  }

  /* Set XSL File reference in the Declaration String */
  if ((iRetVal = CNTRLXMLSetXSLFile (aXSLFile, &pXSLString)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLSetXSLFile Failed");
    XMLBUFFERFREE (pXmlString);
    return iRetVal;
  }

  if ((iRetVal = PSPReturnXMLResultString (NULL, pXSLString))
      != CNTRLXML_SUCCESS)
  {
    XML_Error ("PSPReturnXMLResultString Failed");
    XMLBUFFERFREE (pXSLString);
    XMLBUFFERFREE (pXmlString);
    return iRetVal;
  }

  /*Write XML string buffer to XML Handler response */
  if ((iRetVal = PSPReturnXMLResultString (NULL, pXmlString))
      != CNTRLXML_SUCCESS)
  {
    XML_Error ("PSPReturnXMLResultString Failed ");
    XMLBUFFERFREE (pXSLString);
    XMLBUFFERFREE (pXmlString);
    return iRetVal;
  }

  /*Free the Memory Allocated */
  XMLBUFFERFREE (pXSLString);
  XMLBUFFERFREE (pXmlString);

  return CNTRLXML_SUCCESS;
}
int32_t
CNTRLXMLCreateInXMLDOM (char * pFileName,
                      void ** pXmlDOM,
                      void * pData, uint32_t ulMaxKeyAttribs)
{
  uint32_t ulIndex = 0, ulNoOfkeys = 0;
  int32_t iRetVal;
  XMLAttribs aXmlParams[ulMaxKeyAttribs];
  PSPGetParams_t *pKeyAttribs = NULL;

  if (!pData)
  {
    XML_Error ("Bad Input Parameters");
    return CNTRLXML_SUCCESS;
  }

  if ((iRetVal = CNTRLXMLLoadXML (pFileName, pXmlDOM)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLLoadXMLDOM Failed");
    return CNTRLXML_FAILURE;
  }

  memset (aXmlParams, 0, sizeof (aXmlParams));
  pKeyAttribs = (PSPGetParams_t *) pData;

  for (ulIndex = 0; ulIndex < ulMaxKeyAttribs; ulIndex++)
  {
    if (strcmp (pKeyAttribs[ulIndex].ucValue, ""))
    {
      strcpy (aXmlParams[ulIndex].AttribName, pKeyAttribs[ulIndex].ucName);
      strcpy (aXmlParams[ulIndex].AttribValue,
                pKeyAttribs[ulIndex].ucValue);
		XML_DebugArg("Name=",aXmlParams[ulIndex].AttribName);
		XML_DebugArg("Value=",aXmlParams[ulIndex].AttribValue);
      ulNoOfkeys++;
    }
  }

  if ((iRetVal = CNTRLXMLPopulateXMLDOM (*pXmlDOM, NULL, aXmlParams,
                                       ulNoOfkeys)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLPopulateXMLDOM Failed");
    return CNTRLXML_FAILURE;
  }

  return CNTRLXML_SUCCESS;
}

int32_t
CNTRLXMLCreateErrorXMLDOM (char * pModTag, int32_t iErrCode,
                         char * pErrMsg, void ** ppOutXmlDom)
{
  char pInXMLBuffer[] =
    "<Response><ErrorCode/><ErrorMsg/><Module/><Method/></Response>";
  char aModInfo[MAX_NAME_LEN];
  char *pModName = NULL;
  char *pModMethod = NULL;
  int32_t iCount = 0;
  void *pNextRecord = NULL;
  void *pRootNode = NULL;
  void *pErrorXMLDoc = NULL;
  void *pErrorRootNode = NULL;
  XMLAttribs ErrorDescAttribs[4] = {
    {"ErrorCode", ""}
    ,
    {"ErrorMsg", ""}
    ,
    {"Module", ""}
    ,
    {"Method", ""}
  };

  XML_Debug ("Entered");

  if (iErrCode == 0)
    strcpy (ErrorDescAttribs[0].AttribValue, "0");
  else
    PSPitoa (iErrCode, ErrorDescAttribs[0].AttribValue);

  if (pErrMsg)
    strcpy (ErrorDescAttribs[1].AttribValue, pErrMsg);
  else
    strcpy (ErrorDescAttribs[1].AttribValue, "No Error Msg");

  if (pModTag)
  {
    strcpy (aModInfo, pModTag);

    pModName = strtok (aModInfo, "_");
    pModMethod = strtok (NULL, "_");
  }

  if (pModName)
    strcpy (ErrorDescAttribs[2].AttribValue, pModName);
  else
    strcpy (ErrorDescAttribs[2].AttribValue, "NoModule");
  if (pModMethod)
    strcpy (ErrorDescAttribs[3].AttribValue, pModMethod);
  else
    strcpy (ErrorDescAttribs[3].AttribValue, "NoMethod");

  if ((iErrCode =
       CNTRLXMLParseXML (pInXMLBuffer, &pErrorXMLDoc)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLParseXML Failed");
    return CNTRLXML_FAILURE;
  }

  if((CNTRLXMLGetRootNode (pErrorXMLDoc, &pErrorRootNode)) != CNTRLXML_SUCCESS)
  {
     XML_Error("CNTRLXMLGetRootNode Failed for pErrorXMLDoc,");
     CNTRLXMLFreeXMLDoc (&pErrorXMLDoc);
     return CNTRLXML_FAILURE;
  }


  for (iCount = 0; iCount < 4; iCount++)
  {
    iErrCode =
      CNTRLXMLGetFirstChildByTagName (ErrorDescAttribs[iCount].AttribName,
                                    pErrorRootNode, &pNextRecord);
    if (iErrCode == CNTRLXML_SUCCESS)
    {
      iErrCode =
        CNTRLXMLSetElementValue (pNextRecord,
                               ErrorDescAttribs[iCount].AttribValue);
    }
    if (iErrCode != CNTRLXML_SUCCESS)
    {
      CNTRLXMLFreeXMLDoc (&pErrorXMLDoc);
      return CNTRLXML_FAILURE;
    }
    pNextRecord = NULL;
  }


  if( *ppOutXmlDom == NULL )/* Is a Post Request */
   {
       *ppOutXmlDom=pErrorXMLDoc;
   }
  else 
  {
    if((CNTRLXMLGetRootNode (*ppOutXmlDom, &pRootNode)) != CNTRLXML_SUCCESS)
    {
     XML_Error("CNTRLXMLGetRootNode Failed for ppOutXmlDom");
     CNTRLXMLFreeXMLDoc (&pErrorXMLDoc);
     return CNTRLXML_FAILURE;
    }
    if ((iErrCode=CNTRLXMLAppendChildNode (&pRootNode, pErrorRootNode)) != CNTRLXML_SUCCESS)
    {
       XML_Error("CNTRLXMLAppendChildNode Failed");
       CNTRLXMLFreeXMLDoc (&pErrorXMLDoc);
       return CNTRLXML_FAILURE;
     }
   }
  return CNTRLXML_SUCCESS;
}

int32_t
CNTRLXMLCreateSuccessXMLDOM (char * pModTag, int32_t iSuccessCode,
                           char * pSuccessMsg, void ** ppOutXmlDom)
{
  char pInXMLBuffer[] =
    "<Response><SuccessCode/><SuccessMsg/><Module/><Method/></Response>";
  char aModInfo[MAX_NAME_LEN];
  char *pModName = NULL;
  char *pModMethod = NULL;
  int32_t iCount = 0;
  int32_t iErrCode = 0;
  void *pNextRecord = NULL;
  void *pRootNode = NULL;
  XMLAttribs RespAttribs[4] = {
    {
     "SuccessCode", ""}
    ,
    {
     "SuccessMsg", ""}
    ,
    {
     "Module", ""}
    ,
    {
     "Method", ""}
  };

  XML_Debug ("Entered");

  if (iSuccessCode == 0)
    strcpy (RespAttribs[0].AttribValue, "0");
  else
    PSPitoa (iSuccessCode, RespAttribs[0].AttribValue);

  if (pSuccessMsg)
    strcpy (RespAttribs[1].AttribValue, pSuccessMsg);
  else
    strcpy (RespAttribs[1].AttribValue, "No Success Msg");

  if (pModTag)
  {
    strcpy (aModInfo, pModTag);

    pModName = strtok (aModInfo, "_");
    pModMethod = strtok (NULL, "_");
  }

  if (!pModName)
    strcpy (pModName, "NoModule");
  if (!pModMethod)
    strcpy (pModMethod, "NoMethod");

  strcpy (RespAttribs[2].AttribValue, pModName);
  strcpy (RespAttribs[3].AttribValue, pModMethod);

  if ((iErrCode =
       CNTRLXMLParseXML (pInXMLBuffer, ppOutXmlDom)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("CNTRLXMLParseXML Failed");
    return CNTRLXML_FAILURE;
  }
  CNTRLXMLGetRootNode (*ppOutXmlDom, &pRootNode);

  for (iCount = 0; iCount < 4; iCount++)
  {
    iSuccessCode =
      CNTRLXMLGetFirstChildByTagName (RespAttribs[iCount].AttribName, pRootNode,
                                    &pNextRecord);
    if (iErrCode == CNTRLXML_SUCCESS)
    {
      iErrCode =
        CNTRLXMLSetElementValue (pNextRecord, RespAttribs[iCount].AttribValue);
    }
    else
    {
      return CNTRLXML_FAILURE;
    }
    pNextRecord = NULL;
  }

  return CNTRLXML_SUCCESS;

}

#endif
int32_t
CNTRLXMLPopulateXMLDOM (void * pXMLDoc,
                      char * pKeyTagName,
                      XMLAttribs Params[], uint32_t uiCount)
{
  int iErrCode = 0;
  void *pRootNode = NULL;
  void *pSearchTagNode = NULL;

  XML_Debug ("Entered ");

  if ((iErrCode = CNTRLXMLGetRootNode (pXMLDoc, &pRootNode)) != CNTRLXML_SUCCESS)
  {
    XML_Error ("ROOT NODE not found");
    return CNTRLXML_FAILURE;
  }

  XML_DebugArg ("ROOT NODE = ", ((IXML_Node *) pRootNode)->nodeName);

  if (pRootNode)
  {
    if (pKeyTagName)            /* Start Searching from KeyTagName */
    {
      iErrCode =
        CNTRLXMLGetFirstChildByTagName ((unsigned char *)pKeyTagName, pRootNode,
                                      &pSearchTagNode);
      if (pSearchTagNode == NULL)
      {

        if ((iErrCode =
             CNTRLXMLGetNodebyTagName (pRootNode, pKeyTagName,
                                     &pSearchTagNode)) != CNTRLXML_SUCCESS)
        {
          XML_ErrorArg (pKeyTagName, "pKeyTagName NOT FOUND");
          return CNTRLXML_FAILURE;
        }
        if (!pSearchTagNode)
        {
          XML_ErrorArg (pKeyTagName, "pKeyTagName NOT FOUND");
          return CNTRLXML_FAILURE;
        }
      }

    }
    else                        /* Start Searching from Root Node */
      pSearchTagNode = pRootNode;

    if (pSearchTagNode)
    {
      if ((iErrCode =
           CNTRLXMLPopulateXMLValues (pSearchTagNode, Params,
                                    uiCount)) != CNTRLXML_SUCCESS)
      {
        XML_Debug ("CNTRLXMLPopulateXMLValues returned error");
      }
    }                           /* if(pSearchTagNode) */
  }                             /* if pRootNode */

  return CNTRLXML_SUCCESS;
}

int32_t
CNTRLXMLPopulateXMLValues (void * pSearchTagNode,
                         XMLAttribs Params[], uint32_t uiCount)
{

  uint32_t ulIndex = 0;
  int32_t iErrCode;
  void *pNewNode = NULL;
  unsigned char aNodeValue[XML_ATTRIB_VALUE_MAX_LEN] = { };
  void *pChildRecord = NULL;

  memset(aNodeValue, 0, sizeof(aNodeValue));

  XML_Debug ("Entered ");
  if (!pSearchTagNode)
  {
    XML_Error ("pSearchTagNode is NULL");
    return CNTRLXML_FAILURE;
  }

  for (ulIndex = 0; ulIndex < uiCount; ulIndex++)
  {
    iErrCode =
      CNTRLXMLGetFirstChildByTagName (Params[ulIndex].AttribName,
                                    pSearchTagNode, &pChildRecord);
    if (iErrCode == CNTRLXML_SUCCESS)
    {

      memset (aNodeValue, 0, sizeof (aNodeValue));
      iErrCode = CNTRLXMLGetNodeValue (pChildRecord, aNodeValue);

      if ((ulIndex == 0) && (iErrCode != CNTRLXML_NOTFOUND)
          && (iErrCode != CNTRLXML_BADPARAM))
      {

        XML_Debug ("Clone element and set the VALUES");
        if ((iErrCode = CNTRLXMLCloneElementNode (pSearchTagNode,
                                                &pNewNode)) != CNTRLXML_SUCCESS)
        {

          XML_Error ("Unable to clone element");
          return CNTRLXML_FAILURE;
        }
        pSearchTagNode = pNewNode;
        pChildRecord = NULL;
        ulIndex = 0;
        if ((iErrCode =
             CNTRLXMLGetFirstChildByTagName (Params[ulIndex].AttribName,
                                           pSearchTagNode,
                                           &pChildRecord)) != CNTRLXML_SUCCESS)
        {
          return CNTRLXML_FAILURE;
        }
      }
      if (strcmp ((char *)Params[ulIndex].AttribValue, ""))
      {
        iErrCode =
          CNTRLXMLSetElementValue ((IXML_Element *)
                                 pChildRecord, (char *)Params[ulIndex].AttribValue);
        if (iErrCode != CNTRLXML_SUCCESS)
        {

          XML_DebugArg (Params[ulIndex].AttribName, "is not set");
        }
        else
        {
          XML_DebugArg (Params[ulIndex].AttribName, "is set");
        }
      }                         /* if strcmp */
    }

    pChildRecord = NULL;
  }                             /* for loop */
  return CNTRLXML_SUCCESS;
}

void PSPTestAppXmlResp(void *pInOpaqueElement,int32_t iRet,char *pErrMsg)
{
  unsigned char Status[]="<Response><Status></Status><ErrMsg></ErrMsg></Response>";
  void   *pRootNode=NULL,*pOutXML=NULL,*pTempNode=NULL;

  CNTRLXMLParseXML((unsigned char *)&Status,&pOutXML);
  CNTRLXMLGetRootNode(pOutXML,&pRootNode);

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"Status",pRootNode,(void **)&pTempNode);
  if(iRet == PSP_SUCCESS)
    CNTRLXMLSetElementValue(pTempNode,"success");
  else
    CNTRLXMLSetElementValue(pTempNode,"failure");

  if(pErrMsg)
  {
    CNTRLXMLGetFirstChildByTagName((unsigned char *)"ErrMsg",pRootNode,(void **) &pTempNode);
    CNTRLXMLSetElementValue(pTempNode,(char *)pErrMsg);
  }

  CNTRLXMLAppendChildNode((void **)&pInOpaqueElement,pRootNode);
  CNTRLXMLFreeXMLDoc((void **) &pOutXML);
}
#endif /* PSP_XML_SUPPORT */
