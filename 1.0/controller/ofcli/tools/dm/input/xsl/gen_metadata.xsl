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
 * File name: gen_metadata.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file will generate metadata files.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0"
  xmlns:java_func="com.fsl.utils.XMLUtils" extension-element-prefixes="java_func">
<xsl:include href="ucm_typs.xsl"/>
<xsl:output method="xml" indent="yes" />

<!-- 
 /*************************************************************************
 *  This template will generate oid's from DataModel xml file.
 *************************************************************************/ 
-->   
  <xsl:template match="/">
  <xsl:for-each select="/root/ModuleNavModel/Module">
  <xsl:variable name="navfile">  
    <xsl:value-of select="@name"/>
  </xsl:variable>  

  <xsl:variable name="navFile">
    <xsl:value-of select="concat('../xml/', $navfile)"/>
    </xsl:variable>
    
  <xsl:variable name="navData" select="document($navFile)/root"/>
  <xsl:for-each select="$navData/node">
    <xsl:variable name="menuNamelvl1">
    <xsl:value-of select="@label"/>
    </xsl:variable>
     <xsl:for-each select=".">
       <xsl:variable name="dmfile">
         <xsl:choose>
           <xsl:when test="@file">
             <xsl:value-of select="@file" />
           </xsl:when>
           <xsl:otherwise>
             <xsl:text>tmpdm.xml</xsl:text>
           </xsl:otherwise>
         </xsl:choose>
       </xsl:variable>
       <xsl:variable name="smfile">
           <xsl:value-of select="@screen" />
       </xsl:variable>
       <xsl:variable name="mtname">
           <xsl:value-of select="@name" />
       </xsl:variable>  
    
       <xsl:if test="$smfile!=''">
       <xsl:call-template name="genMetadata" >
         <xsl:with-param name="dmFile" select="$dmfile"/>
         <xsl:with-param name="smFile" select="$smfile"/>
         <xsl:with-param name="mtName" select="$mtname"/>
         <xsl:with-param name="menu" select="$menuNamelvl1"/>
       </xsl:call-template>
    </xsl:if>
       <xsl:if test="./node">
         <xsl:for-each select="./node">
            <xsl:apply-templates select="."/>
         </xsl:for-each>
       </xsl:if>
     </xsl:for-each>

      <!-- xsl:apply-templates select="node">
        <xsl:with-param name="menuName" select="$menuNamelvl1"/>
      </xsl:apply-templates -->
  </xsl:for-each>
  </xsl:for-each>
  </xsl:template>
  
  <xsl:template match="node">
  <xsl:param name="menuName" select="'Not Available'" />
     <xsl:for-each select=".">
       <xsl:variable name="dmfile">
         <xsl:choose>
           <xsl:when test="@file">
             <xsl:value-of select="@file" />
           </xsl:when>
           <xsl:otherwise>
             <xsl:text>tmpdm.xml</xsl:text>
           </xsl:otherwise>
         </xsl:choose>
       </xsl:variable>
       <xsl:variable name="smfile">
           <xsl:value-of select="@screen" />
       </xsl:variable>
       <xsl:variable name="mtname">
           <xsl:value-of select="@name" />
       </xsl:variable>  
       <xsl:if test="$smfile!=''">
       <xsl:call-template name="genMetadata" >
         <xsl:with-param name="dmFile" select="$dmfile"/>
         <xsl:with-param name="smFile" select="$smfile"/>
         <xsl:with-param name="mtName" select="$mtname"/>
         <xsl:with-param name="menu" select="$menuName"/>
       </xsl:call-template>
    </xsl:if>
       <xsl:if test="./node">
         <xsl:for-each select="./node">
            <xsl:apply-templates select="."/>
         </xsl:for-each>
       </xsl:if>
     </xsl:for-each>
  </xsl:template>   

  <xsl:template name="genMetadata">
    <xsl:param name="dmFile" select="'Not Available'" />
    <xsl:param name="smFile" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />
    <xsl:param name="menu" select="'Not Available'" />

    <xsl:variable name="file">
    <xsl:value-of select="concat('../../output/xml/dm/', $dmFile)"/>
    </xsl:variable>
    <xsl:variable name="sFile">
    <xsl:value-of select="concat('../xml/', $smFile)"/>
    </xsl:variable>
    <xsl:variable name="dmDoc" select="document($file)/DataModel" />
    <!--xsl:value-of select="java_func:debugMsg($file)" /-->
    <xsl:variable name="smDoc" select="document($sFile)/UIModel" />
    <!--xsl:value-of select="java_func:debugMsg($sFile)" /-->
    
    <xsl:text disable-output-escaping="yes">#&#xa;</xsl:text>
  <xsl:text disable-output-escaping="yes">&lt;root&gt;&#xa;</xsl:text>
  <xsl:text disable-output-escaping="yes">&#x20;&lt;dmpath&gt;</xsl:text>
      <xsl:value-of select="$smDoc/DMPath"/>
  <xsl:text disable-output-escaping="yes">&lt;/dmpath&gt;&#xa;</xsl:text>
  <xsl:text disable-output-escaping="yes">&#x20;&lt;breadcrumb&gt;</xsl:text>
    <xsl:value-of select="$menu"/>
  <xsl:text>&#x20;&gt;&#x20;</xsl:text>
  <xsl:value-of select="$smDoc/title"/>
  <xsl:text disable-output-escaping="yes">&lt;/breadcrumb&gt;&#xa;</xsl:text>
  <xsl:if test="$smDoc/vrtlInst">
     <xsl:text disable-output-escaping="yes">&#x20;&lt;vrtlInst&#x20;oid="</xsl:text>
     <xsl:variable name="vrtInstance" select="$smDoc/vrtlInst" />
     <xsl:for-each select="$dmDoc/DataModelNode">
         <xsl:variable name="installPath" select="InstallPath"/> 
         <xsl:if test="$vrtInstance = $installPath">
           <xsl:value-of select="NodeID" />
         </xsl:if>
     </xsl:for-each>
     <xsl:text disable-output-escaping="yes">"&#x20;name="</xsl:text>
     <xsl:value-of select="$smDoc/cookie/@name"/>
     <xsl:text disable-output-escaping="yes">"&#x20;timeout="</xsl:text>
     <xsl:value-of select="$smDoc/cookie/@timeout"/>
     <xsl:text disable-output-escaping="yes">"&#x20;/&gt;&#xa;</xsl:text>
  </xsl:if>
  
  <!-- Create help tag with source file name -->
  <xsl:if test="$mtName">
      <xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'" />
      <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />
    <xsl:text disable-output-escaping="yes">&#x20;&lt;help&#x20;src="ucm</xsl:text>
      <xsl:value-of select="translate($mtName, $uppercase, $smallcase)" />
    <xsl:text disable-output-escaping="yes">.htm"&#x20;/&gt;&#xa;</xsl:text>
  </xsl:if>
  
   <xsl:if test="$smDoc/depends">
     <xsl:text disable-output-escaping="yes">&#x20;&lt;depends&#x20;vrtlInst="</xsl:text>
     <xsl:variable name="vrtInstance" select="$smDoc/depends/@vrtlInst" />
     <xsl:value-of select="$vrtInstance" />
     <!--<xsl:for-each select="document($file)/DataModel/DataModelNode">
         <xsl:variable name="installPath" select="InstallPath"/> 
         <xsl:if test="$vrtInstance = $installPath">
           <xsl:value-of select="NodeID" />
         </xsl:if>
     </xsl:for-each>-->
     <xsl:text disable-output-escaping="yes">"&#x20;mtpath="</xsl:text>
     <xsl:value-of select="$smDoc/depends"/>
     <xsl:text disable-output-escaping="yes">_MetaData.xml"&#x20;/&gt;&#xa;</xsl:text>
  </xsl:if>
    <xsl:if test="$smDoc/useparenttx">
     <xsl:text disable-output-escaping="yes">&#x20;&lt;useparenttx&#x20;mtpath="</xsl:text>
           <xsl:variable name="smName" select="$smDoc/useparenttx/@parentsm" />
           <xsl:variable name="smMapFile" select="document('../xml/smtometa.xml')/data"/>
           <xsl:value-of select="$smMapFile//map[@screen=$smName]/@meta"/>
     <xsl:text disable-output-escaping="yes">"&#x20;ppath="</xsl:text>
           <xsl:value-of select="$smDoc/useparenttx/@ppath"/>
     <xsl:text disable-output-escaping="yes">"&#x20;/&gt;&#xa;</xsl:text>
  </xsl:if>

    <xsl:text disable-output-escaping="yes">&lt;!-- ***** METADATA OBJECTS START ***** --&gt;&#xa;</xsl:text> 
    <xsl:for-each select="$dmDoc/DataModelNode">
      <xsl:variable name="nodeType" select="NodeAttributes/NodeType"/>
      <xsl:if test="$nodeType='CM_DMNODE_TYPE_SCALAR_VAR'">
        <!-- Populate oid, name, label, type and key=yes/no  -->
        <xsl:text disable-output-escaping="yes">&#x20;&lt;obj oid="</xsl:text>
        <xsl:value-of select="NodeID" />
        <xsl:variable name="instllPath" select="InstallPath" />
        <xsl:text>"&#x20;name="</xsl:text><xsl:value-of select="Name" />
      <xsl:text>"&#x20;lbl="</xsl:text>
      <xsl:value-of select="$smDoc//DMNode[@path=$instllPath]/@label|$smDoc//Component[@path=$instllPath]/@label"/>
      <xsl:text>"&#x20;type="</xsl:text>
        <!--<xsl:value-of select="$smDoc//DMNode[@path=$instllPath]/@type"/>-->
        <xsl:call-template name="getUCMType" >
          <xsl:with-param name="type" select="DataAttributes/DataType"/>
        </xsl:call-template>
      <xsl:variable name="key" select="NodeAttributes/IsKey"/>
      <xsl:if test="($key = 'TRUE')">
        <xsl:text>"&#x20;key="yes</xsl:text>
        </xsl:if>
        <xsl:variable name="mandatory" select="NodeAttributes/Mandatory"/>
      <xsl:if test="($mandatory = 'TRUE')">
        <xsl:text>"&#x20;mand="yes</xsl:text>
        </xsl:if>
        <xsl:if test="(DataAttributes/Type='CM_DATA_ATTRIB_STR_RANGE') or 
            (DataAttributes/Type='CM_DATA_ATTRIB_INT_RANGE') ">
             <xsl:if test="(DataAttributes/DefaultValue) and (DataAttributes/Type='CM_DATA_ATTRIB_STR_RANGE')">
                 <xsl:text>"&#x20;defVal="</xsl:text><xsl:value-of select="DataAttributes/DefaultValue/DefaultString"/>
             </xsl:if>
             <xsl:if test="(DataAttributes/DefaultValue) and (DataAttributes/Type='CM_DATA_ATTRIB_INT_RANGE')">
                 <xsl:text>"&#x20;defVal="</xsl:text><xsl:value-of select="DataAttributes/DefaultValue/DefaultIntValue"/>
             </xsl:if>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"&gt;&#xa;</xsl:text>

        <xsl:choose>
        <xsl:when test="((DataAttributes/DataAttrib/MinLength != 0) and (DataAttributes/DataAttrib/MaxLength != 0))">
          <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;range&gt;</xsl:text>
          <xsl:value-of select="DataAttributes/DataAttrib/MinLength" />
          <xsl:text>-</xsl:text>
            <xsl:value-of select="DataAttributes/DataAttrib/MaxLength" />
            <xsl:text disable-output-escaping="yes">&lt;/range&gt;&#xa;</xsl:text>
          </xsl:when>
        <xsl:when test="((DataAttributes/DataAttrib/StartValue) and (DataAttributes/DataAttrib/EndValue))">
          <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;range&gt;</xsl:text>
          <xsl:value-of select="DataAttributes/DataAttrib/StartValue" />
          <xsl:text>-</xsl:text>
            <xsl:value-of select="DataAttributes/DataAttrib/EndValue" />
            <xsl:text disable-output-escaping="yes">&lt;/range&gt;&#xa;</xsl:text>
          </xsl:when>

          <xsl:when test="(DataAttributes/DataAttrib/StrEnum)">
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;list&gt;&#xa;</xsl:text>
            <xsl:for-each select="DataAttributes/DataAttrib/StrEnum/Value">
              <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&lt;item name="</xsl:text>
                          <xsl:variable name="name" select="."/>
                          <!-- User Friendly Name support start -->  
                          <xsl:variable name="fName">
                             <xsl:value-of select="@friendlyName"/>
                          </xsl:variable>
                          <xsl:choose>
                            <xsl:when test="$fName != ''">
                               <xsl:value-of select="$fName"/>
                            </xsl:when>
                            <xsl:otherwise>
                   <xsl:value-of select="$name"/>
                            </xsl:otherwise>
                          </xsl:choose>
                          <!-- User Friendly Name support end -->
              <xsl:text disable-output-escaping="yes">"&#x20;value="</xsl:text>
              <xsl:value-of select="." />
              <xsl:variable name="defaultVal" select="../../../DefaultValue/DefaultString"/>
              <xsl:text>"</xsl:text>
              <xsl:if test="$name = $defaultVal">
                 <xsl:text>&#x20;default="yes"</xsl:text>
              </xsl:if>
              <xsl:text disable-output-escaping="yes">/&gt;&#xa;</xsl:text> 
            </xsl:for-each>
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/list&gt;&#xa;</xsl:text>
          </xsl:when>
  
          <xsl:when test="(DataAttributes/DataAttrib/IntEnum)">
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;list&gt;&#xa;</xsl:text>
            <xsl:for-each select="DataAttributes/DataAttrib/IntEnum/Value">
              <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&lt;item name="</xsl:text>
                          <xsl:variable name="name" select="."/>
                          <!-- User Friendly Name support start -->  
                          <xsl:variable name="fName">
                             <xsl:value-of select="@friendlyName"/>
                          </xsl:variable>
                          <xsl:choose>
                            <xsl:when test="$fName != ''">
                               <xsl:value-of select="$fName"/>
                            </xsl:when>
                            <xsl:otherwise>
                   <xsl:value-of select="$name"/>
                            </xsl:otherwise>
                          </xsl:choose>
                          <!-- User Friendly Name support end -->              
              <xsl:text disable-output-escaping="yes">"&#x20;value="</xsl:text>
              <xsl:value-of select="." />
              <xsl:variable name="defaultVal" select="../../../DefaultValue/DefaultIntValue"/>
              <xsl:text>"</xsl:text>
              <xsl:if test="$name = $defaultVal">
                 <xsl:text>&#x20;default="yes"</xsl:text>
              </xsl:if>
              <xsl:text disable-output-escaping="yes">/&gt;&#xa;</xsl:text> 
            </xsl:for-each>
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/list&gt;&#xa;</xsl:text>
          </xsl:when>
      </xsl:choose>
        <xsl:text disable-output-escaping="yes">&#x20;&lt;/obj&gt;&#xa;</xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&lt;!-- ***** METADATA OBJECTS END ***** --&gt;&#xa;</xsl:text>
    <xsl:call-template name="genTable" >
     <xsl:with-param name="dmDoc" select="$dmDoc"/>
     <xsl:with-param name="smDoc" select="$smDoc"/>
       <xsl:with-param name="mtName" select="$mtName"/>
       <xsl:with-param name="menu" select="$menu"/>
    </xsl:call-template>
   <xsl:call-template name="genForm" >
     <xsl:with-param name="dmDoc" select="$dmDoc"/>
     <xsl:with-param name="smDoc" select="$smDoc"/>
     <xsl:with-param name="mtName" select="$mtName"/>
     <xsl:with-param name="menu" select="$menu"/>
    </xsl:call-template>
      <xsl:text disable-output-escaping="yes">&lt;/root&gt;&#xa;</xsl:text>  
  </xsl:template>
  
  <!-- 
 /*************************************************************************
 *  This template will generate metadata for table type. 
 *************************************************************************/ 
-->
  <xsl:template name="genTable">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />
    <xsl:param name="menu" select="'Not Available'" />
 
  <xsl:text disable-output-escaping="yes">&lt;!-- ***** TABLE DATA START ***** --&gt;</xsl:text>
    <xsl:for-each select="$smDoc/Component">
      <xsl:if test = "@type = 'TABLE' or @type='Table' or @type='SCALAR'">
        <xsl:choose>
          <xsl:when test="@type='SCALAR'">
            <xsl:text disable-output-escaping="yes">&#xa;&#x20;&lt;tbl</xsl:text>
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;typ="scalar"&#x20;</xsl:text>
            <xsl:if test="@title">
              <xsl:text disable-output-escaping="yes">&#x20;&#x20;title="</xsl:text>
              <xsl:value-of select="@title" />
              <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
            </xsl:if>
            <xsl:if test="@screenModel">
              <xsl:variable name="sName">
                <xsl:value-of select="@screenModel" />
              </xsl:variable>
              <xsl:variable name="sNamePath">
                <xsl:text>../xml/</xsl:text><xsl:value-of select="@screenModel" />
              </xsl:variable>
              <xsl:variable name="sDoc" select="document($sNamePath)/UIModel" />
              <xsl:variable name="smMapFil" select="document('../xml/smtometa.xml')/data"/>
              <xsl:variable name="extMtData" select="$smMapFil//map[@screen=$sName]/@meta"/>
              <xsl:text disable-output-escaping="yes">&#x20;&#x20;mtpath="</xsl:text>
              <xsl:value-of select="$extMtData" />
              <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
              <xsl:text disable-output-escaping="yes">&#x20;&#x20;dmpath="</xsl:text>
              <xsl:value-of select="$sDoc/DMPath" />
              <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
            </xsl:if>
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&gt;&#xa;</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text disable-output-escaping="yes">&#xa;&#x20;&lt;tbl&gt;&#xa;</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:for-each select="child::node()[name()!='']">
          <xsl:call-template name="processTblNodes">
            <xsl:with-param name="dmDoc" select="$dmDoc" />
            <xsl:with-param name="smDoc" select="$smDoc"/>
            <xsl:with-param name="mtName" select="$mtName"/>
          </xsl:call-template>
        </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&#x20;&lt;/tbl&gt;&#xa;</xsl:text>
      </xsl:if>
    </xsl:for-each>
  <xsl:text disable-output-escaping="yes">&lt;!-- ***** TABLE DATA END ***** --&gt;&#xa;</xsl:text>    
  </xsl:template>

  <xsl:template name="processTblNodes">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:choose>
      <xsl:when test="name()='GroupNodes' and not(@style)">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;col name="</xsl:text>
        <xsl:value-of select="@title"/>
        <xsl:if test="@rndr != ''">
          <xsl:text>"&#x20;rndr="</xsl:text><xsl:value-of select="@rndr" />
        </xsl:if>
        <xsl:if test="@dlem != ''">
          <xsl:text>"&#x20;dlem="</xsl:text><xsl:value-of select="@dlem" />
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"&gt;&#xa;</xsl:text>
        <xsl:for-each select="child::node()[name()!='']">
          <xsl:call-template name="processTblNodes">
            <xsl:with-param name="dmDoc" select="$dmDoc" />
            <xsl:with-param name="smDoc" select="$smDoc"/>
            <xsl:with-param name="mtName" select="$mtName"/>
          </xsl:call-template>
        </xsl:for-each>
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;/col&gt;&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="name()='DMNode'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&#x20;&lt;objref oid="</xsl:text>
        <xsl:variable name="path" select="@path"/>
        <xsl:value-of select="$dmDoc/DataModelNode[InstallPath=$path]/NodeID" />
        <xsl:text>"</xsl:text>
        <xsl:if test = "@snam = 'yes'" >
          <xsl:text>&#x20;snam="yes"</xsl:text>
        </xsl:if>   
        <xsl:text disable-output-escaping="yes">/&gt;&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="name()='link'">
        <xsl:call-template name="processTblLinks">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='Button'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&#x20;&lt;btn name="</xsl:text>
        <xsl:value-of select="@name" />
        <xsl:if test="@label != ''">
          <xsl:text disable-output-escaping="yes">"&#x20;lbl="</xsl:text>
          <xsl:value-of select="@label"/>
        </xsl:if>
        <xsl:if test="@type != ''">
          <xsl:text>"&#x20;type="</xsl:text><xsl:value-of select="@type" />
        </xsl:if>
        <xsl:if test="@action != ''">
          <xsl:text disable-output-escaping="yes">"&#x20;action="</xsl:text>
          <xsl:value-of select="@action"/>
        </xsl:if>
        <xsl:if test="@opcode != ''">
          <xsl:text disable-output-escaping="yes">"&#x20;opcode="</xsl:text>
          <xsl:value-of select="@opcode"/>
        </xsl:if>
        <xsl:if test="@screenModel">
          <xsl:variable name="sName" select="@screenModel" />
          <xsl:variable name="smMapFil" select="document('../xml/smtometa.xml')/data"/>
          <xsl:variable name="extMtData" select="$smMapFil//map[@screen=$sName]/@meta"/>
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20; mtpath="</xsl:text>
          <xsl:value-of select="$extMtData" />
        </xsl:if>
        <xsl:if test="@dmpath">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;dmpath="</xsl:text>
          <xsl:value-of select="@dmpath"/>
        </xsl:if>
        <xsl:if test="@key">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;key="</xsl:text>
          <xsl:value-of select="@key"/>
        </xsl:if>
        <xsl:if test="@value">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;value="</xsl:text>
          <xsl:value-of select="@value"/>
        </xsl:if>
        <xsl:if test="@spg != ''">
          <xsl:text>"&#x20;spg="</xsl:text><xsl:value-of select="@spg" />
        </xsl:if>
        <xsl:if test="@reqtyp != ''">
          <xsl:text>"&#x20;reqtyp="</xsl:text><xsl:value-of select="@reqtyp" />
        </xsl:if>
        <xsl:if test="@fpg != ''">
          <xsl:text>"&#x20;fpg="</xsl:text><xsl:value-of select="@fpg" />
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="name()='Component' and @type='text'">
        <xsl:call-template name="proc_frmDMNode">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc" />
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='Component'">
        <xsl:call-template name="proc_frmComp">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="processTblLinks">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:choose>
      <xsl:when test="@name= 'general'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;link name="</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">"&#x20;&#x20;lbl="</xsl:text>
        <xsl:value-of select="@label"/>
        <xsl:if test="@spg">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;spg="</xsl:text>
          <xsl:value-of select="@spg"/>
        </xsl:if>
        <xsl:if test="@screenModel">
          <xsl:variable name="sName" select="@screenModel" />
          <xsl:variable name="smMapFil" select="document('../xml/smtometa.xml')/data"/>
          <xsl:variable name="extMtData" select="$smMapFil//map[@screen=$sName]/@meta"/>
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20; mtpath="</xsl:text>
          <xsl:value-of select="$extMtData" />
        </xsl:if>
        <xsl:if test="@dmpath">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;dmpath="</xsl:text>
          <xsl:value-of select="@dmpath"/>
        </xsl:if>
        <xsl:if test="@key">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;key="</xsl:text>
          <xsl:value-of select="@key"/>
        </xsl:if>
        <xsl:if test="@value">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;value="</xsl:text>
          <xsl:value-of select="@value"/>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="@name= 'refresh'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;link name="</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text disable-output-escaping="yes">"&#x20;&#x20;lbl="</xsl:text>
        <xsl:value-of select="@label"/>
        <xsl:if test="@spg">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;spg="</xsl:text>
          <xsl:value-of select="@spg"/>
        </xsl:if>
        <xsl:if test="@action">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;action="</xsl:text>
          <xsl:value-of select="@action"/>
        </xsl:if>
        <xsl:if test="@opcode">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;opcode="</xsl:text>
          <xsl:value-of select="@opcode"/>
        </xsl:if>
        <xsl:if test="@interval">
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20;intrvl="</xsl:text>
          <xsl:value-of select="@interval"/>
        </xsl:if>
        <xsl:if test="@screenModel">
          <xsl:variable name="sName" select="@screenModel" />
          <xsl:variable name="smMapFil" select="document('../xml/smtometa.xml')/data"/>
          <xsl:variable name="extMtData" select="$smMapFil//map[@screen=$sName]/@meta"/>
          <xsl:text disable-output-escaping="yes">"&#x20;&#x20; mtpath="</xsl:text>
          <xsl:value-of select="$extMtData" />
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
      </xsl:when>
      <xsl:when test="@name= 'Add'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;link name="Add Record"</xsl:text>
      <xsl:if test="string-length(@label) != 0">
        <xsl:text disable-output-escaping="yes"> lbl="</xsl:text><xsl:value-of select="@label"/>    
        <xsl:text disable-output-escaping="yes">" </xsl:text>           
      </xsl:if>
      <xsl:if test="@parentTx='yes'">
        <xsl:text disable-output-escaping="yes"> ptx="y"</xsl:text>
      </xsl:if>
        <xsl:text disable-output-escaping="yes"> opcode="ADD" spg="</xsl:text>
        <xsl:if test="@spg">
          <xsl:value-of select="@spg"/>
        </xsl:if>
        <xsl:if test="string-length(@spg) = 0">
          <xsl:text>frm_meta.xsl</xsl:text>
        </xsl:if>
        <xsl:if test="@target != ''">
          <xsl:text>"&#x20;target="</xsl:text>
          <xsl:choose>
            <xsl:when test="@target='self'">
              <xsl:text>ldiv</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="@target" />
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
        <xsl:choose>
          <xsl:when test="./@screenModel">
            <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
            <xsl:call-template name="processParentKeys" />
            <xsl:text disable-output-escaping="yes">/&gt;&#xa;</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>" mtpath="</xsl:text>
            <xsl:value-of select="$mtName"/>
            <xsl:text disable-output-escaping="yes">_MetaData.xml"/&gt;&#xa;</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="@name= 'Next'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;link name="More" opcode="MORE" spg="tbl_meta.xsl" recCnt="10" mtpath="</xsl:text>
        <xsl:value-of select="$mtName"/>
        <xsl:text disable-output-escaping="yes">_MetaData.xml"/&gt;&#xa;</xsl:text>          
      </xsl:when>
      <xsl:when test="@name= 'Previous'">
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;link name="Previous" opcode="PREV" spg="tbl_meta.xsl" recCnt="10" mtpath="</xsl:text>
        <xsl:value-of select="$mtName"/>
        <xsl:text disable-output-escaping="yes">_MetaData.xml"/&gt;&#xa;</xsl:text>          
      </xsl:when>
      <xsl:otherwise>
        <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&#x20;&lt;link name="</xsl:text>
        <xsl:value-of select="@name" />
        <xsl:if test="@label != ''">
          <xsl:text>"&#x20;lbl="</xsl:text><xsl:value-of select="@label" />
        </xsl:if>
        <xsl:if test="@dmpath != ''">
          <xsl:text>"&#x20;dmpath="</xsl:text><xsl:value-of select="@dmpath" />
        </xsl:if>
        <xsl:if test="@opcode != ''">
          <xsl:text>"&#x20;opcode="</xsl:text><xsl:value-of select="@opcode" />
        </xsl:if>
        <xsl:if test="@mtpath != ''">
          <xsl:text>"&#x20;mtpath="</xsl:text><xsl:value-of select="@mtpath" />
        </xsl:if>
        <xsl:if test="@instancekey != ''">
          <xsl:text>"&#x20;instancekey="</xsl:text><xsl:value-of select="@instancekey" />
        </xsl:if>
        <xsl:if test="@subtbl != ''">
          <xsl:text>"&#x20;subtbl="</xsl:text><xsl:value-of select="@subtbl" />
        </xsl:if>
        <xsl:if test="@target != ''">
          <xsl:text>"&#x20;target="</xsl:text>
          <xsl:choose>
            <xsl:when test="@target='self'">
              <xsl:text>ldiv</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="@target" />
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
        <xsl:if test="@spg != ''">
          <xsl:text>"&#x20;spg="</xsl:text><xsl:value-of select="@spg" />
        </xsl:if>
        <xsl:if test="@fpg != ''">
          <xsl:text>"&#x20;fpg="</xsl:text><xsl:value-of select="@fpg" />
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- 
 /*************************************************************************
 *  This template will generate metadata for Form type. 
 *************************************************************************/ 
-->
  <xsl:template name="genForm">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />
    <xsl:param name="menu" select="'Not Available'" />
   
    <xsl:text disable-output-escaping="yes">&lt;!-- ***** FORM DATA START ***** --&gt;</xsl:text>
    <xsl:text disable-output-escaping="yes">&#xa;&#x20;&lt;form&#x20;</xsl:text>

    <xsl:for-each select="$smDoc/Component">
      <xsl:if test = "@type = 'Form'">
    <xsl:if test="@frmType='grpscalar'">
      <xsl:text disable-output-escaping="yes">frmType="</xsl:text>
        <xsl:value-of select="@frmType"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@note != ''">
      <xsl:text disable-output-escaping="yes">note="</xsl:text>
         <xsl:value-of select="@note"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">&gt;&#xa;</xsl:text>

        <xsl:for-each select="child::node()[name()!='']">
          <xsl:call-template name="processFrmComp">
            <xsl:with-param name="dmDoc" select="$dmDoc" />
            <xsl:with-param name="smDoc" select="$smDoc"/>
            <xsl:with-param name="mtName" select="$mtName"/>
          </xsl:call-template>
        </xsl:for-each>
      </xsl:if>
    </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&#x20;&lt;/form&gt;&#xa;</xsl:text>
    <xsl:text disable-output-escaping="yes">&lt;!-- ***** FORM DATA END ***** --&gt;&#xa;</xsl:text>
  </xsl:template>


  <xsl:template name="processFrmComp">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:choose>
      <xsl:when test="name()='DMNode'">
        <xsl:call-template name="proc_frmDMNode">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc" />
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='link'">
        <xsl:call-template name="processTblLinks">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='Component'">
        <xsl:call-template name="proc_frmComp">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='Condition'">
        <xsl:call-template name="prosFrmCondition">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='GroupNodes' and @style">
        <xsl:call-template name="prosFrmGroupNodes">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='Button'">
        <xsl:call-template name="prosFrmButton">
          <xsl:with-param name="dmDoc" select="$dmDoc" />
          <xsl:with-param name="smDoc" select="$smDoc"/>
          <xsl:with-param name="mtName" select="$mtName"/>
        </xsl:call-template>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="proc_frmDMNode">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />

    <xsl:variable name="path" select="@path"/>
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&lt;field </xsl:text>
    <xsl:if test="contains(@file,'m.xml')">
      <xsl:if test="string-length(@depends)!=0">
        <xsl:call-template name="getdmpath">
          <xsl:with-param name="smdoc" select="@file" />
          <xsl:with-param name="depends" select="@depends" />
        </xsl:call-template> 
      </xsl:if>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;oid="</xsl:text>
    <xsl:value-of select="$dmDoc/DataModelNode[InstallPath=$path]/NodeID" />
    <xsl:variable name="isdynamic">
         <xsl:value-of select="@dynamic"/>
    </xsl:variable>
    <xsl:if test="$isdynamic='true'">
      <xsl:text disable-output-escaping="yes">"&#x20;dynamic="true"&#x20;dmpath="</xsl:text> 
      <xsl:variable name="tmpPath">
          <xsl:value-of select="concat($smDoc/DMPath,'.')" />
      </xsl:variable>
          <xsl:value-of select="concat($tmpPath,$path)" />
      <xsl:text disable-output-escaping="yes">"&#x20;spg="</xsl:text>
         <xsl:value-of select="@spg"/>
      <xsl:text disable-output-escaping="yes">"&#x20;fpg="</xsl:text>
         <xsl:value-of select="@fpg"/>
    </xsl:if>
    <xsl:if test="@note">
      <xsl:text disable-output-escaping="yes">"&#x20;note="</xsl:text>
         <xsl:value-of select="@note"/>
    </xsl:if>
    <xsl:if test="@hide='true'" >
      <xsl:text>"&#x20;hide="true</xsl:text>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="processParentKeys">
        <xsl:variable name="smName" select="./@screenModel" />
        <xsl:variable name="smMapFile" select="document('../xml/smtometa.xml')/data"/>
        <xsl:variable name="extMetaData" select="$smMapFile//map[@screen=$smName]/@meta"/>
        <xsl:text disable-output-escaping="yes">&#x20; mtpath="</xsl:text>
        <xsl:value-of select="$extMetaData" />
        <xsl:variable name="pKeys" select="@parentKeys"/>
        <xsl:if test="@parentKeys='yes'">
          <xsl:text disable-output-escaping="yes">"&#x20; pkeys="</xsl:text>
          <xsl:variable name="smFile" select="concat('../xml/',$smName)"/>
          <xsl:variable name="instPath" select="document($smFile)/UIModel/DMPath"/>
          <xsl:for-each select="$smMapFile/map">
             <xsl:variable name="matchFound">
               <xsl:call-template name="screenModelTmpl">
                 <xsl:with-param name="instPath" select="$instPath"/>
                 <xsl:with-param name="path" select="@path"/>
               </xsl:call-template>
             </xsl:variable>
             <xsl:if test="$matchFound = 'yes'">
               <xsl:variable name="tempDM" select="concat('../../output/xml/dm/',@datamodel)"/>
               <xsl:variable name="tempDMDoc" select="document($tempDM)/DataModel"/>
               <xsl:for-each select="$tempDMDoc/DataModelNode[NodeAttributes/IsKey='TRUE']">
                  <xsl:value-of select="Name"/><xsl:text>+</xsl:text>
                  <xsl:call-template name="getUCMType" >
                    <xsl:with-param name="type" select="DataAttributes/DataType"/>
                  </xsl:call-template>
                  <xsl:text>$$</xsl:text><xsl:value-of select="NodeID"/>
                  <xsl:text>,</xsl:text>
               </xsl:for-each>
               <xsl:text>|</xsl:text>
             </xsl:if>
          </xsl:for-each>
          <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
        </xsl:if>
  </xsl:template>

  <xsl:template name="prosFrmCondition">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:variable name="value" select="@value" />
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&#x20;&lt;option val="</xsl:text>
    <xsl:value-of select="$value" />
    <xsl:if test="./@screenModel">
      <xsl:call-template name="processParentKeys" />
    </xsl:if>
    <xsl:text disable-output-escaping="yes">"&gt;&#xa;</xsl:text>

    <xsl:for-each select="child::node()[name()!='']">
      <xsl:call-template name="processFrmComp">
        <xsl:with-param name="dmDoc" select="$dmDoc" />
        <xsl:with-param name="smDoc" select="$smDoc"/>
        <xsl:with-param name="mtName" select="$mtName"/>
      </xsl:call-template>
    </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&#x20;&#x20;&#x20;&lt;/option&gt;&#xa;</xsl:text>  
  </xsl:template>

    <!-- screenModelTmpl-->
  <xsl:template name="screenModelTmpl">
    <xsl:param name="instPath" select="'Not Available'" />
    <xsl:param name="path" select="'Not Available'" />
    <xsl:value-of select="java_func:compareTokens($instPath,$path)" />
  </xsl:template>

  <xsl:template name="prosFrmButton">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;btn lbl="</xsl:text>
    <xsl:value-of select="@label"/><xsl:text>"&#x20;</xsl:text>
    <xsl:text disable-output-escaping="yes">type="submit"&#x20;mtpath="</xsl:text>
    <xsl:value-of select="$mtName"/> 
    <xsl:text disable-output-escaping="yes">_MetaData.xml"&#x20;spg="</xsl:text>

    <xsl:if test="@sredirect">
      <xsl:value-of select="@sredirect"/>
    </xsl:if>
    <xsl:if test="string-length(@sredirect) = 0">
      <xsl:text>tbl_meta.xsl</xsl:text>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">" fpg="</xsl:text>
    <xsl:if test="@fredirect">
      <xsl:value-of select="@fredirect"/>
    </xsl:if>
    <xsl:if test="string-length(@fredirect) = 0">
      <xsl:text>frm_meta.xsl</xsl:text>
    </xsl:if>
    <xsl:if test="@opcode">
      <xsl:text disable-output-escaping="yes">" opcode="</xsl:text>
      <xsl:value-of select="@opcode"/>
    </xsl:if>
    <xsl:if test="@action!=''">
      <xsl:text disable-output-escaping="yes">" action="</xsl:text>
      <xsl:value-of select="@action"/>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="prosFrmGroupNodes">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;grp </xsl:text>
    <xsl:if test="@name">
      <xsl:text disable-output-escaping="yes">&#x20;name="</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@style">
      <xsl:text disable-output-escaping="yes">&#x20;style="</xsl:text>
      <xsl:value-of select="@style"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@action">
      <xsl:text disable-output-escaping="yes">&#x20;act="</xsl:text>
      <xsl:value-of select="@action"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@clearOnColapse">
      <xsl:text disable-output-escaping="yes">&#x20;clr="</xsl:text>
      <xsl:value-of select="@clearOnColapse"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@showMore">
      <xsl:text disable-output-escaping="yes">&#x20;showMore="</xsl:text>
      <xsl:value-of select="@showMore"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@formType">
      <xsl:text disable-output-escaping="yes">&#x20;ftyp="</xsl:text>
      <xsl:value-of select="@formType"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@title">
      <xsl:text disable-output-escaping="yes">&#x20;title="</xsl:text>
      <xsl:value-of select="@title"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>
    <xsl:if test="@target">
      <xsl:text disable-output-escaping="yes">&#x20;target="</xsl:text>
      <xsl:value-of select="@target"/>
      <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
    </xsl:if>

    <xsl:if test="./@screenModel">
        <xsl:call-template name="processParentKeys" />
        <xsl:variable name="smName" select="./@screenModel" />
        <!--xsl:variable name="smMapFile" select="document('../xml/smtometa.xml')/data"/>
        <xsl:variable name="extMetaData" select="$smMapFile//map[@screen=$smName]/@meta"/>
        <xsl:text disable-output-escaping="yes">&#x20; mtpath="</xsl:text>
        <xsl:value-of select="$extMetaData" />
        <xsl:variable name="pKeys" select="@parentKeys"/>
        <xsl:if test="@parentKeys='yes'">
          <xsl:text disable-output-escaping="yes">"&#x20; pkeys="</xsl:text>
          <xsl:variable name="smFile" select="concat('../xml/',$smName)"/>
          <xsl:variable name="instPath" select="document($smFile)/UIModel/DMPath"/>
          <xsl:for-each select="$smMapFile/map">
             <xsl:variable name="matchFound">
               <xsl:call-template name="screenModelTmpl">
                 <xsl:with-param name="instPath" select="$instPath"/>
                 <xsl:with-param name="path" select="@path"/>
               </xsl:call-template>
             </xsl:variable>
             <xsl:if test="$matchFound = 'yes'">
               <xsl:variable name="tempDM" select="concat('../../output/xml/dm/',@datamodel)"/>
               <xsl:variable name="tempDMDoc" select="document($tempDM)/DataModel"/>
               <xsl:for-each select="$tempDMDoc/DataModelNode[NodeAttributes/IsKey='TRUE']">
                  <xsl:value-of select="Name"/><xsl:text>+</xsl:text>
                  <xsl:call-template name="getUCMType" >
                    <xsl:with-param name="type" select="DataAttributes/DataType"/>
                  </xsl:call-template>
                  <xsl:text>$$</xsl:text><xsl:value-of select="NodeID"/>
                  <xsl:text>,</xsl:text>
               </xsl:for-each>
               <xsl:text>|</xsl:text>
             </xsl:if>
          </xsl:for-each>
        </xsl:if>
        <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text-->
        <xsl:variable name="smFileStr" select="concat('../xml/',$smName)"/>
        <!--xsl:text>&#x20;dmpath="</xsl:text-->
        <xsl:text disable-output-escaping="yes">&#x20;dmpath="</xsl:text>
          <xsl:value-of select="document($smFileStr)/UIModel/DMPath"/>
        <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
        <!--xsl:text>"</xsl:text-->
    </xsl:if>

    <xsl:text disable-output-escaping="yes">&gt;&#xa;</xsl:text>

    <xsl:choose>
      <xsl:when test="@style='landscape'">
        <xsl:for-each select="child::node()[name()='GroupNodes' and @type='row']">
          <xsl:call-template name="prosFrmRow">
            <xsl:with-param name="dmDoc" select="$dmDoc" />
            <xsl:with-param name="smDoc" select="$smDoc"/>
            <xsl:with-param name="mtName" select="$mtName"/>
          </xsl:call-template>
        </xsl:for-each>
      </xsl:when>
      <xsl:when test="@style='portrait'">
        <xsl:for-each select="child::node()[name()!='']">
          <xsl:call-template name="processFrmComp">
            <xsl:with-param name="dmDoc" select="$dmDoc" />
            <xsl:with-param name="smDoc" select="$smDoc"/>
            <xsl:with-param name="mtName" select="$mtName"/>
          </xsl:call-template>
        </xsl:for-each>
      </xsl:when>
    </xsl:choose>

    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/grp&gt;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="prosFrmRow">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;row&gt;&#xa;</xsl:text>
      <xsl:for-each select="child::node()[name()!='']">
        <xsl:choose>
          <xsl:when test="name()='DMNode'">
            <xsl:call-template name="proc_frmDMNode">
              <xsl:with-param name="dmDoc" select="$dmDoc" />
              <xsl:with-param name="smDoc" select="$smDoc" />
            </xsl:call-template>
          </xsl:when>
          <xsl:when test="name()='Component'">
            <xsl:call-template name="proc_frmComp">
              <xsl:with-param name="dmDoc" select="$dmDoc" />
              <xsl:with-param name="smDoc" select="$smDoc"/>
              <xsl:with-param name="mtName" select="$mtName"/>
            </xsl:call-template>
          </xsl:when> 
          <xsl:when test="name()='GroupNodes' and @type='column'">
            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;col </xsl:text>
            <xsl:if test="@name">
              <xsl:text disable-output-escaping="yes">&#x20;name="</xsl:text>
              <xsl:value-of select="@name"/>
              <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
            </xsl:if>
            <xsl:text disable-output-escaping="yes">&gt;&#xa;</xsl:text>

            <xsl:for-each select="child::node()[name()!='']">
              <xsl:call-template name="processFrmComp">
                <xsl:with-param name="dmDoc" select="$dmDoc" />
                <xsl:with-param name="smDoc" select="$smDoc"/>
                <xsl:with-param name="mtName" select="$mtName"/>
              </xsl:call-template>
            </xsl:for-each>

            <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/col&gt;&#xa;</xsl:text>
          </xsl:when> 
        </xsl:choose>
      </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/row&gt;&#xa;</xsl:text>

  </xsl:template>

  <xsl:template name="proc_frmComp">
    <xsl:param name="dmDoc" select="'Not Available'" />
    <xsl:param name="smDoc" select="'Not Available'" />
    <xsl:param name="mtName" select="'Not Available'" />

    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;field oid="</xsl:text>
    <xsl:variable name="path" select="@path"/>
    <xsl:value-of select="$dmDoc/DataModelNode[InstallPath=$path]/NodeID" />
    <xsl:text>"</xsl:text>
    <xsl:choose>
   
    <xsl:when test="./depends">
    <!-- Multi depends, multi smFile handling --> 
    <xsl:text>&#x20;dmpath="</xsl:text>
    <xsl:for-each select="./depends">  
    <xsl:if test="position() = 1">
     <xsl:variable name="dep">
      <xsl:value-of select="@key"/>
     </xsl:variable>
     <xsl:variable name="fil">
      <xsl:value-of select="@file"/>
     </xsl:variable>
     <xsl:variable name="smfile">
      <xsl:value-of select="concat('../xml/',$fil)"/>
     </xsl:variable>
       <xsl:if test="contains($smfile,'m.xml')">     
          <xsl:value-of select="document($smfile)//DMPath"/>          
       </xsl:if> 
     </xsl:if>
     </xsl:for-each>
     <xsl:text>"</xsl:text>

    <xsl:text>&#x20;mdmpath="</xsl:text>
    <xsl:for-each select="./depends">   
     <xsl:variable name="dep">
      <xsl:value-of select="@key"/>
     </xsl:variable>
     <xsl:variable name="fil">
      <xsl:value-of select="@file"/>
     </xsl:variable>
     <xsl:variable name="smfile">
      <xsl:value-of select="concat('../xml/',$fil)"/>
     </xsl:variable>
     <xsl:if test="contains($smfile,'m.xml')">     
     <xsl:value-of select="document($smfile)//DMPath"/>
     <xsl:text>,</xsl:text> 
     </xsl:if>  
     </xsl:for-each>
     <xsl:text>"</xsl:text>

    <xsl:text>&#x20;mwhere="</xsl:text>
     <xsl:for-each select="./depends"> 
      <xsl:if test="@where">
        <xsl:value-of select="@where"/>
        <xsl:text>,</xsl:text> 
      </xsl:if>
     </xsl:for-each>
    <xsl:text>"</xsl:text>

    <xsl:text>&#x20;where="</xsl:text>
     <xsl:for-each select="./depends"> 
     <xsl:if test="position() = 1">
      <xsl:if test="@where">
        <xsl:value-of select="@where"/>
        <xsl:text>,</xsl:text> 
      </xsl:if>
     </xsl:if>
     </xsl:for-each>
    <xsl:text>"</xsl:text>

    <xsl:text>&#x20;depends="</xsl:text>
    <xsl:for-each select="./depends">
    <xsl:if test="position() = 1">
     <xsl:variable name="dep">
      <xsl:value-of select="@key"/>
     </xsl:variable>
     <xsl:variable name="fil">
      <xsl:value-of select="@file"/>
     </xsl:variable>
     <xsl:variable name="smfile">
      <xsl:value-of select="concat('../xml/',$fil)"/>
     </xsl:variable>
      <xsl:variable name="dependstype">
        <xsl:value-of select="document($smfile)//DMNode[@path=$dep]/@type"/>
      </xsl:variable>
      <xsl:variable name="depconcat">
        <xsl:value-of select="concat($dep,'+')"/>
      </xsl:variable> 
      <xsl:choose>
        <xsl:when test="$dependstype != ''">
          <xsl:value-of select="concat($depconcat,$dependstype)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat($depconcat,'STR')"/>
        </xsl:otherwise>
     </xsl:choose>  
    </xsl:if>  
    </xsl:for-each>
    <xsl:text>"</xsl:text>

    <xsl:text>&#x20;mdepends="</xsl:text>
    <xsl:for-each select="./depends">
     <xsl:variable name="dep">
      <xsl:value-of select="@key"/>
     </xsl:variable>
     <xsl:variable name="fil">
      <xsl:value-of select="@file"/>
     </xsl:variable>
     <xsl:variable name="smfile">
      <xsl:value-of select="concat('../xml/',$fil)"/>
     </xsl:variable>
      <xsl:variable name="dependstype">
        <xsl:value-of select="document($smfile)//DMNode[@path=$dep]/@type"/>
      </xsl:variable>
      <xsl:variable name="depconcat">
        <xsl:value-of select="concat($dep,'+')"/>
      </xsl:variable> 
      <xsl:choose>
        <xsl:when test="$dependstype != ''">
          <xsl:value-of select="concat($depconcat,$dependstype)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat($depconcat,'STR')"/>
        </xsl:otherwise>
     </xsl:choose>
     <xsl:text>,</xsl:text> 
    </xsl:for-each>
    <xsl:text>"</xsl:text>

     </xsl:when>   
     <xsl:otherwise test="contains(@file,'m.xml')">
      <xsl:if test="string-length(@depends)!=0">
        <xsl:call-template name="getdmpath">
          <xsl:with-param name="smdoc" select="@file"/>
          <xsl:with-param name="depends" select="@depends"/>
        </xsl:call-template> 
      </xsl:if>
     </xsl:otherwise>
   </xsl:choose>

    <xsl:text>&#x20;name="</xsl:text><xsl:value-of select="@name"/>
    <xsl:text>"&#x20;type="</xsl:text><xsl:value-of select="@type"/>
    
    <xsl:variable name="isdynamic">
         <xsl:value-of select="@dynamic"/>
    </xsl:variable>
    <xsl:if test="$isdynamic='true'">
      <xsl:text disable-output-escaping="yes">"&#x20;dynamic="true"&#x20;dmpath="</xsl:text> 
      <xsl:variable name="tmpPath">
          <xsl:value-of select="concat($smDoc/DMPath,'.')" />
      </xsl:variable>
          <xsl:value-of select="concat($tmpPath,$path)" />
      <xsl:text disable-output-escaping="yes">"&#x20;spg="</xsl:text>
         <xsl:value-of select="@spg"/>
      <xsl:text disable-output-escaping="yes">"&#x20;fpg="</xsl:text>
         <xsl:value-of select="@fpg"/>
    </xsl:if>
    <xsl:if test="@note">
      <xsl:text disable-output-escaping="yes">"&#x20;note="</xsl:text>
         <xsl:value-of select="@note"/>
    </xsl:if>
    <xsl:if test="@where">
      <xsl:text disable-output-escaping="yes">"&#x20;where="</xsl:text>
         <xsl:value-of select="@where"/>
    </xsl:if>
    <xsl:if test="@rows">
      <xsl:text disable-output-escaping="yes">"&#x20;rows="</xsl:text>
         <xsl:value-of select="@rows"/>
    </xsl:if>
    <xsl:if test="@cols">
      <xsl:text disable-output-escaping="yes">"&#x20;cols="</xsl:text>
         <xsl:value-of select="@cols"/>
    </xsl:if>
    <xsl:if test="@size">
      <xsl:text disable-output-escaping="yes">"&#x20;size="</xsl:text>
      <xsl:value-of select="@size"/>
    </xsl:if>
    <xsl:if test="@action">
      <xsl:text disable-output-escaping="yes">"&#x20;action="</xsl:text>
      <xsl:value-of select="@action"/>
    </xsl:if>
    <xsl:if test="@basedonkey">
      <xsl:text disable-output-escaping="yes">"&#x20;key="</xsl:text>
      <xsl:value-of select="@basedonkey"/>
    </xsl:if>
    <xsl:if test="@onselectload">
      <xsl:text disable-output-escaping="yes">"&#x20;load="</xsl:text>
      <xsl:value-of select="@onselectload"/>
    </xsl:if>
    <xsl:if test="(@type='checkbox')">
      <xsl:if test="@checkval">
      <xsl:text>"&#x20;checkval="</xsl:text><xsl:value-of select="@checkval"/>
      </xsl:if>
    </xsl:if>
    <xsl:if test="(@type='label')">
      <xsl:text>"&#x20;lbl="</xsl:text><xsl:value-of select="@label"/>
      <xsl:text>"&#x20;style="</xsl:text><xsl:value-of select="@style"/>
    </xsl:if>
    <xsl:text disable-output-escaping="yes">"&gt;&#xa;</xsl:text>

    <xsl:for-each select="Condition">
      <xsl:call-template name="prosFrmCondition">
        <xsl:with-param name="dmDoc" select="$dmDoc" />
        <xsl:with-param name="smDoc" select="$smDoc"/>
        <xsl:with-param name="mtName" select="$mtName"/>
      </xsl:call-template>
    </xsl:for-each>
    <xsl:text disable-output-escaping="yes">&#x20;&#x20;&lt;/field&gt;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="getdmpath">
    <xsl:param name="smdoc" select="'Not Available'" />
    <xsl:param name="depends" select="'Not Available'" />
  
    <xsl:variable name="smfile">
      <xsl:value-of select="concat('../xml/',$smdoc)" />
    </xsl:variable>
    <xsl:if test="contains($smfile,'m.xml')">
    <xsl:text>&#x20;dmpath="</xsl:text>
      <xsl:value-of select="document($smfile)//DMPath"/>
    <xsl:text>"</xsl:text>
    <xsl:text>&#x20;depends="</xsl:text>
      <xsl:variable name="dependstype">
        <xsl:value-of select="document($smfile)//DMNode[@path=$depends]/@type"/>
      </xsl:variable>
      <xsl:variable name="depconcat">
        <xsl:value-of select="concat($depends,'+')"/>
      </xsl:variable> 
      <xsl:choose>
        <xsl:when test="$dependstype != ''">
          <xsl:value-of select="concat($depconcat,$dependstype)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat($depconcat,'STR')"/>
        </xsl:otherwise>
     </xsl:choose>
    <xsl:text>"</xsl:text>
    </xsl:if> 
 </xsl:template>

  <xsl:template name="replace-string">
    <xsl:param name="text"/>
    <xsl:param name="replace"/>
    <xsl:param name="with"/>
    <xsl:choose>
      <xsl:when test="contains($text,$replace)">
        <xsl:value-of select="substring-before($text,$replace)"/>
        <xsl:value-of select="$with"/>
        <xsl:call-template name="replace-string">
          <xsl:with-param name="text" select="substring-after($text,$replace)"/>
          <xsl:with-param name="replace" select="$replace"/>
          <xsl:with-param name="with" select="$with"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$text"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
