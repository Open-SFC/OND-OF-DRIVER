<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

 <xsl:include href="utilmeta.xsl" />

   <xsl:template name="popl_Div">
    <xsl:param name="mData" select="'Not Available'" />           
    <xsl:param name="objName" select="'Not Available'" />
    <xsl:param name="frmType" select="'Not Available'" />
    <xsl:param name="sData" select="'Not Available'" />
    <xsl:param name="cookieName" select="'Not Available'" />
    <xsl:param name="isExtFrmField" select="'Not Available'"/>
    <xsl:param name="parentKeys" select="'Not Available'"/>

    <div style="display:none">
      <xsl:variable name="divId">
        <xsl:text>oDefault</xsl:text>
        <xsl:text>_</xsl:text>
        <xsl:value-of select="$objName"/>
      </xsl:variable>
      <xsl:attribute name="id">
        <xsl:value-of select="$divId"/>
      </xsl:attribute>

      <xsl:for-each select="option">
        <xsl:variable name="innerDivId">
          <!--xsl:value-of select="$objName"/--><xsl:value-of select="$mData/obj[@name=$objName]/@oid"/><xsl:text>_</xsl:text>
          <xsl:value-of select="@val"/>
        </xsl:variable>

        <!-- POPULATE Hidden DIVs Here -->
        <div style="display:none">
          <xsl:attribute name="id">
            <xsl:value-of select="$innerDivId"/>
          </xsl:attribute>
          <xsl:choose>
            <xsl:when test="./@mtpath">
              <xsl:attribute name="mtpath">
                  <xsl:value-of select="@mtpath" />
              </xsl:attribute>
              <xsl:variable name="extMetaData" select="document(@mtpath)/root"/>
                <xsl:call-template name="popl_frm">
                  <xsl:with-param name="sData" select="$sData"/>
                  <xsl:with-param name="mData" select="$extMetaData"/>
                  <xsl:with-param name="frmType" select="$frmType"/>
                  <xsl:with-param name="cookieName" select="$cookieName" /> 
                  <xsl:with-param name="isVisible" select="'false'" /> 
                  <xsl:with-param name="isExtFrmRef" select="'true'" />
                  <xsl:with-param name="parentKeys" select="@pkeys"/>
                  <xsl:with-param name="dependsOn" select="@depends"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="poplFrmFields">
               <xsl:with-param name="sData" select="$sData" />
               <xsl:with-param name="mData" select="$mData" />
               <xsl:with-param name="frmType" select="$frmType" />
               <xsl:with-param name="cookieName" select="$cookieName" />
               <xsl:with-param name="isVisible" select="'false'" />
               <xsl:with-param name="isExtFrmField" select="$isExtFrmField" /> 
               <xsl:with-param name="parentKeys" select="$parentKeys"/>
               <xsl:with-param name="dependsOn" select="@depends"/>
             </xsl:call-template>
            </xsl:otherwise>
          </xsl:choose>
        </div>
       
      </xsl:for-each>
    </div>
  </xsl:template>

  <xsl:template name="popl_field">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'"/>
    <xsl:param name="isVisible" select="'Not Available'"/>
    <xsl:param name="isExtFrmField" select="'Not Available'" />
    <xsl:param name="parentKeys" select="'Not Available'" />
    <!--<xsl:value-of select="$frmType" /> -->
      
    <xsl:variable name="objID">
      <xsl:value-of select="@oid"/>
    </xsl:variable>
    <xsl:variable name="objName">
      <xsl:value-of select="$mData/obj[@oid=$objID]/@name|$mData/obj[@oid=$objID]/@lbl"/>
    </xsl:variable>
    <xsl:variable name="objLbl">
      <xsl:choose>
        <xsl:when test="$mData/obj[@oid=$objID]/@lbl">
          <xsl:value-of select="$mData/obj[@oid=$objID]/@lbl"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$mData/obj[@oid=$objID]/@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="range">
       <xsl:if test="$mData/obj[@oid=$objID]/range">
          <xsl:value-of select="$mData/obj[@oid=$objID]/range" />
       </xsl:if>
    </xsl:variable>
    <xsl:variable name="isMand">
      <xsl:choose>
        <xsl:when test="$mData/obj[@oid=$objID]/@mand='yes'">
            <xsl:text>true</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>false</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="noteStr">
      <xsl:value-of select="$mData/obj[@oid=$objID]/@note"/>
    </xsl:variable>
    <xsl:variable name="dataType">
        <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/>
    </xsl:variable>
    <xsl:choose>
      <!-- *****   TEXT FIELD TYPE    ***** -->
      <xsl:when test="not(@type)">
        <input style="WIDTH:250px;">
          <xsl:variable name="fieldName">
             <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
             <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/><xsl:text>$$</xsl:text>
             <xsl:value-of select="@oid"/>
          </xsl:variable>
          <xsl:attribute name="name">
            <xsl:value-of select="$fieldName"/>
          </xsl:attribute>
          <xsl:choose>
            <xsl:when test="@hide='true'">
              <xsl:attribute name="type">hidden</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="type">text</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>          
          <xsl:if test="$frmType='ADD'">
            <xsl:if test="$mData/obj[@oid=$objID]/@defVal">
                <xsl:attribute name="value">
                   <xsl:value-of select="$mData/obj[@oid=$objID]/@defVal"/>
                </xsl:attribute>
            </xsl:if>
          </xsl:if>
          <xsl:if test="$mData/obj[@oid=$objID]/range">
            <xsl:choose>
              <xsl:when test="$dataType='STR'">
                <xsl:attribute name="maxlength">
                  <xsl:value-of select="substring-after($mData/obj[@oid=$objID]/range,'-')"/>
                </xsl:attribute>
                <xsl:attribute name="title">
                  <xsl:text>String</xsl:text>
                  <xsl:text>; Maximum Length: </xsl:text>
                  <xsl:value-of select="substring-after($mData/obj[@oid=$objID]/range,'-')"/>
                </xsl:attribute>
              </xsl:when>
              <xsl:when test="$dataType='IPADDR'">
                <xsl:attribute name="maxlength">15</xsl:attribute>                
	        <xsl:attribute name="title"><xsl:text>IP Address</xsl:text></xsl:attribute>
              </xsl:when>
              <xsl:when test="$dataType='INT'">
                <xsl:variable name="maxLenStr">
                  <xsl:value-of select="substring-after($mData/obj[@oid=$objID]/range,'-')"/>	
                </xsl:variable>
                <xsl:attribute name="maxlength">
                  <xsl:value-of select="string-length($maxLenStr)"/>
                </xsl:attribute>
                <xsl:attribute name="title">
                  <xsl:text>Integer</xsl:text>
                  <xsl:text>; Range: </xsl:text><xsl:value-of select="$mData/obj[@oid=$objID]/range"/>
                </xsl:attribute>
              </xsl:when>
              <xsl:when test="$dataType='UINT'">
                <xsl:variable name="maxLenStr">
                  <xsl:value-of select="substring-after($mData/obj[@oid=$objID]/range,'-')"/>	
                </xsl:variable>
                <xsl:attribute name="maxlength">
                  <xsl:value-of select="string-length($maxLenStr)"/>
                </xsl:attribute>
                <xsl:attribute name="title">
                  <xsl:text>Unsigned Integer</xsl:text>
                  <xsl:text>; Range: </xsl:text><xsl:value-of select="$mData/obj[@oid=$objID]/range"/>
                </xsl:attribute>
              </xsl:when>
            </xsl:choose>
          </xsl:if>
              <!--xsl:if test="$dataType='UINT'">
                <xsl:attribute name="title">
                  <xsl:text>Unsigned Integer</xsl:text>                  
                </xsl:attribute>
              </xsl:if-->
          <xsl:if test="$frmType='EDIT'">
            <xsl:variable name="fldVal">
              <xsl:value-of select="$sData/Config/record/param[@name=$objName]/@value"/>
            </xsl:variable>
            <xsl:attribute name="value">
              <xsl:value-of select="$fldVal" />
            </xsl:attribute>
            <xsl:if test="$mData/obj[@oid=$objID]/@key='yes'">
              <xsl:attribute name="disabled" value="disabled"/>
            </xsl:if>
          </xsl:if>
          <xsl:if test="contains(@dmpath,'{0}')">
            <xsl:attribute name="onFocus">
              <xsl:text>dynamicList(this.name);</xsl:text>
            </xsl:attribute>
            <xsl:attribute name="dmpath">
              <xsl:value-of select="@dmpath"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:if test="contains(@depends,'+')">
            <xsl:attribute name="depends">
              <xsl:value-of select="@depends"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:if test="@dynamic='true'">
             <xsl:attribute name="dynamic">
               <xsl:text>true</xsl:text>
             </xsl:attribute>
             <xsl:attribute name="dmpath">
               <xsl:value-of select="@dmpath" />
             </xsl:attribute> 
             <xsl:if test="@ppath">
               <xsl:attribute name="ppath">
                 <xsl:value-of select="@ppath" />
               </xsl:attribute>
             </xsl:if>
          </xsl:if> 
        <xsl:if test="$isExtFrmField='true'">
           <xsl:attribute name="dmpath"> 
               <xsl:value-of select="$mData/dmpath" />
           </xsl:attribute>
           <xsl:if test="$parentKeys">
              <xsl:attribute name="parentkeys">
                <xsl:value-of select="$parentKeys" />
              </xsl:attribute>
           </xsl:if>
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
        </xsl:if>
        <xsl:attribute name="visible">
            <xsl:value-of select="$isVisible"/>
        </xsl:attribute>
        <xsl:if test="$isVisible='true'">
        <script>
           <xsl:text>define('</xsl:text><xsl:value-of select="$fieldName"/><xsl:text>',
                             '</xsl:text><xsl:value-of select="$objLbl"/><xsl:text>',
                             '</xsl:text><xsl:value-of select="$dataType"/><xsl:text>',
                             '</xsl:text><xsl:value-of select="$range"/><xsl:text>',
                 '</xsl:text><xsl:value-of select="$isMand"/><xsl:text>');</xsl:text>
        </script>
        </xsl:if>
        </input>
      </xsl:when>
      <xsl:when test="@type='textarea'">
        <!-- *****   TEXTAREA FIELD TYPE  ***** -->
        <xsl:call-template name="gen_TextArea">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='radio'">
        <!-- *****   RADIO GROUP FIELD TYPE  ***** -->
        <xsl:call-template name="gen_RadioBtn">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
        <xsl:call-template name="popl_Div">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="cookieName" select="$cookieName" />
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='password'">
        <!-- *****   PASSWORD FIELD TYPE  ***** -->
        <xsl:call-template name="gen_Password">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='label'">
        <!-- *****   LABEL FIELD TYPE  ***** -->
        <xsl:call-template name="gen_lbl">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='selcrtl'">
        <!-- Selection Control start -->
        <xsl:call-template name="genSelCrtl">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='file'">
        <!-- *****   FILE FIELD TYPE  ***** -->
        <xsl:call-template name="gen_fileinput">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='checkbox'">
        <!-- *****   Check Box FIELD TYPE ***** -->
        <xsl:call-template name="gen_CheckBox">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="cookieName" select="$cookieName"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
        <xsl:call-template name="popl_Div">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="cookieName" select="$cookieName" />
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='list'">
        <!-- *****   LIST GROUP FIELD TYPE  ***** -->
        <xsl:call-template name="gen_List">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
        <xsl:call-template name="popl_Div">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="cookieName" select="$cookieName" />
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="@type='mlist'">
        <!-- *****   MULTIPLE LIST GROUP FIELD TYPE  ***** -->
        <xsl:call-template name="gen_mList">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objID" select="$objID"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="isVisible" select="$isVisible"/>
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
        <xsl:call-template name="popl_Div">
          <xsl:with-param name="mData" select="$mData"/>
          <xsl:with-param name="objName" select="$objName"/>
          <xsl:with-param name="frmType" select="$frmType"/>
          <xsl:with-param name="sData" select="$sData"/>
          <xsl:with-param name="cookieName" select="$cookieName" />
          <xsl:with-param name="isExtFrmField" select="$isExtFrmField"/>
          <xsl:with-param name="parentKeys" select="$parentKeys"/>
        </xsl:call-template>
      </xsl:when>     
    </xsl:choose>
    <i>&#160;&#160;<xsl:value-of select="@note"/></i>
  </xsl:template>

  <xsl:template name="processRow">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'"/>
    <xsl:param name="isVisible" select="'Not Available'" />
    <tr height="1">
      <xsl:for-each select="child::node()[name()!='']">
        <td class="tdlandscape" width="10%" height="1">
          <xsl:if test="name()='field'">
            <xsl:call-template name="popl_field">
              <xsl:with-param name="sData" select="$sData" />
              <xsl:with-param name="mData" select="$mData" />
              <xsl:with-param name="frmType" select="$frmType" />
              <xsl:with-param name="cookieName" select="$cookieName" />
              <xsl:with-param name="isVisible" select="$isVisible" />
              <xsl:with-param name="isExtFrmField" select="'false'" />
            </xsl:call-template>
          </xsl:if>
          <xsl:if test="name()='link'">
            <!-- ***** Process Link ***** -->
            <xsl:call-template name="processLink">
              <xsl:with-param name="mData" select="$mData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="sData" select="$sData"/>
              <xsl:with-param name="tblID" select="''"/>
            </xsl:call-template>
          </xsl:if>
          <xsl:for-each select="child::node()[name()!='']">
            <xsl:choose>
              <xsl:when test="name()='field'">
                <xsl:call-template name="popl_field">
                  <xsl:with-param name="sData" select="$sData" />
                  <xsl:with-param name="mData" select="$mData" />
                  <xsl:with-param name="frmType" select="$frmType" />
                  <xsl:with-param name="cookieName" select="$cookieName" />
                  <xsl:with-param name="isVisible" select="$isVisible" />
                  <xsl:with-param name="isExtFrmField" select="'false'" />
                </xsl:call-template>
              </xsl:when>
              <xsl:when test="name()='link'">
                <!-- ***** Process Link ***** -->
                <xsl:call-template name="processLink">
                  <xsl:with-param name="mData" select="$mData"/>
                  <xsl:with-param name="srcParm" select="''"/>
                  <xsl:with-param name="cookie" select="$cookieName"/>
                  <xsl:with-param name="sData" select="$sData"/>
                  <xsl:with-param name="tblID" select="''"/>
                </xsl:call-template>
              </xsl:when>
            </xsl:choose>
          </xsl:for-each>
        </td>
      </xsl:for-each>
    </tr>
  </xsl:template>

  <xsl:template name="poplFrmFields">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'" />
    <xsl:param name="isVisible" select="'Not Available'" />
    <xsl:param name="isExtFrmField" select="'Not Available'" />
    <xsl:param name="parentKeys" select="'Not Available'" />
    <xsl:param name="dependsOn" select="'Not Available'"/>

      <xsl:variable name="tblId">
        <xsl:value-of select="generate-id()"/>       
      </xsl:variable>

    <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="100%" height="1">
      <xsl:for-each select="child::node()[name()!='']">
        <xsl:choose>
          <xsl:when test="name()='field'">
            <xsl:variable name="objID">
              <xsl:value-of select="@oid"/>
            </xsl:variable>        
            <xsl:variable name="objLbl">
              <xsl:choose>
              <xsl:when test="$mData/obj[@oid=$objID]/@lbl">
                <xsl:value-of select="$mData/obj[@oid=$objID]/@lbl"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$mData/obj[@oid=$objID]/@name"/>
              </xsl:otherwise>
              </xsl:choose>
            </xsl:variable>        
            <xsl:variable name="objName">
              <xsl:value-of select="$mData/obj[@oid=$objID]/@name|$mData/obj[@oid=$objID]/@lbl"/>
            </xsl:variable>
            <xsl:variable name="fieldName">
               <xsl:value-of select="$objName"/><xsl:text>#</xsl:text>
               <xsl:value-of select="$mData/obj[@oid=$objID]/@type"/><xsl:text>$$</xsl:text>
               <xsl:value-of select="@oid"/>
            </xsl:variable>
            <xsl:choose> 
              <xsl:when test="@hide='true'">
                <xsl:call-template name="popl_field">
                  <xsl:with-param name="sData" select="$sData" />
                  <xsl:with-param name="mData" select="$mData" />
                  <xsl:with-param name="frmType" select="$frmType" />
                  <xsl:with-param name="cookieName" select="$cookieName" />
                  <xsl:with-param name="isVisible" select="$isVisible" />
                  <xsl:with-param name="isExtFrmField" select="$isExtFrmField" />
                  <xsl:with-param name="parentKeys" select="$parentKeys" />
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:if test="contains($parentKeys,$objName)!='true'">
                  <tr class="trform">
                    <td width="25%" class="tdformlabel">
                      <xsl:value-of select="$objLbl"/>
                      <xsl:if test="$mData/obj[@oid=$objID]/@mand='yes'">
                          <span class="error-msg">*</span>
                      </xsl:if>
                    </td>
                    <!--td width="1%" class="tdform">
                      <xsl:text>:</xsl:text>
                    </td-->
                    <td width="75%" class="tdform">
                    <xsl:call-template name="popl_field">
                      <xsl:with-param name="sData" select="$sData" />
                      <xsl:with-param name="mData" select="$mData" />
                      <xsl:with-param name="frmType" select="$frmType" />
                      <xsl:with-param name="cookieName" select="$cookieName" />
                      <xsl:with-param name="isVisible" select="$isVisible" />
                      <xsl:with-param name="isExtFrmField" select="$isExtFrmField" />
                      <xsl:with-param name="parentKeys" select="$parentKeys" />
                    </xsl:call-template>
                    </td>
                  </tr>
                </xsl:if>
              </xsl:otherwise>
            </xsl:choose> 
          </xsl:when>
          <xsl:when test="name()='link'">
            <tr height="25"><td colspan="3" valign="bottom">
            <!-- ***** Process Link ***** -->
            <xsl:call-template name="processLink">
              <xsl:with-param name="mData" select="$mData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="sData" select="$sData"/>
              <xsl:with-param name="tblID" select="''"/>
            </xsl:call-template>
            </td>
           </tr>
          </xsl:when>
          <xsl:when test="name()='grp' and @style='table'"> 
                  <tr><td colspan="3">
                    <b><xsl:value-of select="@title"/></b>
                   </td>
                  </tr>
                  <tr><td colspan="3">
                  <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="100%" height="1">         
            <xsl:call-template name="processGrpTable">
              <xsl:with-param name="mData" select="$mData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="sData" select="$sData"/>
              <xsl:with-param name="tblID" select="$tblId"/>
              <xsl:with-param name="depends" select="$dependsOn"/>
              <xsl:with-param name="style" select="'table'"/>
            </xsl:call-template>   
               </table>
                  </td>
                  </tr>      
          </xsl:when>
          <xsl:when test="name()='grp' and @style='form'"> 
                  <tr><td colspan="3">
                    <b><xsl:value-of select="@title"/></b>
                   </td>
                  </tr>
                  <tr><td colspan="3">
                  <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="100%" height="1">         
            <xsl:call-template name="processGrpTable">
              <xsl:with-param name="mData" select="$mData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="sData" select="$sData"/>
              <xsl:with-param name="tblID" select="$tblId"/>
              <xsl:with-param name="depends" select="$dependsOn"/>
              <xsl:with-param name="style" select="'form'"/>
            </xsl:call-template>   
               </table>
                  </td>
                  </tr>      
          </xsl:when>
          <xsl:when test="name()='grp' and @style='landscape'">
             <xsl:choose>
               <xsl:when test="@ftyp">
                 <xsl:if test="@ftyp=$frmType">
                  <!--br/-->
                  <tr><td colspan="3">
                    <b><xsl:value-of select="@title" /></b>
                   </td>
                  </tr>
                  <tr><td colspan="3">
                  <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="100%" height="1">
                    <xsl:for-each select="child::node()[name()!='']">
                      <xsl:call-template name="processRow">
                        <xsl:with-param name="sData" select="$sData" />
                        <xsl:with-param name="mData" select="$mData" />
                        <xsl:with-param name="frmType" select="$frmType" />
                        <xsl:with-param name="cookieName" select="$cookieName" />
                        <xsl:with-param name="isVisible" select="$isVisible" />
                      </xsl:call-template>
                    </xsl:for-each>
                  </table>
                  </td>
                  </tr>
                  </xsl:if>
               </xsl:when>
               <xsl:otherwise>
                  <!--br/-->
                  <tr><td colspan="3">
                  <b><xsl:value-of select="@title" /></b>
                  </td>
                  </tr>
                  <tr><td colspan="3">
                  <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="100%" height="1">
                    <xsl:for-each select="child::node()[name()!='']">
                      <xsl:call-template name="processRow">
                        <xsl:with-param name="sData" select="$sData" />
                        <xsl:with-param name="mData" select="$mData" />
                        <xsl:with-param name="frmType" select="$frmType" />
                        <xsl:with-param name="cookieName" select="$cookieName" />
                        <xsl:with-param name="isVisible" select="$isVisible" />
                      </xsl:call-template>
                    </xsl:for-each>
                  </table>
                  </td></tr>
               </xsl:otherwise>
             </xsl:choose>
          </xsl:when>
          <xsl:when test="name()='grp' and @style='portrait'">
            <xsl:choose>
              <xsl:when test="@ftyp">
                <xsl:if test="@ftyp=$frmType">
                  <tr><td colspan="3">
                  <xsl:choose>
                    <xsl:when test="@title and @act='expcol'">
                      <div class="addRecord">
                        <a href="#">
                        <xsl:attribute name="onclick">
                          <xsl:text>toggleContent('</xsl:text>
                          <xsl:value-of select="@name" />
                          <xsl:text>', 'yes');</xsl:text>
                        </xsl:attribute>
                        <b><xsl:value-of select="@title" /></b>
                      </a></div>
                    </xsl:when>
                    <xsl:when test="@title and not(@act)">
                      <b><xsl:value-of select="@title" /></b>
                    </xsl:when>
                  </xsl:choose>
                  <div>
                    <xsl:if test="@act='expcol'">
                      <xsl:attribute name="style" >
                        <xsl:text>display:none</xsl:text>
                      </xsl:attribute>
                    </xsl:if>
                    <xsl:if test="@clr">
                      <xsl:attribute name="clr" >
                        <xsl:value-of select="@clr"/>
                      </xsl:attribute>
                    </xsl:if>
                      <xsl:variable name="childGrp" select="document(@mtpath)/root"/>
                      <xsl:variable name="childGrpTyp" select="$childGrp/form/@frmType"/>
                      <xsl:if test="$childGrpTyp='grpscalar'">
                        <xsl:attribute name="grppath">
                          <xsl:value-of select="$childGrp/dmpath" />
                        </xsl:attribute>
                        <xsl:if test="@pkeys != ''">
                          <xsl:attribute name="parentkeys">
                            <xsl:value-of select="@pkeys" />
                          </xsl:attribute>
                        </xsl:if>
                        <xsl:if test="$cookieName !=''">
                          <xsl:attribute name="inst">
                            <xsl:value-of select="$cookieName" />
                          </xsl:attribute>
                        </xsl:if>
                      </xsl:if>
                    <xsl:attribute name="id">
                      <xsl:value-of select="@name" />
                    </xsl:attribute>
                    <xsl:variable name="isVisi">
                    <xsl:choose>
                       <xsl:when test="@act='expcol'">
                            <xsl:value-of select="'false'"/>
                       </xsl:when>
                       <xsl:otherwise>
                            <xsl:value-of select="'true'"/>
                       </xsl:otherwise>
                    </xsl:choose>
                    </xsl:variable>
                    <xsl:choose>
                    <xsl:when test="./@mtpath">
                      <xsl:attribute name="mtpath">
                         <xsl:value-of select="@mtpath" />
                      </xsl:attribute>
                      <xsl:variable name="extMetaData" select="document(@mtpath)/root"/>
                      <xsl:call-template name="popl_frm">
                         <xsl:with-param name="sData" select="$sData"/>
                         <xsl:with-param name="mData" select="$extMetaData"/>
                         <xsl:with-param name="frmType" select="$frmType"/>
                         <xsl:with-param name="cookieName" select="$cookieName" /> 
                         <xsl:with-param name="isVisible" select="$isVisi" />
                         <xsl:with-param name="isExtFrmRef" select="'true'" /> 
                         <xsl:with-param name="parentKeys" select="@pkeys"/>
                         <xsl:with-param name="dependsOn" select="@depends"/>
                      </xsl:call-template>
                    </xsl:when>
                    <xsl:otherwise>
                    <xsl:call-template name="poplFrmFields">
                      <xsl:with-param name="sData" select="$sData" />
                      <xsl:with-param name="mData" select="$mData" />
                      <xsl:with-param name="frmType" select="$frmType" />
                      <xsl:with-param name="cookieName" select="$cookieName" />
                      <xsl:with-param name="isVisible" select="$isVisi" />
                      <xsl:with-param name="isExtFrmField" select="$isExtFrmField" /> 
                      <xsl:with-param name="parentKeys" select="$parentKeys"/>
                      <xsl:with-param name="dependsOn" select="@depends"/>
                    </xsl:call-template>
                    </xsl:otherwise>
                    </xsl:choose>
                  </div>
                  <xsl:if test="$frmType = 'EDIT'">
                    <xsl:choose>
                       <xsl:when test="@name != ''">
                    <script>
                      <xsl:text>toggleContent('</xsl:text>
                      <xsl:value-of select="@name" />
                      <xsl:text>', 'no');</xsl:text>
                    </script>
                       </xsl:when>
                    </xsl:choose> 
                  </xsl:if>
                  </td></tr>
                </xsl:if>
              </xsl:when>
              <xsl:when test="not(@ftyp)">
                  <tr><td colspan="3">
                  <xsl:choose>
                    <xsl:when test="@title and @act='expcol'">
                      <xsl:variable name="iconId" select="@name" />              
                      <span style="position:relative;" class="addRecord">
                        <xsl:attribute name="onclick">
                          <xsl:text>toggleContent('</xsl:text>
                          <xsl:value-of select="@name" />
                          <xsl:text>', 'yes');</xsl:text>
                        </xsl:attribute>
                        <xsl:if test="@act='expcol' and @expand='true'">
                          <img src="images/Lminus.gif" border="0" align="absMiddle" id="icon{$iconId}"/>
                        </xsl:if>
                        <xsl:if test="@act='expcol' and not(@expand)">
                          <img src="images/Lplus.gif" border="0" align="absMiddle" id="icon{$iconId}"/>
                        </xsl:if>
                        <a href="#">
                        <xsl:value-of select="@title"/>
                      </a></span>
                    </xsl:when>
                    <xsl:when test="@title and not(@act)">
                      <b><xsl:value-of select="@title" /></b>
                    </xsl:when>
                  </xsl:choose>
                  <div class="grpDiv">
                    <xsl:if test="@act='expcol' and @expand='true'">
                      <xsl:attribute name="style" >
                        <xsl:text>display:block</xsl:text>
                      </xsl:attribute>
                    </xsl:if>
                    <xsl:if test="@act='expcol' and not(@expand)">
                      <xsl:attribute name="style" >
                        <xsl:text>display:none</xsl:text>
                      </xsl:attribute>
                    </xsl:if>
                    <xsl:if test="@clr">
                      <xsl:attribute name="clr" >
                        <xsl:value-of select="@clr"/>
                      </xsl:attribute>
                    </xsl:if>
                    <xsl:variable name="childGrp" select="document(@mtpath)/root"/>
                    <xsl:variable name="childGrpTyp" select="$childGrp/form/@frmType"/>
                    <xsl:if test="$childGrpTyp ='grpscalar'">
                        <xsl:attribute name="grppath">
                          <xsl:value-of select="$childGrp/dmpath" />
                        </xsl:attribute>
                        <xsl:if test="@pkeys != ''">
                          <xsl:attribute name="parentkeys">
                            <xsl:value-of select="@pkeys" />
                          </xsl:attribute>
                        </xsl:if>
                        <xsl:if test="$cookieName !=''">
                          <xsl:attribute name="inst">
                            <xsl:value-of select="$cookieName" />
                          </xsl:attribute>
                        </xsl:if>
                      </xsl:if>
                    <xsl:attribute name="id">
                      <xsl:value-of select="@name" />
                    </xsl:attribute>
                    <xsl:variable name="isVisi">
                    <xsl:choose>
                       <xsl:when test="@act='expcol' and @expand='true'">
                            <xsl:value-of select="'true'"/>
                       </xsl:when>                    
                       <xsl:when test="@act='expcol' and not(@expand)">
                            <xsl:value-of select="'false'"/>
                       </xsl:when>
                       <xsl:otherwise>
                            <xsl:value-of select="'true'"/>
                       </xsl:otherwise>
                    </xsl:choose>
                    </xsl:variable>
                    <xsl:choose>
                    <xsl:when test="./@mtpath">
                      <xsl:attribute name="mtpath">
                         <xsl:value-of select="@mtpath" />
                      </xsl:attribute>
                      <xsl:variable name="extMetaData" select="document(@mtpath)/root"/>
                      <xsl:call-template name="popl_frm">
                         <xsl:with-param name="sData" select="$sData"/>
                         <xsl:with-param name="mData" select="$extMetaData"/>
                         <xsl:with-param name="frmType" select="$frmType"/>
                         <xsl:with-param name="cookieName" select="$cookieName" /> 
                         <xsl:with-param name="isVisible" select="$isVisi" />
                         <xsl:with-param name="isExtFrmRef" select="'true'" /> 
                         <xsl:with-param name="parentKeys" select="@pkeys"/>
                         <xsl:with-param name="dependsOn" select="@depends"/>
                      </xsl:call-template>
                    </xsl:when>
                    <xsl:otherwise>
                    <xsl:call-template name="poplFrmFields">
                      <xsl:with-param name="sData" select="$sData" />
                      <xsl:with-param name="mData" select="$mData" />
                      <xsl:with-param name="frmType" select="$frmType" />
                      <xsl:with-param name="cookieName" select="$cookieName" />
                      <xsl:with-param name="isVisible" select="$isVisi" />
                      <xsl:with-param name="isExtFrmField" select="$isExtFrmField" /> 
                      <xsl:with-param name="parentKeys" select="$parentKeys"/>
                      <xsl:with-param name="dependsOn" select="@depends"/>
                    </xsl:call-template>
                    </xsl:otherwise>
                    </xsl:choose>
                  </div>
                  <xsl:if test="$frmType = 'EDIT' and @act='expcol'">
                    <xsl:choose>
                       <xsl:when test="@name != ''">
                    <script>
                      <xsl:text>toggleContent('</xsl:text>
                      <xsl:value-of select="@name" />
                      <xsl:text>', 'no');</xsl:text>
                    </script>
                       </xsl:when>
                    </xsl:choose>
                  </xsl:if>
                  </td></tr>
              </xsl:when>
              <xsl:otherwise>
                <xsl:if test="@title">
                  <tr><td colspan="3">
                    <b><xsl:value-of select="@title" /></b>
                   </td>
                  </tr>
                </xsl:if>
                <tr><td colspan="3">
                  <xsl:call-template name="poplFrmFields">
                    <xsl:with-param name="sData" select="$sData" />
                    <xsl:with-param name="mData" select="$mData" />
                    <xsl:with-param name="frmType" select="$frmType" />
                    <xsl:with-param name="cookieName" select="$cookieName" />
                    <xsl:with-param name="isVisible" select="'true'" />
                    <xsl:with-param name="isExtFrmField" select="$isExtFrmField" />
                    <xsl:with-param name="parentKeys" select="$parentKeys"/>
                    <xsl:with-param name="dependsOn" select="@depends"/>
                  </xsl:call-template>
                </td></tr>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>        
        </xsl:choose>
      </xsl:for-each>
      </table>
  </xsl:template>

  <xsl:template name="refreshTmpl">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'" />

    <xsl:text>setCookieValue('</xsl:text>
      <xsl:value-of select="$sData/dmpath"/>
      <xsl:text>','BYKEY</xsl:text>
      <xsl:text>','</xsl:text>  
      <xsl:value-of select="@mtpath"/>
      <xsl:text>','</xsl:text>  
      <xsl:choose>
        <xsl:when test="not(@spg)">
          <xsl:text>tbl_meta.xsl</xsl:text>
        </xsl:when>
        <xsl:otherwise>
	  <xsl:value-of select="@spg"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>','</xsl:text>  
      <!--Enable if required in future
      <xsl:for-each select="key">
        <xsl:value-of select="@name"/>
        <xsl:text>^</xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:text>!</xsl:text>
        <xsl:value-of select="@value"/>
        <xsl:text>|</xsl:text>
      </xsl:for-each>-->
      <xsl:text>','</xsl:text>
      <xsl:value-of select="$cookieName"/>
      <xsl:text>','</xsl:text>  
      <xsl:text>','</xsl:text>
      <xsl:text>');</xsl:text>
  </xsl:template>

  <xsl:template name="procssGenrlAtribs">
    <xsl:param name="sdata" select="'Not Available'" />
    <xsl:param name="mtpat" select="'Not Available'" />
    <xsl:param name="cookieName" select="'Not Available'" />
    <xsl:param name="tblID" select="'Not Available'" />

    <xsl:if test="@dmpath">
      <!--xsl:value-of select="@dmpath" /><xsl:text>aaa</xsl:text> <xsl:value-of select="$mtpat/dmpath"/-->
      <!--xsl:choose>
        <xsl:when test="@dmpath=$mtpat/dmpath">
          <xsl:attribute name="dmpath">
            <xsl:value-of select="$sdata/dmpath"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="dmpath">
            <xsl:value-of select="@dmpath"/>
          </xsl:attribute>
        </xsl:otherwise>
      </xsl:choose-->
      <xsl:attribute name="dmpath">
        <xsl:value-of select="@dmpath"/>
      </xsl:attribute>
      <xsl:if test="@action">
        <xsl:attribute name="action">
          <xsl:value-of select="@action"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@opcode">
        <xsl:attribute name="opcode">
          <xsl:value-of select="@opcode"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@key">
        <xsl:attribute name="key">
          <xsl:value-of select="@key"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@value">
        <xsl:attribute name="val">
          <xsl:value-of select="@value"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@mtpath">
        <xsl:attribute name="mtpath">
          <xsl:value-of select="@mtpath"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="onclick">
        <xsl:text>submitLinkOrBtnReq(this,'</xsl:text>
        <xsl:value-of select="$cookieName"/><xsl:text>','</xsl:text>
        <xsl:value-of select="$tblID"/>
        <xsl:text>');</xsl:text>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>

  <xsl:template name="processBtn">
    <xsl:param name="sData" select="'Not Available'"/>
    <xsl:param name="mData" select="'Not Available'"/>
    <xsl:param name="frmType" select="'Not Available'"/>
    <xsl:param name="cookieName" select="'Not Available'" />
    <xsl:param name="tblID" select="'Not Available'" />

    <xsl:choose>
      <xsl:when test="@type='submit'">
        <xsl:variable name="vrtInstVar">
          <xsl:choose>
            <xsl:when test="$mData/vrtlInst">
              <xsl:text>yes</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>no</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
          </xsl:variable>
          <xsl:variable name="applyOpt">
            <xsl:choose>
              <xsl:when test="@action">
                <xsl:text>'</xsl:text>
                <xsl:value-of select="@action" />
                <xsl:text>'</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>''</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:variable>
          <input type="submit" name="Submit" value="Apply">
            <xsl:attribute name="onclick">
              <xsl:text>sendReq('</xsl:text> 
		    <xsl:value-of select="$cookieName"/>
	  	    <xsl:text>','</xsl:text>
		    <xsl:value-of select="$vrtInstVar"/>
	  	    <xsl:text>',</xsl:text>
                    <xsl:value-of select="$applyOpt" />
	  	    <xsl:text>,'</xsl:text>
                    <xsl:for-each select="$mData/obj[@key='yes']">
                      <xsl:variable name="keyName" select="@name"/>
                        <xsl:value-of select="$keyName"/>
                        <xsl:text>^</xsl:text>
                        <xsl:value-of select="@type"/>
                        <xsl:text>!nil</xsl:text>
                        <xsl:text>|</xsl:text>
                    </xsl:for-each>
	  	    <xsl:text>')</xsl:text>
	    </xsl:attribute>	
          </input>&#160;
      </xsl:when>
      <xsl:when test="@type='general'">
          <input type="button">
            <xsl:call-template name="procssGenrlAtribs">
              <xsl:with-param name="sdata" select="$sData" />
              <xsl:with-param name="mtpat" select="$mData" />
              <xsl:with-param name="cookieName" select="$cookieName" />
              <xsl:with-param name="tblID" select="$tblID" />
            </xsl:call-template>
            <xsl:attribute name="tblID">
               <xsl:value-of select="$tblID"/>
            </xsl:attribute>
            <xsl:attribute name="value">
              <xsl:if test="string-length(@lbl) = 0">
               <xsl:value-of select="@name"/>
              </xsl:if>
              <xsl:if test="string-length(@lbl) != 0">
               <xsl:value-of select="@lbl"/>
              </xsl:if>
            </xsl:attribute>
         </input>&#160;
      </xsl:when>
      <xsl:when test="@type='refresh'">
         <input type="button">
            <xsl:attribute name="onclick">
              <!--xsl:call-template name="refreshTmpl">
                <xsl:with-param name="sData" select="$sData" />
                <xsl:with-param name="mData" select="$mData" />
                <xsl:with-param name="frmType" select="$frmType" />
                <xsl:with-param name="cookieName" select="$cookieName" />
              </xsl:call-template-->
              refreshTable('<xsl:value-of select="$tblID"/>', '');
            </xsl:attribute>
            <xsl:attribute name="tblID">
               <xsl:value-of select="$tblID"/>
            </xsl:attribute>
            <xsl:attribute name="value">
              <xsl:if test="string-length(@lbl) = 0">
               <xsl:value-of select="@name"/>
              </xsl:if>
              <xsl:if test="string-length(@lbl) != 0">
               <xsl:value-of select="@lbl"/>
              </xsl:if>
            </xsl:attribute>
         </input>&#160;
      </xsl:when>
    </xsl:choose>
  </xsl:template>

<xsl:template name="processTblLink">
  <xsl:param name="metaData" select="'Not Available'" />
  <xsl:param name="srcParm" select="'Not Available'" />
  <xsl:param name="cookie" select="'Not Available'" />
  <xsl:param name="srcData" select="'Not Available'" />
  <xsl:param name="tblID" select="'Not Available'" />

  <xsl:for-each select="col/link[@target='ldiv']">
    <xsl:apply-templates select="."/>
    <br/>
    <xsl:variable name="subDivId">
      <xsl:value-of select="substring-before(@mtpath,'.')" />
      <xsl:text>_</xsl:text>
      <xsl:value-of select="$tblID"/>
    </xsl:variable>
    <div style="display:none;padding-left:5px;">
      <xsl:attribute name="id">
        <xsl:value-of select="$subDivId"/>
      </xsl:attribute>
      <xsl:value-of select="@mtpath"/>
      <iframe align="center" src="" width="100%" height="300" frameborder="no">
        <xsl:attribute name="name">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="id">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
      </iframe>
    </div>
  </xsl:for-each>
</xsl:template>

<xsl:template name="processGrpTable">
  <xsl:param name="metaData" select="'Not Available'" />
  <xsl:param name="srcParm" select="'Not Available'" />
  <xsl:param name="cookie" select="'Not Available'" />
  <xsl:param name="srcData" select="'Not Available'" />
  <xsl:param name="tblID" select="'Not Available'" />
  <xsl:param name="depends" select="'Not Available'" />
  <xsl:param name="style" select="'Not Available'" />
  
    <xsl:variable name="subDivId">
      <xsl:value-of select="substring-before(@mtpath,'.')" />
      <xsl:text>_</xsl:text>
      <xsl:value-of select="$tblID"/>
    </xsl:variable>
    <div style="display:block;padding-left:5px;">
      <xsl:attribute name="id">
        <xsl:value-of select="$subDivId"/>
      </xsl:attribute>
      <!--xsl:value-of select="@mtpath"/-->
      <iframe align="center" src="" width="100%" height="300" frameborder="yes">
        <xsl:attribute name="name">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="id">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="mtpath">
          <xsl:value-of select="@mtpath"/>
        </xsl:attribute>
        <xsl:attribute name="dmpath">
          <xsl:value-of select="@dmpath"/>
        </xsl:attribute>
        <xsl:attribute name="srcParm">
          <xsl:value-of select="$srcParm"/>
        </xsl:attribute>
        <xsl:attribute name="cookie">
          <xsl:value-of select="$cookie"/>
        </xsl:attribute>
        <xsl:attribute name="srcData">
          <xsl:value-of select="$srcData"/>
        </xsl:attribute>
        <xsl:attribute name="tblID">
          <xsl:value-of select="$tblID"/>
        </xsl:attribute>
        <xsl:attribute name="pkeys">
          <xsl:value-of select="@pkeys"/>
        </xsl:attribute>
        <xsl:attribute name="depends">
          <xsl:value-of select="@depends"/>
        </xsl:attribute>
        <xsl:attribute name="disStyle">
          <xsl:value-of select="$style"/>
        </xsl:attribute>
      </iframe>
    </div>

      <!--xsl:variable name="mtdoc" select="document(@mtpath)/root"/>
      <xsl:for-each select="$mtdoc/form/grp[@target='ldiv']">
        <xsl:apply-templates select="."/>
      </xsl:for-each-->
  
  <!--xsl:for-each select="col/link[@target='ldiv']">
    <xsl:apply-templates select="."/>
    <br/>
    <xsl:variable name="subDivId">
      <xsl:value-of select="substring-before(@mtpath,'.')" />
      <xsl:text>_</xsl:text>
      <xsl:value-of select="$tblID"/>
    </xsl:variable>
    <div style="display:none;padding-left:5px;">
      <xsl:attribute name="id">
        <xsl:value-of select="$subDivId"/>
      </xsl:attribute>
      <xsl:value-of select="@mtpath"/>
      <iframe align="center" src="" width="100%" height="300" frameborder="no">
        <xsl:attribute name="name">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="id">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
      </iframe>
    </div>
  </xsl:for-each-->
</xsl:template>

<xsl:template match="link">
    <xsl:variable name="subDivId">
      <xsl:value-of select="substring-before(@mtpath,'.')" />
    </xsl:variable>
       <!--xsl:value-of select="@name"/-->
    <div style="display:none;">
      <xsl:attribute name="id">
        <xsl:value-of select="$subDivId"/>
      </xsl:attribute>
      <!--xsl:value-of select="@mtpath"/-->
      <iframe align="center" src="" width="100%" onload="rsizeIfrmToFit(this);" frameborder="no">
        <xsl:attribute name="name">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="id">
          <xsl:value-of select="$subDivId"/><xsl:text>_ifrm</xsl:text>
        </xsl:attribute>
      </iframe>
      <xsl:variable name="mtdoc" select="document(@mtpath)/root"/>
      <xsl:for-each select="$mtdoc/tbl/col/link[@target='ldiv']">
        <xsl:apply-templates select="."/>
      </xsl:for-each>
    </div>
</xsl:template>

<xsl:template name="processLink">
    <xsl:param name="metaData" select="'Not Available'" />
    <xsl:param name="srcParm" select="'Not Available'" />
    <xsl:param name="cookie" select="'Not Available'" />
    <xsl:param name="srcData" select="'Not Available'" />
    <xsl:param name="tblID" select="'Not Available'" />
    
    <xsl:choose>
      <xsl:when test="@name='general'">
       <div class="addRecord" id="addRecordDiv">
        <a>
          <xsl:call-template name="procssGenrlAtribs">
            <xsl:with-param name="sdata" select="$srcData" />
            <xsl:with-param name="mtpat" select="$metaData" />
            <xsl:with-param name="cookieName" select="$cookie" />
            <xsl:with-param name="tblID" select="$tblID" />
          </xsl:call-template>
          <xsl:attribute name="href">
            <xsl:choose>
              <xsl:when test="not(@spg)">
                <xsl:text>#</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="@spg"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
          <xsl:attribute name="tblID">
            <xsl:value-of select="$tblID"/>
          </xsl:attribute>
          <xsl:value-of select="@lbl"/>
        </a>
        </div>
      </xsl:when>
      <xsl:when test="@name='Reboot'">
        <xsl:variable name="obName">
          <xsl:value-of select="$metaData/obj[@key='yes']/@name"/>
        </xsl:variable>
        <xsl:variable name="obVal">
          <xsl:value-of select="$srcParm[@name=$obName]/@value" />
        </xsl:variable>
        <xsl:text>[</xsl:text>
        <a href="#">
          <xsl:attribute name="onclick">
            <xsl:text>rebootAction('</xsl:text>
              <xsl:value-of select="@dmpath" />
            <xsl:text>','</xsl:text>
              <xsl:value-of select="$obVal" />
            <xsl:text>');</xsl:text>
          </xsl:attribute>
          <xsl:value-of select="@lbl"/>
        </a>
        <xsl:text>]</xsl:text>
      </xsl:when>
      <xsl:when test="@name='refresh' and @action='auto'">
        <xsl:variable name="opt">
          <xsl:choose>
            <xsl:when test="@opcode">
              <xsl:value-of select="@opcode" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>BYKEY</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="spg">
          <xsl:choose>
            <xsl:when test="@spg">
              <xsl:value-of select="@spg" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>tbl_meta.xsl</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="intrvl">
          <xsl:choose>
            <xsl:when test="@intrvl">
              <xsl:value-of select="@intrvl" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>120000</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="keys">
          <xsl:choose>
            <xsl:when test="@keys">
              <xsl:value-of select="@keys" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:for-each select="$metaData/obj[@key='yes']">
                <xsl:value-of select="@name"/>
                <xsl:text>^</xsl:text>
		<xsl:value-of select="@type"/>
                <xsl:text>!</xsl:text>
                <xsl:text>nil</xsl:text>
                <xsl:text>|</xsl:text>
              </xsl:for-each>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <script language="JavaScript">
          autoRefreshPage('<xsl:value-of select="$srcData/dmpath"/>',
                         '<xsl:value-of select="$opt"/>',
                         '<xsl:value-of select="$srcData/MetaData"/>',
                         '<xsl:value-of select="$spg"/>',
                         '<xsl:value-of select="$keys"/>',
                         '<xsl:value-of select="$cookie"/>',
                         '<xsl:value-of select="$intrvl"/>',
                         '<xsl:value-of select="$tblID"/>');
        </script>
      </xsl:when>
      <xsl:when test="@name = 'refresh' and not(@action)">
        <div class="addRecord" id="addRecordDiv">
          <a href="#">
            <xsl:attribute name="onclick">
              <xsl:call-template name="refreshTmpl">
                <xsl:with-param name="sData" select="$srcData" />
                <xsl:with-param name="mData" select="$metaData" />
                <xsl:with-param name="frmType" select="''" />
                <xsl:with-param name="cookieName" select="$cookie" />
              </xsl:call-template>
            </xsl:attribute>
            <xsl:value-of select="@lbl"/>
          </a>
        </div>
      </xsl:when>
      <xsl:when test="@name = 'Previous'">
        <!--a>
          <xsl:attribute name="href">
            <xsl:text>#</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="onClick">
            <xsl:text>parent.parent.frameTop.toolbarFrame.previous();</xsl:text>
          </xsl:attribute>
          <span style="float:right; margin:6px;">
           <xsl:value-of select="@name"/>
            <xsl:text>&#160;&#160;</xsl:text>
          </span>  
        </a-->
      </xsl:when>
      <xsl:when test="@name = 'More'">
        <xsl:variable name="keypairs"> 
          <xsl:for-each select="$metaData/obj[@key='yes']">
            <xsl:variable name="keyName" select="@name"/>
            <xsl:value-of select="$keyName"/>
            <xsl:text>^</xsl:text>
            <xsl:value-of select="@type"/>
            <xsl:text>!</xsl:text>
            <xsl:value-of select="$srcData/Config/record[last()]/param[@name=$keyName]/@value"/>
            <xsl:text>|</xsl:text>
          </xsl:for-each>
        </xsl:variable>
        <a>
          <xsl:attribute name="id">
            <xsl:text>more_link</xsl:text>
            <xsl:if test="$tblID !='' and $tblID!='Not Available'">
              <xsl:value-of select="$tblID" />
            </xsl:if>
          </xsl:attribute>
          <xsl:attribute name="dmpath">
            <xsl:choose>
              <xsl:when test="not($srcData/dmpath)">
                <xsl:value-of select="$metaData/dmpath" />
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$srcData/dmpath" />
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
          <xsl:attribute name="mtpath">
            <xsl:value-of select="@mtpath" />
          </xsl:attribute>
          <xsl:attribute name="spg">
            <xsl:value-of select="@spg" />
          </xsl:attribute>
          <xsl:attribute name="cookiename">
            <xsl:value-of select="$cookie"/>
          </xsl:attribute> 
          <xsl:attribute name="opcode">
            <xsl:call-template name="getOPCode">
              <xsl:with-param name="tOpt" select="@opcode"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="keys"> 
            <xsl:value-of select="$keypairs"/>
          </xsl:attribute>
            
          <xsl:attribute name="onClick">
            <xsl:choose>
              <xsl:when test="not($srcData/Config/*)">
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>parent.parent.frameTop.toolbarFrame.more(location.href);</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>            
          <!--span style="float:right; clear:right;">
           <xsl:value-of select="@name"/>
          </span-->
        </a>
        <script language="JavaScript">
          setMoreKeys('<xsl:value-of select="$keypairs"/>');
        </script>
      </xsl:when>
      <xsl:when test="@instancekey">
        <!-- Tabular link -->
        <xsl:variable name="lnkName">
          <xsl:value-of select="@name" />
        </xsl:variable>
        <xsl:variable name="spg">
          <xsl:value-of select="@spg" />
        </xsl:variable>
        <xsl:variable name="lnkmtpath">
          <xsl:value-of select="@mtpath" />
        </xsl:variable>
        <xsl:variable name="opcode">
          <xsl:value-of select="@opcode" />
        </xsl:variable>
        <xsl:variable name="instancekey">
          <xsl:value-of select="@instancekey" />
        </xsl:variable>
        <xsl:variable name="keysArr">
          <xsl:text>&amp;keys=</xsl:text>
          <xsl:value-of select="document($lnkmtpath)/root/obj[@key='yes']/@name" />
          <xsl:text>^</xsl:text>
          <xsl:value-of select="document($lnkmtpath)/root/obj[@key='yes']/@type" />
          <xsl:text>!nil|</xsl:text>
        </xsl:variable>
        <xsl:text>[</xsl:text>
        <a href="#">
         <xsl:variable name="subDivId">
           <xsl:value-of select="substring-before(@mtpath,'.')" />
           <!--xsl:text>_</xsl:text>
           <xsl:value-of select="$tblID"/-->
         </xsl:variable>
          <xsl:if test="@target='ldiv'">
            <xsl:attribute name="ldivid">
              <xsl:value-of select="$subDivId"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:attribute name="tgt">
            <xsl:value-of select="@target" />
          </xsl:attribute>
          <xsl:attribute name="onclick">
            <xsl:text>setTblValueInstance(this,'</xsl:text>
            <!--<xsl:value-of select="$metaData/dmpath" />-->
            <xsl:choose>
              <xsl:when test="not($srcData/dmpath)">
                <xsl:value-of select="$metaData/dmpath" />
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$srcData/dmpath" />
              </xsl:otherwise>
            </xsl:choose>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$cookie"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$lnkmtpath"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$opcode"/>                                              
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$spg"/>                                              
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$instancekey"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$keysArr"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="@target"/>
            <xsl:text>');</xsl:text>
          </xsl:attribute>
          <xsl:value-of  select="$lnkName" />
        </a>  
        <xsl:text>]</xsl:text>
        <br/>
      </xsl:when>
      <xsl:when test="@subtbl">        
        <xsl:variable name="lnkName">
          <xsl:value-of select="@name" />
        </xsl:variable>
        <xsl:variable name="spg">
          <xsl:value-of select="@spg" />
        </xsl:variable>
        <xsl:variable name="lnkmtpath">
          <xsl:value-of select="@mtpath" />
        </xsl:variable>
        <xsl:variable name="opcode">
          <xsl:value-of select="@opcode" />
        </xsl:variable>
        <!--xsl:variable name="instancekey">
          <xsl:value-of select="@instancekey" />
        </xsl:variable-->
        <xsl:variable name="keysArr">
          <xsl:text>&amp;keys=</xsl:text>
          <xsl:value-of select="document($lnkmtpath)/root/obj[@key='yes']/@name" />
          <xsl:text>^</xsl:text>
          <xsl:value-of select="document($lnkmtpath)/root/obj[@key='yes']/@type" />
          <xsl:text>!nil|</xsl:text>
        </xsl:variable>
        <xsl:variable name="lnkdmpath">
          <xsl:value-of select="document($lnkmtpath)/root/dmpath" />
        </xsl:variable>
        <xsl:text>[</xsl:text>
        <a href="#">
          <xsl:variable name="subDivId">
            <xsl:value-of select="substring-before(@mtpath,'.')" />
            <!--xsl:text>_</xsl:text>
            <xsl:value-of select="$tblID"/-->
          </xsl:variable>
          <xsl:if test="@target='ldiv'">
            <xsl:attribute name="ldivid">
              <xsl:value-of select="$subDivId"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:attribute name="tgt">
            <xsl:value-of select="@target" />
          </xsl:attribute>
          <xsl:attribute name="onclick">
            <xsl:text>setTblValueInstance(this,'</xsl:text>           
            <xsl:choose>
              <xsl:when test="@subtbl='no'">
                <xsl:value-of select="$lnkdmpath" />
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$srcData/dmpath" />
              </xsl:otherwise>
            </xsl:choose>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$cookie"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$lnkmtpath"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$opcode"/>                                              
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$spg"/>                                              
            <xsl:text>', '</xsl:text>           
            <!--xsl:value-of select="$instancekey"/-->
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$keysArr"/>
            <xsl:text>','</xsl:text>
            <xsl:value-of select="@target"/>
            <xsl:text>');</xsl:text>
          </xsl:attribute>
          <xsl:value-of  select="$lnkName" />
        </a>  
        <xsl:text>]</xsl:text>
        <br/>
      </xsl:when>
      <xsl:otherwise>
       <div class="addRecord" id="addRecordDiv">
        <input type="button">
          <!--xsl:attribute name="href">
            <xsl:text>#</xsl:text>
          </xsl:attribute--> 
          <xsl:variable name="addEditDivId">
            <xsl:value-of select="$tblID"/>
            <xsl:text>_addedit</xsl:text>
          </xsl:variable>
          <xsl:if test="@target='ldiv'">
            <xsl:attribute name="ldivid">
              <xsl:value-of select="$addEditDivId"/>
            </xsl:attribute>
          </xsl:if>
          <!-- set pkeys if available -->
          <xsl:if test="@pkeys">
            <xsl:attribute name="pkeys">
              <xsl:value-of select="@pkeys"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:if test="@ptx">
            <xsl:attribute name="dmpath">
              <xsl:choose>
                <xsl:when test="$srcData/dmpath=''">
                  <xsl:value-of select="$metaData/dmpath"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$srcData/dmpath"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="ptx">
              <xsl:value-of select="@ptx"/>
            </xsl:attribute>
          </xsl:if>
          <xsl:attribute name="tgt">
            <xsl:value-of select="@target" />
          </xsl:attribute>
          <xsl:attribute name="ifrm">
            <xsl:value-of select="substring-before(@mtpath,'.')" />
            <xsl:text>_ifrm</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="onclick">
            <xsl:text>resetEditIndex(); setTargetView(this);setCookieValue('</xsl:text>
            <xsl:choose>
              <xsl:when test="$srcData/dmpath=''">
                <xsl:value-of select="$metaData/dmpath"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$srcData/dmpath"/>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:text>','</xsl:text> 
            <xsl:call-template name="getOPCode">
              <xsl:with-param name="tOpt" select="@opcode"/>
            </xsl:call-template>
            <xsl:text>','</xsl:text> 
            <xsl:value-of select="@mtpath" />
            <xsl:text>','</xsl:text>  
            <xsl:value-of select="@spg" />
            <xsl:text>','</xsl:text>  
            <xsl:text>','</xsl:text>
            <xsl:value-of select="$cookie"/>
            <xsl:text>','</xsl:text>
            <xsl:text>','</xsl:text>
            <xsl:choose>
              <xsl:when test="@target">
                <xsl:value-of select="@target"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:text>new</xsl:text>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:text>');</xsl:text>
          </xsl:attribute> 
          <xsl:attribute name="value">
            <xsl:if test="string-length(@lbl) = 0">
             <xsl:value-of select="@name"/>
            </xsl:if>
            <xsl:if test="string-length(@lbl) != 0">
             <xsl:value-of select="@lbl"/>
            </xsl:if>
          </xsl:attribute>         
        </input>&#160;<input type="button" id="bClose" name="bClose" value="Close" onclick="javascript: back();" />
       </div>
      </xsl:otherwise>  
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
