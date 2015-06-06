#ifdef OF_XML_SUPPORT

#if defined(PSP_XML_SCHEMA_VAL_SUPPORT) || defined(PSP_XPATH_SUPPORT) || defined(PSP_XSLT_SUPPORT)

#ifndef __XML_PARSERUTIL_H__
#define __XML_PARSERUTIL_H__

#include <libxml.h>
#include <xmlstring.h>
#include <xmlmemory.h>
#include <xmlstrbuffutil.h>



#define XML_XML_NAMESPACE \
   (const xmlChar *) "http://www.w3.org/XML/1998/namespace"


XMLPUBFUN void ixmlSearchNs(IXML_Document *doc,
			                 IXML_Node *node,
		                     const xmlChar *nameSpace,
			                 const xmlChar **nsName);
XMLPUBFUN xmlChar *
ixmlEncodeEntitiesReentrant(IXML_Document *doc, const xmlChar *input);

XMLPUBFUN xmlChar *
ixmlNodeListGetString(IXML_Document *doc, IXML_Node *list, int inLine);

XMLPUBFUN xmlChar *
ixmlSplitQName2(const xmlChar *name, xmlChar **prefix);

XMLPUBFUN xmlChar *
ixmlBuildQName(const xmlChar *ncname, const xmlChar *prefix,
	      xmlChar *memory, int len);

XMLPUBFUN xmlChar *
ixmlGetNsProp(IXML_Node *node, const xmlChar *name, const xmlChar *nameSpace);

XMLPUBFUN xmlChar *
ixmlNodeGetLang(IXML_Node *cur);

XMLPUBFUN int
ixmlValidateQName(const xmlChar *value, int space);

XMLPUBFUN int
ixmlStringCurrentChar(const xmlChar * cur, int *len);
#define CUR_SCHAR(s, l) ixmlStringCurrentChar(s, &l)

XMLPUBFUN int
ixmlIsBlankNode(IXML_Node *node);

#endif /* ! __XML_PARSERUTIL_H__ */
#endif /*if defined(PSP_XML_SCHEMA_VAL_SUPPORT) || defined(PSP_XPATH_SUPPORT) || defined(PSP_XSLT_SUPPORT)*/
#endif /*if defined(PSP_XML_SCHEMA_VAL_SUPPORT) || defined(PSP_XPATH_SUPPORT) || defined(PSP_XSLT_SUPPORT)*/
