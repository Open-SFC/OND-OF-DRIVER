<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
  <xsl:include href="utilmeta.xsl" />
  <xsl:include href="tblfrm.xsl" />
  <xsl:template match="/">
   <html>
       <head>
        <link rel="stylesheet" type="text/css" href="form.css" />
        <script language="JavaScript1.2" src="cookie.js"></script>
         <script language="JavaScript1.2" src="host.js"/>
         <script language="JavaScript1.2" src="com_utlils.js"/>
         <script type="text/JavaScript" language="JavaScript">
         <![CDATA[
           function init(opcode,status, url, msg) {
             if(status == "error" && msg !="nomsg") {
               msgStr = "";
               if(null != opcode)
                 msgStr += "OPCODE: " + opcode + " : " ;

               msgStr += msg;
               alert(msgStr);
             } else if(status == "error" && (msg =="" || msg =="nomsg")) {
               msgStr = "";
							 // if(null != opcode)
							 //  msgStr += "OPCODE: " + opcode + " : " ;

               msgStr += "Server returned failure.";
               alert(msgStr);
             } else if(status == "ok" && (msg !="")) {
               alert(msg);
               //Need to refresh the parent window.
	           window.open('','_parent','');
	           window.close();
						 }
						 //alert(url);
             location.href = url;
           }
         ]]>  
         </script>
       </head>
     <body>
       <xsl:variable name="stat"> 
          <xsl:value-of select="/root/@status" />
       </xsl:variable> 
       <xsl:variable name="opt"> 
          <xsl:value-of select="/root/OPCode" />
       </xsl:variable> 

       <xsl:choose>    
        <xsl:when test="$opt='SAVE'">
          <br/>
          <br/>
          <xsl:choose>    
            <xsl:when test="$stat='ok'">
              <xsl:variable name="msg"> 
                <xsl:text>Successfully saved the configuration</xsl:text>
              </xsl:variable>
               <xsl:value-of select="$msg" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:variable name="msg"> 
                <xsl:text>Save operation failed.</xsl:text>
              </xsl:variable>
               <xsl:value-of select="$msg" />
            </xsl:otherwise>
          </xsl:choose>
          &#160;
       <input type="button" name="back" onclick="back();" value="Back" />  
        </xsl:when>
        <xsl:when test="$opt='FACRES'">
          <br/>
          <br/>
          <xsl:choose>    
            <xsl:when test="$stat='ok'">
              <xsl:variable name="msg"> 
                <xsl:text>Successfully loaded the factory default configuration.</xsl:text>
              </xsl:variable>
               <xsl:value-of select="$msg" />
            </xsl:when>
            <xsl:otherwise>
              <xsl:variable name="msg"> 
                <xsl:text>Reset to factory defaults operation failed.</xsl:text>
              </xsl:variable>
               <xsl:value-of select="$msg" />
            </xsl:otherwise>
          </xsl:choose>
          &#160;
        <input type="button" name="back" onclick="back();" value="Back" />    
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="onload">
            <xsl:variable name="mtVar">
              <xsl:value-of select="/root/MetaData" />
            </xsl:variable>
            <xsl:variable name="mtData" select="document($mtVar)/root"/>
            <xsl:text>init('</xsl:text>
            <xsl:value-of select="/root/OPCode" />
            <xsl:text>','</xsl:text>
            <xsl:value-of select="/root/@status" />
            <xsl:text>','ucm?dmpath=</xsl:text>
               <xsl:value-of select="/root/dmpath" />
               <xsl:text>&amp;mtpath=</xsl:text>
               <xsl:value-of select="/root/MetaData" />
               <xsl:text>&amp;sredirect=</xsl:text>
							 <xsl:choose>
                 <xsl:when test="$stat='ok'">
                    <xsl:value-of select="/root/Sredirect" />
								 </xsl:when>
								 <xsl:otherwise>
										<xsl:value-of select="/root/Fredirect" />
								 </xsl:otherwise>
						   </xsl:choose>
               <xsl:text>&amp;opcode=</xsl:text>
               <xsl:choose>
                  <xsl:when test="$opt='SETROL'">
                    <xsl:variable name="tOpt">
                      <xsl:text>GETROLE</xsl:text>
                    </xsl:variable>
                    <xsl:call-template name="getOPCode">
                      <xsl:with-param name="tOpt" select="$tOpt"/>
                    </xsl:call-template>
                  </xsl:when>
                  <xsl:when test="$opt='ISETROL'">
                    <xsl:variable name="tOpt">
                      <xsl:text>IGETROLE</xsl:text>
                    </xsl:variable>
                    <xsl:call-template name="getOPCode">
                      <xsl:with-param name="tOpt" select="$tOpt"/>
                    </xsl:call-template>
                  </xsl:when>
                  <xsl:when test="$opt='IDELROL'">
                    <xsl:variable name="tOpt">
                      <xsl:text>IGETROLE</xsl:text>
                    </xsl:variable>
                    <xsl:call-template name="getOPCode">
                      <xsl:with-param name="tOpt" select="$tOpt"/>
                    </xsl:call-template>
                  </xsl:when>
                  <xsl:when test="$opt='DELROL'">
                    <xsl:variable name="tOpt">
                      <xsl:text>GETROLE</xsl:text>
                    </xsl:variable>
                    <xsl:call-template name="getOPCode">
                      <xsl:with-param name="tOpt" select="$tOpt"/>
                    </xsl:call-template>
								 </xsl:when>
								   <xsl:otherwise>
                    <xsl:call-template name="getOPCode">
                      <xsl:with-param name="tOpt" select="$opt"/>
									 </xsl:call-template>
								</xsl:otherwise>
               </xsl:choose>
               <xsl:text>&amp;keys=</xsl:text>
               <xsl:for-each select="$mtData/obj[@key='yes']">
                 <xsl:value-of select="@name" />
                 <xsl:text>+</xsl:text>
                 <xsl:value-of select="@type" />
                 <xsl:text>!</xsl:text>
                 <xsl:text>nil</xsl:text>
                 <xsl:text>|</xsl:text>
               </xsl:for-each>
               <xsl:text>','</xsl:text>
               <xsl:choose>
                 <xsl:when test="/root/Msg">
                   <xsl:value-of select="/root/Msg" />
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:text>nomsg</xsl:text>
                 </xsl:otherwise>
               </xsl:choose>
            <xsl:text>');</xsl:text>
          </xsl:attribute>
        </xsl:otherwise>
       </xsl:choose>
       <xsl:call-template name="tbl-frm" />
     </body>
   </html>
  </xsl:template>
</xsl:stylesheet>
