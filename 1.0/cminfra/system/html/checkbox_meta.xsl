<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template name="gen_CheckBox">  
	          <!-- Required Data Variables -->
           <xsl:param name="mData" select="'Not Available'" />           
           <xsl:param name="objID" select="'Not Available'" />
           <xsl:param name="objName" select="'Not Available'" />
           <xsl:param name="frmType" select="'Not Available'" />
           <xsl:param name="sData" select="'Not Available'" />
           <xsl:param name="cookieName" select="'Not Available'" />
           <xsl:param name="isVisible" select="'Not Available'" /> 
           <xsl:param name="isExtFrmField" select="'Not Available'" /> 
           <xsl:param name="parentKeys" select="'Not Available'" /> 
		
            <input type="checkbox">
                 <xsl:if test="@dynamic='true'">
                    <xsl:attribute name="dynamic">
                       <xsl:text>true</xsl:text>
                    </xsl:attribute>
                    <xsl:attribute name="dmpath">
                       <xsl:value-of select="@dmpath" />
                    </xsl:attribute>
                    <xsl:attribute name="sredirect">
                       <xsl:value-of select="@spg" />
                    </xsl:attribute>
                    <xsl:attribute name="fredirect">
                        <xsl:value-of select="@fpg" />
                     </xsl:attribute>
                 </xsl:if>
		 <xsl:attribute name="name">
                    <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
                    <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/>
                 </xsl:attribute>
		         <xsl:attribute name="id">
                    <xsl:value-of select="$objID"/>
                 </xsl:attribute>
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
                    <xsl:text>toggleCheckBox(this,'</xsl:text><xsl:value-of select="$cookieName"/><xsl:text>');</xsl:text>
                 </xsl:attribute>

                 <xsl:if test="$frmType='EDIT'">
                  <xsl:variable name="fVal">
                    <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
                  </xsl:variable>
                  <xsl:variable name="posCheckVal">
                    <xsl:choose>
                      <xsl:when test="@checkval">
                        <xsl:variable name="checkVal">
                           <xsl:value-of select="@checkval"/>
                        </xsl:variable>
 
                        <xsl:value-of select="substring-before($checkVal,'/')"/>
                      </xsl:when>
                      <xsl:otherwise>
                         <xsl:value-of select="1"/>
                      </xsl:otherwise>
                    </xsl:choose>
                 </xsl:variable>		
                <xsl:if test="$fVal=$posCheckVal">
                  <xsl:attribute name="checked">
                    <xsl:text>true</xsl:text>
                  </xsl:attribute> 
                </xsl:if>
             </xsl:if>
             <xsl:attribute name="value">
               <xsl:value-of select="$mData/obj[@value]"/>
             </xsl:attribute> 
           </input>
	</xsl:template>
</xsl:stylesheet>  		
