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
 * File name: dminst.c
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   02/15/2013
 * Description: This file contains source code implementing 
 *              configuration middle-ware data model tree. 
*/

#ifdef CM_DM_SUPPORT
#ifdef CM_DM_TEST_SUPPORT

#include "dmincld.h"

#define CM_DM_TEST_ROOT_PATH        "IGD"

char TestPath[CM_DM_MAX_PATH_LEN + 1];

struct cm_dm_data_element RootChildNodes[] = {

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "VSG",                       /* Element name. Not an FQN */
   CM_DM_VSG_NODE_ID,          /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "VSG Table",                 /* Short description */
   "vsg",                       /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_TABLE,      /* anchor/table/struct/variable */
    FALSE,                    /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    FALSE,                    /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_UNKNOWN,      /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_NONE,       /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

};

struct cm_dm_data_element VsgChildNodes[] = {

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "Firewall",                  /* Element name. Not an FQN */
   CM_DM_FW_NODE_ID,           /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "Firewall Anchor",           /* Short description */
   "firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_ANCHOR,     /* anchor/table/struct/variable */
    FALSE,                    /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    FALSE,                    /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_UNKNOWN,      /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_NONE,       /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "IPSec",                     /* Element name. Not an FQN */
   CM_DM_IPSEC_NODE_ID,        /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "IPSec Anchor",              /* Short description */
   "ipsec",                     /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_ANCHOR,     /* anchor/table/struct/variable */
    FALSE,                    /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    FALSE,                    /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_UNKNOWN,      /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_NONE,       /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "IPS",                       /* Element name. Not an FQN */
   CM_DM_IPS_NODE_ID,          /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "IPS Anchor",                /* Short description */
   "ips",                       /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_ANCHOR,     /* anchor/table/struct/variable */
    FALSE,                    /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    FALSE,                    /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_UNKNOWN,      /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_NONE,       /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

};

struct cm_dm_data_element ipdbDataElements[] = {

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "IPObject",                  /* Element name. Not an FQN */
   CM_DM_IPDB_NODE_ID,         /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "IPDB Table",                /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_TABLE,      /* anchor/table/struct/variable */
    FALSE,                    /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    FALSE,                    /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_NO_HTTP,         /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_UNKNOWN,      /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_NONE,       /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "IPName",                    /* Element name. Not an FQN */
   CM_DM_IPDB_NAME_ID,         /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "IPDB Name",                 /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    TRUE,                     /* Is this a key for the container? */
    TRUE,                     /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    TRUE,                     /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_RANGE,  /* range/enum/etc */
    {
     1, 255}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "RecType",                   /* Element name. Not an FQN */
   CM_DM_IPDB_RECORDTYPE_ID,   /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "Record Type",               /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_ENUM,   /* range/enum/etc */
    {.string_enum.count_ui = 5,
     .string_enum.array[0] = "Single",
     .string_enum.array[1] = "Range",
     .string_enum.array[2] = "Subnet",
     .string_enum.array[3] = "Domain",
     .string_enum.array[4] = "Any",

     }
    ,
    {
     .default_string = "Any",
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "DomainName",                /* Element name. Not an FQN */
   CM_DM_IPDB_DOMAIN_NAME_ID,  /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "Domain Name",               /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_RANGE,  /* range/enum/etc */
    {
     10, 255}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "StartIP",                   /* Element name. Not an FQN */
   CM_DM_IPDB_START_IP_ADDR_ID,        /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "Start IP Address",          /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_ENUM,   /* range/enum/etc */
    {.string_enum.count_ui = 2,
     .string_enum.array[0] = "0.0.0.0",
     .string_enum.array[1] = "255.255.255.255",

     }
    ,
    {
     .default_string = "0.0.0.0",
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "EndIP",                     /* Element name. Not an FQN */
   CM_DM_IPDB_END_IP_ADDR_ID,  /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "End IP Address",            /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_ENUM,   /* range/enum/etc */
    {.string_enum.count_ui = 2,
     .string_enum.array[0] = "0.0.0.0",
     .string_enum.array[1] = "255.255.255.255",

     }
    ,
    {
     .default_string = "0.0.0.0",
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "Prefix",                    /* Element name. Not an FQN */
   CM_DM_IPDB_PREFIX_ID,       /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "If record type is subnet(sn),this field contains the prefix",       /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    FALSE,                    /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    FALSE,                    /* Inform if any config change on this element */
    CM_NOT_CHANGEABLE_BY_ACS,  /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_INT,          /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_INT_RANGE,  /* range/enum/etc */
    {
     0, 0}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

  {
   /* Reserved section */
   NULL,                        /* Reserved : internal use */
   NULL,                        /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */
   {NULL, NULL}
   ,                            /* Reserved : internal use */
   {{NULL, NULL}
    , 0}
   ,                            /* Reserved : internal use */

   /* User section */
   "SnetName",                  /* Element name. Not an FQN */
   CM_DM_IPDB_SNET_ID,         /* Unique ID with in siblings */
   NULL,                        /* Reference name array */
   NULL,                        /* Function ptr to validate data */
   "This field contains the SnetID to which this record belongs",       /* Short description */
   "Firewall",                  /* If NULL, use parent's filename */
   {
    CM_DMNODE_TYPE_SCALAR_VAR, /* anchor/table/struct/variable */
    TRUE,                     /* should we expose for configuration? */
    FALSE,                    /* Is this a key for the container? */
    TRUE,                     /* Is this a mandatory param? */
    FALSE,                    /* Is this an ordered list? (table) */
    FALSE,                    /* Is this specifies priority of container table? */
    TRUE,                     /* Inform if any config change on this element */
    CM_ACTIVE_NOTIFICATION,    /* 1 = ActiveNotify, 
                                   2 = PassiveNotify, 
                                   4 = Not changeable by ACS */
    FALSE,                    /* Reboot required if modified */
    TRUE,                     /* Piggyback this configuration with parent's */
    TRUE,                     /*      Save this element in parent instance's
                                   directory. For example, firewall is 
                                   saved in "vsg-1", etc. */
    CM_ACCESS_ALL,             /*      0 = Allowed from ALL
                                   1 = HTTP Can not Edit
                                   2 = CLI Can not Edit */
    }
   ,
   {
    CM_DATA_TYPE_STRING,       /* int/string/ip_addr/etc */
    CM_DATA_ATTRIB_STR_RANGE,  /* range/enum/etc */
    {
     1, 16}
    ,
    {
     0,
     }
    ,
    }
   ,
   }
  ,

};

int32_t ucmCMDM_RegisterDMNodes ();
int32_t igwCMDM_TestGetNodeInfo (uint32_t * pTestCount, uint32_t * pFailCount);
int32_t igwCMDM_TestGetNodeAndChildInfo (uint32_t * pTestCount,
                                         uint32_t * pFailCount);
/**************************************************************************
  Test Routines
 *************************************************************************/
/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t ucmCMDM_TestInit ()
{
  printf ("%s\n\r", __FUNCTION__);
  ucmCMDM_RegisterDMNodes ();
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t ucmCMDM_RegisterDMNodes ()
{
  int32_t return_value;
  struct cm_dm_data_element *pNode = NULL;

  return_value = cm_dm_register_data_elements ("IGD",
                                     (sizeof (RootChildNodes) /
                                      sizeof (struct cm_dm_data_element)),
                                     &(RootChildNodes[0]));
  if (return_value != OF_SUCCESS)
  {
    printf ("igwCMDM_RegisterDataElements : RootChildNodes, Error : %d\n",
            return_value);
    return OF_FAILURE;
  }

  return_value = cm_dm_register_data_elements ("VSG",
                                     (sizeof (VsgChildNodes) /
                                      sizeof (struct cm_dm_data_element)),
                                     &(VsgChildNodes[0]));
  if (return_value != OF_SUCCESS)
  {
    printf ("igwCMDM_RegisterDataElements : VsgChildNodes, Error : %d\n", return_value);
    return OF_FAILURE;
  }

  return_value = cm_dm_register_data_elements ("VSG.Firewall",
                                     (sizeof (ipdbDataElements) /
                                      sizeof (struct cm_dm_data_element)),
                                     &(ipdbDataElements[0]));
  if (return_value != OF_SUCCESS)
  {
    printf ("igwCMDM_RegisterDataElements : FirewallChildNodes, Error : %d\n",
            return_value);
    return OF_FAILURE;
  }

  pNode = cm_dm_get_root_node ();

  cm_dm_print_tree (pNode);
  return OF_SUCCESS;
}

#if 0
/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t igwCMDM_ExecuteTests ()
{
  int32_t return_value;
  uint32_t uiFail = 0;
  uint32_t uiTestCount = 0;
  uint32_t uiFailCount = 0;
  uint32_t uiVal = 0;
  char Path[513];

  uiTestCount++;
  printf ("Test Tree Traversal...(");
  return_value = cm_dm_print_tree (igwCMDM_GetRootNode ());
  if (return_value != OF_SUCCESS)
  {
    printf (")  Failed!\n");
    uiFailCount++;
  }
  printf (")  OK!\n");

#if 0
  igwCMDM_TestGetNodeInfo (&uiTestCount, &uiFailCount);

  igwCMDM_TestGetNodeAndChildInfo (&uiTestCount, &uiFailCount);
#endif

  of_strncpy (Path, "/vsg", 512);
  return_value = UCMDM_GetNewInstanceID_UsingNamePath ("/vsg", &uiVal);
  if (return_value == OF_SUCCESS)
  {
    printf ("Test _GetNewInstanceIDByNamePath : %s, ID = %d\n", Path, uiVal);
  }
  else
  {
    printf ("Test _GetNewInstanceIDByNamePath Failed : Ret 0x%x\n", return_value);
  }

  printf ("Test Results : %d/%d Tests Passed\n", uiTestCount - uiFailCount,
          uiTestCount);
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t igwCMDM_TestGetNodeInfo (uint32_t * pTestCount, uint32_t * pFailCount)
{
  uint32_t i;
  int32_t return_value;
  struct cm_dm_node_info *attrib_p;
  struct cm_array_of_uints ArrayOfUints;
  uint32_t uiNodeCount = sizeof (VsgChildNodes) / sizeof (struct cm_dm_data_element);
  of_memset (TestPath, 0, CM_DM_MAX_PATH_LEN + 1);

  printf ("Test Extracting Node Information...");
  /* Retrieve valid node information */
  for (i = 0; i < uiNodeCount; i++)
  {
    *pTestCount = *pTestCount + 1;

    of_strcpy (TestPath, "/vsg/");
    of_strncat (TestPath, VsgChildNodes[i].name_p,
               (CM_DM_MAX_PATH_LEN - of_strlen (TestPath)));
    attrib_p = NULL;
    return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
    if (return_value == OF_SUCCESS)
    {
      // igwCMDM_PrintAttribs(attrib_p);
      if (igwCMDM_TestCompareAttribs (attrib_p, &(VsgChildNodes[i])) !=
          OF_SUCCESS)
      {
        printf (" Failed to extract info about node : %s\n", TestPath);
        *pFailCount = *pFailCount + 1;
      }
      of_free (attrib_p);
    }
    else
    {
      *pFailCount = *pFailCount + 1;
    }
  }

  /* Retrieve Invalid Node information */
  *pTestCount = *pTestCount + 1;
  of_strncpy (TestPath, "trash", CM_DM_MAX_PATH_LEN);
  return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
  if (return_value == OF_SUCCESS)
  {
    printf (" Failed to identify invalid node : %s\n", TestPath);
    *pFailCount = *pFailCount + 1;
  }

  /* Retrieve Invalid Node information */
  *pTestCount = *pTestCount + 1;
  of_strcpy (TestPath, "/");
  of_strcat (TestPath, "trash");
  return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
  if (return_value == OF_SUCCESS)
  {
    printf (" Failed to identify invalid node : %s\n", TestPath);
    *pFailCount = *pFailCount + 1;
  }

  /* Retrieve Invalid Node information */
  *pTestCount = *pTestCount + 1;
  of_strncpy (TestPath, CM_DM_TEST_ROOT_PATH, CM_DM_MAX_PATH_LEN);
  of_strcat (TestPath, "/trash");
  return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
  if (return_value == OF_SUCCESS)
  {
    printf (" Failed to identify invalid node : %s\n", TestPath);
    *pFailCount = *pFailCount + 1;
  }

  /* Retrieve Invalid Node information */
  *pTestCount = *pTestCount + 1;
  of_strncpy (TestPath, CM_DM_TEST_ROOT_PATH, CM_DM_MAX_PATH_LEN);
  of_strcat (TestPath, "/fw/trash");
  return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
  if (return_value == OF_SUCCESS)
  {
    printf (" Failed to identify invalid node : %s\n", TestPath);
    *pFailCount = *pFailCount + 1;
  }

  /* Retrieve Invalid Node information */
  *pTestCount = *pTestCount + 1;
  of_strcpy (TestPath, "/vsg/");
  of_strcat (TestPath, "fw///trash");
  return_value = UCMCMDM_GetNodeInfo (TestPath, &attrib_p);
  if (return_value == OF_SUCCESS)
  {
    printf (" Failed to identify invalid node : %s\n", TestPath);
    *pFailCount = *pFailCount + 1;
  }

  /* Retrieve node info by path IDs */
  ArrayOfUints.uint_arr_p = (uint32_t *) of_calloc (2, sizeof (uint32_t));
  if (ArrayOfUints.uint_arr_p == NULL)
  {
    return OF_FAILURE;
  }

  ArrayOfUints.count_ui = 2;
  ArrayOfUints.uint_arr_p[0] = CM_DM_ROOT_NODE_ID;

  for (i = 0; i < uiNodeCount; i++)
  {
    *pTestCount = *pTestCount + 1;
    ArrayOfUints.uint_arr_p[1] = VsgChildNodes[i].id_ui;
    attrib_p = NULL;
    return_value = UCMCMDM_GetNodeInfoByPathIDs (&ArrayOfUints, &attrib_p);
    if (return_value == OF_SUCCESS)
    {
      // igwCMDM_PrintAttribs(attrib_p);
      if (igwCMDM_TestCompareAttribs (attrib_p, &(VsgChildNodes[i])) !=
          OF_SUCCESS)
      {
        printf (" Failed to extract info about node : %s\n", TestPath);
        *pFailCount = *pFailCount + 1;
      }
      of_free (attrib_p);
    }
    else
    {
      *pFailCount = *pFailCount + 1;
    }
  }
  of_free (ArrayOfUints.uint_arr_p);

  printf ("... OK\n");
  return OF_SUCCESS;
}

/**************************************************************************
 Function Name :
 Input Args    :
 Output Args   :
 Description   :
 Return Values :
 *************************************************************************/
int32_t igwCMDM_TestGetNodeAndChildInfo (uint32_t * pTestCount,
                                         uint32_t * pFailCount)
{
  uint32_t i;
  int32_t return_value;
  struct cm_dm_node_info *attrib_p;
  uint32_t RootID = CM_DM_ROOT_NODE_ID;
  struct cm_array_of_structs *child_attrib_arr_p;
  struct cm_array_of_uints PathIDs;

  of_memset (TestPath, 0, CM_DM_MAX_PATH_LEN + 1);

  *pTestCount = *pTestCount + 1;
  printf ("Test Extracting Node&Child Info By Name...");
  return_value = cm_dm_get_node_and_child_info (CM_DM_TEST_ROOT_PATH,
                                    &attrib_p, &child_attrib_arr_p);
  if (return_value == OF_SUCCESS)
  {
    if ((!attrib_p) || (!child_attrib_arr_p))
    {
      if (attrib_p)
      {
        of_free (attrib_p);
      }
      if (child_attrib_arr_p)
      {
        of_free (child_attrib_arr_p);
      }
      *pFailCount = *pFailCount + 1;
      return OF_FAILURE;
    }

    // printf("Parent Info :\n");
    // igwCMDM_PrintAttribs(attrib_p);
    of_free (attrib_p);

    // printf("Children Info : \n");
    attrib_p = (struct cm_dm_node_info *) child_attrib_arr_p->struct_arr_p;
    for (i = 0; i < child_attrib_arr_p->count_ui; i++)
    {
      // igwCMDM_PrintAttribs((struct cm_dm_node_info*)(child_attrib_arr_p->struct_arr_p[i]));
      if (igwCMDM_TestCompareAttribs (&(attrib_p[i]),
                                      &(VsgChildNodes[i])) != OF_SUCCESS)
      {

        of_strncpy (TestPath, CM_DM_TEST_ROOT_PATH, CM_DM_MAX_PATH_LEN);
        of_strcat (TestPath, VsgChildNodes[i].name_p);
        printf (" Failed to retrieve info about node : %s\n", TestPath);
        *pFailCount = *pFailCount + 1;
      }
    }
  }

  printf ("\n");

  cm_dm_cleanup_array_of_node_attribs (child_attrib_arr_p);

  *pTestCount = *pTestCount + 1;
  printf ("Test Extracting Complete Node Info using Path IDs...");
  PathIDs.count_ui = 1;
  PathIDs.uint_arr_p = &RootID;
  return_value = UCMCMDM_GetNodeAndChildInfoByPathIDs (&PathIDs,
                                               &attrib_p, &child_attrib_arr_p);
  if (return_value == OF_SUCCESS)
  {
    if ((!attrib_p) || (!child_attrib_arr_p))
    {
      if (attrib_p)
      {
        of_free (attrib_p);
      }
      if (child_attrib_arr_p)
      {
        of_free (child_attrib_arr_p);
      }
      *pFailCount = *pFailCount + 1;
      return OF_FAILURE;
    }

    printf ("Parent Info :\n");
    igwCMDM_PrintAttribs (attrib_p);
    of_free (attrib_p);

    printf ("Children Info : \n");
    attrib_p = (struct cm_dm_node_info *) child_attrib_arr_p->struct_arr_p;
    for (i = 0; i < child_attrib_arr_p->count_ui; i++)
    {
      igwCMDM_PrintAttribs (&attrib_p[i]);
      if (igwCMDM_TestCompareAttribs (&(attrib_p[i]),
                                      &(VsgChildNodes[i])) != OF_SUCCESS)
      {
        printf (" Failed to retrieve info about nodeID : %d\n", attrib_p->id_ui);
        *pFailCount = *pFailCount + 1;
      }
    }
  }

  cm_dm_cleanup_array_of_node_attribs (child_attrib_arr_p);
  printf ("... OK\n");
  return OF_SUCCESS;
}
#endif /* if 0 */

#endif /* CM_DM_TEST_SUPPORT */
#endif /* CM_CMDM_SUPPORT */
