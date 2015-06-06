<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
<xsl:template match="/">
<html>
  <head>   
   <title></title> 
   <style>
	#input [type="button"], [type="button"]
	{
		color: #000000;
		background-color: #CCCCCC;
		border: 1px solid #000000;
	}
    </style>
    <script language="JavaScript1.2" src="productinfo.js"></script>
    <script language="JavaScript1.2" src="com_utlils.js"></script>
    <script>
	function deleteCookies() {
           document.cookie = "Name-Login=;EXPIRES=" + getexpirydateUTC(-1);
           document.cookie = "Name-SNET=;EXPIRES=" + getexpirydateUTC(-1);
           //defaultcookie
           //Name-wizard
        }
	function getexpirydateUTC(nodays)
	{
	  Today = new Date();
	  milli = Date.parse(Today);
	  Today.setTime(milli+ nodays*24*60*60*1000);
	  return Today.toUTCString();
	}
    </script>
  </head>

  <body onLoad="deleteCookies();setProductNameInBlackFont(this.document);setProductNameAsTitle(this.document);" bgcolor="#FFFFFF" text="#000000" link="#003399" vlink="#666666" alink="#CC0000" leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
    <table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td align="center" valign="middle">
          <font color="#726B63" size="6" face="Verdana, Arial, Helvetica, sans-serif">&#160;</font>
        </td>
      </tr>
      <tr>
        <td width="109" align="left" valign="bottom">
          <!--img src="company.gif" width="150" height="72"-->
        </td>
        <td align="center" valign="middle">
            <img src='images/VortiqaLogo.gif' border="0"/><br/>
            <span id="productNameDiv"></span>
        </td>
        <td width="100" align="right" valign="top">
        </td>
        <td width="10" align="right" valign="top">&#160;</td>
      </tr>
      <tr>
        <td align="center" valign="middle">
          <font color="#726B63" size="2" face="Verdana, Arial, Helvetica, sans-serif">&#160;</font>
        </td>
      </tr>
      <tr>
        <td colspan="4"><br/>
          <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0">
           <tr>
             <td colspan="15">
              <div style="color:#1F60A9;" ><hr />
             </div>
            </td>
           </tr>
            <tr>
              <td bgcolor="#D0D0D0" align="center">
                <table border="0" cellspacing="0" cellpadding="0">
                  <tr class="toolbar">
                    <!--<td class="toolbar"><font color="#002E4F" size="5" face="Verdana, Arial, Helvetica, sans-serif">Welcome</font></td>-->
                    <td valign="middle" align="center">
                          <b><font size="3" face="Verdana, Arial, Helvetica, sans-serif">
                                <xsl:value-of select="/root/Msg"/>
                              </font></b>
                               <br/>
				<font size = "3" face="Verdana, Arial, Helvetica, sans-serif">
                               <a>
                               <xsl:attribute name="href">
                               <xsl:value-of select="/root/rdpage"/>
                               </xsl:attribute>
                               Login Again
                               </a></font>                            
                    </td>
                  </tr>
                </table>
              </td>
            </tr>
      <tr>
        <td colspan="15">
          <div class="hr"><hr />
          </div>
        </td>
      </tr>
          </table>
        </td>
      </tr>
      <tr>
        <td colspan="4">&#160;</td>
      </tr>
    </table>
  </body>
</html>
</xsl:template>
</xsl:stylesheet>
