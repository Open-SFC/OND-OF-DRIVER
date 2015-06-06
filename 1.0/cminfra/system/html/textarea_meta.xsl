<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template name="gen_TextArea">
    <!-- Required Data Variables -->
    <xsl:param name="mData" select="'Not Available'" />           
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'" />
    <xsl:param name="sData" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'"/>
    <xsl:param name="isExtFrmField" select="'Not Available'"/>
    <xsl:param name="parentKeys" select="'Not Available'"/>

      <textarea>
        <xsl:variable name="fieldName">
          <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
          <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/><xsl:text>$$</xsl:text>
          <xsl:value-of select="@oid"/>
        </xsl:variable>
        <xsl:attribute name="visible">
          <xsl:value-of select="$isVisible"/>
        </xsl:attribute>
        <xsl:attribute name="name">
          <xsl:value-of select="$fieldName"/>
        </xsl:attribute>
        <xsl:attribute name="id">
          <xsl:value-of select="$fieldName"/>
        </xsl:attribute>
        <!-- Populate Text Area value -->
        <xsl:variable name="fVal">
          <xsl:choose>
            <xsl:when test="$frmType='EDIT'">
              <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$mData/obj[@value]"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <!-- Text Area row length -->
        <xsl:variable name="rowcnt">
          <xsl:choose>
            <xsl:when test="@rows">
              <xsl:value-of select="@rows"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>3</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:attribute name="rows">
          <xsl:value-of select="$rowcnt"/>
        </xsl:attribute>
        <!-- Text Area column length -->
        <xsl:choose>
          <xsl:when test="@cols">
            <xsl:attribute name="cols">
              <xsl:value-of select="@cols"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="style">
              <xsl:text>WIDTH:250px;</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <!-- To limit the value length -->
        <xsl:variable name="maxlen">
          <xsl:value-of select="substring-after($mData/obj[@oid=$objID]/range,'-')" />	
        </xsl:variable>	
        <xsl:attribute name="onkeyup">
          <xsl:text>txtAreaLimit(this,</xsl:text>
          <xsl:value-of select="$maxlen"/>
          <xsl:text>);</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="onkeydown">
          <xsl:text>txtAreaLimit(this,</xsl:text>
          <xsl:value-of select="$maxlen"/>
          <xsl:text>);</xsl:text>
        </xsl:attribute>
        <xsl:variable name="loadGrp">          
          <xsl:value-of select="@load"/>
        </xsl:variable>
	<xsl:if test="$loadGrp != ''">
         <xsl:attribute name="onBlur">
          <xsl:text>loadGrpIframe(this,</xsl:text>
          <xsl:value-of select="$loadGrp"/>
          <xsl:text>);</xsl:text>
         </xsl:attribute>
	</xsl:if>
        <xsl:attribute name="title">
          <!--xsl:text>Data Type: String</xsl:text-->
          <xsl:text>Maximum Length: </xsl:text><xsl:value-of select="$maxlen"/>
        </xsl:attribute>
        <!-- Actual Value-->
        <xsl:value-of select="$fVal"/>
      </textarea>
  </xsl:template>
</xsl:stylesheet>
