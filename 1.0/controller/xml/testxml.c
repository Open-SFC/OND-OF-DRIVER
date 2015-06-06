
#include <stdio.h>
#include "ixml.h"
#include "ixmlmembuf.h"
#include "ixmlparser.h"
#include "cntrlxml.h"
#include "oftype.h"

#define NAME_MAX_LEN			20
#define GROUP_REC "Group"
#define IP_REC "IPRec"

static int32_t xmlPopulateValues(void *doc, 
							 char *ValArray[],
							 unsigned char *recType,
							 int32_t recSize,
							 IXML_NODE_TYPE nodeType)
{
#ifdef XMLDEBUG
	printf("\n**** Entered xmlPopulateValue ....  \n");
#endif

	int iErrCode = 0;
	char CommandName[NAME_MAX_LEN];
	void *pCurrentCommand=NULL, *pNextCommand=NULL;
	void *rootNode = NULL;

	CNTRLXMLGetRootNode(doc, &rootNode);
#ifdef XMLDEBUG
	printf("\n**** ROOT NODE = %s\n",((IXML_Node *)rootNode)->nodeName);
#endif

	iErrCode = CNTRLXMLGetFirstChildElement(rootNode, &pCurrentCommand);
	
	if(iErrCode != 0)
	{
		return OF_FAILURE;
	}

	iErrCode = CNTRLXMLGetNodeName(pCurrentCommand, CommandName);
	
	if(iErrCode != 0)
	{
		return OF_FAILURE;
	}
#ifdef XMLDEBUG
	printf("\n**** CommandName = %s\n",CommandName);
#endif

	pNextCommand = pCurrentCommand;

	while(pNextCommand)
	{
		unsigned char pOutVal[20];
		void *pNextRecord =NULL, *pCurrentRecord = NULL;

		iErrCode = CNTRLXMLGetFirstChildByTagName(recType, pCurrentCommand,
					   										&pNextRecord);


		if(pNextRecord)
		{
			#ifdef XMLDEBUG
				printf("\n**** FOUND RECORD = %s\n", recType);
			#endif


			pCurrentRecord = pNextRecord;


			if(nodeType == eATTRIBUTE)
			{
			#ifdef XMLDEBUG
				printf("\n**** SET ATTRIBUTES ...\n");
			#endif
				CNTRLXMLFillAttributeValues(&pCurrentRecord, ValArray, recSize);
			}
			else if(nodeType == eELEMENT)
			{
				void *child = NULL;
				unsigned char pOutValue[20];

				#if 0
				iErrCode = CNTRLXMLGetFirstChildElement(pCurrentRecord, &child);

				if(iErrCode != CNTRLXML_SUCCESS)
				{
				#ifdef XMLDEBUG
					printf("\n*** No child nodes to populate. \n");
				#endif
					return OF_FAILURE;
				}

				iErrCode = CNTRLXMLGetNodeValue(child, pOutValue);

				#ifdef XMLDEBUG
					printf("\n*** Element VALUE = %s \n", pOutValue);
					printf("\n*** iErrCode = %d \n", iErrCode);
				#endif
				#endif /* #if 0 */

				iErrCode = CNTRLXMLGetFirstChildValueByTagName(recType,
											pCurrentCommand,pOutVal,20);
				printf("\n*** New iErrCode = %d \n", iErrCode);
				printf("\n*** New Element VALUE = %s \n", pOutVal);

				if(iErrCode != CNTRLXML_NOTFOUND)
				{
				#ifdef XMLDEBUG
					printf("\n*** Clone element and set the VALUES...\n");
				#endif
					/*
					 * Clone the node and set the values.
					 */ 
					void *newNode = NULL;

					iErrCode = CNTRLXMLCloneElementNode(pCurrentRecord, &newNode);
					pCurrentRecord = newNode;

					if(iErrCode != CNTRLXML_SUCCESS)
					{
						#ifdef XMLDEBUG
							printf("\n*** Unable to clone element...\n");
						#endif
					}
					else
					{
						CNTRLXMLFillChildElementValues(&pCurrentRecord, ValArray, recSize);
						break;
					}
				}
				else
				{
					CNTRLXMLFillChildElementValues(&pCurrentRecord, ValArray, recSize);
					
					if(CNTRLXMLSetFirstChildValueByTagName(recType, 
                          pCurrentCommand, "NEW-REC-VAL") != CNTRLXML_SUCCESS)
					{
						/*#ifdef XMLDEBUG*/
							printf("\n*** Unable to Set elementi value...\n");
						/*#endif*/
					}
				}
			}
			break;
		} /*if*/
		pNextRecord = NULL;
		pNextCommand = NULL;

		CNTRLXMLGetNextCommand(NULL, pCurrentCommand, &pNextCommand, CommandName);
		if(pNextCommand)
			pCurrentCommand = pNextCommand;

	} /*while*/
	
	return OF_SUCCESS;
}

int main(int argc, char **argv)
{
		void *doc = NULL;
		void *rootNode = NULL;
		void *finalDoc = NULL;
		char *xmlstr = NULL;
		char *finalstr = NULL;
		char *declStr = NULL;
		/*char xslFile[30] ={"test"};*/
		char xslFile[30];

		/*char *valArry[] ={"test1", "test2", "test3", "test4", "test5",
				"test6", "test7", "test8", "test9", "test10", "test11"};*/

		char *valArry[] ={"rec1", "1.1.1.1", "2.2.2.2"};
		char *valArry1[] ={"rec2", "2.2.2.2", "2.2.2.2"};
		char *valArry2[] ={"rec3", "3.3.3.3", "2.2.2.2"};

		int i = 1, errCode = 0, strBufLen = 0;

		printf("\n ***** CALLING :: xmlReadFile:> %s ", argv[i]);

		/*doc = ixmlLoadDocument(argv[i]);*/
		errCode = CNTRLXMLLoadXML(argv[i], &doc);

		errCode = CNTRLXMLProcessIncludeTags(&doc);

		/*xmlPopulateValues(((IXML_Document *)pXmlDoc)->n.firstChild);*/

		/*errCode = xmlPopulateValues(doc, valArry,"BkupAuthSvr",11, eELEMENT);*/
		/*errCode = xmlPopulateValues(doc, valArry,"IPRec",2, eATTRIBUTE);*/
		
		CNTRLXMLGetRootNode(doc, &rootNode);

		errCode = CNTRLXMLGetAttributeValue("xslref",rootNode,xslFile);
		printf("\n*** xslFile = %s\n", xslFile);

		errCode = xmlPopulateValues(doc, valArry,"IPRec",3, eELEMENT);

		errCode = xmlPopulateValues(doc, valArry,"IPRangeRec",3, eELEMENT);
		errCode = xmlPopulateValues(doc, valArry,"SubnetRec",3, eELEMENT);

		errCode = xmlPopulateValues(doc, valArry1,"IPRec",3, eELEMENT);
		errCode = xmlPopulateValues(doc, valArry2,"IPRec",3, eELEMENT);

		errCode = CNTRLXMLAddSoapEnvelope(rootNode, "envelope.xml", &finalDoc);
		
		if (errCode != CNTRLXML_SUCCESS)
		{
			printf("\n***** COULD NOT ADD THE SOAP ENVELOPE... \n" );
		}

		/*errCode = CNTRLXMLtoBuffer(doc, &xmlstr);*/
		errCode = CNTRLXMLtoBuffer(finalDoc, &xmlstr);

		/*
		 * Update set XSL file name.
		 */

		CNTRLXMLSetXSLFile(xslFile,&declStr);

		printf("\n*** declStr = %s\n", declStr);

		strBufLen = strlen(declStr) + strlen(xmlstr);

		finalstr = (char *) malloc (strBufLen);

		strcpy(finalstr, declStr);
		strcat(finalstr, xmlstr);

		printf("\n***** XML-STR = %s \n", finalstr);

		free(finalstr);
		free(xmlstr);
		free(declStr);

		/*ixmlFreeDOMString(xmlstr);*/
		CNTRLXMLFreeXMLDoc(&finalDoc);
		CNTRLXMLFreeXMLDoc(&doc);

		return(0);
}

