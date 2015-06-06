<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template name="gen_lbl">
    <!-- Required Data Variables -->
    <xsl:param name="mData" select="'Not Available'" />
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="sData" select="'Not Available'"/>

    <xsl:choose>
      <xsl:when test="@style" >
        <span>
          <xsl:attribute name="style">
            <xsl:value-of select="@style"/>
          </xsl:attribute>
          <xsl:call-template name="getLbl">
            <xsl:with-param name="mData" select="$mData"/>
            <xsl:with-param name="objID" select="$objID"/>
            <xsl:with-param name="objName" select="$objName"/>
            <xsl:with-param name="frmType" select="$frmType"/>
            <xsl:with-param name="sData" select="$sData"/>
          </xsl:call-template>
        </span>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="getLbl">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="getLbl">
    <!-- Required Data Variables -->
    <xsl:param name="mData" select="'Not Available'" />
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="sData" select="'Not Available'"/>

    <xsl:choose>
      <!--xsl:when test="@lbl!=''" >
        <xsl:value-of select="@lbl"/>
      </xsl:when-->
      <xsl:when test="$frmType='EDIT'">
        <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
      </xsl:when>
      <xsl:when test="$mData/obj[@oid=$objID]/@lbl" >
        <xsl:value-of select="$mData/obj[@oid=$objID]/@lbl"/>
      </xsl:when>
      <xsl:when test="$objName" >
        <xsl:value-of select="$objName"/>
      </xsl:when>
    </xsl:choose>

  </xsl:template>
</xsl:stylesheet>
