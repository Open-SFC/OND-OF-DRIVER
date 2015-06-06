<?xml version="1.0"?>
<!-- 
/* 
 *
 * Copyright  2012, 2013  Freescale Semiconductor
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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
 * File name: gen_id.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This XSL stylesheet adds IDs to each NodeID element in XML file.
 * 
 */
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>
 <!-- This global lastid variable holds the current largest ID value in the 
 xml file (if it has IDs already), else it holds "0",
 the starting value of IDs 
 -->
  <xsl:variable name="lastid">
 		<xsl:call-template name="biggest" />
  </xsl:variable>
 
<!-- 
 /*************************************************************************
 *  This main template loops through all nodes and if they have an ID 
 *  already, it skips them. If they dont have an ID, it calls the 
 *  generate-id template which assigns the new ID to the node.                                    
 *************************************************************************/ 
 -->
  
 <xsl:template match="*">
		<xsl:copy>
			<!--<xsl:copy-of select="@*"/>-->			
			<xsl:apply-templates select="@*|*|text()"/>
		</xsl:copy>
 </xsl:template>

<!-- 
 /*************************************************************************
 *  This template will find the NodeID element in xml and fills it with 
 *  generated ID. 	                                 
 *************************************************************************/ 
 -->
 	
 <xsl:template match="NodeID">
	  <NodeID> 
			<xsl:call-template name="generate-id"/>
	  </NodeID>  
 </xsl:template>
	
 <xsl:template match="@*|text()">
		<xsl:copy/>
 </xsl:template>
 
<!-- 
 /*************************************************************************
 *  This template generates a unique ID based on the largestID(lastid) + 
 *  preceding closed nodes without IDs + ancestors (which are not closed) 
 *  without IDs                                    
 *************************************************************************/ 
-->
 <xsl:template name="generate-id">
  <xsl:variable name="last" select="number($lastid)"/>
  <xsl:variable name="count-closed-nodes" select="count(preceding::NodeID)+1"/>
  <xsl:variable name="count-open-ancestors" select="count(ancestor::NodeID)"/>
  <xsl:variable name="temp" select="$last + $count-closed-nodes + $count-open-ancestors"/>
  <xsl:value-of select="$temp"/>
 </xsl:template>

<!-- 
 /*************************************************************************
 *  This template finds the biggest ID currently in the document by sorting 
 *  the list of IDs and returning the last/largest one.
 *  If there are no IDs in the document, it assumes 0 as the current 
 *  largest ID.
 *************************************************************************/ 
--> 
 <xsl:template name="biggest">
 <xsl:choose>
   <xsl:when test="string-length(/DataModel/DataModelNode/NodeID) &gt; 0">
     <xsl:for-each select="/DataModel/DataModelNode">
       <xsl:sort select="NodeID" />
       <xsl:if test="position()=last()">
         <xsl:value-of select="NodeID"/>
       </xsl:if>
     </xsl:for-each>
   </xsl:when>
   <xsl:when test="document('../xml/id.xml')/root/nodeId">
   	   <xsl:value-of select="document('../xml/id.xml')/root/nodeId"/>
   </xsl:when>
   <xsl:otherwise>0</xsl:otherwise>  
 </xsl:choose>  
 </xsl:template>
 
 </xsl:stylesheet>
