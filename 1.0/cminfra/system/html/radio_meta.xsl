<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
       <xsl:template name="gen_RadioBtn">        
        <xsl:param name="mData" select="'Not Available'" />           
        <xsl:param name="objID" select="'Not Available'" />
        <xsl:param name="objName" select="'Not Available'" />
        <xsl:param name="frmType" select="'Not Available'" />
        <xsl:param name="sData" select="'Not Available'" />
        <xsl:param name="isVisible" select="'Not Available'" />
        <xsl:param name="isExtFrmField" select="'Not Available'" />
        <xsl:param name="parentKeys" select="'Not Available'" />
           
           <xsl:for-each select="$mData/obj[@oid=$objID]/list/item">
             <xsl:variable name="itemName">
                <xsl:value-of select="@name"/>
              </xsl:variable>
              <xsl:variable name="obIDval">
                <xsl:value-of select="$objName"/><xsl:text>_</xsl:text>
                <xsl:value-of select="@value"/>
              </xsl:variable>
              <input type="radio">
                <xsl:attribute name="name">
                  <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
                  <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/>
                </xsl:attribute>
                <xsl:attribute name="id">
                  <xsl:value-of select="$objID"/>
                </xsl:attribute>
                <xsl:attribute name="value">
                  <xsl:value-of select="@value"/>
                </xsl:attribute>
                <xsl:variable name="defaultVal">
                    <xsl:value-of select="@default"/>
                </xsl:variable>
                <xsl:if test="$isExtFrmField='true'">
                   <xsl:attribute name="dmpath"> 
                      <xsl:value-of select="$mData/dmpath" />
                   </xsl:attribute>
                   <xsl:attribute name="opcode">
                      <xsl:choose>
                        <xsl:when test="$mData/form/@frmType='grpscalar'">
                          <xsl:value-of select="'EDIT'" />
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:value-of select="$frmType" />
                        </xsl:otherwise>
                      </xsl:choose>
                    </xsl:attribute>
                   <xsl:if test="$parentKeys">
                      <xsl:attribute name="parentkeys">
                        <xsl:value-of select="$parentKeys" />
                   </xsl:attribute>
                  </xsl:if>
                </xsl:if>
                <xsl:attribute name="visible">
                   <xsl:value-of select="$isVisible"/>
                </xsl:attribute>
                <xsl:attribute name="onClick">
                 <xsl:text>toggleRadio('</xsl:text>
                  <xsl:value-of select="$objID"/>
                  <xsl:text>_</xsl:text>
                  <xsl:value-of select="@value"/>
                  <xsl:text>');</xsl:text>
                </xsl:attribute>
                <xsl:if test="$frmType='ADD'">
                  <xsl:if test="$defaultVal='yes'" >
                     <xsl:attribute name="checked" value="checked"/>
                  </xsl:if>
                </xsl:if>
                <xsl:if test="$frmType='EDIT'">
                  <xsl:variable name="fVal">
                    <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
                      </xsl:variable>
                      <xsl:variable name="curVal">
                         <xsl:value-of select="@value"/>
                      </xsl:variable>
                      <xsl:if test="@value=$fVal" >
                         <xsl:attribute name="checked" value="checked"/>
                      </xsl:if>
                </xsl:if>
               </input> 
               <!-- Enable comment for debug
                  <xsl:text>src val = </xsl:text>
                     <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/> <br/>
                  <xsl:text>cur val = </xsl:text>
                            <xsl:value-of select="@value"/> <br/>
              --> 
              <xsl:value-of select="$itemName"/>
              <xsl:text>&#160;</xsl:text>
           </xsl:for-each>
           <br/><br/>
       </xsl:template>
</xsl:stylesheet>            
