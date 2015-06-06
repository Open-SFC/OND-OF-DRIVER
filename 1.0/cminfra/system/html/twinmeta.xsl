<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
  <xsl:include href="frmgmeta.xsl"/>

  <xsl:template name="popl_frm">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'" />
    <xsl:param name="isExtFormRef" select="'Not Available'" />

    <xsl:for-each select="$mData/form">
      <xsl:call-template name="poplFrmFields">
        <xsl:with-param name="sData" select="$sData" />
        <xsl:with-param name="mData" select="$mData" />
        <xsl:with-param name="frmType" select="$frmType" />
        <xsl:with-param name="cookieName" select="$cookieName" />
        <xsl:with-param name="isVisible" select="$isVisible'" />
        <xsl:with-param name="isExtFormField" select="'false'" />
      </xsl:call-template>
    </xsl:for-each>
    <script language="JavaScript1.2">
      dynamiclist('<xsl:value-of select="$cookieName" />');      
    </script>
  </xsl:template>

</xsl:stylesheet>
