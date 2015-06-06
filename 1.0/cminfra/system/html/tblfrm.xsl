<?xml version="1.0" encoding="UTF-8"?> 
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
  <xsl:template name="tbl-frm">
    <form name="tbl-form" id="tbl-form" action="ucmreq.igw"  method="post">
      <input type="hidden" name="dmpath"  />
      <input type="hidden" name="opcode" />
      <input type="hidden" name="mtpath" />
      <input type="hidden" name="sredirect" />
      <input type="hidden" name="fredirect" />
      <input type="hidden" name="ucmreq" />
      <input type="hidden" name="keyprms" />
      <input type="hidden" name="reqtype" />
      <input type="hidden" name="host" />
    </form>
  </xsl:template>
</xsl:stylesheet>
