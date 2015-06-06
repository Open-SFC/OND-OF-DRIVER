<?xml version="1.0" encoding="UTF-8"?>
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
 * File name: gen_clidocs.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: Template loops through all nodes and generates the output in html
 *              format. Generates the Help documents.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:template match="/">

   <html>
    <body>
      <head>
        <link rel="stylesheet" type="text/css" href="../help.css" />
        <title>
         CLI Commands - User Guide
        </title>
      </head>
      <h3>CLI Commands Index:</h3>
      <xsl:call-template name="gen_index" />
      <br/>
	<xsl:for-each select="/root/ModuleNavModel/Module">
	<xsl:variable name="navfile">	
		<xsl:value-of select="@name"/>
	</xsl:variable>	
	<xsl:variable name="navFile">
		<xsl:value-of select="concat('../xml/', $navfile)"/>
        </xsl:variable>    
	<xsl:variable name="navData" select="document($navFile)/root"/>
	<xsl:for-each select="$navData//node">
	  <xsl:variable name="menuNamelvl1">
		<xsl:value-of select="@label"/>
	  </xsl:variable>
     <xsl:for-each select=".">
		   <xsl:variable name="dmfile">
			     <xsl:value-of select="@file" />
		   </xsl:variable>
                        
                   <xsl:variable name="pDmfile">
                     <xsl:choose> 
                       <xsl:when test="parent::node()[name()='node']">
			     <xsl:value-of select="parent::node()/@file" />
                       </xsl:when>
                       <xsl:otherwise>
                         <xsl:value-of select="@file" />
                       </xsl:otherwise>
                     </xsl:choose>
		   </xsl:variable>

		   <xsl:variable name="mtname">
			     <xsl:value-of select="@name" />
		   </xsl:variable>	
	  <!--<xsl:if test="$smfile!=''">-->
	  <xsl:if test="$dmfile!=''">
	    <xsl:call-template name="genHelpDocs" >
              <xsl:with-param name="dmFile" select="$dmfile"/>
              <!-- xsl:with-param name="smFile" select="$smfile"/-->
              <xsl:with-param name="mtName" select="$mtname"/>
              <xsl:with-param name="menuName" select="$menuNamelvl1"/>
              <xsl:with-param name="parDmfile" select="$pDmfile"/>
            </xsl:call-template>	
          </xsl:if>
      <!--</xsl:if>-->
      <xsl:if test="./node">
         <xsl:for-each select="./node">
            <xsl:apply-templates select="."/>
         </xsl:for-each>
       </xsl:if>
	   </xsl:for-each>
	</xsl:for-each>
	<br/> 
  </xsl:for-each>
     </body>
    </html>
  </xsl:template>
  
  <xsl:template match="node">
    <xsl:param name="menuName" select="'Not Available'" />
    <xsl:param name="indx" select="'Not Available'" />
    <xsl:choose>
    <xsl:when test="$indx='yes'">
      <xsl:text disable-output-escaping="yes">&#x20;&lt;a href="#</xsl:text>
	    <!-- <xsl:value-of select="@name"/>-->
	    <xsl:value-of select="translate($menuName, ' ', '_')" />
      <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <xsl:value-of select="$menuName"/>
	  <xsl:text disable-output-escaping="yes">&lt;/a&gt;&#xa;</xsl:text>
	  <br/>
    </xsl:when>
    <xsl:otherwise>
    <xsl:for-each select=".">
      <xsl:variable name="dmfile">
        <xsl:value-of select="@file" />
      </xsl:variable>
    
      <xsl:variable name="pDmfile">
        <xsl:choose>
        <xsl:when test="parent::node()[name()='node']">
	<xsl:value-of select="parent::node()/@file" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@file" />
        </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <xsl:variable name="mtname">
        <xsl:value-of select="@name" />
      </xsl:variable>	
      <!--xsl:if test="$smfile!=''"-->
      <xsl:if test="$dmfile!=''">
        <xsl:call-template name="genHelpDocs" >
          <xsl:with-param name="dmFile" select="$dmfile"/>
          <!-- xsl:with-param name="smFile" select="$smfile"/-->
          <xsl:with-param name="mtName" select="$mtname"/>
          <xsl:with-param name="menuName" select="@label"/>
          <xsl:with-param name="parDmfile" select="$pDmfile"/>
        </xsl:call-template>
      </xsl:if>
      <!--/xsl:if-->
       <xsl:if test="./node">
         <xsl:for-each select="./node">
            <xsl:apply-templates select=".">
              <xsl:with-param name="menuName" select="@label" />
              <xsl:with-param name="indx" select="'no'" />
            </xsl:apply-templates>
         </xsl:for-each>
       </xsl:if>
	   </xsl:for-each>
	  </xsl:otherwise>
	  </xsl:choose>
	</xsl:template>
<!-- 
 /*************************************************************************
 *  This template loops through all nodes and generates the index in html
 *  format.
 *************************************************************************/ 
--> 
<xsl:template name="gen_index">
  <xsl:for-each select="/root/ModuleNavModel/Module">
	<xsl:variable name="navfile">	
		<xsl:value-of select="@name"/>
	</xsl:variable>	

	<xsl:variable name="navFile">
		<xsl:value-of select="concat('../xml/', $navfile)"/>
    </xsl:variable>
    
	<xsl:variable name="navData" select="document($navFile)/root"/>
	<xsl:for-each select="$navData//node">
	  <xsl:variable name="menuNamelvl1">
		<xsl:value-of select="@label"/>
	  </xsl:variable>
	  <xsl:text disable-output-escaping="yes">&#x20;&lt;a href="#</xsl:text>
	    <!-- <xsl:value-of select="@name"/>-->
	    <xsl:value-of select="translate($menuNamelvl1, ' ', '_')" />
      <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <xsl:value-of select="$menuNamelvl1"/>
	  <xsl:text disable-output-escaping="yes">&lt;/a&gt;&#xa;</xsl:text>
	  <br/>
	  <!--xsl:if test="./node">
         <xsl:for-each select="./node">
           <xsl:variable name="indx"><xsl:text>yes</xsl:text></xsl:variable>
            <xsl:apply-templates  select=".">
              <xsl:with-param name="menuName" select="@label"/>
              <xsl:with-param name="indx" select="$indx"/>
            </xsl:apply-templates>
         </xsl:for-each>
       </xsl:if-->
	</xsl:for-each>
  </xsl:for-each>
</xsl:template>
	
  <xsl:template name="genHelpDocs">
    <xsl:param name="dmFile" select="'Not Available'" />
    <!--xsl:param name="smFile" select="'Not Available'" /-->
    <xsl:param name="mtName" select="'Not Available'" />
    <xsl:param name="menuName" select="'Not Available'" />
    <xsl:param name="parDmfile" select="'Not Available'" />

    <xsl:variable name="file">
		<xsl:value-of select="concat('../../output/xml/dm/', $dmFile)"/>
    </xsl:variable><!--
    <xsl:variable name="sFile">
		<xsl:value-of select="concat('../xml/', $smFile)"/>
    </xsl:variable>-->
    <xsl:variable name="pdmfile">
		<xsl:value-of select="concat('../../output/xml/dm/', $parDmfile)"/>
    </xsl:variable>
    <xsl:variable name="pdmDoc" select="document($pdmfile)/DataModel" />
    <xsl:variable name="dmDoc" select="document($file)/DataModel" />
    <!--<xsl:variable name="smDoc" select="document($sFile)/UIModel" />-->
    <xsl:variable name="hlpDoc" select="document('../hlp/helptmpl.xml')/help" />
      
    <xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'" />
    <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />
     <b><xsl:text disable-output-escaping="yes">&#x20;&lt;span id="</xsl:text>
     <xsl:value-of select="translate(@label, ' ', '_')" />
     <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
     <xsl:value-of select="@label" />
     <xsl:text disable-output-escaping="yes">&lt;/span&gt;</xsl:text></b>
     <table border="0" cellspacing="0" cellpadding="0" class="pagetitle" width="100%">
      <tr class="toolbar">
      <td width="85%">
         <span id="appln" class="titlefont">
          <xsl:text>&#160;&#160;&#160;&#160;Module Name: </xsl:text><xsl:value-of select="$menuName"/><br/>
          <xsl:text>&#160;&#160;&#160;&#160;Data Model Path: </xsl:text>
          <I>
            <xsl:value-of select="$dmDoc/InstallPath" />
          </I>
         </span><br/>&#160;&#160;&#160;&#160;
       </td>
     </tr>
  </table>
  <hr/>
  <!-- Enable for debug
  <xsl:text>menuName= </xsl:text><xsl:value-of select="$menuName"/> -->
  <xsl:if test="$hlpDoc/topic[@title=$menuName]">
    <b>Description:</b><br/>
    <xsl:for-each select="$hlpDoc/topic[@title=$menuName]/Description/l" >
    <xsl:value-of select="."/><br/>
    </xsl:for-each>
    <br/>
  </xsl:if> 
  
  <!-- CLI Usage Start -->
   <b>
     <xsl:text><u>CLI Command Usage:</u> </xsl:text></b><br/> 
       <b><xsl:text>Step 1: </xsl:text></b>      
       <font color="blue">
       <xsl:value-of select="translate($dmDoc/@ModuleName,$uppercase,$smallcase)" /><xsl:text> </xsl:text>
         <xsl:for-each select="$dmDoc/DataModelNode">
          <xsl:variable name="key">
             <xsl:value-of select="NodeAttributes/IsKey" />
           </xsl:variable>
             <xsl:if test="$key = 'TRUE'">
               <xsl:text> &lt;</xsl:text><xsl:value-of select="Name" /><xsl:text>&gt; </xsl:text>
             </xsl:if>
        </xsl:for-each>
        </font><!--b><xsl:text> press Enter</xsl:text></b>
       <br/><b><xsl:text>Step 2 for Add/Edit: </xsl:text></b-->
     <font color="blue"> 

     <!--xsl:text> set </xsl:text-->
     <xsl:for-each select="$dmDoc/DataModelNode[NodeAttributes/NodeType='CM_DMNODE_TYPE_SCALAR_VAR']">         
          <xsl:variable name="mandatory">
             <xsl:value-of select="NodeAttributes/Mandatory" />
           </xsl:variable>
          <xsl:variable name="iskey">
             <xsl:value-of select="NodeAttributes/IsKey" />
           </xsl:variable> 
        <xsl:if test="$iskey = 'FALSE'">             
           <xsl:choose>
             <xsl:when test="$mandatory = 'TRUE'">
               <xsl:value-of select="Name" />
             </xsl:when>
             <xsl:otherwise>
               <xsl:text>[</xsl:text><xsl:value-of select="Name" /><xsl:text>]</xsl:text>
             </xsl:otherwise>
           </xsl:choose>
          <i><xsl:text> &lt;value&gt; </xsl:text></i>
       </xsl:if>
     </xsl:for-each></font>
     <br/>     
     <!--xsl:value-of select="$parDmfile"/>
     <xsl:value-of select="translate($mtName,$uppercase,$smallcase)"/--> 
     <xsl:variable name="installPathName" select="translate($mtName,$uppercase,$smallcase)"/>
       
    <xsl:choose>
     <xsl:when test="$pdmDoc/DataModelNode[InstallPath=$installPathName]/NodeAttributes[UseParentTrans='TRUE']">
     	<b><xsl:text>Step 2: </xsl:text></b><xsl:text><font color="blue">done </font> <b>press Enter</b></xsl:text><br/><br/>
     </xsl:when>
     <xsl:otherwise>
     	<b><xsl:text>Step 2: </xsl:text></b><xsl:text><font color="blue">commit </font> <b>press Enter</b></xsl:text><br/><br/>
     </xsl:otherwise>
    </xsl:choose>
  <!-- CLI Usage End -->
  
	  <table border="1" cellspacing="0" cellpadding="4" class="pagetitle" width="100%">
        <tr class="subhead">
          <th>Name</th>
          <th>Command Identifier</th>
          <th>Node Type</th>
          <th>Data Type</th>
          <th>Mandatory</th>
          <th>Permission</th>
          <th>Details</th>
          <th>Default Value</th>
        </tr>
	    <xsl:for-each select="$dmDoc/DataModelNode">
	      <xsl:variable name="nodeType" select="NodeAttributes/NodeType"/>
          <xsl:if test="$nodeType='CM_DMNODE_TYPE_SCALAR_VAR'">
        <tr>
         <td>
           <xsl:variable name="nodeName">
             <xsl:value-of select="Name" />
           </xsl:variable>
           <xsl:variable name="instllPath" select="InstallPath" />
           <!--<xsl:variable name="nodeLbl">
             <xsl:value-of select="$smDoc//DMNode[@path=$instllPath]/@label|$smDoc//Component[@path=$instllPath]/@label"/>
           </xsl:variable>
           <xsl:choose>
             <xsl:when test="$nodeLbl!=''">
               <xsl:value-of select="$nodeLbl" />
             </xsl:when>
             <xsl:otherwise>
               <xsl:value-of select="$nodeName" />
             </xsl:otherwise>
           </xsl:choose>-->
           <xsl:value-of select="$nodeName" />
         </td>
         <td>
             <xsl:value-of select="NodeAttributes/CliIdentifier" />
         </td>
         <td>
           <xsl:call-template name="getUCMNodeType" >
             <xsl:with-param name="type" select="NodeAttributes/NodeType"/>
           </xsl:call-template>
         </td>
         <td>
           <xsl:call-template name="getUCMDataType" >
             <xsl:with-param name="type" select="DataAttributes/DataType"/>
           </xsl:call-template>
         </td>
         <td>
           <xsl:variable name="mandatory">
             <xsl:value-of select="NodeAttributes/Mandatory" />
           </xsl:variable>
           <xsl:choose>
             <xsl:when test="$mandatory = 'TRUE'">
               <xsl:text>yes</xsl:text>
             </xsl:when>
             <xsl:otherwise>
               <xsl:text>No</xsl:text>
             </xsl:otherwise>
           </xsl:choose>
         </td>
         <td>
           <xsl:variable name="permiss">
             <xsl:value-of select="NodeAttributes/NonConfigLeafNode" />
           </xsl:variable>
           <xsl:choose>
             <xsl:when test="$permiss = 'FALSE'">
               <xsl:text>Read, Write</xsl:text>
             </xsl:when>
             <xsl:otherwise>
               <xsl:text>Read</xsl:text>
             </xsl:otherwise>
           </xsl:choose>
         </td>
         <td>
           <xsl:if test="(Description != '')">
             <b>
             <xsl:text>Description: </xsl:text>
             </b>
             <xsl:value-of select="Description" />
             <br/>
           </xsl:if>
           <xsl:if test="(DataAttributes/DataAttrib/StartValue != '') or (DataAttributes/DataAttrib/EndValue != '') or (DataAttributes/DataAttrib/MinLength != '') or (DataAttributes/DataAttrib/MaxLength != '')">
             <b>
             <xsl:text>Range: </xsl:text>
             </b>            
             <xsl:if test="(DataAttributes/DataAttrib/StartValue != '') or (DataAttributes/DataAttrib/EndValue != '')">
               <xsl:value-of select="DataAttributes/DataAttrib/StartValue" />
               <xsl:text> - </xsl:text>
               <xsl:value-of select="DataAttributes/DataAttrib/EndValue" />
             </xsl:if>
             <xsl:if test="(DataAttributes/DataAttrib/MinLength != '') or (DataAttributes/DataAttrib/MaxLength != '')">
               <xsl:value-of select="DataAttributes/DataAttrib/MinLength" />
               <xsl:text> - </xsl:text>
               <xsl:value-of select="DataAttributes/DataAttrib/MaxLength" />
             </xsl:if>
             <br/>
           </xsl:if>
           <xsl:if test="(DataAttributes/DataAttrib/StrEnum/Value != '') or (DataAttributes/DataAttrib/IntEnum/Value != '')">
            <b>
            <xsl:text>Enum: </xsl:text>
            </b>
            <br/>
	       <xsl:for-each select="DataAttributes/DataAttrib/StrEnum/Value">
                    <xsl:text> - </xsl:text>
                    <xsl:value-of select="." />
                    <br/>
	       </xsl:for-each>
	       <xsl:for-each select="DataAttributes/DataAttrib/IntEnum/Value">
                    <xsl:value-of select="." />
                    <br/>
	       </xsl:for-each>
           </xsl:if>
         </td>
         <td>
           <xsl:choose>
             <xsl:when test="DataAttributes/DefaultValue/DefaultString">
               <xsl:value-of select="DataAttributes/DefaultValue/DefaultString" />
             </xsl:when>
             <xsl:when test="DataAttributes/DefaultValue/DefaultIntValue">
               <xsl:value-of select="DataAttributes/DefaultValue/DefaultIntValue" />
             </xsl:when>
             <xsl:otherwise>
             <xsl:text> - </xsl:text>
             </xsl:otherwise>
           </xsl:choose>
         </td>
        </tr>
        </xsl:if>
	    </xsl:for-each>
      </table>
      <br/>
    <!--</body>
    </html>
  --></xsl:template>
  
  <xsl:template name="genCLIUsage">
    <xsl:param name="dmFile" select="'Not Available'" />
  </xsl:template>
  
  <xsl:template name="getUCMNodeType">
    <xsl:param name="type" select="'Not Available'" />
    <xsl:choose>
      <xsl:when test="$type='CM_DMNODE_TYPE_ANCHOR'">
        <xsl:text>Anchor</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DMNODE_TYPE_TABLE'">
        <xsl:text>Table</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DMNODE_TYPE_SCALAR_VAR'">
        <xsl:text>Scalar Variable</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
   
  <xsl:template name="getUCMDataType">
    <xsl:param name="type" select="'Not Available'" />
    <xsl:choose>
      <xsl:when test="$type='CM_DATA_TYPE_STRING'">
        <xsl:text>String</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_INT'">
        <xsl:text>Integer</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UINT'">
        <xsl:text>Unsigned Integer</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_INT64'">
        <xsl:text>Long Integer</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UINT64'">
        <xsl:text>Unsigned Long Integer</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_BOOLEAN'">
        <xsl:text>Boolean</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_DATETIME'">
        <xsl:text>Date and Time</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_BASE64'">
        <xsl:text>Base 64</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_IPADDR'">
        <xsl:text>IP Address</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UNKNOWN'">
        <xsl:text>&#x20;-&#x20;</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
  
</xsl:stylesheet>
