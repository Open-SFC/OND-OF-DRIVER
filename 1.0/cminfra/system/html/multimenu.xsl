<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">
  <xsl:output method="html" version="4.0" />
  <xsl:template match="top">
    <html>
      <head>
        <script language="javascript" src="host.js" />
        <script language="javascript" src="com_utlils.js" />  
        <script language="JavaScript1.2" src="cookie.js"></script> 
        <style type="text/css">
          a:link{color:#FFFFFF;text-decoration:none;font-size:11px;font-family:Verdana, Arial, Helvetica, sans-serif;}
          a:visited{color:#FFFFFF;text-decoration:none;font-size:11px;font-family:Verdana, Arial, Helvetica, sans-serif;}
          a:hover{color:#ff3300;text-decoration:none;font-size:11px;font-family:Verdana, Arial, Helvetica, sans-serif;}

	  body {
		scrollbar-3dlight-color:#CCCCCC; 
		scrollbar-arrow-color:#FFFFFF;
		scrollbar-base-color:#FF6347;
		scrollbar-darkshadow-color:#666666;
		scrollbar-face-color: #444755;
		scrollbar-highlight-color:#CCCCCC;
		scrollbar-shadow-color:#000000;
                margin: 0;
	  }
	.greyOutDiv
	 {    filter: alpha(opacity=50);
	      -moz-opacity: .50;
	      opacity: .50;
	      background-color: #f9f6f4;/*#BFDFFF;#c0c0c0; #f9f6f4;*/
	      position: absolute;
	      top: 0px;
	      left: 0px;
	      width: 100%; height: 100%;
	 }
        </style>
      </head>
      <body bgcolor="#444755">
       <xsl:apply-templates />
      <span id="screenHeightDiv" style="position: relative;">&#160;<img src="images/spacer.gif" align="absMiddle" height="1" width="1"/> &#160;</span></body>
    </html>
  </xsl:template>

  <xsl:template match="model|menu">
    <xsl:variable name="hasChild">
      <xsl:if test="model">true</xsl:if>
      <xsl:if test="@file">true</xsl:if>
    </xsl:variable>
    <xsl:variable name="prefix">
      <xsl:text>L</xsl:text>
    </xsl:variable>
    <table width="100%" cellspacing="0" cellpadding="0">
      <tr>
        <td>
        <xsl:choose>
            <xsl:when test="$hasChild='true'">
              <xsl:variable name="myId" select="generate-id()" />
              <xsl:variable name="icon1">
                <xsl:value-of select="'images/Lplus.gif'" />
              </xsl:variable>
              <xsl:variable name="icon2">
                <xsl:value-of select="'images/Lminus.gif'" />
              </xsl:variable>
              <xsl:variable name="style">
                <xsl:if test="position() &lt; last()">
                  <xsl:text>border-left:0 px dotted gainsboro;</xsl:text>
                </xsl:if>
              </xsl:variable>
              <xsl:variable name="display">
                <xsl:choose>
                  <xsl:when test="@opened='true'">
                    <xsl:text>block</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>none</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:variable>
              <xsl:variable name="img">
                <xsl:choose>
                  <xsl:when test="@opened='true'">
                    <xsl:text>minus</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>plus</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:variable>
              <div onclick="toggle('{$myId}','{$icon1}','{$icon2}','{$prefix}');showScreen(this);"> 
                <xsl:choose>
                 <xsl:when test="@mtpath">
                   <xsl:attribute name="urlRef">
                    <xsl:text>fqn=</xsl:text>
                    <xsl:value-of select="@fqn"/>
                    <xsl:text>&amp;opcode=</xsl:text>
                    <xsl:value-of select="@opcode"/>
                    <xsl:text>&amp;mtpath=</xsl:text>
                    <xsl:value-of select="@mtpath" />
                    <xsl:text>&amp;spg=</xsl:text>
                    <xsl:value-of select="@spg" />
                    <xsl:text>&amp;keys=</xsl:text>  
                    <xsl:for-each select="key">
                     <xsl:value-of select="@name"/>
                      <xsl:text>^</xsl:text>
		              <xsl:value-of select="@type"/>
                      <xsl:text>!</xsl:text>
                      <xsl:value-of select="@value"/>
                      <xsl:text>|</xsl:text>
                    </xsl:for-each>
                    <xsl:text>&amp;cookie=</xsl:text>
                    <xsl:value-of select="/root/vrtlInst/@name"/>
                    <xsl:text>&amp;vrtInstPath=</xsl:text>
                    <xsl:value-of select="//model[@vrtInst='yes']/@fqn"/>
                    <xsl:text>&amp;vrtInstName=</xsl:text>
                    <xsl:value-of select="//model[@vrtInst='yes']/@name"/>
                    <xsl:text>&amp;host=</xsl:text>
                    <xsl:choose>
                      <xsl:when test="not(@host)">
                        <xsl:value-of select="parent::model[@host!='']/@host"/>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="@host"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:attribute> 
                  </xsl:when>
                  <xsl:otherwise>
                   <xsl:attribute name="urlRef"> </xsl:attribute> 
                  </xsl:otherwise>
                </xsl:choose>
                <img src="{$icon1}" border="0" align="absMiddle"  id="icon{$myId}" />
                <xsl:value-of select="' '" />
                <a href="#">
                  <xsl:value-of select="@name" />
                </a>
              </div>
              <div style="padding-left:8px;">
                <table border="0" style="{$style}" cellspacing="0" cellpadding="0">
                  <tr>
                    <td>
                      <span id="{$myId}" style="display:{$display};padding-left:8px;">
                        <xsl:choose>
                          <xsl:when test="@file">
                            <xsl:apply-templates select="document(@file)"/>    
                          </xsl:when>
                          <xsl:otherwise>
                            <xsl:apply-templates />
                          </xsl:otherwise>
                        </xsl:choose>
                      </span>
                    </td>
                  </tr>
                </table>
              </div>
            </xsl:when>
            <xsl:otherwise>
              <xsl:variable name="doc">
                <xsl:choose>
                  <xsl:when test="position()=last()">
                    <xsl:text>images/l.gif</xsl:text>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:text>images/t.gif</xsl:text>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:variable>
              <div>
                <img src="images/{$prefix}.gif" border="0" align="absMiddle" />
                <xsl:value-of select="' '" />
                <a> 
                  <xsl:attribute name="href">
                    <xsl:text>#</xsl:text>
                  </xsl:attribute> 
                  <xsl:attribute name="onclick">
                      <xsl:text>setHostID('</xsl:text>
                      <xsl:choose>
                        <xsl:when test="not(@host)">
                          <xsl:value-of select="parent::model[@host!='']/@host"/>
                        </xsl:when>
                        <xsl:otherwise>
                          <xsl:value-of select="@host"/>
                        </xsl:otherwise>
                      </xsl:choose>
                      <xsl:text>');</xsl:text>
                      <xsl:text>updateHostInfo();</xsl:text>
                      <xsl:text>setCookieValue('</xsl:text>
                      <xsl:value-of select="@fqn"/>
                      <xsl:text>','</xsl:text> 
                      <xsl:value-of select="@opcode"/>
                      <xsl:text>','</xsl:text> 
                      <xsl:value-of select="@mtpath" />
                      <xsl:text>','</xsl:text>  
                      <xsl:value-of select="@spg" />
                      <xsl:text>','</xsl:text>  
                      <xsl:for-each select="key">
                        <xsl:value-of select="@name"/>
                        <xsl:text>^</xsl:text>
		                <xsl:value-of select="@type"/>
                        <xsl:text>!</xsl:text>
                        <xsl:value-of select="@value"/>
                        <xsl:text>|</xsl:text>
                      </xsl:for-each>
                      <xsl:text>','</xsl:text>
                      <xsl:value-of select="/root/vrtlInst/@name"/>
                      <xsl:text>','</xsl:text>  
                      <xsl:text>');</xsl:text>
                  </xsl:attribute> 
                   <xsl:value-of select="@name" />
                </a>
              </div>
            </xsl:otherwise>
          </xsl:choose>
        </td>
      </tr>
    </table>    
  </xsl:template>
</xsl:stylesheet>
