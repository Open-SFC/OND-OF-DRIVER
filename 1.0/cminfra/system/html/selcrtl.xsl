<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:template name="genSelCrtl">
    <xsl:param name="mData" select="'Not Available'" />
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'" />
    <xsl:param name="sData" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'" />
    <xsl:param name="isExtFrmField" select="'Not Available'" />
    <xsl:param name="parentKeys" select="'Not Available'" />

 <div> 
   <xsl:variable name="selName">
     <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
     <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/>
   </xsl:variable>
   <table cellpadding="0" cellspacing="0" border="0">
   <tr><td width="12%">&#160;</td>
   <td width="31%" style="vertical-align: middle;">
     <xsl:value-of select="$mData/obj[@name=$objName]/@lbl"/>
     <xsl:text>&#160;List &#160;</xsl:text><br />
     <select multiple="true" size="7" style="WIDTH: 140px;">
       <xsl:attribute name="name">
         <xsl:value-of select="$selName"/>
       </xsl:attribute>
       <xsl:attribute name="id">
         <xsl:value-of select="$selName"/>
       </xsl:attribute>
      <xsl:if test="@depends">
      <xsl:if test="contains(@dmpath,'{0}')">
        <xsl:attribute name="depdmpath">
          <xsl:value-of select="@dmpath"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="contains(@depends,'+')">
        <xsl:attribute name="depends">
          <xsl:value-of select="@depends"/>
        </xsl:attribute>
      </xsl:if>
      </xsl:if>
<!--
       <xsl:if test="@depends">
       <xsl:if test="contains(@dmpath,'{0}')">
         <xsl:attribute name="depdmpath">
           <xsl:choose>
             <xsl:when test="not($sData/dmpath)">
               <xsl:value-of select="@dmpath"/>
             </xsl:when>
             <xsl:otherwise>
               <xsl:value-of select="$sData/dmpath" />
             </xsl:otherwise>
           </xsl:choose>
         </xsl:attribute>
       </xsl:if>
       <xsl:if test="contains(@depends,'+')">
         <xsl:attribute name="depends">
           <xsl:value-of select="@depends"/>
         </xsl:attribute>
       </xsl:if>
       </xsl:if>
-->
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
       <xsl:if test="@action">
         <xsl:attribute name="action">
           <xsl:value-of select="@action"/>
         </xsl:attribute>
       </xsl:if>
       <xsl:if test="@mdmpath">
         <xsl:attribute name="mdmpath">
           <xsl:value-of select="@mdmpath"/>
         </xsl:attribute>
       </xsl:if>
       <xsl:if test="@mdepends">
         <xsl:attribute name="mdepends">
           <xsl:value-of select="@mdepends"/>
         </xsl:attribute>
       </xsl:if>
       <xsl:if test="@mwhere">
         <xsl:attribute name="mwhere">
           <xsl:value-of select="@mwhere"/>
         </xsl:attribute>
       </xsl:if>
       <xsl:if test="@where">
         <xsl:attribute name="where">
           <xsl:value-of select="@where"/>
         </xsl:attribute>
       </xsl:if>
       <!-- Populate options -->
       <xsl:for-each select="$mData/obj[@oid=$objID]/list/item">
         <xsl:variable name="itemName">
           <xsl:value-of select="@name"/>
         </xsl:variable>
         <xsl:variable name="obIDval">
           <xsl:value-of select="$objName"/><xsl:text>_</xsl:text>
           <xsl:value-of select="@value"/>
         </xsl:variable>

         <option>
           <xsl:attribute name="value">
             <xsl:value-of select="@value"/>
           </xsl:attribute>
           <xsl:value-of select="$itemName"/>
            <!--
              <xsl:value-of select="$debugTxt"/>
            -->
           <xsl:element name="key">
             <xsl:attribute name="idval">
               <xsl:value-of select="$obIDval"/>
             </xsl:attribute>
           </xsl:element>
         </option>
       </xsl:for-each>
     </select>      
   </td>
   <td class="tdselctrl" width ="15%"><br />
     <input type ="button" name ="" value ="&gt;&gt;" title="Move Right">
        <xsl:attribute name="onclick">
          <xsl:text>addEntry('</xsl:text>
          <xsl:value-of select="$selName"/>
          <xsl:text>','</xsl:text>
          <xsl:text>targetList_</xsl:text><xsl:value-of select="$selName"/>		
          <xsl:text>');</xsl:text>
        </xsl:attribute>
     </input>
   <br /><br />
     <input type ="button" name ="" value ="&lt;&lt;" title="Move Left">
        <xsl:attribute name="onclick">
          <xsl:text>removeEntry('</xsl:text>
          <xsl:value-of select="$selName"/>
          <xsl:text>','</xsl:text>
          <xsl:text>targetList_</xsl:text><xsl:value-of select="$selName"/>
          <xsl:text>');</xsl:text>
        </xsl:attribute>
     </input>
   <br/><br />
   </td>
   <td width="30" style="vertical-align: middle;">
   <xsl:text>Selected&#160;</xsl:text>
     <xsl:value-of select="$mData/obj[@name=$objName]/@lbl"/>
   <xsl:text>&#160;List</xsl:text><br />
   <select name="targetList" multiple="multiple" size="7" style="WIDTH: 140px;">
    <xsl:attribute name="id">
       <xsl:text>targetList_</xsl:text><xsl:value-of select="$selName"/>
    </xsl:attribute>
    <xsl:attribute name="dmpath"> 
       <xsl:value-of select="$mData/dmpath" />
    </xsl:attribute>
   </select>
   </td>
   <td width="12%">&#160;</td>
   </tr>
   </table>
   <!--<script language="JavaScript1.2">
     updateMSelectionList('<xsl:value-of select="$selName"/>',
        '<xsl:value-of select="substring-before($selName,'_')"/><xsl:text>SelArry</xsl:text>');
   </script>-->
   </div>
  </xsl:template>

</xsl:stylesheet>
