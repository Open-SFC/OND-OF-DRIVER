/*
 * Summary: implementation of XML Schema Datatypes
 * Description: module providing the XML Schema Datatypes implementation
 *              both definition and validity checking
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

 #ifdef OF_XML_SUPPORT
 #ifdef PSP_XML_SCHEMA_VAL_SUPPORT
	 

#ifndef __XML_SCHEMA_TYPES_H__
#define __XML_SCHEMA_TYPES_H__

#include <xmlversion.h>

#include "ixml.h"

#ifdef LIBXML_SCHEMAS_ENABLED

#include <schemasinternals.h>
#include <xmlschemas.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IS_LEAP(y)						\
	(((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0))


XMLPUBFUN void XMLCALL		
    		xmlSchemaInitTypes		(void);
XMLPUBFUN void XMLCALL		
		xmlSchemaCleanupTypes		(void);
XMLPUBFUN xmlSchemaTypePtr XMLCALL 
		xmlSchemaGetPredefinedType	(const xmlChar *name,
						 const xmlChar *ns);
XMLPUBFUN int XMLCALL		
		xmlSchemaValidatePredefinedType	(xmlSchemaTypePtr type,
						 const xmlChar *value,
						 xmlSchemaValPtr *val);
XMLPUBFUN int XMLCALL		
		xmlSchemaValPredefTypeNode	(xmlSchemaTypePtr type,
						 const xmlChar *value,
						 xmlSchemaValPtr *val,
						 IXML_Node *node);
XMLPUBFUN int XMLCALL		
		xmlSchemaValidateFacet		(xmlSchemaTypePtr base,
						 xmlSchemaFacetPtr facet,
						 const xmlChar *value,
						 xmlSchemaValPtr val);	
XMLPUBFUN void XMLCALL		
		xmlSchemaFreeValue		(xmlSchemaValPtr val);
XMLPUBFUN xmlSchemaFacetPtr XMLCALL 
		xmlSchemaNewFacet		(void);
XMLPUBFUN int XMLCALL		
		xmlSchemaCheckFacet		(xmlSchemaFacetPtr facet,
						 xmlSchemaTypePtr typeDecl,
						 xmlSchemaParserCtxtPtr ctxt,
						 const xmlChar *name);
XMLPUBFUN void XMLCALL		
		xmlSchemaFreeFacet		(xmlSchemaFacetPtr facet);
XMLPUBFUN int XMLCALL		
		xmlSchemaCompareValues		(xmlSchemaValPtr x,
						 xmlSchemaValPtr y);
XMLPUBFUN xmlSchemaTypePtr XMLCALL		
		xmlSchemaGetBuiltInListSimpleTypeItemType(xmlSchemaTypePtr type);
XMLPUBFUN int XMLCALL
xmlSchemaValidateListSimpleTypeFacet(xmlSchemaFacetPtr facet,
				     const xmlChar *value,
				     unsigned long actualLen,
				     unsigned long *expectedLen);
XMLPUBFUN xmlSchemaTypePtr XMLCALL
xmlSchemaGetBuiltInType(xmlSchemaValType type);
XMLPUBFUN int XMLCALL
xmlSchemaIsBuiltInTypeFacet(xmlSchemaTypePtr type, 
			    int facetType);
XMLPUBFUN xmlChar * XMLCALL
xmlSchemaCollapseString(const xmlChar *value);
XMLPUBFUN unsigned long  XMLCALL
xmlSchemaGetFacetValueAsULong(xmlSchemaFacetPtr facet);
XMLPUBFUN int XMLCALL
xmlSchemaValidateLengthFacet(xmlSchemaTypePtr type, 
			     xmlSchemaFacetPtr facet,
			     const xmlChar *value,
			     xmlSchemaValPtr val,
			     unsigned long *length) ;
XMLPUBFUN int XMLCALL
xmlSchemaValPredefTypeNodeNoNorm(xmlSchemaTypePtr type, const xmlChar *value,
				 xmlSchemaValPtr *val, IXML_Node *node);

XMLPUBFUN int ixmlValidateQName(const xmlChar *value, int space);

XMLPUBFUN xmlChar * ixmlSplitQName2(const xmlChar *name, xmlChar **prefix);

XMLPUBFUN xmlChar * XMLCALL	
		ixmlBuildQName(const xmlChar *ncname,
					 const xmlChar *prefix,
					 xmlChar *memory,
					 int len);
XMLPUBFUN int ixmlStringCurrentChar(const xmlChar * cur, int *len);

XMLPUBFUN int ixmlValidateNCName(const xmlChar *value, int space);

#define CUR_SCHAR(s, l) ixmlStringCurrentChar(s, &l)

#ifdef __cplusplus
}
#endif

#endif /* LIBXML_SCHEMAS_ENABLED */
#endif /* __XML_SCHEMA_TYPES_H__ */

#endif /* PSP_SCHEMA_VAL_SUPPORT*/
#endif /* PSP_XML_SUPPORT */
