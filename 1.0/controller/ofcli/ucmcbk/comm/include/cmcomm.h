
#ifndef CM_COMM_H
#define CM_COMM_H

#define CM_MAX_CPDP_CMCCNTXT_BUF 2048
//#define  CM_COMM_DEBUG

#ifdef CM_COMM_DEBUG
#define CM_COMM_DEBUG_PRINT(format,args...) printf("\r\n%s(%d)::"format,__FUNCTION__,__LINE__,##args)
#else
#define CM_COMM_DEBUG_PRINT(format,args...)
#endif

int32_t UCMCreateCommChannelwithController();


#endif /* CM_CNTRLR_H */
