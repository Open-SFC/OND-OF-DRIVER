<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:template name="gen_List">
    <xsl:param name="mData" select="'Not Available'" />
    <xsl:param name="objID" select="'Not Available'" />
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'" />
    <xsl:param name="sData" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'" /> 
    <xsl:param name="isExtFrmField" select="'Not Available'" />
    <xsl:param name="parentKeys" select="'Not Available'" />
   
     <select onChange="selectedValue(this);">    
      <xsl:if test="$sData/MetaData != 'TimeWindow_MetaData.xml'">
        <xsl:attribute name="style">
          <xsl:text>WIDTH:250px;</xsl:text>
        </xsl:attribute>
      </xsl:if>

        <xsl:variable name="loadGrp">                                                   
             <xsl:value-of select="@load"/>                                                
        </xsl:variable>                                  
                                                
        <xsl:attribute name="load">                                                
             <xsl:value-of select="$loadGrp"/>
        </xsl:attribute>                        
                                                                                         
      <xsl:attribute name="name">
        <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
        <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/>
        <xsl:text>$$</xsl:text>
        <xsl:value-of select="@oid"/>
      </xsl:attribute>
      <xsl:attribute name="id">            
            <xsl:value-of select="$objName"/>  
            <xsl:if test="$objName = 'status'">
                 <xsl:text> </xsl:text>  
            </xsl:if>     
            <!--xsl:value-of select="$objID"/-->
      </xsl:attribute> 
      <xsl:attribute name="obid">            
            <xsl:value-of select="$objID"/>
      </xsl:attribute>     
      <!-- 
        Following attributes are populated to provide 
        input to Ajax function  to dynamically populate 
        select optionsi using the specified dmpath. 
      -->
      <!--Create attributes for depends and dmpath start-->
      <!--Create attributes at option/sub level -->
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
      <xsl:if test="@depends">
      <!--xsl:if test="contains(@dmpath,'{0}')"-->
        <xsl:attribute name="depdmpath">
          <xsl:value-of select="@dmpath"/>
        </xsl:attribute>
        <xsl:attribute name="key">
          <xsl:value-of select="@key"/>
        </xsl:attribute>
         <xsl:if test="@load != ''">
            <xsl:attribute name="load">
              <xsl:value-of select="@load"/>
            </xsl:attribute>
        </xsl:if>
      <!--/xsl:if-->
      <xsl:if test="contains(@depends,'+')">
        <xsl:attribute name="depends">
          <xsl:value-of select="@depends"/>
        </xsl:attribute>
      </xsl:if>
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
      <xsl:if test="@where != ''">
          <xsl:attribute name="where">
             <xsl:value-of select="@where"/>
          </xsl:attribute>
      </xsl:if>
      <xsl:if test="@mwhere != ''">
          <xsl:attribute name="mwhere">
             <xsl:value-of select="@mwhere"/>
          </xsl:attribute>
      </xsl:if>
      <!--Create attributes end -->

      <xsl:if test="$frmType='EDIT'">
        <xsl:attribute name="curval">
          <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:choose>      
      <xsl:when test="($mData/obj[@oid=$objID]/@type='INT') and not($mData/obj[@oid=$objID]/list)">
         <xsl:variable name="intRange">
               <xsl:value-of select="$mData/obj[@oid=$objID]/range"/>
         </xsl:variable>

         <xsl:variable name="lowerRange">
               <xsl:value-of select="substring-before($intRange,'-')"/>
         </xsl:variable>
         <xsl:variable name="upperRange">
               <xsl:value-of select="substring-after($intRange,'-')"/>
         </xsl:variable>
         <option>
             <xsl:attribute name="value">
               <xsl:value-of select="''"/>
             </xsl:attribute>
             <xsl:value-of select="''"/>
         </option>
         <xsl:call-template name="createIntOption">
              <xsl:with-param name="lowerRange" select="$lowerRange" />
              <xsl:with-param name="upperRange" select="$upperRange" />
              <xsl:with-param name="frmType" select="$frmType" />
              <xsl:with-param name="sData" select="$sData" />
              <xsl:with-param name="objName" select="$objName" />
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:if test="not($mData/obj[@oid=$objID]/@mand)">        
         <option>
           <xsl:attribute name="value">
            <xsl:value-of select="''"/>
           </xsl:attribute>
           <xsl:value-of select="'-Select Option-'"/>
         </option>        
       </xsl:if>
      <xsl:for-each select="$mData/obj[@oid=$objID]/list/item">
        <xsl:variable name="itemName">
          <xsl:value-of select="@name"/>
        </xsl:variable>
        <xsl:variable name="obIDval">
          <xsl:value-of select="$objName"/><xsl:text>_</xsl:text>
          <xsl:value-of select="@value"/>
        </xsl:variable>
        <xsl:variable name="defaultVal">
          <xsl:value-of select="@default"/>
        </xsl:variable>
        <!--
          <xsl:variable name="debugTxt">
            <xsl:value-of select="@value"/> <xsl:text> == </xsl:text>
            <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
          </xsl:variable>
        -->
        <xsl:variable name="fVal">
          <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
        </xsl:variable>
        <!--
          Create select options using mtdata/obj/list and 
          check the default or value as received from the server 
        -->
        <option>
          <xsl:if test="$frmType='ADD'">
            <xsl:if test="$defaultVal='yes'" >
              <xsl:attribute name="selected" value="selected"/>
            </xsl:if>
          </xsl:if>
          <xsl:if test="$frmType='EDIT'">
            <xsl:variable name="curVal">
              <xsl:value-of select="@value"/>
            </xsl:variable>
            <xsl:if test="$curVal=$fVal" >
              <xsl:attribute name="selected" value="selected"/>
            </xsl:if>
             <!-- workaround code needs to be removed after ucm is fixed -->
             <xsl:if test="($sData/MetaData = 'ROLE_MetaData.xml') and ($curVal = '2') and
                  (($sData/Config/record/param[@name=$objName]/@value='3') or 
                  ($sData/Config/record/param[@name=$objName]/@value='4'))">
              <xsl:attribute name="selected" value="selected"/>
            </xsl:if>
            <xsl:if test="($sData/MetaData = 'AROLE_MetaData.xml') and ($curVal = '0') and
                  (($sData/Config/record/param[@name=$objName]/@value='1') or 
                  ($sData/Config/record/param[@name=$objName]/@value='2'))">
              <xsl:attribute name="selected" value="selected"/>
            </xsl:if>
            <!-- workaround code end -->
          </xsl:if>
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
      </xsl:otherwise>
      </xsl:choose>
    </select>
  </xsl:template>        

   <xsl:template name="createIntOption">
      <xsl:param name="lowerRange" select="'Not Available'"/>
      <xsl:param name="upperRange" select="'Not Available'"/>
      <xsl:param name="frmType" select="'Not Available'" />
      <xsl:param name="sData" select="'Not Available'" />
      <xsl:param name="objName" select="'Not Available'" />

      <xsl:if test="$lowerRange &lt;= $upperRange">
          <option>
            <xsl:if test="$frmType='EDIT'">
              <xsl:variable name="fVal">
                <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
              </xsl:variable>

              <xsl:variable name="curVal">
                <xsl:value-of select="$lowerRange"/>
              </xsl:variable>
              <xsl:if test="$curVal=$fVal">
                <xsl:attribute name="selected" value="selected"/>
              </xsl:if>
            </xsl:if>

            <xsl:attribute name="value">
              <xsl:value-of select="$lowerRange"/>
            </xsl:attribute>
            <xsl:value-of select="$lowerRange"/> 
          </option>
          <xsl:call-template name="createIntOption">
             <xsl:with-param name="lowerRange" select="$lowerRange+1"/>
             <xsl:with-param name="upperRange" select="$upperRange"/>
             <xsl:with-param name="frmType" select="$frmType" />
             <xsl:with-param name="sData" select="$sData" />
             <xsl:with-param name="objName" select="$objName" />
          </xsl:call-template>
      </xsl:if>
   </xsl:template> 

</xsl:stylesheet>
