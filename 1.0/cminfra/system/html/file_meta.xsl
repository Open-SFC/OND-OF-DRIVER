<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template name="gen_fileinput">
    <!-- Required Data Variables -->
    <xsl:param name="mData" select="'Not Available'" />           
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'" />
    <xsl:param name="sData" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'" />
    <xsl:param name="isExtFrmField" select="'Not Available'" />
    <xsl:param name="parentKeys" select="'Not Available'" />
    
    <!-- Parse through <obj oid="39" name="RecordType" lbl="Record Type" type="STR"> line ,
      if function passed objID and the parsed oid are same go a head -->
    <xsl:for-each select="$mData/obj[@oid=$objID]">
      <input type="file">
        <xsl:attribute name="name">
          <xsl:value-of select="@name"/>
        </xsl:attribute>
        <xsl:if test="$frmType='EDIT'">
          <xsl:variable name="fVal">
            <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
          </xsl:variable>
        </xsl:if>
        <xsl:attribute name="value">
          <xsl:value-of select="$mData/obj[@value]"/>
        </xsl:attribute> 
        <xsl:if test="$isExtFrmField='true'">
          <xsl:attribute name="dmpath"> 
            <xsl:value-of select="$mData/dmpath" />
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
        <xsl:attribute name="size">
          <xsl:value-of select="@size"/>
        </xsl:attribute>
      </input>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>
