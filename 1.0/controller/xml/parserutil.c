#ifdef OF_XML_SUPPORT
#if defined(PSP_XPATH_SUPPORT) || defined (PSP_XSLT_SUPPORT)
#include <stdio.h>
#include "ixml.h"
#include <xmlcommon.h>
#include <xmlmemory.h>
#include <parserutil.h>


/*
 *  Macro used to grow the current buffer.
 */
#define growBufferReentrant() {						\
	    buffer_size *= 2;							\
	    buffer = (xmlChar *)						\
   		realloc(buffer, buffer_size * sizeof(xmlChar));	\
	    if (buffer == NULL) {						\
				printf("\nrealloc failed\n");	\
				return(NULL);							\
	    }									\
}

/**
 * xpathGetNsProp:
 * @node:  the node
 * @name:  the attribute name
 * @nameSpace:  the URI of the namespace
 *
 * Similar to xmlGetNsProp() but with a slightly different semantic
 *
 * Search and get the value of an attribute associated to a node
 * This attribute has to be anchored in the namespace specified,
 * or has no namespace and the element is in that namespace.
 *
 * This does the entity substitution.
 * This function looks in DTD attribute declaration for #FIXED or
 * default declaration values unless DTD use has been turned off.
 *
 * Returns the attribute value or NULL if not found.
 *     It's up to the caller to free the memory.
 */
xmlChar*
ixmlGetNsProp(IXML_Node *node, const xmlChar *name, const xmlChar *nameSpace)
{
    IXML_Node *prop;
    IXML_Node *attrNode;
    /*IXML_Document *doc;*/
    xmlChar *propVal = NULL;
    /*xmlChar *nsVal;*/
    /*int retVal = 0;*/

    if (node == NULL)
	return(NULL);

    /*prop = node->properties;*/

    /*if (nameSpace == NULL)
	return(xmlGetProp(node, name));*/
    if (nameSpace == NULL)
    {
    	/*retVal = CNTRLXMLGetAttributeValue((xmlChar*)  name,
    							(void*) node, (xmlChar*)propVal);*/
    	attrNode = ixmlNode_getAttributeByName(node, (char *)name);

    	if(attrNode == NULL)
    	{
    		return NULL;
    	}
    	else
 		{
			if(attrNode->nodeValue)
			{
 				propVal = strdup(attrNode->nodeValue);

			}
			else
			{
				propVal = NULL;
			}
			return(propVal);

    	}
    }
#if 0
    while (prop != NULL) {
	/*
	 * One need to have
	 *   - same attribute names
	 *   - and the attribute carrying that namespace
	 */
        if ((xmlStrEqual(prop->name, name)) &&
	    (((prop->ns == NULL) && (node->ns != NULL) &&
	      (xmlStrEqual(node->ns->href, nameSpace))) ||
	     ((prop->ns != NULL) &&
	      (xmlStrEqual(prop->ns->href, nameSpace))))) {
	    xmlChar *ret;

	    ret = xmlNodeListGetString(node->doc, prop->children, 1);
	    if (ret == NULL) return(xmlStrdup((xmlChar *)""));
	    return(ret);
        }
	prop = prop->next;
    }
#endif
	prop = ixmlNode_getAttributeByName(node, (xmlChar *)name);
	if(prop != NULL && (xmlStrEqual(node->namespaceURI, nameSpace)))
	{
		xmlChar *ret;
		ret = ixmlNodeListGetString(node->ownerDocument, prop->firstChild, 1);
		if (ret == NULL) return(xmlStrdup((xmlChar *)""));
    		return(ret);
	}

    /*
     * Check if there is a default declaration in the internal
     * or external subsets
     */
#if 0
	/*TO BE FIXED*/
    doc =  node->ownerDocument;

    if (doc != NULL) {
        if (doc->intSubset != NULL) {
	    xmlAttributePtr attrDecl;

	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, node->name, name); /*TO BE FIXED*/
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset, node->name, name);  /*TO BE FIXED*/

	    if ((attrDecl != NULL) && (attrDecl->prefix != NULL)) {
	        /*
		 * The DTD declaration only allows a prefix search
		 */
		/*ns = xmlSearchNs(doc, node, attrDecl->prefix);*/
	        ixmlSearchNs(doc, node, attrDecl->prefix, &nsVal);
		if ((nsVal != NULL) && (xmlStrEqual(nsVal, nameSpace)))
		    return(xmlStrdup(attrDecl->defaultValue));
	    }
	}
    }
#endif
    return(NULL);
}


void ixmlSearchNs(IXML_Document *doc,
				IXML_Node *node,
				const xmlChar *nameSpace,
				const xmlChar **nsName)
{

	*nsName = NULL;
    if (node == NULL) return;
    if ((nameSpace != NULL) &&
	(xmlStrEqual(nameSpace, (const xmlChar *)"xml"))) {
	if ((doc == NULL) && (node->nodeType == eELEMENT_NODE)) {
	    /*
	     * The XML-1.0 namespace is normally held on the root
	     * element. In this case exceptionally create it on the
	     * node element.
	     */
	    *nsName = xmlStrdup(XML_XML_NAMESPACE);
	    return;
	}
    }

    while (node != NULL) {
	if ((node->nodeType == eENTITY_REFERENCE_NODE) ||
	    (node->nodeType == eENTITY_NODE))
/*	    ||(node->type == XML_ENTITY_DECL)) */
	    return;
	if (node->nodeType == eELEMENT_NODE) {
/*	    cur = node->nsDef; */


	if ((node->prefix == NULL) && (nameSpace == NULL) &&
		(node->namespaceURI != NULL))
	{
		*nsName = xmlStrdup(node->namespaceURI);
		return;
	}

	if ((node->prefix != NULL) && (nameSpace != NULL) &&
		(node->namespaceURI != NULL) && (xmlStrEqual(node->prefix, nameSpace)))
	{
		*nsName = xmlStrdup(node->namespaceURI);
		return;
	}

	}
		node = node->parentNode;
    }
    return;
}

/**
 * ixmlStringCurrentChar:
 * @cur:  pointer to the beginning of the char
 * @len:  pointer to the length of the char read
 *
 * The current char value, if using UTF-8 this may actually span multiple
 * bytes in the input buffer.
 *
 * Returns the current char value and its length
 */
int
ixmlStringCurrentChar(const xmlChar * cur, int *len)
{
		/* In IXML ctxt is always NULL */
/*   if ((ctxt == NULL) || (ctxt->charset == XML_CHAR_ENCODING_UTF8)) { */
        /*
         * We are supposed to handle UTF8, check it's valid
         * From rfc2044: encoding of the Unicode values on UTF-8:
         *
         * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
         * 0000 0000-0000 007F   0xxxxxxx
         * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
         * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
         *
         * Check for the 0x110000 limit too
         */
        unsigned char c;
        unsigned int val;

        c = *cur;
        if (c & 0x80) {
            if ((cur[1] & 0xc0) != 0x80)
                goto encoding_error;
            if ((c & 0xe0) == 0xe0) {

                if ((cur[2] & 0xc0) != 0x80)
                    goto encoding_error;
                if ((c & 0xf0) == 0xf0) {
                    if (((c & 0xf8) != 0xf0) || ((cur[3] & 0xc0) != 0x80))
                        goto encoding_error;
                    /* 4-byte code */
                    *len = 4;
                    val = (cur[0] & 0x7) << 18;
                    val |= (cur[1] & 0x3f) << 12;
                    val |= (cur[2] & 0x3f) << 6;
                    val |= cur[3] & 0x3f;
                } else {
                    /* 3-byte code */
                    *len = 3;
                    val = (cur[0] & 0xf) << 12;
                    val |= (cur[1] & 0x3f) << 6;
                    val |= cur[2] & 0x3f;
                }
            } else {
                /* 2-byte code */
                *len = 2;
                val = (cur[0] & 0x1f) << 6;
                val |= cur[1] & 0x3f;
            }
            if (!IS_CHAR(val)) {
	        	/*xmlErrEncodingInt(ctxt, XML_ERR_INVALID_CHAR,
				  "Char 0x%X out of allowed range\n", val);*/
				printf("\n*** Char out of allowed range...\n");
            }
            return (val);
        } else {
            /* 1-byte code */
            *len = 1;
            return ((int) *cur);
        }
 /* } */
    /*
     * Assume it's a fixed length encoding (1) with
     * a compatible encoding for the ASCII set, since
     * XML constructs only use < 128 chars
     */
    *len = 1;
    return ((int) *cur);
encoding_error:

    /*
     * If we detect an UTF8 error that probably mean that the
     * input encoding didn't get properly advertised in the
     * declaration header. Report the error and switch the encoding
     * to ISO-Latin-1 (if you don't like this policy, just declare the
     * encoding !)
     */
    /*__xmlErrEncoding(ctxt, XML_ERR_INVALID_CHAR,
		   "Input is not proper UTF-8, indicate encoding !\n",
		   NULL, NULL);*/

    printf("\nInput is not proper UTF-8, indicate encoding !\n");

    /*if ((ctxt != NULL) && (ctxt->sax != NULL) && (ctxt->sax->error != NULL)) {
	ctxt->sax->error(ctxt->userData,
			 "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			 ctxt->input->cur[0], ctxt->input->cur[1],
			 ctxt->input->cur[2], ctxt->input->cur[3]);
    }*/
    *len = 1;
    return ((int) *cur);
}

/**
 * ixmlNodeGetLang:
 * @cur:  the node being checked
 *
 * Searches the language of a node, i.e. the values of the xml:lang
 * attribute or the one carried by the nearest ancestor.
 *
 * Returns a pointer to the lang value, or NULL if not found
 *     It's up to the caller to free the memory with xmlFree().
 */
xmlChar *
ixmlNodeGetLang(IXML_Node *cur) {
    xmlChar *lang;

    while (cur != NULL) {
        lang = ixmlGetNsProp(cur, BAD_CAST "lang", XML_XML_NAMESPACE);
	if (lang != NULL)
	    return(lang);
	cur = cur->parentNode;
    }
    return(NULL);
}

/**
 * ixmlSplitQName2:
 * @name:  the full QName
 * @prefix:  a xmlChar **
 *
 * parse an XML qualified name string
 *
 * [NS 5] QName ::= (Prefix ':')? LocalPart
 *
 * [NS 6] Prefix ::= NCName
 *
 * [NS 7] LocalPart ::= NCName
 *
 * Returns NULL if not a QName, otherwise the local part, and prefix
 *   is updated to get the Prefix if any.
 */

xmlChar *
ixmlSplitQName2(const xmlChar *name, xmlChar **prefix) {
    int len = 0;
    xmlChar *ret = NULL;

    *prefix = NULL;
    if (name == NULL) return(NULL);

#ifndef XML_XML_NAMESPACE
    /* xml: prefix is not really a namespace */
    if ((name[0] == 'x') && (name[1] == 'm') &&
        (name[2] == 'l') && (name[3] == ':'))
	return(NULL);
#endif

    /* nasty but valid */
    if (name[0] == ':')
	return(NULL);

    /*
     * we are not trying to validate but just to cut, and yes it will
     * work even if this is as set of UTF-8 encoded chars
     */
    while ((name[len] != 0) && (name[len] != ':'))
	len++;

    if (name[len] == 0)
	return(NULL);

    *prefix = xmlStrndup(name, len);
    if (*prefix == NULL) {
	/*xmlTreeErrMemory("QName split");*/
	printf("QName split");
	return(NULL);
    }
    ret = xmlStrdup(&name[len + 1]);
    if (ret == NULL) {
    /*xmlTreeErrMemory("QName split");*/
	printf("QName split");
	if (*prefix != NULL) {
	    xmlFree(*prefix);
	    *prefix = NULL;
	}
	return(NULL);
    }

    return(ret);
}

/**
 * ixmlBuildQName:
 * @ncname:  the Name
 * @prefix:  the prefix
 * @memory:  preallocated memory
 * @len:  preallocated memory length
 *
 * Builds the QName @prefix:@ncname in @memory if there is enough space
 * and prefix is not NULL nor empty, otherwise allocate a new string.
 * If prefix is NULL or empty it returns ncname.
 *
 * Returns the new string which must be freed by the caller if different from
 *         @memory and @ncname or NULL in case of error
 */
xmlChar *
ixmlBuildQName(const xmlChar *ncname, const xmlChar *prefix,
	      xmlChar *memory, int len) {
    int lenn, lenp;
    xmlChar *ret;

    if (ncname == NULL) return(NULL);
    if (prefix == NULL) return((xmlChar *) ncname);

    lenn = strlen((char *) ncname);
    lenp = strlen((char *) prefix);

    if ((memory == NULL) || (len < lenn + lenp + 2)) {
	ret = (xmlChar *) xmlMallocAtomic(lenn + lenp + 2);
	if (ret == NULL) {
	   /* xmlTreeErrMemory("building QName");*/
	    return(NULL);
	}
    } else {
	ret = memory;
    }
    memcpy(&ret[0], prefix, lenp);
    ret[lenp] = ':';
    memcpy(&ret[lenp + 1], ncname, lenn);
    ret[lenn + lenp + 1] = 0;
    return(ret);
}

/**
 * ixmlEncodeEntitiesReentrant:
 * @doc:  the document containing the string
 * @input:  A string to convert to XML.
 *
 * Do a global encoding of a string, replacing the predefined entities
 * and non ASCII values with their entities and CharRef counterparts.
 * Contrary to xmlEncodeEntities, this routine is reentrant, and result
 * must be deallocated.
 *
 * Returns A newly allocated string with the substitution done.
 */
xmlChar*
ixmlEncodeEntitiesReentrant(IXML_Document *doc, const xmlChar *input)
{
    const xmlChar *cur = input;
    xmlChar *buffer = NULL;
    xmlChar *out = NULL;
    int buffer_size = 0;
    int html = 0;

    if (input == NULL) return(NULL);
    if (doc != NULL)
        html = (doc->n.nodeType == eDOCUMENT_NODE);

    /*
     * allocate an translation buffer.
     */
    buffer_size = 1000;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
	xmlGenericError(xmlGenericErrorContext, "malloc failed\n");
	return(NULL);
    }
    out = buffer;

    while (*cur != '\0') {
        if (out - buffer > buffer_size - 100) {
	    int indx = out - buffer;

	    growBufferReentrant();
	    out = &buffer[indx];
	}

	/*
	 * By default one have to encode at least '<', '>', '"' and '&' !
	 */
	if (*cur == '<') {
	    *out++ = '&';
	    *out++ = 'l';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '>') {
	    *out++ = '&';
	    *out++ = 'g';
	    *out++ = 't';
	    *out++ = ';';
	} else if (*cur == '&') {
	    *out++ = '&';
	    *out++ = 'a';
	    *out++ = 'm';
	    *out++ = 'p';
	    *out++ = ';';
	} else if (((*cur >= 0x20) && (*cur < 0x80)) ||
	    (*cur == '\n') || (*cur == '\t') || ((html) && (*cur == '\r'))) {
	    /*
	     * default case, just copy !
	     */
	    *out++ = *cur;
	} else if (*cur >= 0x80) {
	    if (((doc != NULL) /*&& (doc->encoding != NULL)*/) || (html)) {
		/*
		 * Bjørn Reese <br@sseusa.com> provided the patch
	        xmlChar xc;
	        xc = (*cur & 0x3F) << 6;
	        if (cur[1] != 0) {
		    xc += *(++cur) & 0x3F;
		    *out++ = xc;
	        } else
		 */
		    *out++ = *cur;
	    } else {
		/*
		 * We assume we have UTF-8 input.
		 */
		char buf[11], *ptr;
		int val = 0, l = 1;

		if (*cur < 0xC0) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlEncodeEntitiesReentrant : input not UTF-8\n");
		    /*if (doc != NULL)
			doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");*/
		    T_snprintf(buf, sizeof(buf), "&#%d;", *cur);
		    buf[sizeof(buf) - 1] = 0;
		    ptr = buf;
		    while (*ptr != 0) *out++ = *ptr++;
		    cur++;
		    continue;
		} else if (*cur < 0xE0) {
                    val = (cur[0]) & 0x1F;
		    val <<= 6;
		    val |= (cur[1]) & 0x3F;
		    l = 2;
		} else if (*cur < 0xF0) {
                    val = (cur[0]) & 0x0F;
		    val <<= 6;
		    val |= (cur[1]) & 0x3F;
		    val <<= 6;
		    val |= (cur[2]) & 0x3F;
		    l = 3;
		} else if (*cur < 0xF8) {
                    val = (cur[0]) & 0x07;
		    val <<= 6;
		    val |= (cur[1]) & 0x3F;
		    val <<= 6;
		    val |= (cur[2]) & 0x3F;
		    val <<= 6;
		    val |= (cur[3]) & 0x3F;
		    l = 4;
		}
		if ((l == 1) || (!IS_CHAR(val))) {
		    xmlGenericError(xmlGenericErrorContext,
			"xmlEncodeEntitiesReentrant : char out of range\n");
		   /* if (doc != NULL)
			doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1"); */
		    T_snprintf(buf, sizeof(buf), "&#%d;", *cur);
		    buf[sizeof(buf) - 1] = 0;
		    ptr = buf;
		    while (*ptr != 0) *out++ = *ptr++;
		    cur++;
		    continue;
		}
		/*
		 * We could do multiple things here. Just save as a char ref
		 */
		if (html)
		    T_snprintf(buf, sizeof(buf), "&#%d;", val);
		
		else
		    T_snprintf(buf, sizeof(buf), "&#x%X;", val);
		buf[sizeof(buf) - 1] = 0;
		ptr = buf;
		while (*ptr != 0) *out++ = *ptr++;
		cur += l;
		continue;
	    }
	} else if (IS_BYTE_CHAR(*cur)) {
	    char buf[11], *ptr;

	    T_snprintf(buf, sizeof(buf), "&#%d;", *cur);
	    buf[sizeof(buf) - 1] = 0;
            ptr = buf;
	    while (*ptr != 0) *out++ = *ptr++;
	}
	cur++;
    }
    *out++ = 0;
    return(buffer);
}

/**
 * ixmlNodeListGetString:
 * @doc:  the document
 * @list:  a Node list
 * @inLine:  should we replace entity contents or show their external form
 *
 * Build the string equivalent to the text contained in the Node list
 * made of TEXTs and ENTITY_REFs
 *
 * Returns a pointer to the string copy, the caller must free it with xmlFree().
 */
xmlChar *
ixmlNodeListGetString(IXML_Document *doc, IXML_Node *list, int inLine)
{
    IXML_Node *node = list;
    xmlChar *ret = NULL;
    /*xmlEntityPtr ent;*/

    if (list == NULL)
        return (NULL);

    while (node != NULL) {
        if ((node->nodeType== eTEXT_NODE) ||
            (node->nodeType == eCDATA_SECTION_NODE)) {
            if (inLine) {
                ret = xmlStrcat(ret, node->nodeValue);
            } else {
                xmlChar *buffer;

                buffer = ixmlEncodeEntitiesReentrant(doc, node->nodeValue);
                if (buffer != NULL) {
                    ret = xmlStrcat(ret, buffer);
                    xmlFree(buffer);
                }
            }
        }
#if 0 /* entities to be fixed later */
        else if (node->nodeType == eENTITY_REFERENCE_NODE) {
            if (inLine) {
                ent = xmlGetDocEntity(doc, node->nodeName);
                if (ent != NULL) {
                    xmlChar *buffer;

                    /* an entity content can be any "well balanced chunk",
                     * i.e. the result of the content [43] production:
                     * http://www.w3.org/TR/REC-xml#NT-content.
                     * So it can contain text, CDATA section or nested
                     * entity reference nodes (among others).
                     * -> we recursive  call xmlNodeListGetString()
                     * which handles these types */
                    buffer = ixmlNodeListGetString(doc, ent->children, 1);
                    if (buffer != NULL) {
                        ret = xmlStrcat(ret, buffer);
                        xmlFree(buffer);
                    }
                } else {
                    ret = xmlStrcat(ret, node->content);
                }
            } else {
                xmlChar buf[2];

                buf[0] = '&';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
                ret = xmlStrcat(ret, node->name);
                buf[0] = ';';
                buf[1] = 0;
                ret = xmlStrncat(ret, buf, 1);
            }
        }
#endif
#if 0
        else {
            xmlGenericError(xmlGenericErrorContext,
                            "xmlGetNodeListString : invalid node type %d\n",
                            node->type);
        }
#endif
        node = node->nextSibling;
    }
    return (ret);
}

/**
 * ixmlValidateQName:
 * @value: the value to check
 * @space: allow spaces in front and end of the string
 *
 * Check that a value conforms to the lexical space of QName
 *
 * Returns 0 if this validates, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
ixmlValidateQName(const xmlChar *value, int space) {
    const xmlChar *cur = value;
    int c = 0,l = 0;

    /*
     * First quick algorithm for ASCII range
     */
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (((*cur >= 'a') && (*cur <= 'z')) || ((*cur >= 'A') && (*cur <= 'Z')) ||
	(*cur == '_'))
	cur++;
    else
	goto try_complex;
    while (((*cur >= 'a') && (*cur <= 'z')) ||
	   ((*cur >= 'A') && (*cur <= 'Z')) ||
	   ((*cur >= '0') && (*cur <= '9')) ||
	   (*cur == '_') || (*cur == '-') || (*cur == '.'))
	cur++;
    if (*cur == ':') {
	cur++;
	if (((*cur >= 'a') && (*cur <= 'z')) ||
	    ((*cur >= 'A') && (*cur <= 'Z')) ||
	    (*cur == '_'))
	    cur++;
	else
	    goto try_complex;
	while (((*cur >= 'a') && (*cur <= 'z')) ||
	       ((*cur >= 'A') && (*cur <= 'Z')) ||
	       ((*cur >= '0') && (*cur <= '9')) ||
	       (*cur == '_') || (*cur == '-') || (*cur == '.'))
	    cur++;
    }
    if (space)
	while (IS_BLANK_CH(*cur)) cur++;
    if (*cur == 0)
	return(0);

try_complex:
    /*
     * Second check for chars outside the ASCII range
     */
    cur = value;
    c = CUR_SCHAR(cur, l);
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if ((!IS_LETTER(c)) && (c != '_'))
	return(1);
    cur += l;
    c = CUR_SCHAR(cur, l);
    while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
	   (c == '-') || (c == '_') || IS_COMBINING(c) ||
	   IS_EXTENDER(c)) {
	cur += l;
	c = CUR_SCHAR(cur, l);
    }
    if (c == ':') {
	cur += l;
	c = CUR_SCHAR(cur, l);
	if ((!IS_LETTER(c)) && (c != '_'))
	    return(1);
	cur += l;
	c = CUR_SCHAR(cur, l);
	while (IS_LETTER(c) || IS_DIGIT(c) || (c == '.') ||
	       (c == '-') || (c == '_') || IS_COMBINING(c) ||
	       IS_EXTENDER(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (space) {
	while (IS_BLANK(c)) {
	    cur += l;
	    c = CUR_SCHAR(cur, l);
	}
    }
    if (c != 0)
	return(1);
    return(0);
}
/**
 * ixmlIsBlankNode:
 * @node:  the node
 *
 * Checks whether this node is an empty or whitespace only
 * (and possibly ignorable) text-node.
 *
 * Returns 1 yes, 0 no
 */
int
ixmlIsBlankNode(IXML_Node *node) {
    const xmlChar *cur;
    if (node == NULL) return(0);

    if ((node->nodeType != eTEXT_NODE) &&
        (node->nodeType != eCDATA_SECTION_NODE))
	return(0);
    if (ixmlNode_getNodeValueEx(node) == NULL) return(1);
    cur = ixmlNode_getNodeValueEx(node);
    while (*cur != 0) {
	if (!IS_BLANK_CH(*cur)) return(0);
	cur++;
    }

    return(1);
}
#endif /*PSP_XPATH_SUPPORT*/
#endif /*PSP_XPATH_SUPPORT*/
