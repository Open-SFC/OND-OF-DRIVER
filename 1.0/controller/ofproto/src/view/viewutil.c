
#include "controller.h"
#include "dprm.h"
#include "cntlr_epoll.h"
#include "cntrl_queue.h"
#include "of_utilities.h"
#include "of_msgapi.h"
#include "of_multipart.h"
#include "cntlr_tls.h"
#include "cntlr_transprt.h"
#include "cntlr_event.h"
#include "of_view.h"
#include "viewldef.h"
#include "dprmldef.h"






uint32_t dprm_get_hashval_bynames(char* name1_p,char *name2_p,uint32_t no_of_buckets)
{
  uint32_t hashkey,param1,param2,param3,param4;
  char name1_hash[8];
  char name2_hash[8];
  memset(name1_hash,0,8);
  memset(name2_hash,0,8);
  strncpy(name1_hash,name2_p,8);
  strncpy(name2_hash,name2_p,8);
  param1 = *(uint32_t *)(name1_hash +0);
  param2 = *(uint32_t *)(name1_hash +4);
  param3 = *(uint32_t *)(name2_hash +0);
  param4 = *(uint32_t *)(name2_hash +4);
  DPRM_COMPUTE_HASH(param1,param2,param3,param4,no_of_buckets,hashkey);
  return hashkey;
}

