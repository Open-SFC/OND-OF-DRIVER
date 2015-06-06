#ifdef OF_XML_SUPPORT
#if defined(PSP_XML_SCHEMA_VAL_SUPPORT) || defined(PSP_XPATH_SUPPORT)

#ifndef __XML_COMMON_H__
#define __XML_COMMON_H__

#include <chvalid.h>

/**
 * IS_CHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD]
 *                  | [#x10000-#x10FFFF]
 * any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
 */
#define IS_CHAR(c)   xmlIsCharQ(c)

/**
 * IS_BLANK_CH:
 * @c:  an xmlChar value (normally unsigned char)
 *
 * Behaviour same as IS_BLANK
 */
#define IS_BLANK_CH(c)  xmlIsBlank_ch(c)

/**
 * IS_BASECHAR:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [85] BaseChar ::= ... long list see REC ...
 */
#define IS_BASECHAR(c) xmlIsBaseCharQ(c)

/**
 * IS_IDEOGRAPHIC:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [86] Ideographic ::= [#x4E00-#x9FA5] | #x3007 | [#x3021-#x3029]
 */
#define IS_IDEOGRAPHIC(c) xmlIsIdeographicQ(c)

/**
 * IS_BLANK:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [3] S ::= (#x20 | #x9 | #xD | #xA)+
 */
#define IS_BLANK(c)  xmlIsBlankQ(c)

/**
 * IS_LETTER:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [84] Letter ::= BaseChar | Ideographic
 */
#define IS_LETTER(c) (IS_BASECHAR(c) || IS_IDEOGRAPHIC(c))

/**
 * IS_DIGIT:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [88] Digit ::= ... long list see REC ...
 */
#define IS_DIGIT(c) xmlIsDigitQ(c)

/**
 * IS_COMBINING:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [87] CombiningChar ::= ... long list see REC ...
 */
#define IS_COMBINING(c) xmlIsCombiningQ(c)

/**
 * IS_EXTENDER:
 * @c:  an UNICODE value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 *
 * [89] Extender ::= #x00B7 | #x02D0 | #x02D1 | #x0387 | #x0640 |
 *                   #x0E46 | #x0EC6 | #x3005 | [#x3031-#x3035] |
 *                   [#x309D-#x309E] | [#x30FC-#x30FE]
 */
#define IS_EXTENDER(c) xmlIsExtenderQ(c)

/**
 * IS_BYTE_CHAR:
 * @c:  an byte value (int)
 *
 * Macro to check the following production in the XML spec:
 *
 * [2] Char ::= #x9 | #xA | #xD | [#x20...]
 * any byte character in the accepted range
 */
#define IS_BYTE_CHAR(c)	 xmlIsChar_ch(c)

XMLPUBVAR double xmlXPathNAN;
XMLPUBVAR double xmlXPathPINF;
XMLPUBVAR double xmlXPathNINF;

#endif /* __XML_COMMON_H__ */
#endif /* PSP_XML_SCHEMA_VAL_SUPPORT */
#endif /* PSP_XML_SUPPORT */

