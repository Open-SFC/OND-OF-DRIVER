

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



extern void *of_view_node_mempool_g;
extern int32_t of_no_of_view_node_buckets_g;
struct database_view dprm_db_view[DPRM_MAX_VIEW_DATABASE];

char *of_view_database_name[DPRM_MAX_VIEW_DATABASE]=
{
	OF_VIEW_NSNAME_DPID,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED,
	OF_VIEW_RESERVED
};

uint8_t of_view_get_uniqueness(char *view_name)
{
  if(!strcmp( view_name, OF_VIEW_NSNAME_DPID))
    return FALSE;  

  return FALSE;  
}

int32_t of_get_view_index(char *view_name)
{
  int32_t index;

  OF_GET_VIEW_DATABASE_INDEX(view_name,index);
  if((index < 0) || (index >= DPRM_MAX_VIEW_DATABASE) )
  { 
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "no database found"); 
    return OF_FAILURE;
  }
  return index;
}

int32_t of_get_view_db_name(int32_t index, char *view_name)
{

  if (view_name == NULL)
  { 
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "no database found"); 
    return OF_FAILURE;
  }

  if ((index <0 ) || (index  >= DPRM_MAX_VIEW_DATABASE) )
  { 
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "no database found"); 
    return OF_FAILURE;
  }

  strcpy(view_name, of_view_database_name[index]);
  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "view name %s", view_name); 
  return OF_SUCCESS;
}

int8_t of_view_db_valid(int32_t view_index)
{
	int32_t index;

	if(index >= DPRM_MAX_VIEW_DATABASE || index < 0 ) 
	{ 
		OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "no database found"); 
		return FALSE;
	}
	return   dprm_db_view[view_index].valid_b;

}

int32_t of_create_view(char *view_name)
{
  int32_t index;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  OF_GET_VIEW_DATABASE_INDEX(view_name,index);
  if(index >= DPRM_MAX_VIEW_DATABASE || index < 0 ) 
  { 
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "no database found"); 
    return OF_FAILURE;
  } 

  if (!strcmp( dprm_db_view[index].view, view_name))
  {
	  OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR, "view is already created");
	  if ( dprm_db_view[index].valid_b != TRUE)
		  dprm_db_view[index].valid_b = TRUE;
	  return index;
  }

  strcpy( dprm_db_view[index].view, view_name);
  of_view_hash_table_init(&dprm_db_view[index].db_viewtbl_p);

  dprm_db_view[index].uniqueness_flag=of_view_get_uniqueness(view_name);
  dprm_db_view[index].valid_b=TRUE;


  return index;
}

struct database_view_node *of_alloc_view_entry_and_set_values(int64_t saferef, char *node_name, char *view_value)
{

  struct database_view_node *view_node_entry_p;
  int32_t ret_value;
  uint8_t heap_b;
  int32_t status;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  ret_value = mempool_get_mem_block(of_view_node_mempool_g,(uchar8_t**)&view_node_entry_p,&heap_b);
  if(ret_value != MEMPOOL_SUCCESS)
    return NULL;

  view_node_entry_p->node_name=(char *)calloc(1,OF_MAX_NODE_NAME_LEN);
  if (view_node_entry_p->node_name == NULL)
  {
    status = OF_FAILURE;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"memory allocation failed");
    mempool_release_mem_block(of_view_node_mempool_g,(uchar8_t*)view_node_entry_p,FALSE);
    return NULL;
  }

  view_node_entry_p->view_value=(char *)calloc(1,OF_MAX_VIEW_VALUE_LEN);
  if (view_node_entry_p->view_value == NULL)
  {
    status = OF_FAILURE;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_ERROR,"memory allocation failed");
    free(view_node_entry_p->node_name);
    mempool_release_mem_block(of_view_node_mempool_g,(uchar8_t*)view_node_entry_p,FALSE);
    return NULL;
  }

  strcpy(view_node_entry_p->view_value,view_value);
  strcpy(view_node_entry_p->node_name,node_name);
  view_node_entry_p->node_saferef=saferef;

  return view_node_entry_p;
}

int32_t  of_create_view_entry(int32_t view_index, char *node_name, char *view_value, uint64_t node_saferef)
{
  struct database_view_node *view_node_entry_p;
  uint32_t index,magic;
  int32_t  status=OF_SUCCESS;
  uint32_t hashkey;
  uint8_t status_b;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  do
  {

    view_node_entry_p=of_alloc_view_entry_and_set_values( node_saferef, node_name, view_value);
    if (view_node_entry_p == NULL)
    {
      status = OF_FAILURE;
      break;
    }

    if ( dprm_db_view[view_index].uniqueness_flag)
    {
      hashkey = dprm_get_hashval_byname(view_value,of_no_of_view_node_buckets_g);
    }
    else
    {
      hashkey = dprm_get_hashval_bynames(view_value,node_name,of_no_of_view_node_buckets_g);
    }

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "hashkey = %d",hashkey);

    CNTLR_RCU_READ_LOCK_TAKE();

    MCHASH_APPEND_NODE(dprm_db_view[view_index].db_viewtbl_p,hashkey,view_node_entry_p,index,magic,view_node_entry_p,status_b);
    if(FALSE == status_b)
    {
      status = OF_FAILURE;
      CNTLR_RCU_READ_LOCK_RELEASE();
      break;
    }

    view_node_entry_p->magic = magic;
    view_node_entry_p->index = index;

    CNTLR_RCU_READ_LOCK_RELEASE();

  }while(0);

  return status;
}


int32_t  of_remove_view_entry(int32_t view_index, char *node_name, char *view_value)
{
  struct database_view_node *view_node_entry_p;
  int32_t  status=OF_FAILURE;
  int32_t offset, hashkey;
  uint8_t status_b;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
    CNTLR_RCU_READ_LOCK_TAKE();
  do
  {
    offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
    if ( dprm_db_view[view_index].uniqueness_flag)
    {
      hashkey = dprm_get_hashval_byname(view_value,of_no_of_view_node_buckets_g);

    }
    else
    {
      hashkey = dprm_get_hashval_bynames(view_value,node_name,of_no_of_view_node_buckets_g);
    }

    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if (( !strcmp(view_node_entry_p->view_value, view_value)) &&
          ( !strcmp(view_node_entry_p->node_name, node_name)))
      {
        status_b = FALSE;
        offset = 0;
        MCHASH_DELETE_NODE_BY_REF( dprm_db_view[view_index].db_viewtbl_p,view_node_entry_p->index,view_node_entry_p->magic,struct database_view_node *,offset,status_b);
        if ( status_b == FALSE)
        {
          status=OF_FAILURE;
          break;
        }
        status=OF_SUCCESS;
        break;
      }
    }

  }while(0);

    CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t  of_view_get_view_handle_by_name(int32_t view_index, struct database_view_node_info *db_view_node_info,  uint64_t  *view_handle_p)
{
	int32_t hashkey;
	int32_t offset;
	struct database_view_node *view_node_entry_p;
	int32_t status=OF_FAILURE;

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
	if ( dprm_db_view[view_index].uniqueness_flag)
	{
		hashkey = dprm_get_hashval_byname(db_view_node_info->view_value,of_no_of_view_node_buckets_g);
	}
	else
	{
		hashkey = dprm_get_hashval_bynames(db_view_node_info->view_value,db_view_node_info->node_name,of_no_of_view_node_buckets_g);
	}

	OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "hashkey = %d",hashkey);
	offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
	MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
	{

		if((!strcmp(db_view_node_info->view_value,view_node_entry_p->view_value)) &&
					(!strcmp(db_view_node_info->node_name,view_node_entry_p->node_name)))
		{
			*view_handle_p = view_node_entry_p->magic;
			*view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));

			OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "matching entry found");
			status=OF_SUCCESS;
			break;
		}

	}
	return status;
}

int32_t  of_view_get_first_view_entry( int32_t view_index, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  struct database_view_node *view_node_entry_p;
  int32_t  offset, status=OF_FAILURE;
  uint32_t hashkey;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();
  offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
  MCHASH_TABLE_SCAN(dprm_db_view[view_index].db_viewtbl_p,hashkey)
  {

    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {

      strcpy( db_view_node_info->view_value,view_node_entry_p->view_value);
      strcpy( db_view_node_info->node_name,view_node_entry_p->node_name);

      *view_handle_p = view_node_entry_p->magic;
      *view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));
      status=OF_SUCCESS;
      break;

    }
    if ( status == OF_SUCCESS)
      break;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;

}

int32_t  of_view_get_next_view_entry( int32_t view_index, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  uint8_t current_entry_found_b;
  struct database_view_node *view_node_entry_p;

  int32_t  offset, status=OF_FAILURE;
  uint32_t hashkey;
    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();

  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(dprm_db_view[view_index].db_viewtbl_p,hashkey)
  {

    offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(!(( view_node_entry_p->magic  == ((uint32_t)(*view_handle_p >>32)) ) &&
              ( view_node_entry_p->index  == ((uint32_t)(*view_handle_p ) ))))
        {

                                  continue;
        }
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
      }

      strcpy( db_view_node_info->view_value,view_node_entry_p->view_value);
      strcpy( db_view_node_info->node_name,view_node_entry_p->node_name);

      *view_handle_p = view_node_entry_p->magic;
      *view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));

      status=OF_SUCCESS;
      break;

    }
    if ( status == OF_SUCCESS)
      break;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}



int32_t  of_view_get_first_view_entry_using_key(int32_t view_index, char *view_value, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  struct database_view_node *view_node_entry_p;
  int32_t  status=OF_FAILURE;
  uint32_t hashkey;
  int32_t offset;
  uint8_t current_entry_found_b;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_TABLE_SCAN(dprm_db_view[view_index].db_viewtbl_p,hashkey)
  {
    offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(!(( view_node_entry_p->magic  == ((uint32_t)(*view_handle_p >>32)) ) &&
              ( view_node_entry_p->index  == ((uint32_t)(*view_handle_p ) ))))
        {
          continue;
        }
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
      }
      if(!strcmp( view_node_entry_p->view_value, view_value))
      {

        strcpy( db_view_node_info->view_value,view_node_entry_p->view_value);
        strcpy( db_view_node_info->node_name,view_node_entry_p->node_name);

        *view_handle_p = view_node_entry_p->magic;
        *view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));

        status=OF_SUCCESS;
        break;
      }

    }
    if (status == OF_SUCCESS)
      break;
  }

  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t  of_view_get_next_view_entry_using_key (int32_t view_index, char *view_value, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  struct database_view_node *view_node_entry_p;
  int32_t  status=OF_FAILURE;
  uint32_t offset, hashkey;
  uint8_t current_entry_found_b;


    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();

  current_entry_found_b = FALSE;

  MCHASH_TABLE_SCAN(dprm_db_view[view_index].db_viewtbl_p,hashkey)
  {
    offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(!strcmp( view_node_entry_p->view_value, view_value))
      {

        strcpy( db_view_node_info->view_value,view_node_entry_p->view_value);
        strcpy( db_view_node_info->node_name,view_node_entry_p->node_name);

        *view_handle_p = view_node_entry_p->magic;
        *view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));

        status=OF_SUCCESS;
        break;
      }

    }
    if (status == OF_SUCCESS)
      break;
  }
  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

int32_t  of_view_get_exact_view_entry(int32_t view_index, char *view_value, char *node_name_p, struct database_view_node_info *db_view_node_info, uint64_t  *view_handle_p)
{
  struct database_view_node *view_node_entry_p;
  int32_t  status=OF_FAILURE;
  uint32_t hashkey;
  int32_t offset;
  uint8_t current_entry_found_b;

    OF_LOG_MSG(OF_LOG_MOD, OF_LOG_DEBUG, "entered"); 
  CNTLR_RCU_READ_LOCK_TAKE();

  MCHASH_TABLE_SCAN(dprm_db_view[view_index].db_viewtbl_p,hashkey)
  {
    offset=OF_VIEW_ENTRY_NODE_HASHTBLE_LINK_OFFSET;
    MCHASH_BUCKET_SCAN( dprm_db_view[view_index].db_viewtbl_p,hashkey, view_node_entry_p,struct database_view_node  *,offset)
    {
      if(current_entry_found_b == FALSE)
      {
        if(!(( view_node_entry_p->magic  == ((uint32_t)(*view_handle_p >>32)) ) &&
              ( view_node_entry_p->index  == ((uint32_t)(*view_handle_p ) ))))
        {
          continue;
        }
        else
        {
          current_entry_found_b = TRUE;
          continue;
        }
      }
      if ((!strcmp( view_node_entry_p->view_value, view_value))  && 
      (!strcmp( view_node_entry_p->node_name, node_name_p)) )
      {

        strcpy( db_view_node_info->view_value,view_node_entry_p->view_value);
        strcpy( db_view_node_info->node_name,view_node_entry_p->node_name);

        *view_handle_p = view_node_entry_p->magic;
        *view_handle_p = ((*view_handle_p <<32) | (view_node_entry_p->index));

        status=OF_SUCCESS;
        break;
      }

    }
    if (status == OF_SUCCESS)
      break;
  }

  CNTLR_RCU_READ_LOCK_RELEASE();
  return status;
}

