<?xml version="1.0" encoding="UTF-8"?> 
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">  
  <xsl:include href="frm_meta.xsl"/>  
  <xsl:template match="/">
    <html>
      <head>
        <link rel="stylesheet" type="text/css" href="form.css"/>
        <script language="JavaScript1.2" src="host.js"></script>
        <script language="JavaScript1.2" src="com_utlils.js"></script>
        <script language="JavaScript1.2" src="cookie.js"></script>
        <script language="JavaScript1.2" src="tblmeta.js"></script>
        <title></title>
      </head>      
      <body onload="renderRecord(); hideCloseButton();" onUnload="removeGreyDiv();" id="body">          
 <!-- Modal window DIV -->
 <div id="MouseX" style="display:none;"></div>
 <div id="MouseY" style="display:none;"></div>      
      <xsl:variable name="tmpVar" select="/root/MetaData"/>
      <xsl:variable name="srcData" select="/root"/>
      <xsl:variable name="metaData" select="document($tmpVar)/root"/>
      <xsl:variable name="cookieName">
        <xsl:choose>
          <xsl:when test="not($metaData/depends)">
            <xsl:value-of select="$metaData/vrtlInst/@name"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:variable name="mtPath">
              <xsl:value-of select="$metaData/depends/@mtpath"/>
            </xsl:variable>
            <xsl:value-of select="document($mtPath)/root/vrtlInst/@name"/>
          </xsl:otherwise>
        </xsl:choose>   
      </xsl:variable>
      <script language="JavaScript">
               <xsl:text>tblInit('</xsl:text>
               <xsl:choose>
                 <xsl:when test="$metaData/opcode='ROLESET'">
                   <xsl:text>roleset</xsl:text>
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:text>config</xsl:text>
                 </xsl:otherwise>
               </xsl:choose>  
	       <xsl:text>');</xsl:text>
      </script>
      <table border="0" cellspacing="0" cellpadding="0" class="pagetitle" width="99%" height="1">
        <tr class="toolbar">
          <td width="85%" height="20">
            <!--img class="helpImg" src="images/blueball.gif"/-->&#160;
            <span id="appln" class="titlefont">
              <script language="JavaScript">
                applName('<xsl:value-of select="$metaData/breadcrumb"/>');
              </script>
            </span>
            <br/>&#160;
            <span id="ovsg" class="breadcrumb">
              <script language="JavaScript">
                breadcrumb('<xsl:value-of select="$metaData/dmpath"/>', 
                           '<xsl:value-of select="$cookieName"/>',
                           '<xsl:value-of select="$srcData/dmpath"/>');
              </script>
            </span>
          </td>
          <xsl:if test="($tmpVar!='ROLE_MetaData.xml') and ($tmpVar!='AROLE_MetaData.xml')" >
            <xsl:variable name="dmPathVar">
              <xsl:choose>
                <xsl:when test="not($srcData/dmpath)">
                  <xsl:value-of select="$metaData/dmpath"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$srcData/dmpath"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:variable>

            <td width="15%" nowrap="nowrap">
              <span class="breadcrumb" style="float:right; clear:right;">
                <a href="#">
                  <xsl:attribute name="onclick">
                     <xsl:text>showHelp('</xsl:text>
                     <xsl:value-of select="document($tmpVar)/root/help/@src"/>
                     <xsl:text>');</xsl:text>
                  </xsl:attribute>                   
                  <xsl:text>Help</xsl:text>                   
                </a>&#160;<font color="#c4cfd1">|</font>&#160;<a href="#">
                  <xsl:attribute name="onclick">
                    <xsl:text>setRoleValue('</xsl:text>
                    <xsl:value-of select="$dmPathVar" />
                    <xsl:text>','</xsl:text> 
                    <xsl:if test="$metaData/depends/@vrtlInst">
                      <xsl:value-of select="$cookieName"/>
                    </xsl:if>
                    <xsl:text>','</xsl:text> 
                    <xsl:text>ROLE_MetaData.xml</xsl:text>
                    <xsl:text>');</xsl:text>
                  </xsl:attribute> 
                  <xsl:text>Manage Permissions</xsl:text>
                </a> &#160;<!--font color="#c4cfd1">|</font>&#160;<a href="#">
                  <xsl:attribute name="onclick">
                    <xsl:text>instanceSave('</xsl:text>
                    <xsl:value-of select="$cookieName"/>
                    <xsl:text>','</xsl:text> 
                    <xsl:value-of select="$dmPathVar" />
                    <xsl:text>','</xsl:text> 
                      <xsl:value-of select="$srcData/MetaData"/>
                    <xsl:text>');</xsl:text>
                  </xsl:attribute>                   
                  <xsl:text>Save</xsl:text>               
                </a>&#160;<font color="#c4cfd1">|</font>&#160;<a href="#">
                  <xsl:attribute name="onclick">
                    <xsl:text>instanceFactoryReset('</xsl:text>
                    <xsl:value-of select="$cookieName"/>
                    <xsl:text>','</xsl:text> 
                    <xsl:value-of select="$dmPathVar" />
                    <xsl:text>','</xsl:text> 
                      <xsl:value-of select="$srcData/MetaData"/>
                    <xsl:text>');</xsl:text>
                  </xsl:attribute>                   
                  <xsl:text>Factory Reset&#160;&#160;</xsl:text>                   
                </a-->
              </span>
            </td>
          </xsl:if>
        </tr>
      </table>     

    <!-- For each table start -->
    <xsl:for-each select="$metaData/tbl">
      <xsl:variable name="tblId">
        <xsl:value-of select="generate-id()"/>       
      </xsl:variable>
      <input type="hidden" value="-1">
        <xsl:attribute name="id">
          <xsl:text>ika</xsl:text>
          <xsl:value-of select="$tblId"/>
        </xsl:attribute>
      </input>
      <input type="hidden" value="0">
        <xsl:attribute name="id">
          <xsl:text>ml</xsl:text>
          <xsl:value-of select="$tblId"/>
        </xsl:attribute>
      </input>
      <xsl:choose>
        <xsl:when test="not(@typ)">  
          <table border="1" cellpadding="0" cellspacing="0" class="tablev" width="99%" height="1" bordercolor="#ffffff" bordercolorlight="#ffffff" bordercolordark="#ffffff" bgcolor="#ffffff">
            <xsl:attribute name="id">
              <xsl:value-of select="$tblId"/>
            </xsl:attribute>
            <xsl:attribute name="titl">
              <xsl:value-of select="@title"/>
            </xsl:attribute>              
              <!-- CALL Table Template ON EVENT -->
              <xsl:call-template name="gen_tbl">
                <xsl:with-param name="srcData" select="$srcData"/>
                <xsl:with-param name="metaData" select="$metaData"/>
                <xsl:with-param name="cookie" select="$cookieName"/>
                <xsl:with-param name="tblID" select="$tblId"/>
              </xsl:call-template>
          </table>
          <!--/div-->
          <div>
            <hr/>
          </div>
          <xsl:for-each select="link">
            <xsl:call-template name="processLink">
              <xsl:with-param name="metaData" select="$metaData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="srcData" select="$srcData"/>
              <xsl:with-param name="tblID" select="$tblId"/>
            </xsl:call-template>
          </xsl:for-each>
          <xsl:for-each select="btn">
            <xsl:call-template name="processBtn">
              <xsl:with-param name="sData" select="$srcData"/>
              <xsl:with-param name="mData" select="$metaData"/>
              <xsl:with-param name="frmType" select="''"/>
              <xsl:with-param name="cookieName" select="$cookieName"/>
              <xsl:with-param name="tblID" select="$tblId"/>
            </xsl:call-template>
          </xsl:for-each>
           <!-- Process table divs -->
           <xsl:if test="@nested='true'">
            <xsl:call-template name="processTblLink">
              <xsl:with-param name="metaData" select="$metaData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="srcData" select="$srcData"/>
              <xsl:with-param name="tblID" select="$tblId"/>
            </xsl:call-template>
          </xsl:if>
          <script language="JavaScript">
            showcont('<xsl:value-of select="$tblId"/>');
          </script>
        </xsl:when>
        <xsl:when test="@typ='scalar'">
	<table border="0" cellspacing="0" cellpadding="0" class="tablev" width="99%" height="1" bordercolor="#ffffff" bordercolorlight="#ffffff" bordercolordark="#ffffff" bgcolor="#ffffff">
          <xsl:attribute name="id">
            <xsl:value-of select="$tblId"/>
          </xsl:attribute>
          <xsl:attribute name="typ">
            <xsl:value-of select="@typ"/>
          </xsl:attribute>
          <xsl:choose>
            <xsl:when test="@dmpath">
              <xsl:attribute name="dmpath">
                <xsl:value-of select="@dmpath"/>
              </xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="dmpath">
                <xsl:value-of select="$srcData/dmpath"/>
              </xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
              <xsl:attribute name="mtpath">
                <xsl:value-of select="$srcData/MetaData"/>
              </xsl:attribute>
              <xsl:attribute name="spg">
                <xsl:value-of select="tbl_meta.xsl"/>
              </xsl:attribute>
              <xsl:attribute name="cookiename">
                <xsl:choose>
                  <xsl:when test="$metaData/depends/@vrtlInst">
                    <xsl:value-of select="$cookieName"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>no</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
              <xsl:attribute name="opcode">
                <xsl:choose>
                  <xsl:when test="@opcode">
                    <xsl:value-of select="@opcode"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>BYKEY</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:attribute>
          <xsl:attribute name="titl">
            <xsl:value-of select="@title"/>
          </xsl:attribute>
          <xsl:if test="@title">
           <tr>
           <th colspan="2">
            <xsl:value-of select="@title"/>
            </th>
           </tr>
          </xsl:if>
	
          <xsl:for-each select="child::node()[name()!='']">
            <tr class="trform">
            <xsl:choose>
              <xsl:when test="name()='objref'">
                 <xsl:variable name="objId">
                   <xsl:value-of select="@oid"/>
                 </xsl:variable>
                 <xsl:variable name="objName">
                   <xsl:value-of select="$metaData/obj[@oid=$objId]/@name"/>
                 </xsl:variable>
                 <td class="tdformlabel" width="25%"><xsl:value-of select="$metaData/obj[@oid=$objId]/@lbl"/></td>                 
                 <td class="tdform" width="75%"><xsl:value-of select="$srcData/Config/record/param[@name=$objName]/@value"/></td>                 
              </xsl:when>
            </xsl:choose>
            </tr>
          </xsl:for-each>
	</table>
          <div class="hr">
            <hr/>
          </div>
          <xsl:for-each select="link">
            <xsl:call-template name="processLink">
              <xsl:with-param name="metaData" select="$metaData"/>
              <xsl:with-param name="srcParm" select="''"/>
              <xsl:with-param name="cookie" select="$cookieName"/>
              <xsl:with-param name="srcData" select="$srcData"/>
              <xsl:with-param name="tblID" select="$tblId"/>
            </xsl:call-template>
          </xsl:for-each>
          <xsl:for-each select="btn">
            <xsl:call-template name="processBtn">
              <xsl:with-param name="sData" select="$srcData"/>
              <xsl:with-param name="mData" select="$metaData"/>
              <xsl:with-param name="frmType" select="''"/>
              <xsl:with-param name="cookieName" select="$cookieName"/>
              <xsl:with-param name="tblID" select="$tblId"/>
            </xsl:call-template>
          </xsl:for-each>
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
    <!-- For each table end-->
    <span id="screenHeightDiv" style="position: relative;">&#160;<img src="images/spacer.gif" align="absMiddle" height="1" width="1"/></span></body>
      <xsl:call-template name="tbl-frm"/>
    </html>
  </xsl:template>

  <xsl:template name="gen_tbl">
    <xsl:param name="srcData" select="'Not Available'"/>
    <xsl:param name="metaData" select="'Not Available'"/>
    <xsl:param name="cookie" select="'Not Available'"/>
    <xsl:param name="tblID" select="'Not Available'"/>
    <xsl:variable name="tab" select="." />
      <!-- Populate the Column Names  -->
      <tr class="subhead">
        <xsl:for-each select="col">
          <th>
            <img src="images/spacer.gif" align="absMiddle" height="25" width="1"/><xsl:value-of select="@name"/>
          </th>
        </xsl:for-each> 
      </tr>
      <xsl:if test="not($srcData/Config/*)">
        <tr>
          <td colspan="15">
            <div class="hr"><img src="images/spacer.gif" align="absMiddle" height="25" width="1"/>No Records...
            </div>
          </td>
        </tr>
      </xsl:if>
      <!-- Populate the Rows start -->
      <xsl:for-each select="$srcData/Config/record">
        <tr height="1">
          <xsl:variable name="srcParm" select="param"/>            
            <xsl:for-each select="$tab[name()!='']/child::node()[name()!='']"> 
              <xsl:choose>
                <xsl:when test="name()='col'">
                  <td class="tabdata" height="1"><!--hr/--><img src="images/spacer.gif" align="absMiddle" height="25" width="1"/>
                  <xsl:for-each select="child::node()[name()!='']">
                    <xsl:call-template name="poplTblField">
                      <xsl:with-param name="metaData" select="$metaData"/>
                      <xsl:with-param name="srcParm" select="$srcParm"/>
                      <xsl:with-param name="cookie" select="$cookie"/>
                      <xsl:with-param name="srcData" select="$srcData"/>
                      <xsl:with-param name="tblID" select="$tblID"/>
                    </xsl:call-template>
                  </xsl:for-each>
                  </td>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:call-template name="poplTblField">
                    <xsl:with-param name="metaData" select="$metaData"/>
                    <xsl:with-param name="srcParm" select="$srcParm"/>
                    <xsl:with-param name="cookie" select="$cookie"/>
                    <xsl:with-param name="srcData" select="$srcData"/>
                    <xsl:with-param name="tblID" select="$tblID"/>
                  </xsl:call-template>
                </xsl:otherwise>
              </xsl:choose>            
          </xsl:for-each> 
        </tr>
      </xsl:for-each>
      <!-- Populate the Rows end -->
<form id="main-form" name="main-form" action="ucmreq.igw" method="post">
<input type="hidden" name="rowEditIndex" value=""/>
<input type="hidden" name="renderRows" value="yes"/>
<input id="modifiedRecords" type="hidden" name="modifiedRecords" value="no"/>
<input id="roleopt" type="hidden" name="roleopt" value="none"/>
<input id="instPath" type="hidden" name="instPath" value="none"/>
<input type="hidden" name="noRecords">
  <xsl:attribute name="value">
  <xsl:choose>
    <xsl:when test="not($srcData/Config/*)">
       <xsl:text>true</xsl:text>
     </xsl:when>
     <xsl:otherwise>
       <xsl:text>false</xsl:text>
     </xsl:otherwise>
  </xsl:choose>
  </xsl:attribute>
</input>
<input id="ptxRecords" type="hidden" name="ptxRecords">
  <xsl:attribute name="value">
    <xsl:choose>
      <xsl:when test="$metaData/tbl/link[@name='Add Record']/@ptx='y'">
        <xsl:text>true</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>false</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:attribute>
</input>
<input type="hidden" name="ifrmID" value=""/>
<input type="hidden" name="tblID">
  <xsl:attribute name="value">
    <xsl:value-of select="$tblID"/>
  </xsl:attribute>
</input>
<xsl:if test="$metaData/useparenttx/@mtpath">
  <xsl:variable name="pMeta" select="document($metaData/useparenttx/@mtpath)/root"/>
  <input type="hidden" name="useparenttx">
    <xsl:attribute name="ppath" >
      <xsl:value-of select="$metaData/useparenttx/@ppath"/>
    </xsl:attribute>
    <xsl:attribute name="dmpath">
      <xsl:value-of select="$pMeta/dmpath"/>
    </xsl:attribute>
    <xsl:attribute name="value">
      <xsl:for-each select="$pMeta/obj[@key='yes']">
        <xsl:value-of select="@name"/>
        <xsl:text>#</xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:text>,</xsl:text>
      </xsl:for-each>
    </xsl:attribute>
  </input>
  <script language="JavaScript">
    updateParentTxInfo('<xsl:value-of select="$cookie"/>', '<xsl:value-of select="$srcData/dmpath"/>');
  </script>
</xsl:if>
</form>
  </xsl:template>

  <xsl:template name="poplTblField">
    <xsl:param name="metaData" select="'Not Available'"/>
    <xsl:param name="srcParm" select="'Not Available'"/>
    <xsl:param name="cookie" select="'Not Available'"/>
    <xsl:param name="srcData" select="'Not Available'"/>
    <xsl:param name="tblID" select="'Not Available'"/>
    <xsl:choose>
      <xsl:when test="name()='objref'">
        <xsl:call-template name="processObjRef">
          <xsl:with-param name="metaData" select="$metaData"/>
          <xsl:with-param name="srcParm" select="$srcParm"/>
          <xsl:with-param name="cookie" select="$cookie"/>
          <xsl:with-param name="srcData" select="$srcData"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="(name()='link' and @instancekey) or (name()='link' and @subtbl)">
        <xsl:call-template name="processLink">
          <xsl:with-param name="metaData" select="$metaData"/>
          <xsl:with-param name="srcParm" select="$srcParm"/>
          <xsl:with-param name="cookie" select="$cookie"/>
          <xsl:with-param name="srcData" select="$srcData"/>
          <xsl:with-param name="tblID" select="$tblID"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='field'">
        <xsl:call-template name="popl_field">
          <xsl:with-param name="sData" select="$srcData"/>
          <xsl:with-param name="mData" select="$metaData"/>
          <xsl:with-param name="frmType" select="'ADD'"/>
          <xsl:with-param name="cookieName" select="$cookie"/>
          <xsl:with-param name="isExtFrmField" select="'false'"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="name()='btn'">
        <xsl:call-template name="process_Btn">
          <xsl:with-param name="metaData" select="$metaData"/>
          <xsl:with-param name="srcParm" select="$srcParm"/>
          <xsl:with-param name="cookie" select="$cookie"/>
          <xsl:with-param name="srcData" select="$srcData"/>
        </xsl:call-template>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template name="processObjRef">
    <xsl:param name="metaData" select="'Not Available'"/>
    <xsl:param name="srcParm" select="'Not Available'"/>
    <xsl:param name="cookie" select="'Not Available'"/>
    <xsl:param name="srcData" select="'Not Available'"/>    
    <xsl:variable name="objID" select="@oid"/>
    <xsl:variable name="objName">
      <xsl:value-of select="$metaData/obj[@oid=$objID]/@name"/>
    </xsl:variable>
    <xsl:variable name="objVal">
      <xsl:value-of select="$srcParm[@name=$objName]/@value"/>
    </xsl:variable>
    <xsl:variable name="srcName">
      <xsl:value-of select="$srcParm[@name=$objName]/@name"/>
    </xsl:variable>
    <xsl:variable name="objType">
      <xsl:value-of select="$metaData/obj[@oid=$objID]/@type"/>
    </xsl:variable>
    <xsl:if test="$metaData/obj[@oid=$objID]/@key='yes'">
      <xsl:element name="keys">
        <xsl:attribute name="name"><xsl:value-of select="$objName"/></xsl:attribute>
        <xsl:attribute name="type"><xsl:value-of select="$objType"/></xsl:attribute>
        <xsl:attribute name="value"><xsl:value-of select="$objVal"/></xsl:attribute>
      </xsl:element>
    </xsl:if>      
    <xsl:if test="$objVal != ''">
      <xsl:variable name="dispStyle" select="../@rndr"/>
      <xsl:variable name="dlem" select="../@dlem"/>
      <xsl:variable name="ifval" select="@ifval"/>
      <xsl:variable name="mtFile" select="$srcData/MetaData"/>
      <xsl:choose>
        <xsl:when test="($mtFile ='ROLE_MetaData.xml') or ($mtFile ='AROLE_MetaData.xml')">
          <xsl:call-template name="gen_Value">
             <xsl:with-param name="mtFile" select="$mtFile"/>
             <xsl:with-param name="objval" select="$objVal"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:choose>
            <xsl:when test="not(@ifval)">
              <xsl:call-template name="poplValue">
                <xsl:with-param name="mtObj" select="$metaData"/>
                <xsl:with-param name="objid" select="$objID"/>
                <xsl:with-param name="objval" select="$objVal"/>
              </xsl:call-template>
            </xsl:when>
            <xsl:when test="($ifval!='') and ($objVal = $ifval)">
              <xsl:value-of select="$objVal"/>
            </xsl:when>
          </xsl:choose>
          <xsl:variable name="nxtObjId" select="following-sibling::node()[name()='objref']/@oid"/>
          <xsl:if test="$nxtObjId">
            <xsl:variable name="nxtObjName">
              <xsl:value-of select="$metaData/obj[@oid=$nxtObjId]/@name"/>
            </xsl:variable>
            <xsl:variable name="nxtObjVal">
              <xsl:value-of select="$srcParm[@name=$nxtObjName]/@value"/>
            </xsl:variable>
            <xsl:if test="$nxtObjVal!=''">
              <xsl:value-of select="$dlem"/>
            </xsl:if>
          </xsl:if>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>
  
  <xsl:template name="process_Btn">
    <xsl:param name="metaData" select="'Not Available'"/>
    <xsl:param name="srcParm" select="'Not Available'"/>
    <xsl:param name="cookie" select="'Not Available'"/>
    <xsl:param name="srcData" select="'Not Available'"/>      
    <xsl:variable name="btnName">
      <xsl:value-of select="@name"/>
    </xsl:variable>
    <xsl:if test="$btnName='Edit'"><input type="image" src="images/prop.gif" align="absMiddle" title="Edit">
        <xsl:attribute name="onclick">
          <xsl:text>popEditURL(this,'</xsl:text>
          <xsl:choose>
            <xsl:when test="$srcData/dmpath=''">
               <xsl:value-of select="$metaData/dmpath"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$srcData/dmpath"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$srcData/MetaData"/>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="@spg"/>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$metaData/opcode"/>
          <xsl:text>','</xsl:text>
          <xsl:choose>
            <xsl:when test="$srcData/@passive = 'true'">
              <xsl:text>GETPASSV</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>GET</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$cookie"/>
          <xsl:text>');</xsl:text>
        </xsl:attribute>  
     </input>
   </xsl:if>
   <xsl:if test="$btnName='Delete'">&#160;<input type="image" src="images/delete.gif" align="absMiddle" title="Delete">
        <xsl:attribute name="reqtyp">
          <xsl:value-of select="@reqtyp"/>
        </xsl:attribute>
        <xsl:attribute name="onclick">
          <xsl:text>deleteRecord(this,'</xsl:text>
          <xsl:choose>
            <xsl:when test="$srcData/dmpath=''">
               <xsl:value-of select="$metaData/dmpath"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$srcData/dmpath"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$srcData/MetaData"/>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="@spg"/>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="@fpg"/>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$cookie"/>
          <xsl:text>');</xsl:text>
       </xsl:attribute>  
     </input>
   </xsl:if>
    <xsl:variable name="obName">
      <xsl:value-of select="$metaData/obj[@key='yes']/@name"/>
    </xsl:variable>

    <xsl:variable name="obVal">
      <xsl:value-of select="$srcParm[@name=$obName]/@value"/>
    </xsl:variable>
   <xsl:if test="$btnName='Reboot'">&#160;<input type="image" src="images/delete.gif" align="absMiddle" title="Reboot">
        <xsl:attribute name="onclick">
          <xsl:text>rebootAction('</xsl:text>
            <xsl:value-of select="@dmpath"/>
          <xsl:text>','</xsl:text>
            <xsl:value-of select="$obVal"/>
          <xsl:text>');</xsl:text>
       </xsl:attribute>
     </input>
   </xsl:if>
   <xsl:if test="$btnName='Roles'">&#160;<input type="image" src="images/user.gif" align="absMiddle" title="Manage Permissions">
        <xsl:attribute name="onclick">
          <xsl:text>setRoleValueInstance(this,'</xsl:text>
          <xsl:choose>
            <xsl:when test="$srcData/dmpath=''">
               <xsl:value-of select="$metaData/dmpath"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$srcData/dmpath"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>','</xsl:text>
          <xsl:value-of select="$cookie"/>
          <xsl:text>');</xsl:text>
        </xsl:attribute>
     </input>
   </xsl:if>
   <xsl:if test="$btnName='Up'">&#160;<input type="image" src="images/uarrow.gif" align="absMiddle" title="Move Up">
      <xsl:attribute name="onclick">
        <xsl:text>swapRowUp(this.parentNode.parentNode,'</xsl:text>
              <xsl:value-of select="$cookie"/>                  
        <xsl:text>','</xsl:text> 
              <xsl:choose>
                <xsl:when test="$srcData/dmpath=''">
                   <xsl:value-of select="$metaData/dmpath"/>
                </xsl:when>
                <xsl:otherwise>
                   <xsl:value-of select="$srcData/dmpath"/>
                </xsl:otherwise>
          </xsl:choose>
        <xsl:text>','</xsl:text> 
          <xsl:value-of select="$srcData/MetaData"/>
        <xsl:text>');</xsl:text> 
         </xsl:attribute>
     </input>
     <!--br/-->
      </xsl:if>
   <xsl:if test="$btnName='Down'">&#160;<input type="image" src="images/darrow.gif" align="absMiddle" title="Move Down">
      <xsl:attribute name="onclick">
        <xsl:text>swapRowDown(this.parentNode.parentNode,'</xsl:text>
        <xsl:value-of select="$cookie"/>                  
        <xsl:text>','</xsl:text> 
              <xsl:choose>
                <xsl:when test="$srcData/dmpath=''">
                   <xsl:value-of select="$metaData/dmpath"/>
                </xsl:when>
                <xsl:otherwise>
                   <xsl:value-of select="$srcData/dmpath"/>
                </xsl:otherwise>
          </xsl:choose>
        <xsl:text>','</xsl:text> 
          <xsl:value-of select="$srcData/MetaData"/>
        <xsl:text>');</xsl:text>  
      </xsl:attribute>
     </input>
   </xsl:if>
   <xsl:if test="$btnName='radioBtn'">
     <xsl:variable name="radioVal">
       <xsl:choose>
         <xsl:when test="count($metaData/obj[@key='yes']) > 1">
           <xsl:for-each select="$metaData/obj[@key='yes']">
             <xsl:variable name="keyObName">
               <xsl:value-of select="@name"/>
             </xsl:variable>
             <xsl:value-of select="$srcParm[@name=$keyObName]/@name"/>
             <xsl:text>=</xsl:text>
             <xsl:value-of select="$srcParm[@name=$keyObName]/@value"/>
             <xsl:text>,</xsl:text>
           </xsl:for-each>
         </xsl:when>
         <xsl:otherwise>
           <xsl:value-of select="$srcParm[@name=$metaData/obj[@key='yes']/@name]/@value"/>
         </xsl:otherwise>
       </xsl:choose>
     </xsl:variable>
     <xsl:variable name="cardStatus">
       <xsl:choose>
         <xsl:when test="$srcData/dmpath = 'mgmt.devinfo'">
           <xsl:value-of select="$srcParm[@name='cardstatus']/@value"/>
         </xsl:when>
         <xsl:otherwise>
         </xsl:otherwise>
       </xsl:choose>
     </xsl:variable>       
     <input>
       <xsl:attribute name="type">
         <xsl:value-of select="@type"/>
       </xsl:attribute>
       <xsl:attribute name="name">
         <xsl:value-of select="@name"/>
       </xsl:attribute>
       <xsl:attribute name="value">
         <xsl:value-of select="$radioVal"/>
       </xsl:attribute>
       <xsl:attribute name="id">
         <xsl:value-of select="$srcParm[@name=$metaData/obj[@key='yes']/@name]/@value"/>
       </xsl:attribute>
       <xsl:attribute name="onclick">
         <xsl:text>setInstance(this, '</xsl:text> 
         <xsl:value-of select="$metaData/vrtlInst/@name"/>
         <xsl:text>', '</xsl:text>
         <xsl:value-of select="$metaData/vrtlInst/@timeout"/>
         <xsl:text>');</xsl:text>
       </xsl:attribute>
     </input> 
     <script language="JavaScript1.2">
       <xsl:choose>
         <xsl:when test="($srcData/dmpath='mgmt.devinfo') and ($cardStatus='Master')">
       selectMasterRadioBtn('<xsl:value-of select="$radioVal"/>',
        '<xsl:value-of select="@name"/>');
         </xsl:when>
         <xsl:when test="($srcData/dmpath='igd.vsg')">
       updateRadioBtn('<xsl:value-of select="$metaData/vrtlInst/@name"/>',
        '<xsl:value-of select="@name"/>');
         </xsl:when>
         <xsl:otherwise>
         </xsl:otherwise>
       </xsl:choose>
     </script>           
   </xsl:if>
  </xsl:template>
  <xsl:template name="fetch_data">
      <xsl:param name="objVar" select="'Not Available'"/>
      <xsl:value-of select="$objVar" /> <xsl:text> = </xsl:text>
      <xsl:value-of select="param[@name=$objVar]/@value"/>
  </xsl:template>
  
  <xsl:template name="nameval">
     <xsl:param name="var" select="'Not Available'"/>
     <xsl:param name="dlem" select="'Not Available'"/>
     <xsl:choose>
       <xsl:when test="('range'= $var) or ('subnet'= $var) or
           ('ipaddr'= $var) or ('single'= $var) or ('domain'= $var)">
         <!--<xsl:value-of select="$var"/>-->
         <!--<br />-->
       </xsl:when>
       <xsl:otherwise>  
          <xsl:value-of select="$var"/>
          <xsl:text>&#160;&#160;</xsl:text>
       </xsl:otherwise>
     </xsl:choose>
  </xsl:template>
  
  <xsl:template name="arrowStyle">
     <xsl:param name="var" select="'Not Available'"/>
     <xsl:param name="srcName" select="'Not Available'"/>
     <xsl:choose>
       <xsl:when test="('FromNetType'= $srcName)">
          <xsl:value-of select="$var"/>
          <xsl:text>-></xsl:text>
        </xsl:when> 
        <xsl:otherwise>
          <xsl:value-of select="$var"/>
        </xsl:otherwise>
      </xsl:choose>   
  </xsl:template>
  
  <xsl:template name="displayCell">
     <xsl:param name="var" select="'Not Available'"/>
     <xsl:param name="srcName" select="'Not Available'"/>
         <xsl:param name="dlem" select="'Not Available'"/> 
     <xsl:choose>
       <xsl:when test="('FromNetType'= $srcName)">
        <xsl:value-of select="$var"/>
        <xsl:value-of select="$dlem"/>
       </xsl:when> 
       <xsl:when test="('DestPortStartRange'= $srcName) or ('StartPort' = $srcName)">
        <xsl:value-of select="$var"/>
        <xsl:value-of select="$dlem"/>
       </xsl:when> 
       <xsl:when test="('range'= $var) or ('subnet'= $var) or
        ('ipaddr' = $var) or ('single'= $var) or ('domain'= $var)">
       </xsl:when>
       <xsl:otherwise>
          <xsl:value-of select="$var"/>
          <xsl:text>&#160;&#160;</xsl:text>
       </xsl:otherwise>
      </xsl:choose>   
  </xsl:template>
 
  <xsl:template name="poplValue">
    <xsl:param name="mtObj" select="'Not Available'"/>
    <xsl:param name="objid" select="'Not Available'"/>
    <xsl:param name="objval" select="'Not Available'"/>
    <xsl:variable name="val">
        <xsl:value-of select="$mtObj/obj[@oid=$objid]/list/item[@value=$objval]/@name"/>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$val!=''">
        <xsl:value-of select="$val"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$objval"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="gen_Value">
    <xsl:param name="mtFile" select="'Not Available'"/>
    <xsl:param name="objval" select="'Not Available'"/>
    <xsl:variable name="mtRoot" select="document($mtFile)/root"/>
    <xsl:variable name="roleName">
      <xsl:if test="($mtFile = 'ROLE_MetaData.xml') or ($mtFile = 'AROLE_MetaData.xml')">
        <xsl:value-of select="$mtRoot/obj[@oid='2']/list/item[@value=$objval]/@name"/>
      </xsl:if>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$roleName != ''">
        <xsl:value-of select="$roleName"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="($mtFile = 'ROLE_MetaData.xml') and (($objval = '3') or ($objval = '4'))">
             <xsl:value-of select="$mtRoot/obj[@oid='2']/list/item[@value='2']/@name" />
          </xsl:when>
          <xsl:when test="($mtFile = 'AROLE_MetaData.xml') and (($objval = '1') or ($objval = '2'))">
             <xsl:value-of select="$mtRoot/obj[@oid='2']/list/item[@value='0']/@name" />
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$objval"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>

