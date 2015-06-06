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
 * File name: ucm_typs.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file returns UCM opcodes for HTTP.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="xml" indent="yes" version="1.0" />

<!-- 
 /*************************************************************************
 *  This template will generate oid's from DataModel xml file.
 *************************************************************************/ 
--> 	
  <xsl:template name="getUCMType">
    <xsl:param name="type" select="'Not Available'" />
    <xsl:choose>
      <xsl:when test="$type='CM_DATA_TYPE_STRING'">
        <xsl:text>STR</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_INT'">
        <xsl:text>INT</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UINT'">
        <xsl:text>UINT</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_INT64'">
        <xsl:text>INT64</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UINT64'">
        <xsl:text>UINT64</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_BOOLEAN'">
        <xsl:text>BOOL</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_DATETIME'">
        <xsl:text>DTTM</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_BASE64'">
        <xsl:text>BASE64</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_IPADDR'">
        <xsl:text>IPADDR</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_STRING_SPECIAL_CHARS'">
        <xsl:text>STR</xsl:text>
      </xsl:when>
      <xsl:when test="$type='CM_DATA_TYPE_UNKNOWN'">
        <xsl:text>UNKNOWN</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
