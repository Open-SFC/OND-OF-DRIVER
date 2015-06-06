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
 * File name: gen_macro.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file will generate the macro's from the DateModel
 *              xml file.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" indent="yes" />

	<xsl:template match="/">
		<xsl:call-template name="gen-Macro" /> 
	</xsl:template>
<!-- 
 /*************************************************************************
 *  This template loops through all nodes and generates the output in xml
 *  format. It will take the module name (i.e. vsg,firewall),installPath,
 *	name of the node(i.e. vsg name) and convert it to uppercase to generate
 *  the ID.      
 *************************************************************************/ 
--> 
	<xsl:template name="gen-Macro" >
		<xsl:for-each select="/DataModel/DataModelNode">
		 <xsl:variable name="nodeType" select="NodeAttributes/NodeType"/>
		 <xsl:variable name="reg" select="@reg"/>
		 
		 <xsl:if test="$nodeType = 'CM_DMNODE_TYPE_ANCHOR'or $nodeType = 'CM_DMNODE_TYPE_TABLE'
                           or ($nodeType = 'CM_DMNODE_TYPE_SCALAR_VAR' and $reg = 'yes')">
		    <xsl:variable name="installPath" select="../InstallPath"/>
		 	<xsl:variable name="tmpName" select="Name" />
		 	<xsl:variable name="macname" >
		      <xsl:value-of  select="concat($installPath, '_' ,$tmpName)"/>
		    </xsl:variable>
		 	<xsl:variable name="macroname" >
		      <xsl:value-of select="translate($macname, '.', '_')" />
		    </xsl:variable>
		    <xsl:text>#define CM_DM_</xsl:text>
		    <xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'" />
    	            <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />
		    <xsl:value-of select="translate($macroname, $smallcase, $uppercase)" />
		    <xsl:text>_</xsl:text>
		    <xsl:text>PATH&#x20;&#x20;</xsl:text>
		    <xsl:text>"</xsl:text>
		    <xsl:value-of  select="concat($installPath, '.' ,$tmpName)"/>
		    <xsl:text>"</xsl:text>	
		    <xsl:text>&#xa;</xsl:text>
		 </xsl:if>
		 
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>
