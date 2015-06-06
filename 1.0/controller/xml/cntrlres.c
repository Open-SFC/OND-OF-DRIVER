/** 
 ** Copyright (c) 2012, 2013  Freescale.
 *   
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

 /* File: cntrlres.c
 *  
 *  Description:  loading controller resource configuration xml file
 *  
 * Authors:  
 * History
 * 
 */

#ifdef OF_XML_SUPPORT

#include "controller.h"
#include "cntrlxml.h"
#include "dprm.h"
#include "cntlr_transprt.h"
//#include "ipsec_pkt_mbuf.h"


#define CNTRL_MAX_DIR_PATH_LEN    64
  struct dprm_resource_runtime_info dprm_resource_g;
  struct transport_runtime_info transport_info_g;
  struct timer_resource_runtime_info timer_info_g;
  struct ipsec_resource_runtime_info ipsec_info_g;

int32_t execute_cntrl_resource_conf_extract(void *pDOMDoc)
{

  unsigned char OutBuf[CNTRL_MAX_DIR_PATH_LEN+1],pParamName[128];
  void *pRootNode=NULL,*pDatapathNode=NULL,*pTempNode=NULL,*pChildNode=NULL;
  int32_t iErrCode,i,n;
  uint32_t ulCoreId;

  memset(OutBuf,0, CNTRL_MAX_DIR_PATH_LEN+1);

  CNTRLXMLGetRootNode (pDOMDoc, &pRootNode);
  if(!pRootNode)
  {
    printf("%s:  IGWXMLGetRootNode failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

 /*   <DPRMConfig>

        <NoofDatapaths>20</NoofDatapaths>
        <NoofPhysicalSwitches>10</NoofPhysicalSwitches>
        <NoofNamespaces>32</NoofNamespaces>
  </DPRMConfig>

*/
 CNTRLXMLGetFirstChildByTagName((unsigned char *)"DPRMConfig",
                                pRootNode,
                                &pDatapathNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

  /* NoofDatapaths*/
  strcpy((char *)pParamName,"NoofDatapaths");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  dprm_resource_g.number_of_datapaths = atoi((char *)OutBuf);
  printf("%s: NoofDatapaths = %d\n", __FUNCTION__, dprm_resource_g.number_of_datapaths);

  /* NoofPhysicalSwitches */
  strcpy((char *)pParamName,"NoofPhysicalSwitches");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  dprm_resource_g.number_of_physical_switches = atoi((char *)OutBuf);
  printf("%s: NoofPhysicalSwitches = %d\n", __FUNCTION__, dprm_resource_g.number_of_physical_switches);
  
  /*NoofNamespaces */
  strcpy((char *)pParamName,"NoofNamespaces");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  dprm_resource_g.number_of_namespaces = atoi((char *)OutBuf);
  printf("%s: NoofNamespaces= %d\n", __FUNCTION__, dprm_resource_g.number_of_namespaces);

  /*   <Transport>
        <TransportStartingCoreID>0</TransportStartingCoreID>
        <TransportNoofCores>4</TransportNoofCores>
        <NoofTransportThreads>8</NoofTransportThreads>
        <PEthStartingCoreID>4</PEthStartingCoreID>
        <PEthNoofCores>4</PEthNoofCores>
        <NoofPEthThreads>4</NoofPEthThreads>
  </Transport>

*/

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"Transport",
                                pRootNode,
                                &pDatapathNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

  /*NOOF_MSG_POOLS */
  strcpy((char *)pParamName,"TransportStartingCoreID");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.transport_starting_core_id = atoi((char *)OutBuf);
  printf("%s: TransportStartingCoreID = %d\n", __FUNCTION__, transport_info_g.transport_starting_core_id);

  /*TransportNoofCores*/
  strcpy((char *)pParamName,"TransportNoofCores");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.transport_noof_cores = atoi((char *)OutBuf);
  printf("%s: TransportNoofCores = %d\n", __FUNCTION__, transport_info_g.transport_noof_cores);

  /*NoofTransportThreads*/
  strcpy((char *)pParamName,"NoofTransportThreads");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.number_of_transport_threads = atoi((char *)OutBuf);
  printf("%s:NoofTransportThreads = %d\n", __FUNCTION__, transport_info_g.number_of_transport_threads);

  /*PEthStartingCoreID*/
  strcpy((char *)pParamName,"PEthStartingCoreID");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.peth_starting_core_id = atoi((char *)OutBuf);
  printf("%s:NoofTransportThreads = %d\n", __FUNCTION__, transport_info_g.peth_starting_core_id);

  /*PEthNoofCores*/
  strcpy((char *)pParamName,"PEthNoofCores");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.peth_number_of_cores = atoi((char *)OutBuf);
  printf("%s:PEthNoofCores = %d\n", __FUNCTION__, transport_info_g.peth_number_of_cores);

  /*NoofPEthThreads*/
  strcpy((char *)pParamName,"NoofPEthThreads");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  transport_info_g.number_of_peth_threads = atoi((char *)OutBuf);
  printf("%s:NoofPEthThreads = %d\n", __FUNCTION__, transport_info_g.number_of_peth_threads);


  /* TimerConfig  
    <TimerConfig>
        <NoofContainers>16</NoofContainers>
        <Granularity>10000</Granularity>
  </TimerConfig> */

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"TimerConfig",
                                pRootNode,
                                &pDatapathNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

  /* NoofContainers */
  strcpy((char *)pParamName,"NoofContainers");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  timer_info_g.number_of_containers = atoi((char *)OutBuf);
  printf("%s:NoofContainers = %d\n", __FUNCTION__, timer_info_g.number_of_containers);

  /* Granularity */
  strcpy((char *)pParamName,"Granularity");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  timer_info_g.granularity = atoi((char *)OutBuf);
  printf("%s:Granularity = %d\n", __FUNCTION__, timer_info_g.granularity);

  /*   <IPsec>
        <MaxPolicyNodes>512</MaxPolicyNodes>
        <MaxSANodes>2048</MaxSANodes>
        <GenidMaxInstaceidValue>2048</GenidMaxInstaceidValue>
   </IPsec>

 */

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"IPsec",
                                pRootNode,
                                &pDatapathNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

  /* MaxPolicyNodes */
  strcpy((char *)pParamName,"MaxPolicyNodes");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  ipsec_info_g.max_policy_nodes = atoi((char *)OutBuf);
  printf("%s:MaxPolicyNodes = %d\n", __FUNCTION__, ipsec_info_g.max_policy_nodes);

  /* MaxSANodes */
  strcpy((char *)pParamName,"MaxSANodes");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  ipsec_info_g.max_sa_nodes = atoi((char *)OutBuf);
  printf("%s:MaxSANodes = %d\n", __FUNCTION__, ipsec_info_g.max_sa_nodes);

  /* GenidMaxInstaceidValue */
  strcpy((char *)pParamName,"GenidMaxInstaceidValue");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pDatapathNode,
                                    OutBuf,
                                    64);

  ipsec_info_g.genid_max_instaceid_value = atoi((char *)OutBuf);
  printf("%s:GenidMaxInstaceidValue = %d\n", __FUNCTION__, ipsec_info_g.genid_max_instaceid_value);

  /*  <MempoolConfig>
        <NoofPktMbufPools>7</NoofPktMbufPools>
     <PktMbufpool>
             <PoolID>0</PoolID>
                <PacketSize>128</PacketSize>
             <NoofBuffers>500</NoofBuffers>
        </PktMbufpool>
        <PktMbufpool>
             <PoolID>1</PoolID>
                <PacketSize>256</PacketSize>
             <NoofBuffers>500</NoofBuffers>
        </PktMbufpool>
        <PktMbufpool>
             <PoolID>6</PoolID>
                <PacketSize>4096</PacketSize>
             <NoofBuffers>500</NoofBuffers>
        </PktMbufpool>
        <NoofOFMessagePools>33</NoofOFMessagePools>
        <NoofGroups>3</NoofGroups>
        <GroupInfo>
                <GroupId>0</GroupId>
                <Granularity>128</Granularity>
        </GroupInfo>
        <GroupInfo>
                <GroupId>1</GroupId>
                <Granularity>512</Granularity>
        </GroupInfo>
        <GroupInfo>
                <GroupId>2</GroupId>
                <Granularity>2048</Granularity>
        </GroupInfo>
   </MempoolConfig>

 */

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"MempoolConfig",
                                pRootNode,
                                &pDatapathNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }

  CNTRLXMLGetFirstChildByTagName((unsigned char *)"PktMbufpool",
                                pDatapathNode,
                                &pChildNode);
  if(!pDatapathNode)
  {
    printf("%s: CNTRLXMLGetFirstChildByTagName  failed \r\n",__FUNCTION__);
    return OF_FAILURE;
  }
 do {
  /* PoolID */
  strcpy((char *)pParamName,"PoolID");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pChildNode,
                                    OutBuf,
                                    64);

  uiCntrlMaxExpVendors_g = atoi((char *)OutBuf);
  printf("%s:PoolID = %d\n", __FUNCTION__, uiCntrlMaxExpVendors_g);

  /* PacketSize*/
  strcpy((char *)pParamName,"PacketSize");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pChildNode,
                                    OutBuf,
                                    64);

  uiCntrlMaxDatapaths_g = atoi((char *)OutBuf);
  printf("%s:PacketSize = %d\n", __FUNCTION__, uiCntrlMaxDatapaths_g);

  /* NoofBuffers */
  strcpy((char *)pParamName,"NoofBuffers");
  CNTRLXMLGetFirstChildValueByTagName((char *)pParamName,
                                    pChildNode,
                                    OutBuf,
                                    64);

  uiCntrlMaxTables_g = atoi((char *)OutBuf);
  printf("%s:NoofBuffers = %d\n", __FUNCTION__, uiCntrlMaxTables_g);
  CNTRLXMLGetNextSiblingByTagName((unsigned char*)"PktMbufpool", pChildNode, &pChildNode);
  } while (pChildNode);

  return OF_SUCCESS;
}

#endif
