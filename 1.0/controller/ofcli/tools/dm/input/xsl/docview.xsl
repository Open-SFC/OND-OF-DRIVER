<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
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
 * File name: docview.xsl
 * Author: Freescale Semiconductor
 * Date:   03/15/2013
 * Description: 
 * 
 */
-->

<xsl:template match="/">
    <html>
    <body>
     <h2>
     <xsl:text>Module Name: </xsl:text>
     <xsl:value-of select="DataModel/@ModuleName" />
     </h2>
     <br/>
     <b>
     <xsl:text>Data Model Path: </xsl:text>
     <I>
     <xsl:value-of select="DataModel/InstallPath" />
     </I>
     </b>
     
	  <table border="1">
        <tr>
          <th>Name</th>
          <th>Node Type</th>
          <th>Data Type</th>
          <th>Mandatory</th>
          <th>R/W</th>
          <th>Details</th>
          <th>Defaults</th>
        </tr>
	    <xsl:for-each select="DataModel/DataModelNode">
        <tr>
         <td>
           <xsl:value-of select="Name" />
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
               <xsl:text>RW</xsl:text>
             </xsl:when>
             <xsl:otherwise>
               <xsl:text>R</xsl:text>
             </xsl:otherwise>
           </xsl:choose>
         </td>
         <td>
           <b>
           <xsl:text>Description: </xsl:text>
           </b>
           <xsl:value-of select="Description" />
           <br/>
           <b>
           <xsl:text>Range: </xsl:text>
           </b>
           <xsl:value-of select="DataAttributes/DataAttrib/StartValue" />
           <xsl:text> - </xsl:text>
           <xsl:value-of select="DataAttributes/DataAttrib/EndValue" />
           <xsl:value-of select="DataAttributes/DataAttrib/MinLength" />
           <xsl:text> - </xsl:text>
           <xsl:value-of select="DataAttributes/DataAttrib/MaxLength" />
           <br/>
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
	    </xsl:for-each>
      </table>
    </body>
    </html>
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
