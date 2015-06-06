#ifdef PSP_XPATH_SUPPORT

#ifndef __XML_STR_BUFF_H__
#define __XML_STR_BUFF_H__
#include <stdio.h>
#include <xmlstring.h>
#include "libxml.h"
#include <xmlstring.h>

/**
 * BASE_BUFFER_SIZE:
 * default buffer size 4000.
 */
#define BASE_BUFFER_SIZE 4096



/**
 * xmlBufferAllocationScheme:
 *
 * A buffer allocation scheme can be defined to either match exactly the
 * need or double it's allocated size each time it is found too small.
 */

typedef enum {
    XML_BUFFER_ALLOC_DOUBLEIT,
    XML_BUFFER_ALLOC_EXACT,
    XML_BUFFER_ALLOC_IMMUTABLE
} xmlBufferAllocationScheme;


/**
 * xmlBuffer:
 *
 * A buffer structure.
 */
typedef struct _xmlBuffer xmlBuffer;
typedef xmlBuffer * xmlBufferPtr;
struct _xmlBuffer {
    xmlChar *content;		/* The buffer content UTF8 */
    unsigned int use;		/* The buffer size used */
    unsigned int size;		/* The buffer size */
    xmlBufferAllocationScheme alloc; /* The realloc method */
};

/*
 * Handling Buffers.
 */

XMLPUBFUN void XMLCALL		
		xmlSetBufferAllocationScheme(xmlBufferAllocationScheme scheme);
XMLPUBFUN xmlBufferAllocationScheme XMLCALL	 
		xmlGetBufferAllocationScheme(void);

XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreate		(void);
XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreateSize	(size_t size);
XMLPUBFUN xmlBufferPtr XMLCALL	
		xmlBufferCreateStatic	(void *mem,
					 size_t size);
XMLPUBFUN int XMLCALL		
		xmlBufferResize		(xmlBufferPtr buf,
					 unsigned int size);
XMLPUBFUN void XMLCALL		
		xmlBufferFree		(xmlBufferPtr buf);
XMLPUBFUN int XMLCALL		
		xmlBufferDump		(FILE *file,
					 xmlBufferPtr buf);
XMLPUBFUN int XMLCALL		
		xmlBufferAdd		(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlBufferAddHead	(xmlBufferPtr buf,
					 const xmlChar *str,
					 int len);
XMLPUBFUN int XMLCALL		
		xmlBufferCat		(xmlBufferPtr buf,
					 const xmlChar *str);
XMLPUBFUN int XMLCALL	
		xmlBufferCCat		(xmlBufferPtr buf,
					 const char *str);
XMLPUBFUN int XMLCALL		
		xmlBufferShrink		(xmlBufferPtr buf,
					 unsigned int len);
XMLPUBFUN int XMLCALL		
		xmlBufferGrow		(xmlBufferPtr buf,
					 unsigned int len);
XMLPUBFUN void XMLCALL		
		xmlBufferEmpty		(xmlBufferPtr buf);
XMLPUBFUN const xmlChar* XMLCALL	
		xmlBufferContent	(const xmlBufferPtr buf);
XMLPUBFUN void XMLCALL		
		xmlBufferSetAllocationScheme(xmlBufferPtr buf,
					 xmlBufferAllocationScheme scheme);
XMLPUBFUN int XMLCALL		
		xmlBufferLength		(const xmlBufferPtr buf);

XMLPUBFUN int
xmlCopyCharMultiByte(xmlChar *out, int val);

XMLPUBFUN int
xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val);

XMLPUBFUN void
xmlBufferWriteQuotedString(xmlBufferPtr buf, const xmlChar *string);

XMLPUBFUN void
xmlBufferWriteCHAR(xmlBufferPtr buf, const xmlChar *string);

XMLPUBFUN void
xmlBufferWriteChar(xmlBufferPtr buf, const char *string);

#endif /* __XML_STR_BUFF_H__*/

#endif /*PSP_XPATH_SUPPORT */
