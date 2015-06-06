/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
*/

/*
 *
 * File name: portstats.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/13/2013
 * Description: Port Statistics.
 * 
 */

#ifdef OF_CM_SUPPORT


#include "cbcmninc.h"
#include "cmgutil.h"
#include "cntrucminc.h"
#include "of_utilities.h"
#include "of_multipart.h"

int32_t of_portstats_appl_ucmcbk_init (void);

int32_t of_portstats_addrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_portstats_setrec (void * config_trans_p,
    struct cm_array_of_iv_pairs * mand_iv_pairs_p,
    struct cm_array_of_iv_pairs * opt_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_portstats_delrec (void * pConfigTransction,
    struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_app_result ** result_p);

int32_t of_portstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_portstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs * pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_portstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp);

int32_t of_portstats_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_p);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */
int32_t of_portstats_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct cm_app_result **result_pp);

int32_t of_portstats_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** result_pp);


int32_t of_portstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_port_stats_info* port_info,
    struct cm_app_result ** result_pp);

int32_t of_portstats_ucm_set_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct ofi_port_stats_info * port_info,
    struct cm_app_result ** result_pp);


int32_t of_portstats_ucm_getparams (struct ofi_port_stats_info *port_info,
    struct cm_array_of_iv_pairs * result_iv_pairs_p);



struct cm_dm_app_callbacks of_portstats_ucm_callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  of_portstats_getfirstnrecs,
  of_portstats_getnextnrecs,
  of_portstats_getexactrec,
  NULL,
  NULL
};
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * Switch  UCM Init * * * * * * *  * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32_t of_portstats_appl_ucmcbk_init (void)
{
  CM_CBK_DEBUG_PRINT ("Entry");
  cm_register_app_callbacks (CM_ON_DIRECTOR_DATAPATH_STATS_PORTSTATS_APPL_ID,&of_portstats_ucm_callbacks);
  return OF_SUCCESS;
}



int32_t of_portstats_getfirstnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  struct cm_array_of_iv_pairs *result_iv_pairs_p = NULL;
  int32_t return_value = OF_FAILURE;
  uint32_t no_of_records = 0;
  struct cm_app_result *port_result = NULL;
  uint64_t datapath_handle;
  struct ofi_port_stats_info port_info;
  struct dprm_datapath_general_info datapath_info={};

  of_memset (&port_info, 0, sizeof (struct ofi_port_stats_info));

  if ((of_portstats_ucm_set_mand_params (key_iv_pairs_p,&datapath_info, &port_info, &port_result)) !=
      OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Set Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  return_value=dprm_get_datapath_handle(datapath_info.dpid, &datapath_handle);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Get DPRM Handle failed");
    return OF_FAILURE;
  }

  return_value=cntlr_send_port_stats_request(datapath_handle); 
  CM_CBK_DEBUG_PRINT ("retval %d", return_value);
  if ( return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("port stats failed");
    return OF_FAILURE;
  } 

  CM_CBK_DEBUG_PRINT ("Port stats will be sent as multipart response");
  *count_p = no_of_records;
  return CM_CNTRL_MULTIPART_RESPONSE;

}


int32_t of_portstats_getnextnrecs (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs *pPrevRecordKey, uint32_t * count_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  *count_p=0;
  return return_value;
}

int32_t of_portstats_getexactrec (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    struct cm_array_of_iv_pairs ** result_iv_pairs_pp)
{
  int32_t return_value = OF_FAILURE;

  CM_CBK_DEBUG_PRINT ("Entered");
  return return_value;
}

int32_t of_portstats_verifycfg (struct cm_array_of_iv_pairs * key_iv_pairs_p,
    uint32_t command_id, struct cm_app_result ** result_pp)
{
  struct cm_app_result *port_result_p = NULL;
  int32_t return_value = OF_FAILURE;
  struct ofi_port_stats_info port_info = { };

  CM_CBK_DEBUG_PRINT ("Entered");
  of_memset (&port_info, 0, sizeof (struct ofi_port_stats_info));

  return_value = of_portstats_ucm_validate_mand_params (key_iv_pairs_p, &port_result_p);
  if (return_value != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  if (of_portstats_ucm_validate_opt_params (key_iv_pairs_p, &port_result_p)
      != OF_SUCCESS)
  {
    CM_CBK_DEBUG_PRINT ("Validate Mandatory Parameters Failed");
    return OF_FAILURE;
  }

  *result_pp = port_result_p;
  return OF_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * V L A N  U T I L I T Y  F U N C T I O N S * * * * * * * * * * *
 * * * * * * * * * * * * * **  * * * * * * * **  * * * * * * * * * * * * * */

int32_t of_portstats_ucm_validate_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,  struct cm_app_result **result_pp)
{
  uint32_t param_count;
  struct cm_app_result *port_result_p = NULL;
  uint32_t uiPortId;


  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_PORT_PORTNAME_ID:
        if (mand_iv_pairs_p->iv_pairs[param_count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Port Name is NULL");
          fill_app_result_struct (&port_result_p, NULL, CM_GLU_PORT_NAME_NULL);
          *result_pp = port_result_p;
          return OF_FAILURE;
        }
        break;

      case CM_DM_PORT_PORTID_ID: 
        if (mand_iv_pairs_p->iv_pairs[param_count].value_p == NULL)
        {
          CM_CBK_DEBUG_PRINT ("Port ID is NULL");
          fill_app_result_struct (&port_result_p, NULL, CM_GLU_PORT_ID_NULL);
          *result_pp = port_result_p;
          return OF_FAILURE;
        }
        uiPortId =
          of_atoi ((char *) (mand_iv_pairs_p->iv_pairs[param_count].value_p));
        if (uiPortId < 0 ||  uiPortId > 24)
        {
          CM_CBK_DEBUG_PRINT ("Invalid Port ID entered.");
          fill_app_result_struct (&port_result_p, NULL, CM_GLU_PORT_ID_INVALID);
          *result_pp = port_result_p;
          return OF_FAILURE;
        }
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_portstats_ucm_validate_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct cm_app_result ** result_pp)
{
  uint32_t param_count, uiTableCnt;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      default:
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_portstats_ucm_set_mand_params (struct cm_array_of_iv_pairs *
    mand_iv_pairs_p,
    struct dprm_datapath_general_info *datapath_info,
    struct ofi_port_stats_info* port_info,    struct cm_app_result ** result_pp)
{
  uint32_t param_count;
  uint64_t idpId;

  CM_CBK_DEBUG_PRINT ("Entered");
  for (param_count = 0; param_count < mand_iv_pairs_p->count_ui;
      param_count++)
  {
    switch (mand_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      case CM_DM_DATAPATH_DATAPATHID_ID:
        idpId=charTo64bitNum((char *) mand_iv_pairs_p->iv_pairs[param_count].value_p);
        CM_CBK_DEBUG_PRINT("dpid is: %llx\r\n", idpId);
        datapath_info->dpid=idpId;
        break;
    }
  }
  CM_CBK_PRINT_IVPAIR_ARRAY (mand_iv_pairs_p);
  return OF_SUCCESS;
}

int32_t of_portstats_ucm_set_opt_params (struct cm_array_of_iv_pairs *
    opt_iv_pairs_p,
    struct ofi_port_stats_info * port_info,
    struct cm_app_result ** result_pp)
{
  uint32_t param_count;

  CM_CBK_DEBUG_PRINT ("Entered");

  for (param_count = 0; param_count < opt_iv_pairs_p->count_ui; param_count++)
  {
    switch (opt_iv_pairs_p->iv_pairs[param_count].id_ui)
    {
      default:
        break;
    }
  }

  CM_CBK_PRINT_IVPAIR_ARRAY (opt_iv_pairs_p);
  return OF_SUCCESS;
}



int32_t of_portstats_ucm_getparams (struct ofi_port_stats_info *port_info_p,
    struct cm_array_of_iv_pairs * result_iv_pairs_p)
{
  char string[64];
  int32_t index = 0,  count_ui ;

#define CM_PORTSTATS_CHILD_COUNT 15
  count_ui = CM_PORTSTATS_CHILD_COUNT;

  if(port_info_p == NULL)
  {
    return OF_FAILURE;
  }

  //CM_CBK_DEBUG_PRINT ("Entered");
  result_iv_pairs_p->iv_pairs = (struct cm_iv_pair*)of_calloc(count_ui, sizeof(struct cm_iv_pair));
  if(!result_iv_pairs_p->iv_pairs)
  {
    CM_CBK_DEBUG_PRINT (" Memory allocation failed ");
    return OF_FAILURE;
  }

  of_memset(string, 0, sizeof(string));
  of_itoa (port_info_p->port_no, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_PORTNO_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_packets, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXPACKETS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->tx_packets, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_TXPACKETS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_bytes, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXBYTES_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->tx_bytes, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_TXBYTES_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_dropped, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXDROPPED_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->tx_dropped, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_TXDROPPED_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_errors, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXERRORS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->tx_errors, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_TXERRORS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_frame_err, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXFRAMEERRORS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_over_err, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXOVERERR_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->rx_crc_err, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_RXCRCERR_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_ltoa (port_info_p->collisions, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_COLLISIONS_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (port_info_p->duration_sec, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_DURATIONSEC_ID,CM_DATA_TYPE_STRING, string);
  index++;

  of_memset(string, 0, sizeof(string));
  of_itoa (port_info_p->duration_nsec, string);
  FILL_CM_IV_PAIR(result_iv_pairs_p->iv_pairs[index],CM_DM_PORTSTATS_DURATIONNSEC_ID,CM_DATA_TYPE_STRING, string);
  index++;

  result_iv_pairs_p->count_ui = index;

  CM_CBK_PRINT_IVPAIR_ARRAY (result_iv_pairs_p);
  return OF_SUCCESS;
}

#endif /* OF_CM_SUPPORT */
