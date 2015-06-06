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
 * File name: gen_prodinfo.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: This file will generate the product information
 *              details from product.xml file.
 * 
 */
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" indent="yes" />

  <xsl:template match="/">
    <xsl:text>productName="</xsl:text><xsl:value-of select="/root/Title" /><xml:text>"&#xa;</xml:text>
    <xsl:text>productVersion="</xsl:text><xsl:value-of select="/root/Version" /><xml:text>"&#xa;</xml:text>
  </xsl:template>
</xsl:stylesheet>
