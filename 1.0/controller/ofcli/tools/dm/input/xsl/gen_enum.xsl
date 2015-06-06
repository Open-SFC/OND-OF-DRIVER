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
 * File name: gen_enum.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file will generate the ID's from the DateModel.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" indent="yes" />

	<xsl:template match="/">
		<xsl:call-template name="gen-Enum" />
	</xsl:template>
	
<!-- 
 /*************************************************************************
 *  This template loops through all nodes and generates the output in xml
 *  format. It will take the module name (i.e. vsg,firewall), name of the 
 *  node(i.e. vsg name) and convert it to uppercase to generate the ID.      
 *************************************************************************/ 
--> 
	<xsl:template name="gen-Enum" >
		<xsl:variable name="modName" select="/DataModel/@ModuleName" />
		<xsl:text>typedef enum { &#xa;</xsl:text>
		<xsl:for-each select="/DataModel/DataModelNode">
		 <xsl:text>CM_DM_</xsl:text>
		 <xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz'" />
 		 <xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'" />
		 <xsl:value-of select="translate($modName, $smallcase, $uppercase)" />
		 <xsl:text>_</xsl:text>
		 <xsl:variable name="name" select="Name" />
		 <xsl:value-of select="translate($name, $smallcase, $uppercase)" />
		 <xsl:text>_ID,&#xa;</xsl:text>
		</xsl:for-each>
	 <xsl:text>}DMNodeID_e;</xsl:text>	
	</xsl:template>
</xsl:stylesheet>
