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
 * File name: gen_menu.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file will generate menu.xml file.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="xml" indent="yes" version="1.0"/>

<!-- 
 /*************************************************************************
 *  This template loops through all nodes and generates the output in xml
 *  format. It will generate the menu names for first level.
 *  Product.xml file is input for this xsl file.  
 *************************************************************************/ 
--> 
<xsl:template match="/">
  <xsl:text disable-output-escaping="yes">&lt;?xml-stylesheet type="text/xsl" href="menu.xsl"?&gt;&#xa;</xsl:text>
  <root name="Home">
  <xsl:for-each select="/root/ModuleNavModel/Module">
    <xsl:variable name="navfile">  
    <xsl:value-of select="@name"/>
    </xsl:variable>  
    
    <xsl:variable name="navFile">
    <xsl:value-of select="concat('../xml/', $navfile)"/>
      </xsl:variable>
   
    <xsl:variable name="navData" select="document($navFile)/root"/>
    <xsl:for-each select="$navData/node">
      <xsl:call-template name="insertVrtlInstTag" />
    </xsl:for-each>
  <xsl:apply-templates select="$navData/node" />
  </xsl:for-each>
  </root>
</xsl:template>

<xsl:template name="insertVrtlInstTag">
  <xsl:for-each select="node">
    <xsl:if test="@screen">
      <xsl:variable name="smFile">
      <xsl:value-of select="@screen"/>
      </xsl:variable>
      <xsl:variable name="smFilePath">
         <xsl:value-of select="concat('../xml/', $smFile)"/>
      </xsl:variable>
      <xsl:variable name="uiModel" select="document($smFilePath)/UIModel"/>
        <xsl:if test="$uiModel/vrtlInst" >
          <xsl:text disable-output-escaping="yes">&#xa;&lt;vrtlInst name="</xsl:text>
            <xsl:value-of  select="$uiModel/cookie/@name"/>
          <xsl:text disable-output-escaping="yes">"&#x20;timeout="</xsl:text> 
            <xsl:value-of  select="$uiModel/cookie/@timeout" />
          <xsl:text disable-output-escaping="yes">"/&gt;&#xa;</xsl:text>
        </xsl:if>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<!-- 
 /*************************************************************************
 *  This template will generate the menu names for second level.  
 *************************************************************************/ 
--> 
<xsl:template match="node">
 <xsl:if test="(@showMenu!='no') or not(@showMenu)">
  <xsl:for-each select=".">
    <xsl:variable name="menuNamelvl2">
      <xsl:value-of select="@label" />
    </xsl:variable>
    <xsl:variable name="smlvl1">
      <xsl:value-of select="@screen" />
    </xsl:variable>
      
    <xsl:variable name="dmlvl2">
      <xsl:value-of select="@file" />
    </xsl:variable>
    <xsl:variable name="dmFilelvl2">
      <xsl:value-of select="concat('../xml/', $dmlvl2)"/>
    </xsl:variable>
      
    <xsl:if test="string-length($smlvl1) !=0">
      <xsl:variable name="smFilelvl1">
        <xsl:value-of select="concat('../xml/', $smlvl1)"/>
      </xsl:variable>                   
              
      <xsl:variable name="smDoc" select="document($smFilelvl1)/UIModel" />
      <xsl:variable name="vrtlfilename">
        <xsl:value-of select="$smDoc/depends/@filename"/>
      </xsl:variable>     

      <xsl:variable name="vrtInst">
        <xsl:choose>
          <xsl:when test="$smDoc/vrtlInst">
            <xsl:text>yes</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>no</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>     
      <xsl:variable name="actionOpt">
        <xsl:value-of select="$smDoc/Component/Button[@action='FRMTBL']/@opcode" />
      </xsl:variable>
      <xsl:variable name="type">
        <xsl:for-each select="document($smFilelvl1)/UIModel/Component">
          <xsl:if test="@type='TABLE' or @type='SCALAR'">
            <xsl:value-of select="@type" />
          </xsl:if>
        </xsl:for-each>
      </xsl:variable>
       
      <xsl:choose>        
        <xsl:when test="string-length($vrtlfilename) != 0">
          <xsl:variable name="vrtlFilelvl1">
            <xsl:value-of select="concat('../xml/', $vrtlfilename)"/>
          </xsl:variable>
          <xsl:variable name="instance">
            <xsl:value-of select="document($vrtlFilelvl1)/DataModel/InstallPath" />
          </xsl:variable>  
          <xsl:variable name="concatinstance">
            <xsl:value-of select="concat($instance,'{0}')" />
          </xsl:variable>
          <xsl:variable name="installPath" select="document($dmFilelvl2)/DataModel/InstallPath"/>
          <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;model name="</xsl:text>
          <xsl:value-of  select="$menuNamelvl2"/>
          <xsl:text disable-output-escaping="yes">"&#x20;fqn="</xsl:text> 
          <xsl:call-template name="replace-string">
            <xsl:with-param name="text" select="$installPath"/>
            <xsl:with-param name="replace" select="$instance"/>
            <xsl:with-param name="with" select="$concatinstance"/>
          </xsl:call-template>

          <xsl:choose>
            <xsl:when test="$actionOpt ='' and (contains($type,'TABLE') or contains($type,'SCALAR'))" >
              <xsl:text disable-output-escaping="yes">"&#x20;spg="tbl_meta.xsl"&#x20;mtpath="</xsl:text>
            </xsl:when>
            <xsl:when test="$actionOpt != ''" >
              <xsl:text disable-output-escaping="yes">"&#x20;spg="frm_meta.xsl"&#x20;opcode="</xsl:text>
              <xsl:value-of select="$actionOpt" />
              <xsl:text disable-output-escaping="yes">"&#x20;mtpath="</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text disable-output-escaping="yes">"&#x20;spg="frm_meta.xsl"&#x20;mtpath="</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:value-of select="@name"/>
          <xsl:text disable-output-escaping="yes">_MetaData.xml"&#x20;</xsl:text>
          <xsl:if test="document($smFilelvl1)/UIModel/Component[@type='Form']/@frmType = 'grpscalar'">
             <xsl:text disable-output-escaping="yes">opcode="BYKEY"</xsl:text>
          </xsl:if>
          <xsl:if test="@host != ''">
             <xsl:text disable-output-escaping="yes">&#x20;host="</xsl:text>
             <xsl:value-of select="@host"/>
             <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
          </xsl:if>

          <xsl:text disable-output-escaping="yes">&gt;</xsl:text>
          <xsl:if test="$type='TABLE'">
            <xsl:for-each select="document($dmFilelvl2)/DataModel/DataModelNode/NodeAttributes">
              <xsl:variable name="key" select="IsKey"/> 
              <xsl:if test="$key='TRUE'">
                <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&#x20;&#x20;&lt;key name="</xsl:text>
                <xsl:value-of select="../Name"/>
                <xsl:text disable-output-escaping="yes">"&#x20;type="</xsl:text>
                <xsl:variable name="type" select="substring(../DataAttributes/DataType,15)"/>
                <xsl:choose>
                  <xsl:when test="$type='STRING'">
                    <xsl:text>STR</xsl:text> 
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="$type"/>  
                  </xsl:otherwise>
                </xsl:choose> 
                <xsl:text disable-output-escaping="yes">"&#x20;value="nil"/&gt;</xsl:text>
              </xsl:if>
            </xsl:for-each>
          </xsl:if>

          <xsl:if test="./node">
            <xsl:for-each select="./node">
              <xsl:apply-templates select="." />
            </xsl:for-each>
          </xsl:if>
          <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;/model&gt;</xsl:text>
        </xsl:when>  
        <xsl:when test="string-length($vrtlfilename)=0">
          <xsl:variable name="installPath" select="document($dmFilelvl2)/DataModel/InstallPath"/>
          <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;model name="</xsl:text>
          <xsl:value-of  select="$menuNamelvl2"/>
          <xsl:text disable-output-escaping="yes">"&#x20;fqn="</xsl:text> 
          <xsl:value-of  select="$installPath" />
          <xsl:if test="$vrtInst='yes'">
            <xsl:text disable-output-escaping="yes">"&#x20;vrtInst="yes</xsl:text>
          </xsl:if>
          <xsl:choose>
            <xsl:when test="$actionOpt ='' and ($type='TABLE' or $type='SCALAR')" >
              <xsl:text disable-output-escaping="yes">"&#x20;spg="tbl_meta.xsl"&#x20;mtpath="</xsl:text>
            </xsl:when>
            <xsl:when test="$actionOpt != ''" >
              <xsl:text disable-output-escaping="yes">"&#x20;spg="frm_meta.xsl"&#x20;opcode="</xsl:text>
              <xsl:value-of select="$actionOpt" />
              <xsl:text disable-output-escaping="yes">"&#x20;mtpath="</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text disable-output-escaping="yes">"&#x20;spg="frm_meta.xsl"&#x20;mtpath="</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:value-of select="@name"/>
          <xsl:text disable-output-escaping="yes">_MetaData.xml"&#x20;</xsl:text>

          <xsl:if test="document($smFilelvl1)/UIModel/Component[@type='Form']/@frmType = 'grpscalar'">
             <xsl:text disable-output-escaping="yes">opcode="BYKEY"</xsl:text>
          </xsl:if>
          <xsl:if test="@host != ''">
             <xsl:text disable-output-escaping="yes">&#x20;host="</xsl:text>
             <xsl:value-of select="@host"/>
             <xsl:text disable-output-escaping="yes">"&#x20;</xsl:text>
          </xsl:if>
          <xsl:text disable-output-escaping="yes">&gt;</xsl:text>

          <xsl:for-each select="document($dmFilelvl2)/DataModel/DataModelNode/NodeAttributes">
            <xsl:variable name="key" select="IsKey"/> 
            <xsl:if test="$key='TRUE'">
              <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&#x20;&#x20;&lt;key name="</xsl:text>
              <xsl:value-of select="../Name"/>
              <xsl:text disable-output-escaping="yes">"&#x20;type="</xsl:text>
              <xsl:variable name="type" select="substring(../DataAttributes/DataType,15)"/>
              <xsl:choose>
                <xsl:when test="$type='STRING'">
                  <xsl:text>STR</xsl:text> 
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$type"/>  
                </xsl:otherwise>
              </xsl:choose> 
              <xsl:text disable-output-escaping="yes">"&#x20;value="nil"/&gt;</xsl:text>
            </xsl:if>
          </xsl:for-each>
          <xsl:if test="./node">
            <xsl:for-each select="./node">
            <xsl:apply-templates select="." />
          </xsl:for-each>
          </xsl:if>
        <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;/model&gt;</xsl:text>
      </xsl:when>
      </xsl:choose>
    </xsl:if >
    <xsl:if test="string-length($smlvl1)=0">
      <xsl:variable name="installPath" select="document($dmFilelvl2)/DataModel/InstallPath"/>
          <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;model name="</xsl:text>
      <xsl:value-of  select="$menuNamelvl2"/>
          <xsl:text disable-output-escaping="yes">"&#x20;fqn="</xsl:text> 
      <xsl:value-of  select="$installPath" />
      <xsl:if test="$installPath!=''">
        <xsl:choose>
          <xsl:when test="contains($installPath,'.')">
            <xsl:text disable-output-escaping="yes">"&#x20;mtpath="ROLE_MetaData.xml"</xsl:text> 
          </xsl:when>
          <xsl:otherwise>
            <xsl:text disable-output-escaping="yes">"&#x20;mtpath="AROLE_MetaData.xml"</xsl:text> 
          </xsl:otherwise>
        </xsl:choose>
        <xsl:text disable-output-escaping="yes">&#x20;spg="tbl_meta.xsl</xsl:text> 
      </xsl:if>
        <xsl:text disable-output-escaping="yes">"&gt;</xsl:text>
        <xsl:if test="./node">
            <xsl:for-each select="./node">
            <xsl:apply-templates select="." />
          </xsl:for-each>
          </xsl:if>
        <xsl:text disable-output-escaping="yes">&#xa;&#x20;&#x20;&lt;/model&gt;</xsl:text>
      
    </xsl:if>
  </xsl:for-each>
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
