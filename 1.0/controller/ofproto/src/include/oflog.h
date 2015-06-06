#ifndef _OFLOG_H_
#define _OFLOG_H_

#define OF_LOG_MSG_SUPPORT

#ifdef OF_LOG_MSG_SUPPORT
void OFLogMessage (int iModuleId, unsigned int ucLevel, const char * pFunc,
    int ulLine, char * pFormatString, ...);
#endif

enum logmodules
{
  OF_LOG_MOD=0,
  OF_LOG_XPRT_PROTO,
  OF_NEM_LOG,
  OF_RT_LOG,
  OF_LOG_PETH_APP,

  OF_LOG_SAMPLE_APP,
  OF_LOG_ARP_APP,
  OF_LOG_ARP_NF,
  OF_LOG_FWD_NF,
  OF_LOG_MCAST_APP,
  OF_LOG_INFRA_APP,
  OF_LOG_IPSEC,
  OF_LOG_NETLINK,

  OF_LOG_UTIL,
  OF_LOG_CRM,
  OF_LOG_NSRM,
  OF_LOG_CNTLR_TEST,
  OF_LOG_UTIL_L3EXT,
  OF_LOG_EXT_NICIRA,
  OF_LOG_TTP,
  OF_LOG_TSC,
  OF_LOG_BIND_OBJ,

  //These two  should be the last entries in this enum: DO NOT add anything after this
  OF_LOG_MODULE_ALL,
  OF_LOG_MODULE_NONE  
};

typedef struct trace_params_s{
	int is_trace_module_set;
	int trace_level;
}trace_params_t;


#define OF_LOG_CRITICAL 1
#define OF_LOG_ERROR 2
#define OF_LOG_WARN 3
#define OF_LOG_NOTICE 4
#define OF_LOG_INFO 5
#define OF_LOG_DEBUG 6
#define OF_LOG_ALL  7
#define OF_LOG_NONE 8

#define OF_LOG_LEVEL OF_LOG_ALL 
//#define OF_LOG_LEVEL OF_LOG_ERROR 
#define OF_MAX_LOG_LEVEL OF_LOG

#define TOTAL_LOG_MODULES OF_LOG_MODULE_NONE 
#define TRACE_MODULE_SET 1
#define TRACE_MODULE_NOT_SET 0


#define OF_DEBUG_PRINT(ulModId, ulLogLevel,format,args...)\
{\
  if(unlikely(trace_params_values[ulModId].trace_level != OF_LOG_NONE))\
    OFLogMessage(ulModId, ulLogLevel, __FUNCTION__, __LINE__,format,##args);\
}\

#define OF_NO_PRINT(ulModId, ulLogLevel,format,args...)

#ifdef OF_LOG_MSG_SUPPORT
 #define OF_LOG_MSG  OF_DEBUG_PRINT 
#else
 #define OF_LOG_MSG OF_NO_PRINT
#endif


#define OF_ALLOC_PRINT_DEBUG_MSG  \
{\
 OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Allocating of_msg %d", ++alloc_cnt_g);\
}
#define OF_DE_ALLOC_PRINT_DEBUG_MSG  \
{\
 OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG,"Cleanup::De-Allocating of_msg %d", ++de_alloc_cnt_g);\
}
#define OFU_ALLOC_MESSAGE_WITH_DEBUG(a,b)  ofu_alloc_message(a,b);OF_ALLOC_PRINT_DEBUG_MSG
//#define OFU_ALLOC_MESSAGE_WITH_DEBUG(a,b)  ofu_alloc_message(a,b)

/***** GLOBAL VARIBLES DECLARATION SECTION **********/

extern trace_params_t trace_params_values[];

#endif

