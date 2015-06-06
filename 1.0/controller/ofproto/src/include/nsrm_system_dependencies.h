#include <pthread.h>
#include <stdint.h>
#include <urcu.h>
#include <string.h>
#include "cntlr_lock.h"
#include "cntlr_list.h"
#include "cntlr_system.h"
#include "mchash.h"
#include "nsrm.h"
#include "nsrmldef.h"
#include "memapi.h"
#include "oflog.h"
#include "crmapi.h"
#include "viewldef.h"

int32_t of_show_stack_trace(); 


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef __KERNEL__
#ifdef __GNUC__
#ifndef likely
#define likely(a)       __builtin_expect(!!(a), 1)
#endif
#ifndef unlikely
#define unlikely(a)     __builtin_expect(!!(a), 0)
#endif
#else
#ifndef likely
#define likely(a)       (a)
#endif
#ifndef unlikely
#define unlikely(a)     (a)  
#endif
#endif
#endif

#define cntlr_assert(cond)  do {\
                                if(!(cond))\
                                {\
                                  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_CRITICAL,"ASSERT: (%s) failed @ %s:%d\n",                                  #cond,__FILE__,__LINE__);\
                                  of_show_stack_trace();\
                                }\
                             } while(0);


#define OFP_ETH_ALEN 6          /* Bytes in an Ethernet address. */



