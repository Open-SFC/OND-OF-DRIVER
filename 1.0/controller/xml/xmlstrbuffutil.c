#ifdef PSP_XPATH_SUPPORT
#include "libxml.h"

#include <string.h> /* for memset() only ! */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
/*#include <zlib.h>*/
#endif

#include <xmlmemory.h>
#include <globals.h>
#include <xmlstrbuffutil.h>

#ifdef LIBXML_DEBUG_ENABLED
/*#include <debugXML.h>*/
#endif

/************************************************************************
 *									*
 *			Output : to a FILE or in memory			*
 *									*
 ************************************************************************/

/**
 * xmlDefaultBufferSize:
 * Global setting, default buffer size. Default value is BASE_BUFFER_SIZE
 */
int xmlDefaultBufferSize = BASE_BUFFER_SIZE;
/*static int xmlDefaultBufferSizeThrDef = BASE_BUFFER_SIZE;*/

/**
 * xmlBufferAllocScheme:
 * Global setting, default allocation policy for buffers, default is
 * XML_BUFFER_ALLOC_EXACT
 */
xmlBufferAllocationScheme xmlBufferAllocScheme = XML_BUFFER_ALLOC_EXACT;
/*static xmlBufferAllocationScheme xmlBufferAllocSchemeThrDef = XML_BUFFER_ALLOC_EXACT;*/

/**
 * xmlBufferCreate:
 *
 * routine to create an XML buffer.
 * returns the new structure.
 */
xmlBufferPtr
xmlBufferCreate(void) {
    xmlBufferPtr ret;

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (ret == NULL) {
	/*xmlTreeErrMemory("creating buffer");*/
        return(NULL);
    }
    ret->use = 0;
    ret->size = xmlDefaultBufferSize;
    ret->alloc = xmlBufferAllocScheme;
    ret->content = (xmlChar *) xmlMallocAtomic(ret->size * sizeof(xmlChar));
    if (ret->content == NULL) {
	/*xmlTreeErrMemory("creating buffer");*/
	xmlFree(ret);
        return(NULL);
    }
    ret->content[0] = 0;
    return(ret);
}

/**
 * xmlBufferCreateSize:
 * @size: initial size of buffer
 *
 * routine to create an XML buffer.
 * returns the new structure.
 */
xmlBufferPtr
xmlBufferCreateSize(size_t size) {
    xmlBufferPtr ret;

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (ret == NULL) {
	/*xmlTreeErrMemory("creating buffer");*/
        return(NULL);
    }
    ret->use = 0;
    ret->alloc = xmlBufferAllocScheme;
    ret->size = (size ? size+2 : 0);         /* +1 for ending null */
    if (ret->size){
        ret->content = (xmlChar *) xmlMallocAtomic(ret->size * sizeof(xmlChar));
        if (ret->content == NULL) {
	    /*xmlTreeErrMemory("creating buffer");*/
            xmlFree(ret);
            return(NULL);
        }
        ret->content[0] = 0;
    } else
	ret->content = NULL;
    return(ret);
}

/**
 * xmlBufferCreateStatic:
 * @mem: the memory area
 * @size:  the size in byte
 *
 * routine to create an XML buffer from an immutable memory area.
 * The area won't be modified nor copied, and is expected to be
 * present until the end of the buffer lifetime.
 *
 * returns the new structure.
 */
xmlBufferPtr
xmlBufferCreateStatic(void *mem, size_t size) {
    xmlBufferPtr ret;

    if ((mem == NULL) || (size == 0))
        return(NULL);

    ret = (xmlBufferPtr) xmlMalloc(sizeof(xmlBuffer));
    if (ret == NULL) {
	/*xmlTreeErrMemory("creating buffer");*/
        return(NULL);
    }
    ret->use = size;
    ret->size = size;
    ret->alloc = XML_BUFFER_ALLOC_IMMUTABLE;
    ret->content = (xmlChar *) mem;
    return(ret);
}

/**
 * xmlBufferSetAllocationScheme:
 * @buf:  the buffer to tune
 * @scheme:  allocation scheme to use
 *
 * Sets the allocation scheme for this buffer
 */
void
xmlBufferSetAllocationScheme(xmlBufferPtr buf,
                             xmlBufferAllocationScheme scheme) {
    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferSetAllocationScheme: buf == NULL\n");
#endif
        return;
    }
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;

    buf->alloc = scheme;
}

/**
 * xmlBufferFree:
 * @buf:  the buffer to free
 *
 * Frees an XML buffer. It frees both the content and the structure which
 * encapsulate it.
 */
void
xmlBufferFree(xmlBufferPtr buf) {
    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferFree: buf == NULL\n");
#endif
	return;
    }

    if ((buf->content != NULL) &&
        (buf->alloc != XML_BUFFER_ALLOC_IMMUTABLE)) {
        xmlFree(buf->content);
    }
    xmlFree(buf);
}

/**
 * xmlBufferEmpty:
 * @buf:  the buffer
 *
 * empty a buffer.
 */
void
xmlBufferEmpty(xmlBufferPtr buf) {
    if (buf == NULL) return;
    if (buf->content == NULL) return;
    buf->use = 0;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) {
        buf->content = BAD_CAST "";
    } else {
	memset(buf->content, 0, buf->size);
    }
}

/**
 * xmlBufferShrink:
 * @buf:  the buffer to dump
 * @len:  the number of xmlChar to remove
 *
 * Remove the beginning of an XML buffer.
 *
 * Returns the number of #xmlChar removed, or -1 in case of failure.
 */
int
xmlBufferShrink(xmlBufferPtr buf, unsigned int len) {
    if (buf == NULL) return(-1);
    if (len == 0) return(0);
    if (len > buf->use) return(-1);

    buf->use -= len;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) {
        buf->content += len;
    } else {
	memmove(buf->content, &buf->content[len], buf->use * sizeof(xmlChar));
	buf->content[buf->use] = 0;
    }
    return(len);
}

/**
 * xmlBufferGrow:
 * @buf:  the buffer
 * @len:  the minimum free size to allocate
 *
 * Grow the available space of an XML buffer.
 *
 * Returns the new available space or -1 in case of error
 */
int
xmlBufferGrow(xmlBufferPtr buf, unsigned int len) {
    int size;
    xmlChar *newbuf;

    if (buf == NULL) return(-1);

    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return(0);
    if (len + buf->use < buf->size) return(0);

/*
 * Windows has a BIG problem on realloc timing, so we try to double
 * the buffer size (if that's enough) (bug 146697)
 */
#ifdef WIN32
    if (buf->size > len)
        size = buf->size * 2;
    else
        size = buf->use + len + 100;
#else
    size = buf->use + len + 100;
#endif

    newbuf = (xmlChar *) xmlRealloc(buf->content, size);
    if (newbuf == NULL) {
	/*xmlTreeErrMemory("growing buffer");*/
        return(-1);
    }
    buf->content = newbuf;
    buf->size = size;
    return(buf->size - buf->use);
}

/**
 * xmlBufferDump:
 * @file:  the file output
 * @buf:  the buffer to dump
 *
 * Dumps an XML buffer to  a FILE *.
 * Returns the number of #xmlChar written
 */
int
xmlBufferDump(FILE *file, xmlBufferPtr buf) {
    int ret;

    if (buf == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferDump: buf == NULL\n");
#endif
	return(0);
    }
    if (buf->content == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferDump: buf->content == NULL\n");
#endif
	return(0);
    }
    if (file == NULL)
	file = stdout;
    ret = fwrite(buf->content, sizeof(xmlChar), buf->use, file);
    return(ret);
}

/**
 * xmlBufferContent:
 * @buf:  the buffer
 *
 * Function to extract the content of a buffer
 *
 * Returns the internal content
 */

const xmlChar *
xmlBufferContent(const xmlBufferPtr buf)
{
    if(!buf)
        return NULL;

    return buf->content;
}

/**
 * xmlBufferLength:
 * @buf:  the buffer
 *
 * Function to get the length of a buffer
 *
 * Returns the length of data in the internal content
 */

int
xmlBufferLength(const xmlBufferPtr buf)
{
    if(!buf)
        return 0;

    return buf->use;
}

/**
 * xmlBufferResize:
 * @buf:  the buffer to resize
 * @size:  the desired size
 *
 * Resize a buffer to accommodate minimum size of @size.
 *
 * Returns  0 in case of problems, 1 otherwise
 */
int
xmlBufferResize(xmlBufferPtr buf, unsigned int size)
{
    unsigned int newSize;
    xmlChar* rebuf = NULL;

    if (buf == NULL)
        return(0);

    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return(0);

    /* Don't resize if we don't have to */
    if (size < buf->size)
        return 1;

    /* figure out new size */
    switch (buf->alloc){
    case XML_BUFFER_ALLOC_DOUBLEIT:
	/*take care of empty case*/
        newSize = (buf->size ? buf->size*2 : size + 10);
        while (size > newSize) newSize *= 2;
        break;
    case XML_BUFFER_ALLOC_EXACT:
        newSize = size+10;
        break;
    default:
        newSize = size+10;
        break;
    }

    if (buf->content == NULL)
	rebuf = (xmlChar *) xmlMallocAtomic(newSize * sizeof(xmlChar));
    else if (buf->size - buf->use < 100) {
	rebuf = (xmlChar *) xmlRealloc(buf->content,
				       newSize * sizeof(xmlChar));
   } else {
        /*
	 * if we are reallocating a buffer far from being full, it's
	 * better to make a new allocation and copy only the used range
	 * and free the old one.
	 */
	rebuf = (xmlChar *) xmlMallocAtomic(newSize * sizeof(xmlChar));
	if (rebuf != NULL) {
	    memcpy(rebuf, buf->content, buf->use);
	    xmlFree(buf->content);
	    rebuf[buf->use] = 0;
	}
    }
    if (rebuf == NULL) {
	/*xmlTreeErrMemory("growing buffer");*/
        return 0;
    }
    buf->content = rebuf;
    buf->size = newSize;

    return 1;
}

/**
 * xmlBufferAdd:
 * @buf:  the buffer to dump
 * @str:  the #xmlChar string
 * @len:  the number of #xmlChar to add
 *
 * Add a string range to an XML buffer. if len == -1, the length of
 * str is recomputed.
 *
 * Returns 0 successful, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlBufferAdd(xmlBufferPtr buf, const xmlChar *str, int len) {
    unsigned int needSize;

    if ((str == NULL) || (buf == NULL)) {
	return -1;
    }
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAdd: len < 0\n");
#endif
	return -1;
    }
    if (len == 0) return 0;

    if (len < 0)
        len = xmlStrlen(str);

    if (len <= 0) return -1;

    needSize = buf->use + len + 2;
    if (needSize > buf->size){
        if (!xmlBufferResize(buf, needSize)){
	    /*xmlTreeErrMemory("growing buffer");*/
            return XML_ERR_NO_MEMORY;
        }
    }

    memmove(&buf->content[buf->use], str, len*sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
    return 0;
}

/**
 * xmlBufferAddHead:
 * @buf:  the buffer
 * @str:  the #xmlChar string
 * @len:  the number of #xmlChar to add
 *
 * Add a string range to the beginning of an XML buffer.
 * if len == -1, the length of @str is recomputed.
 *
 * Returns 0 successful, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlBufferAddHead(xmlBufferPtr buf, const xmlChar *str, int len) {
    unsigned int needSize;

    if (buf == NULL)
        return(-1);
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: str == NULL\n");
#endif
	return -1;
    }
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: len < 0\n");
#endif
	return -1;
    }
    if (len == 0) return 0;

    if (len < 0)
        len = xmlStrlen(str);

    if (len <= 0) return -1;

    needSize = buf->use + len + 2;
    if (needSize > buf->size){
        if (!xmlBufferResize(buf, needSize)){
	    /*xmlTreeErrMemory("growing buffer");*/
            return XML_ERR_NO_MEMORY;
        }
    }

    memmove(&buf->content[len], &buf->content[0], buf->use * sizeof(xmlChar));
    memmove(&buf->content[0], str, len * sizeof(xmlChar));
    buf->use += len;
    buf->content[buf->use] = 0;
    return 0;
}

/**
 * xmlBufferCat:
 * @buf:  the buffer to add to
 * @str:  the #xmlChar string
 *
 * Append a zero terminated string to an XML buffer.
 *
 * Returns 0 successful, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlBufferCat(xmlBufferPtr buf, const xmlChar *str) {
    if (buf == NULL)
        return(-1);
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (str == NULL) return -1;
    return xmlBufferAdd(buf, str, -1);
}

/**
 * xmlBufferCCat:
 * @buf:  the buffer to dump
 * @str:  the C char string
 *
 * Append a zero terminated C string to an XML buffer.
 *
 * Returns 0 successful, a positive error code number otherwise
 *         and -1 in case of internal or API error.
 */
int
xmlBufferCCat(xmlBufferPtr buf, const char *str) {
    const char *cur;

    if (buf == NULL)
        return(-1);
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferCCat: str == NULL\n");
#endif
	return -1;
    }
    for (cur = str;*cur != 0;cur++) {
        if (buf->use  + 10 >= buf->size) {
            if (!xmlBufferResize(buf, buf->use+10)){
		/*xmlTreeErrMemory("growing buffer");*/
                return XML_ERR_NO_MEMORY;
            }
        }
        buf->content[buf->use++] = *cur;
    }
    buf->content[buf->use] = 0;
    return 0;
}

/**
 * xmlBufferWriteCHAR:
 * @buf:  the XML buffer
 * @string:  the string to add
 *
 * routine which manages and grows an output buffer. This one adds
 * xmlChars at the end of the buffer.
 */
void
xmlBufferWriteCHAR(xmlBufferPtr buf, const xmlChar *string) {
    if (buf == NULL)
        return;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;
    xmlBufferCat(buf, string);
}

/**
 * xmlBufferWriteChar:
 * @buf:  the XML buffer output
 * @string:  the string to add
 *
 * routine which manage and grows an output buffer. This one add
 * C chars at the end of the array.
 */
void
xmlBufferWriteChar(xmlBufferPtr buf, const char *string) {
    if (buf == NULL)
        return;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;
    xmlBufferCCat(buf, string);
}


/**
 * xmlBufferWriteQuotedString:
 * @buf:  the XML buffer output
 * @string:  the string to add
 *
 * routine which manage and grows an output buffer. This one writes
 * a quoted or double quoted #xmlChar string, checking first if it holds
 * quote or double-quotes internally
 */
void
xmlBufferWriteQuotedString(xmlBufferPtr buf, const xmlChar *string) {
    const xmlChar *cur, *base;
    if (buf == NULL)
        return;
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;
    if (xmlStrchr(string, '\"')) {
        if (xmlStrchr(string, '\'')) {
#ifdef DEBUG_BUFFER
	    xmlGenericError(xmlGenericErrorContext,
 "xmlBufferWriteQuotedString: string contains quote and double-quotes !\n");
#endif
	    xmlBufferCCat(buf, "\"");
            base = cur = string;
            while(*cur != 0){
                if(*cur == '"'){
                    if (base != cur)
                        xmlBufferAdd(buf, base, cur - base);
                    xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
                    cur++;
                    base = cur;
                }
                else {
                    cur++;
                }
            }
            if (base != cur)
                xmlBufferAdd(buf, base, cur - base);
	    xmlBufferCCat(buf, "\"");
	}
        else{
	    xmlBufferCCat(buf, "\'");
            xmlBufferCat(buf, string);
	    xmlBufferCCat(buf, "\'");
        }
    } else {
        xmlBufferCCat(buf, "\"");
        xmlBufferCat(buf, string);
        xmlBufferCCat(buf, "\"");
    }
}

/**
 * xmlCopyChar:
 * @len:  Ignored, compatibility
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array
 *
 * Returns the number of xmlChar written
 */

int
xmlCopyChar(int len ATTRIBUTE_UNUSED, xmlChar *out, int val) {
   if (out == NULL) return(0);
	    /* the len parameter is ignored */
	    if  (val >= 0x80) {
			return(xmlCopyCharMultiByte (out, val));
	    }
	    *out = (xmlChar) val;
	    return 1;
}

/**
 * xmlCopyCharMultiByte:
 * @out:  pointer to an array of xmlChar
 * @val:  the char value
 *
 * append the char value in the array
 *
 * Returns the number of xmlChar written
 */
int
xmlCopyCharMultiByte(xmlChar *out, int val)
{
    if (out == NULL) return(0);
    /*
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
     */
    if  (val >= 0x80) {
	xmlChar *savedout = out;
	int bits;
	if (val <   0x800) { *out++= (val >>  6) | 0xC0;  bits=  0; }
	else if (val < 0x10000) { *out++= (val >> 12) | 0xE0;  bits=  6;}
	else if (val < 0x110000)  { *out++= (val >> 18) | 0xF0;  bits=  12; }
	else {
	    /*xmlErrEncodingInt(NULL, XML_ERR_INVALID_CHAR,
		    "Internal error, xmlCopyCharMultiByte 0x%X out of bound\n",
			      val);*/
	    return(0);
	}
	for ( ; bits >= 0; bits-= 6)
	    *out++= ((val >> bits) & 0x3F) | 0x80 ;
	return (out - savedout);
    }
    *out = (xmlChar) val;
    return 1;
}
#endif
