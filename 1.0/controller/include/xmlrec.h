#ifdef OF_XML_SUPPORT
#ifndef __XMLREC_H__
#define __XMLREC_H__

#define XML_DOC_PATH		"/igateway/html/"
#define Config_DOC_PATH         "/igateway/config/"

#ifdef PSP_XML_TEST_SUPPORT 

#define PSP_TEST_GROUP_LEN 16
#define PSP_TEST_DESCR_LEN 100
typedef enum {
 PSP_TEST__ERR_BASE=0,
 PSP_TEST_DUP_REC,
 PSP_TEST_NO_REC,
 PSP_TEST_NO_MEM,
 PSP_TEST_INVALID_PARAM,
 PSP_TEST_INVALID_GRP,
 PSP_TEST_INVALID_IP,
 PSP_TEST_MAX_ERR_NUM
}eTestErrNum;

#define PSP_TEST_ERR_MESS_MAX_LEN 50
/* Type of records */
#define IP_ADDR      0x01
#define IP_SUBNET    0x02
#define IP_RANGE     0x04

typedef struct PSPAPITestRecord_s
{
   t_char8 Group[PSP_TEST_GROUP_LEN + 1];
   t_uchar8 RecType;
   t_uchar8 IpAddr1[16];
   t_uchar8 IpAddr2[16];
}PSPAPITestRecord_t;

#ifdef PSP_XML_MULTIMODSET_SUPPORT
typedef struct PSPResTestRecord_s PSPResTestRecord_t;
struct PSPResTestRecord_s
{
   t_uchar8 moduleName[10];
   t_uchar8 cmdName[20];
   t_uchar8 cmdStatus[10];
   t_uchar8 resultMesg[100];
   PSPResTestRecord_t* next;
};
#endif /* PSP_XML_MULTIMODSET_SUPPORT */

typedef struct PSPTestRecordList_s
{
   t_char8 Group[PSP_TEST_GROUP_LEN + 1];
   t_uchar8 RecType;
   ipaddr ulIpAddr1;
   ipaddr ulIpAddr2;
   t_bool8    bHeap;
   struct PSPTestRecordList_s *pNext;
}PSPTestRecordList_t;

typedef struct PSPTestRecordListFreeQ_s
{
   PSPTestRecordList_t *pHead;
   PSPTestRecordList_t *pTail;
   t_uint32 ulCnt;
}PSPTestRecordListFreeQ_t;


t_int32 PSPTestRecordInit(t_int32 iNoOfFreeNodes_p);
t_int32 PSPTestRecordDeInit(t_void);
PSPTestRecordList_t *PSPTestRecordAlloc(t_void);
t_int32 PSPTestRecordFree(PSPTestRecordList_t *pNode_p);
t_int32 PSPTestRecordAdd(PSPAPITestRecord_t *pNode_p);
t_int32 PSPTestRecordDel(PSPAPITestRecord_t *pNode_p);
t_int32 PSPTestRecordMod(PSPAPITestRecord_t *pNode_p);
t_int32 PSPGetFirstTestRecord(PSPAPITestRecord_t *pNode_p);
t_int32 PSPGetNextTestRecord(PSPAPITestRecord_t *pNode_p);
t_int32 PSPGetTestRecord(PSPAPITestRecord_t *pNode_p);
PSPTestRecordList_t *PSPSearchTestRecord(t_char8 *pGrpName);

#endif /* PSP_XML_TEST_SUPPORT*/
#endif /* __XMLREC_H__ */
#endif /* PSP_XML_SUPPORT*/

