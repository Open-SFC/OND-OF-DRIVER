<?xml version="1.0" encoding="UTF-8"?> 
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 

  <xsl:template name="getOPCode">
    <xsl:param name="tOpt" select="'Not Available'" />
    <xsl:choose>
      <xsl:when test="$tOpt='ADD'">
        <xsl:text>1</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='BYKEY'">
        <xsl:text>2</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='MORE'">
        <xsl:text>3</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='GETROLE'">
        <xsl:text>4</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='IGETROLE'">
        <xsl:text>5</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='RBYKEY'">
        <xsl:text>6</xsl:text>
      </xsl:when>
      <xsl:when test="$tOpt='IRBYKEY'">
        <xsl:text>7</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
