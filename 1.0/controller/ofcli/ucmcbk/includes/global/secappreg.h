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
 * File name: secappreg.h
 * Author: G Atmaram <B38856@freescale.com>
 * Date:   03/25/2013
 * Description: 
 * 
 */

#ifndef _SECAPPL_H_
#define _SECAPPL_H_

#define CM_CONFIG_DIR "/ucmconfig"
//#define CM_FLOW_STATS "/flowstats"

/* Save file names  */
#define OFSDN_DOMAIN_TABLE "domainTable"
#define OFSDN_PORT "port"
#define OFSDN_DPATTRIBUTE "dpattribute"
#define OFSDN_SWATTRIBUTE "switchattribute"
#define OFSDN_DOMAINATTRIBUTE "domainattribute"
#define OFSDN_PORTATTRIBUTE "portattribute"
#define OFSDN_CHANNEL "channel"
#define OFSDN_TABLE_FEATURES "tablefeatures"
#define OFSDN_FLOWMOD "flowmod"
#define OFSDN_ACTION "action"
#define OFSDN_SWITCH "switch"
#define OFSDN_NAMESPACE "namespace"
#define OFSDN_DOMAIN "domain"
#define OFSDN_DATAPATH "datapath"
#define OFSDN_SSL "ssl"
#define OFSDN_DATAPATH_GROUPS "groups"
//#define OFSDN_DATAPATH_IPOPTIONS "ipoptions"
#define OFSDN_DATAPATH_GROUP_BUCKET "groupbucket"
#define OFSDN_DATAPATH_GROUP_FEATURES "groupfeatures"
#define OFSDN_DATAPATH_METER  "meter"
#define OFSDN_DATAPATH_METER_BANDS "meterband"
#define OFSDN_DATAPATH_METER_FEATURES "meterfeatures"
#define OFSDN_DATAPATH_FLOWMOD_MATCHFIELD "matchfield"
#define OFSDN_DATAPATH_FLOWMOD_INSTRUCTION "instruction"
#define OFSDN_DATAPATH_FLOWMOD_ACTIONSET "actionset"

#define OFSDN_CRM    "crm"
#define OFSDN_CRM_VN "crmvn"
#define OFSDN_CRM_COMPUTENODES "crmcomputenodes"
#define OFSDN_CRM_VMPORTS      "crmvmports"
#define OFSDN_CRM_VM "crmvm"
#define OFSDN_CRM_TENANT "crmtenant"
#define OFSDN_CRM_SUBNET "crmsubnet"
#define OFSDN_CRM_NWPORTS "crmnwports"
#define OFSDN_VN_ATTRIBUTE "crmvnattribute"
#define OFSDN_VM_ATTRIBUTE "crmvmattribute"
#define OFSDN_SUBNET_ATTRIBUTE "crmsubnetattribute"
#define OFSDN_TENANT_ATTRIBUTE "crmtenantattribute" 

#define OFSDN_PROTOCOL "protocol"

 
#define OFSDN_NSRM    "nsrm"
#define OFSDN_NSRM_NSCHAIN  "chain"
#define OFSDN_NSRM_CHAIN "nsrmchain"
#define OFSDN_NSRM_CHAINMAP  "chainmap" 
#define OFSDN_NSRM_NWFUNCTION  "nwfunction"
#define OFSDN_NSRM_APPLIANCE   "appliance"
#define OFSDN_NSRM_CHAINSET  "chainset" 
#define OFSDN_NSRM_L2NETWORKMAP  "l2nwmap"
#define OFSDN_NSRM_L3NETWORKMAP  "l3nwmap"
#define OFSDN_NSRM_SERVICE "servicemap"
#define OFSDN_NSRM_CHAIN_SERVICES "chainsrv"
#define OFSDN_NSRM_SERVICE_INSTANCE "srvinstance"
#define OFSDN_NSRM_BYPASSRULE "bypass"
#define OFSDN_NSRM_BYPASS_SERVICES "bypass_services"
#define OFSDN_NSRM_CHAIN_ATTRIBUTE "chainattr"
#define OFSDN_NSRM_CHAINSELRULE "chainrule"
#define OFSDN_NSRM_CHAINSET_ATTRIBUTE "chainsetattr"
#define OFSDN_NSRM_CHAIN_SELECTION_ATTRIBUTE "chainselattr"
#define OFSDN_NSRM_SERVICE_ATTRIBUTE "srvattr"
#define OFSDN_NSRM_L2NETWORKMAP_ATTRIBUTE "l2nwmapattr"
#define OFSDN_NSRM_BYPASSRULE_SERVICENAMES "bypassrule"
#define OFSDN_NSRM_BYPASSRULE_ATTRIBUTE "bypassruleattr"

#define OFSDN_NSRM_CATEGORY    "category"
#define OFSDN_NSRM_VENDORS     "vendor"
#define OFSDN_NSRM_ATTRIBUTE    "nsrmattribute"
#define OFSDN_NSRM_CATEGORY_ATTRIBUTE  "categoryattribute"
#define OFSDN_NSRM_VENDOR_ATTRIBUTE    "vendorattribute"
#define OFSDN_NSRM_NSCHAIN_RULE        "rule"
#define OFSDN_NSRM_NSCHAIN_ATTRIBUTE   "chainattribute"
#define OFSDN_NSRM_APPLIANCE_NETWORK    "network"
#define OFSDN_NSRM_APPLIANCE_ATTRIBUTE  "applianceattribute"
#define OFSDN_TSC   "tsc"
#define OFSDN_TSC_SELECTORS   "tsc_selectors"

/************************** FUNCTION PROTOTYPES *************************************/

#endif
