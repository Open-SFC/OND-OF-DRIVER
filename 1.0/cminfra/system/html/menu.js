  var sFWIPV4 = "";
  var sVPN    = "";
  var sAntiX  = "";
  /* Intentionally assigning the value here so that the "Anti-X" link appears in 
     left menu for all the packages. */
  sAntiX      = "AntiX"; 

  var sPKI    = "";
  var sLDAP   = "";
  var sIIPS   = "";
  var sDIAGD  = "";
  var sDSHBRD = "";
  var sL2FW   ="";
  var str     = "";
  var menuPos ="left";
  var vpnFlag = false;

  var AntiXFlag = false;
  var qosFlag = false;
  var iipsFlag = false;
  var bSslVpnEnabled = false;
  var logoFlag=false;
  var L2TPFlag=false;
  MENU_ADVANCED = true; // true or false
	logoFlag=true;
	  sFWIPV4 = "FWIPV4";
  sPKI = "PKI";
  sDIAGD = "DIAGD"
L2TPFlag=true;

function leftMenu()
{
if(menuPos=="left"){

 
/* System Settings */
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="systemnavArrow0" onclick="js:toggleMenu(menuArray, subMenus, 0, \'systemnavArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 0, \'systemnavArrow0\');">System Settings</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="SystemSettings" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ssname.xml" target="main">System Name</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sstime.xml" target="main">Date & Time</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="passwd.htm" target="main">Admin Preferences</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security HttpSettings UUA Setting Starts */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="uua.htm" target="main">UUA Settings</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr>';
str +='<td width="18"><img alt="" height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td> <div class="subNavTab"><span onclick="submitLicenseManager();" onmouseover="this.className=\'cursor\';">License Manager</span></div>';
str +='</td>';
str +='</tr>';

/* Security HttpSettings UUA Setting Ends */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="RouterClusterAdvancedDdnsArrow0" onclick="js:toggleMenu(System_http_menu_state, System_http__menu, 0, \'RouterClusterAdvancedDdnsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(System_http_menu_state, System_http__menu, 0, \'RouterClusterAdvancedDdnsArrow0\');">HTTP Server</a></div>';
str +='<div align="left" class="navSubCatOff" id="HttpSetting" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sshtport.htm" target="main">Ports</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="hsctab.htm" target="main">Certificates</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="System_snmp_Arrow1" onclick="js:toggleMenu(System_snmp_menu_state, System_snmp_menu, 0, \'System_snmp_Arrow1\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(System_snmp_menu_state, System_snmp_menu, 0, \'System_snmp_Arrow1\');">SNMP</a></div>';
str +='<div class="navSubCatOff"></div>';
str +='<div align="left" class="navSubCatOff" id="SNMP" style="margin-left: auto;">';
str +='<table cellpadding="0" cellspacing="0" style="margin-left: 0px;">';

str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_system.xml" target="main">System </a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmptraps.xml" target="main">Traps</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpcomm.xml" target="main">Communities </a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_mgrtrap.xml" target="main">Managers</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_user.xml" target="main">Users & Groups</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_context.xml" target="main">Contexts</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_view.xml" target="main">Views</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='<tr>';
	str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
	str +='<td><div class="subNavTab"><a href="snmpv3_access.xml" target="main">Access</a></div>';
	str +='</td>';
	str +='</tr>';

	str +='</table>';
	str +='</div>';
	str +='</td>';
	str +='</tr>';

/*==========Log Control Settings Starts============*/
	str +='<tr valign="top">';
	str +='<td><a href="#n"><img height="24" id="LogArrow0" onclick="js:toggleMenu(Log_Array,Log_Menus, 0, \'LogArrow0\');" src="plus.gif" width="24" /></a></td>';
	str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Log_Array,Log_Menus, 0, \'LogArrow0\');">Messaging System</a></div>';
	str +='<div align="left" class="navSubCatOff" id="logdiv" style="margin-left: 2px;"> ';
	str +='<table cellpadding="0" cellspacing="0"> ';


				/* Log Stats Starts */



				str +='<tr> ';
				str +='<td width="18">';
				str +='<img height="24" src="nav_bullet.gif" width="24" />';
				str +='</td> ';
				str +='<td>';
				str +='<div class="subNavTab"><a href="gblmsg.xml" target="main">Message Family Settings</a></div> ';
				str +='</td>'; 
				str +='</tr>';


				/* Statistics Logs Message Throttler Starts */

				str +='</table><table  cellpadding="0" cellspacing="0">'
				str +='<tr id="oVsgEnbl"> ';
				str +='<td width="18">';
				str +='<img height="24" src="nav_bullet.gif" width="24" />';
				str +='</td> ';
				str +='<td>';
				str +='<div class="subNavTab"><a href="vsgmsg.xml" target="main">VSG Settings</a></div> ';
				str +='</td>'; 
				str +='</tr>';


				/* Statistics Logs Message Throttler Ends */

			
			str +='<tr>';
			str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
			str +='<td><div class="subNavTab"><a href="stlogclt.xml" target="main">Log Cleanup</a></div>';
			str +='</td>';
			str +='</tr>';

			str +='<tr>';
			str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
			str +='<td><div class="subNavTab"><a href="stlogset.htm" target="main">Log Destinations</a></div>';
			str +='</td>';
			str +='</tr>';




	str +='</table>';
	str +='</div>';
	str +='</td>';
	str +='</tr>';


/*==========Log Control Settings Ends============*/


/* 
str +='<tr>';
str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td><div class="subNavTab"><a href="ssdom.htm" target="main">Domain Name Mappings</a></div>';
str +='</td>';
str +='</tr>';
*/
str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/*========= System Settings  Closed========== */

/*========= Network Settings starts========== */

str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="routernavArrow0" onclick="js:toggleMenu(menuArray, subMenus, 1, \'routernavArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 1, \'routernavArrow0\');">Network Settings</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="RouterSwitch" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


/*=========Interfaces Start=========*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="Router_Ports_Interface_Arrow0" onclick="js:toggleMenu(Router_Ports_state, Router_Ports_menu, 0, \'Router_Ports_Interface_Arrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Router_Ports_state, Router_Ports_menu, 0, \'Router_Ports_Interface_Arrow0\');">Interfaces</a></div>';
str +='<div class="navSubCatOff"></div>';
str +='<div align="left" class="navSubCatOff" id="Interfaces" style="margin-left: auto;">';
str +='<table cellpadding="0" cellspacing="0" style="margin-left: 0px;">';
str +='<tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ethports.htm" target="main">Ethernet</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="alias.htm" target="main">Aliases</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr>';
str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td><div class="subNavTab"><a href="lbbndl.xml" target="main">LBFO</a></div>';
str +='</td>';
str +='</tr>';



str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="brports.xml" target="main">Bridge</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="wmstab.xml" target="main">WAN</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="p2pifaces.htm" target="main">Show P2P Interfaces</a></div> ';
str +='</td>';
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';


/*========Interfaces Ends========*/


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="intlvlan.xml" target="main">VLAN</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rsnet.htm" target="main">Secure Networks</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rsnetch.htm" target="main">Change Secure Network</a></div> ';
str +='</td>'; 
str +='</tr>';


/*======IPv4 Routes Starts=======*/
str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="RouteArrow0" onclick="js:toggleMenu(Network_Route_menu_state, Network_Route__menu, 0, \'RouteArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Network_Route_menu_state, Network_Route__menu, 0, \'RouteArrow0\');">IPv4 Routes</a></div>';
str +='<div align="left" class="navSubCatOff" id="RouteSetting" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rstatrou.htm" target="main">Route Table</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rdynrout.htm" target="main">Dynamic Routing</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="mstatrou.xml" target="main">Multicast Route Table</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*======IPv4 Routes Ends=======*/

/*======IPv6 Route Ends=======*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ipv6rt.xml" target="main">IPv6 Route</a></div> ';
str +='</td>'; 
str +='</tr>';

/*======IPv6 Route Ends=======*/

/*======ARP Starts=======*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="ArpArrow0" onclick="js:toggleMenu(Network_arp_menu_state, Network_arp__menu, 0, \'ArpArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Network_arp_menu_state, Network_arp__menu, 0, \'ArpArrow0\');">ARP</a></div>';
str +='<div align="left" class="navSubCatOff" id="ArpSetting" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="arp.htm" target="main">ARP Entries</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="prxyarp.htm" target="main">Proxy ARP</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*=========ARP Ends=========*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="v6nedes.xml" target="main">IPV6 Neighbors</a></div> ';
str +='</td>'; 
str +='</tr>';

/*=========IPV4 DNS Starts========*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="oIPV4DnsArrow0" onclick="js:toggleMenu(Advanced_Dns_state, Advanced_Dns_menu, 0, \'oIPV4DnsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Advanced_Dns_state, Advanced_Dns_menu, 0, \'oIPV4DnsArrow0\');">IPV4 DNS</a></div>';
str +='<div align="left" class="navSubCatOff" id="oIPV4DNS" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="RouterClusterAdvanceddDdnsArrow0" onclick="js:toggleMenu(RouterAdvanced_state, RouterAdvanced_menu, 0, \'RouterClusterAdvanceddDdnsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(RouterAdvanced_state, RouterAdvanced_menu, 0, \'RouterClusterAdvanceddDdnsArrow0\');">Dynamic DNS</a></div>';
str +='<div align="left" class="navSubCatOff" id="RouterAdvancedDDNS" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rdyndns.htm" target="main">Service Records</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ddnstrst.xml" target="main">Trusted Certificate</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dnsradsr.xml" target="main">DNS Server</a></div> ';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dnsrltsi.xml" target="main">DNS Static Entries</a></div> ';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dnsrltch.xml" target="main">DNS Cache</a></div> ';
str +='</td>';
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*=========IPV4 DNS Ends========*/

/*=========IPV6 DNS Starts========*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="oIPV6DnsArrow0" onclick="js:toggleMenu(Advanced_Dns_state, Advanced_Dns_menu, 1, \'oIPV6DnsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Advanced_Dns_state, Advanced_Dns_menu, 1, \'oIPV6DnsArrow0\');">IPV6 DNS</a></div>';
str +='<div align="left" class="navSubCatOff" id="oIPV6DNS" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ipv6dser.xml" target="main">DNS Server</a></div> ';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ipv6dnse.xml" target="main">DNS Static Entries</a></div> ';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ipv6dnch.xml" target="main">DNS Cache</a></div> ';
str +='</td>';
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*=========IPV6 DNS Ends========*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="igmpifac.xml" target="main">IGMP Proxy</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="upnpmain.htm" target="main">UPnP</a></div> ';
str +='</td>'; 
str +='</tr>';




 str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="natls.htm" target="main">NAT Rules</a></div> ';
str +='</td>'; 
str +='</tr>';





/* Security Externalnat Nat bind Starts */

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*======= Network Settings Ends======== */


/*======Cluster Groups Starts==========*/
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="ClusterGroupsArrow0" onclick="js:toggleMenu(menuArray, subMenus, 2, \'ClusterGroupsArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 2, \'ClusterGroupsArrow0\');">HA & LoadBalancing</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="ClusterGroups" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


/*========Cluster Configuration===========*/


str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="ClusterHighRedundancyArrow10" onclick="js:toggleMenu(Cluster_Redundancy_state, Cluster_Redundancy_menu, 0, \'ClusterHighRedundancyArrow10\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Cluster_Redundancy_state, Cluster_Redundancy_menu, 0, \'ClusterHighRedundancyArrow10\');">Cluster Configuration</a></div>';
str +='<div align="left" class="navSubCatOff" id="ClusterRedundancy1" style="margin-left: 4px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="clstrcfg.xml" target="main">Cluster</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="clstrdev.xml" target="main">This Device</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="vsrpdevshow.xml" target="main">Show Cluster Devices</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="hashtdwn.xml" target="main">Shutdown This Device</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="hadevsav.xml" target="main">Save Device Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="hasync.xml" target="main">Synchronize Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="vsrpstats.xml" target="main">VSRP Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/*========High Availability===========*/


str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="ClusterHighRedundancyArrow0" onclick="js:toggleMenu(Cluster_Redundancy_state, Cluster_Redundancy_menu, 1, \'ClusterHighRedundancyArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Cluster_Redundancy_state, Cluster_Redundancy_menu, 1, \'ClusterHighRedundancyArrow0\');">High Availability</a></div>';
str +='<div align="left" class="navSubCatOff" id="ClusterRedundancy" style="margin-left: 4px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="vsrphaen.xml" target="main">Admin Control</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="hastats.xml" target="main">HA Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';


str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="ClusterHighLoadBalancerArrow0" onclick="js:toggleMenu(Cluster_LoadBalancer_state, Cluster_LoadBalancer_menu, 0, \'ClusterHighLoadBalancerArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Cluster_LoadBalancer_state, Cluster_LoadBalancer_menu, 0, \'ClusterHighLoadBalancerArrow0\');">Load Balancing</a></div>';
str +='<div align="left" class="navSubCatOff" id="ClusterLoadBalancer" style="margin-left: 4px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbencl.xml" target="main">Admin Control</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbcalgo.xml" target="main">Algorithm</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbrebal.xml" target="main">Load Redistribution</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbsexp.xml" target="main">Unicast IP Exception Records</a></div> ';
str +='</td>'; 
str +='</tr>';
/*
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbsmexp.xml" target="main">Multicast IP Exception Records</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbsbexp.xml" target="main">Broadcast IP Exception Records</a></div> ';
str +='</td>'; 
str +='</tr>';*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="cippshow.xml" target="main">Intra Cluster Communication Settings</a></div> ';
str +='</td>'; 
str +='</tr>';


/*=============High Availability=========*/

/*=============Statistics / Show Starts=========*/
str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="ClusterStatisticsAArrow0" onclick="js:toggleMenu(Cluster_Statistics_state, Cluster_Statistics_menu, 0, \'ClusterStatisticsAArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Cluster_Statistics_state, Cluster_Statistics_menu, 0, \'ClusterStatisticsAArrow0\');">Statistics / Show</a></div>';
str +='<div align="left" class="navSubCatOff" id="ClusterStatistics" style="margin-left: 4px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


/*
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbninf.xml" target="main">Show Nodes</a></div> ';
str +='</td>'; 
str +='</tr>';
*/
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbloadinfo.xml" target="main">Show Devices</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbsess.xml" target="main">Sessions</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dplbstats.xml" target="main">Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*=============Statistics / Show Ends=========*/

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/*======Cluster Groups Ends==========*/

/*Network Objects Starts*/
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="NetworkObjectsArrow0" onclick="js:toggleMenu(menuArray, subMenus, 3, \'NetworkObjectsArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 3, \'NetworkObjectsArrow0\');">Network Objects</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="NetworkObjects" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr>';
str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td><div class="subNavTab"><a href="senetgrp.htm" target="main">IPv4 Objects</a></div>';
str +='</td>';
str +='</tr>';

str +='<tr>';
str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td><div class="subNavTab"><a href="ipv6db.xml?Snet=general" target="main">IPv6 Objects</a></div>';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fappser.htm" target="main">Service Objects</a></div> ';
str +='</td>'; 
str +='</tr>';
/*
Hidding the Net Group and Service Group support for this(3.5.4) release.
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ngrplist.xml" target="main">Network Groups</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sgrplist.xml" target="main">Service Groups</a></div> ';
str +='</td>'; 
str +='</tr>';
*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ftimewin.htm" target="main">Time Window</a></div> ';
str +='</td>'; 
str +='</tr>';
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="inttbprt.xml" target="main">ALG PortMap</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*Network Objects Ends*/

/*Network Access Control Starts*/
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="AccessArrow0" onclick="js:toggleMenu(menuArray, subMenus, 4, \'AccessArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 4, \'AccessArrow0\');">Network Access Control</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="AccessControl" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="seusrgrp.htm" target="main">User Groups</a></div> ';
str +='</td>'; 
str +='</tr>';


/* Radius */
str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="Router_Ports_Wan_Radius_Arrow0" onclick="js:toggleMenu(Router_Port_Wan_state, Router_Port_Wan_menu, 0, \'Router_Ports_Wan_Radius_Arrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Router_Port_Wan_state, Router_Port_Wan_menu, 0, \'Router_Ports_Wan_Radius_Arrow0\');">Radius Client</a></div>';
str +='<div class="navSubCatOff"></div>';
str +='<div align="left" class="navSubCatOff" id="WanRadius" style="margin-left: auto;">';
str +='<table cellpadding="0" cellspacing="0" style="margin-left: 0px;">';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="radclnts.xml?AppName=radd" target="main">Radius Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="raddfatr.xml?AppName=radd" target="main">Wan user Attributes</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="radstat.xml?AppName=radd" target="main">Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Radius Ends */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ldathcnf.htm" target="main">LDAP Auth Servers</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ntdhttp.htm" target="main">NT Domain Authentication</a></div> ';
str +='</td>'; 
str +='</tr>';


/* Security AdcmUam Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityAdcmUamArrow0" onclick="js:toggleMenu(Security_menu_state, Security_menu, 12, \'SecurityAdcmUamArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Security_menu_state, Security_menu, 12, \'SecurityAdcmUamArrow0\');">UAM</a></div>';
str +='<div align="left" class="navSubCatOff" id="SecurityAdcmUam" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

/* Security AdcmUam UAM Policy Starts */


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sadcsnet.htm" target="main">Admin Control</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';

str +='<div class="subNavTab"><a href="sadcmgrp.htm" target="main">UAM Whitelist Group</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sadcmint.htm" target="main">UAM Interface</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table><table  cellpadding="0" cellspacing="0">'
str +='<tr id="oUVsgEbl"> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sadsnet.htm" target="main">UAM VSG</a></div> ';
str +='</td>'; 
str +='</tr>';
  
/* Security AdcmUam UAM Policy Ends */


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/* Security AdcmUam Ends */



str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="NWStatisticsArrow0" onclick="js:toggleMenu(NetworkAcc_Statistics_menu_state, NetworkAcc_Statistics_menu, 0, \'NWStatisticsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(NetworkAcc_Statistics_menu_state, NetworkAcc_Statistics_menu, 0, \'NWStatisticsArrow0\');">Statistics</a></div>';
str +='<div align="left" class="navSubCatOff" id="NetworkAccStatistics" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pgusrsts.htm" target="main">User Access</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pgusers.htm" target="main">Show Active Users</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';



str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/*Network Access Control Starts*/

/*FireWall Starts*/

str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="FirewallArrow0" onclick="js:toggleMenu(menuArray, subMenus, 5, \'FirewallArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 5, \'FirewallArrow0\');">Firewall</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="Firewall" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

/* Security Firewall Filters Starts */

/*  IPV4 Firewall Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="IPV4FirewallArrow0" onclick="js:toggleMenu(Firewall_menu_state_6to4, Firewall_menu_6to4, 0, \'IPV4FirewallArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Firewall_menu_state_6to4, Firewall_menu_6to4, 0, \'IPV4FirewallArrow0\');">IPV4 Firewall</a></div>';
str +='<div align="left" class="navSubCatOff" id="IPv4Firewall" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ffil.htm" target="main">Access Control List</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="mcpol.htm" target="main">Multicast Access Control List</a></div> ';
str +='</td>'; 
str +='</tr>';


/* Security Forward Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityForwardArrow0" onclick="js:toggleMenu(Security_menu_state, Security_menu, 3, \'SecurityForwardArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Security_menu_state, Security_menu, 3, \'SecurityForwardArrow0\');">Security ByPass Rules</a></div>';
str +='<div align="left" class="navSubCatOff" id="SecurityForward" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

/* Security Forward FwdEnable Starts */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="forward.htm" target="main">Bypass Settings</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwdcache.htm" target="main">Cache</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security Forward FwdEnable  Ends */
str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Security Forward Ends */


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="spttrg.htm" target="main">Port Triggering</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="FirewallIpReservationArrow0" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 0, \'FirewallIpReservationArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 0, \'FirewallIpReservationArrow0\');">IP based Reservation Control</a></div>';
str +='<div align="left" class="navSubCatOff" id="FirewallIpReservation" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwar.htm" target="main">Association Reservation</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwrl.htm" target="main">IP Based Rate Limiting</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwatkcnf.htm" target="main">Attacks Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sesertim.htm" target="main">Service Timeouts</a></div> ';
str +='</td>'; 
str +='</tr>';



/* Security Firewall Filters Ends */

/* Security Firewall Algs Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityFirewallAlgsArrow0" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 1, \'SecurityFirewallAlgsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 1, \'SecurityFirewallAlgsArrow0\');">ALGs</a></div>';
str +='<div align="left" class="navSubCatOff" id="SecurityFirewallAlgs" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


/* Security Firewall Algs Traversal Starts */
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="algtravl.xml" target="main">ALG Traversal Control</a></div> ';
str +='</td>'; 
str +='</tr>';
/* Security Firewall Algs Traversal Ends */



/* Security Firewall Algs Dns Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityFirewallAlgsdnsArrow0" onclick="js:toggleMenu(SecurityFirewallAlgs_menu_state, SecurityFirewallAlgs_menu, 0, \'SecurityFirewallAlgsdnsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewallAlgs_menu_state, SecurityFirewallAlgs_menu, 0, \'SecurityFirewallAlgsdnsArrow0\');">DNS</a></div>';
str +='<div align="left" class="navSubCatOff" id="SecurityFirewallAlgsdns" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dnsalg.htm" target="main">DNS Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dnsav4lst.xml" target="main">DNS Policy Record</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Security Firewall Algs Dns Ends */

/* Security Firewall Algs Ftp Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="FirewallAlgsFilterArrow0" onclick="js:toggleMenu(FirewallAlgs_Filters_menu_state, FirewallAlgs_Filters_menu, 0, \'FirewallAlgsFilterArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(FirewallAlgs_Filters_menu_state, FirewallAlgs_Filters_menu, 0, \'FirewallAlgsFilterArrow0\');">Application Filters</a></div>';
str +='<div align="left" class="navSubCatOff" id="FirewallAlgsFilters" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ftpfl.xml" target="main">FTP</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security Firewall Algs Ftp Ends */

/* Security Firewall Algs Nntp Starts */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="nntpfl.xml" target="main">NNTP</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security Firewall Algs Nntp Ends */

/* Security Firewall Algs SMTP Starts */


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="smtpfl.xml" target="main">SMTP</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security Firewall Algs Smtp Ends */
/* Security Firewall Algs Rpc Starts */


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="rpcfl.xml" target="main">RPC</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';  

/* Security Firewall Algs Rpc Ends */


/* Security Firewall Algs NetBios Starts */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="nbtalg.htm" target="main">NetBIOS </a></div> ';
str +='</td>'; 
str +='</tr>';
 


/* Security Firewall Algs NetBios Ends */


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
  

/* Security Firewall Algs Rpc Ends */


/* Security socksproxy Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecuritysocksproxyArrow0" onclick="js:toggleMenu(Security_menu_state, Security_menu, 15, \'SecuritysocksproxyArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Security_menu_state, Security_menu, 15, \'SecuritysocksproxyArrow0\');">Socks Proxy</a></div>';
str +='<div align="left" class="navSubCatOff" id="Securitysocksproxy" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

/* Security socksproxy Policy Starts */

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'fwtbscks.xml\',true,\'\')" target="main">Policy Records</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwsckprt.xml" target="main">Port Setting</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwsckaut.xml" target="main">Authentication Setting</a></div> ';
str +='</td>'; 
str +='</tr>';

 
/* Security socksproxy Policy Ends*/

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Security socksproxy Ends */

// This Link is removed as per srini's comment
 /*Security Webproxy Starts 

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecuritywebproxyArrow0" onclick="js:toggleMenu(Security_menu_state, Security_menu, 5, \'SecuritywebproxyArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Security_menu_state, Security_menu, 5, \'SecuritywebproxyArrow0\');">Web Proxy</a></div>';
str +='<div align="left" class="navSubCatOff" id="SecurityWebProxy" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';



/* Security Webproxy Proxy cong Starts 

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pcqcfg.htm" target="main">Proxy Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pcqavcfg.htm" target="main">AntiVirus Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxykwlst.xml" target="main">Default Keyword List</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxywhlst.xml" target="main">Default White List</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxybllst.xml" target="main">Default Black List</a></div> ';
str +='</td>'; 
str +='</tr>';
 
  if ((sIIPS != "IIPS") && (sDSHBRD  == "DSHBRD")) 
  {
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pcqcerb.htm" target="main">Default Category List</a></div> ';
str +='</td>'; 
str +='</tr>';

}
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxytwlst.xml" target="main">Default Timewindow</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxytrlst.xml" target="main">Proxy Trust IP Address</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pxydplst.xml" target="main">Default Profiles</a></div> ';
str +='</td>'; 
str +='</tr>';

/* Security Webproxy Proxy cong Ends 

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Security Webproxy Ends */

/*=====Local proxy Starts======*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="lplst.htm" target="main">Local Proxy</a></div> ';
str +='</td>'; 
str +='</tr>';

/*=====Local proxy Ends======*/


// This Link is removed as per srini's comment
/*=====Http proxy Starts======

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="seproxy.htm" target="main">HTTP Proxy</a></div> ';
str +='</td>'; 
str +='</tr>';

/*=====Http proxy Ends======*/
/* Security Firewall Compre Starts */





/* Firewall Statistics Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityFirewallStatsvArrow0" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 4, \'SecurityFirewallStatsvArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 4, \'SecurityFirewallStatsvArrow0\');">Statistics</a></div>';
str +='<div align="left" class="navSubCatOff" id="FirewallStats" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="stfattck.htm" target="main">Firewall Attacks</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="stfses.htm" target="main">Firewall Sessions</a></div> ';
str +='</td>'; 
str +='</tr>';
    

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="polstats.htm" target="main">Policy Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Firewall Statistics Ends */

/* Firewall Performance Settings Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityFirewallPerfSetttingsArrow0" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 5, \'SecurityFirewallPerfSetttingsArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 5, \'SecurityFirewallPerfSetttingsArrow0\');">Performance Settings</a></div>';
str +='<div align="left" class="navSubCatOff" id="FirewallPerfSettings" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fpsesrvl.xml" target="main">Session revalidation</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Firewall Performance Settings Ends */

if(MENU_ADVANCED)
{
	
/* Firewall advanced Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="SecurityFirewalladvArrow0" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 6, \'SecurityFirewalladvArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(SecurityFirewall_menu_state, SecurityFirewall_menu, 6, \'SecurityFirewalladvArrow0\');">Advanced</a></div>';
str +='<div align="left" class="navSubCatOff" id="FirewallAdvanced" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ps.htm" target="main">Portscan</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr>';
str +='<td width="18"><img height="24" src="nav_bullet.gif" width="24" /></td>';
str +='<td><div class="subNavTab"><a href="macdbs.htm" target="main">MAC Filtering</a></div>';
str +='</td>';
str +='</tr>';

/* Security Firewall UPNP Starts */
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="seupnp.htm" target="main">UPNP NAT-T Control</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="seipreas.htm" target="main">IP Reassembly</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="setcprs.htm" target="main">TCP Resequencing</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="setlimit.htm" target="main">Connection Limits</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="stelthmd.htm" target="main">Stealth Mode</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="defhost.htm" target="main">Honey Pot Host IP</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
}
/*  IPV4 Firewall Ends*/
/*  IPV6 Firewall Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="IPV6FirewallArrow0" onclick="js:toggleMenu(Firewall_menu_state_6to4, Firewall_menu_6to4, 1, \'IPV6FirewallArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Firewall_menu_state_6to4, Firewall_menu_6to4, 1, \'IPV6FirewallArrow0\');">IPV6 Firewall</a></div>';
str +='<div align="left" class="navSubCatOff" id="IPv6Firewall" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'ffill.xml\',true,\'PolGrpNm= \')" target="main">Access Control List</a></div> ';
str +='</td>';
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="algv6trl.xml" target="main">ALG Traversal Control</a></div> ';
str +='</td>';
str +='</tr>';

/*IPV6 FireWall NATPT Starts*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="IPV6NatptArrow0" onclick="js:toggleMenu(IPV6_Firewall_menu_state, IPV6_Firewall_menu, 0, \'IPV6NatptArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(IPV6_Firewall_menu_state, IPV6_Firewall_menu, 0, \'IPV6NatptArrow0\');">NATPT</a></div>';
str +='<div align="left" class="navSubCatOff" id="IPv6Natpt" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'natpt.xml\',true,\'dir=6to4&nettype=corp\')" target="main">6 to 4</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'natpt.xml\',true,\'dir=4to6&nettype=corp\')" target="main">4 to 6</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'fwtbntpx.xml\',true,\'\')" target="main">Prefix</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*IPV6 FireWall NATPT Ends*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="fwv6ses.xml?Snet=general" target="main">Firewall Sessions</a></div> ';
str +='</td>';
str +='</tr>';

/*IPV6 FireWall Advanced Starts*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="IPV6FirewallAdvArrow0" onclick="js:toggleMenu(IPV6_Firewall_menu_state, IPV6_Firewall_menu, 1, \'IPV6FirewallAdvArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(IPV6_Firewall_menu_state, IPV6_Firewall_menu, 1, \'IPV6FirewallAdvArrow0\');">Advanced</a></div>';
str +='<div align="left" class="navSubCatOff" id="IPv6FirewallAdv" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

/*IPV6 Port scan*/
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="v6ps.xml?Snet=general" target="main">Port Scan</a></div> ';
str +='</td>';
str +='</tr>';
/* end Port scan*/

/*IPV6 FireWall Reassembly Starts*/

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="IPV6ReasArrow0" onclick="js:toggleMenu(IPV6_Reass_menu_state, IPV6_Reass_menu, 0, \'IPV6ReasArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(IPV6_Reass_menu_state, IPV6_Reass_menu, 0, \'IPV6ReasArrow0\');">IPV6 Virtual Reassembly</a></div>';
str +='<div align="left" class="navSubCatOff" id="IPv6Reass" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="intconf.xml" target="main">Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="intstats.xml" target="main">Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';

/*
str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="intbufst.xml" target="main">Buffer Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';
*/

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*IPV6 FireWall Reassembly Ends*/

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*IPV6 FireWall Advanced Ends*/
str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/*  IPV6 Firewall Ends */

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/*FireWall Ends*/

/*========= VPN Starts =========*/

if ((sVPN  == "VPN"  ) ||
      (sPKI  == "PKI"  ) ||
      (sLDAP == "LDAP" ))
  {



if(vpnFlag==true)
	  {
		vpnIpsec();
	  }
	  else
	  {
		str +='<tr>';
		str +='<td align="left">';

		str +='<table cellpadding="0" cellspacing="0" width="240">';
		str +='<tr valign="top">';
		str +='<td width="18">';
		str +='<a href="#n">';
		str +='<img height="24" id="VpnArrow0" onclick="js:toggleMenu(menuArray, subMenus, 6, \'VpnArrow0\');" src="plus.gif" width="24" /></a>';
		str +='</td>';
		str +='<td class="navTab" width="164"> ';
		str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 6, \'VpnArrow0\');">VPN</a>';
		str +='</td> ';
		str +='</tr> ';
		str +='</table> ';

		str +='<div align="left" class="navSubCatOff" id="VPNdivId" style="margin-left: 24px;"> ';
		str +='<table cellpadding="0" cellspacing="0"> ';


		str +='<tr> ';
		str +='<td width="18">';
		str +='<img height="24" src="nav_bullet.gif" width="24" />';
		str +='</td> ';
		str +='<td>';
		str +='<div class="subNavTab">No Package installed</div> ';
		str +='</td>'; 
		str +='</tr>';
	  }





str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

}
/*========= VPN Ends =========*/



/*========= SSLVPN Starts=========*/
if(bSslVpnEnabled==true)
	  {
		sslVpnMenu();
	  }
	  else
	  {
	str +='<tr>';
	str +='<td align="left">';
	str +='<table cellpadding="0" cellspacing="0" width="240">';
	str +='<tr valign="top">';
	str +='<td width="18">';
	str +='<a href="#n">';
	str +='<img height="24" id="SslVpnArrow0" onclick="js:toggleMenu(menuArray, subMenus, 15, \'SslVpnArrow0\');" src="plus.gif" width="24" /></a>';
	str +='</td>';
	str +='<td class="navTab" width="164"> ';
	str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 15, \'SslVpnArrow0\');">SSLVPN</a>';
	str +='</td> ';
	str +='</tr> ';
	str +='</table> ';



	str +='<div align="left" class="navSubCatOff" id="SslVpn" style="margin-left: 24px;"> ';
	str +='<table cellpadding="0" cellspacing="0"> ';



		str +='<tr> ';
		str +='<td width="18">';
		str +='<img height="24" src="nav_bullet.gif" width="24" />';
		str +='</td> ';
		str +='<td>';
		str +='<div class="subNavTab">No Package installed</div> ';
		str +='</td>'; 
		str +='</tr>';


		str +='</table>';
		str +='</div>';
		str +='</td>';
		str +='</tr>';

	  }
/*========= SSLVPN Ends=========*/




/*=========IPS Starts==========*/
if(iipsFlag==true)
	  {
		iipsFun();
	  }
	  else
	  {


		str +='<tr>';
		str +='<td align="left">';

		str +='<table cellpadding="0" cellspacing="0" width="240">';
		str +='<tr valign="top">';
		str +='<td width="18">';
		str +='<a href="#n">';
		str +='<img height="24" id="IPSArrow0" onclick="js:toggleMenu(menuArray, subMenus, 7, \'IPSArrow0\');" src="plus.gif" width="24" /></a>';
		str +='</td>';
		str +='<td class="navTab" width="164"> ';
		str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 7, \'IPSArrow0\');">IIPS</a>';
		str +='</td> ';
		str +='</tr> ';
		str +='</table> ';

		str +='<div align="left" class="navSubCatOff" id="IIPSdivId" style="margin-left: 24px;"> ';
		str +='<table cellpadding="0" cellspacing="0"> ';




		str +='<tr> ';
		str +='<td width="18">';
		str +='<img height="24" src="nav_bullet.gif" width="24" />';
		str +='</td> ';
		str +='<td>';
		str +='<div class="subNavTab">No Package installed</div> ';
		str +='</td>'; 
		str +='</tr>';


		str +='</table>';
		str +='</div>';
		str +='</td>';
		str +='</tr>';

	  }

/*=========IPS Ends==========*/



/*========= AntiX Starts =========*/
if( sAntiX  == "AntiX" )
{
  if(AntiXFlag == true)
  {
    antiXMenu();
  }
  else
  {
    str +='<tr>';
    str +='<td align="left">';

    str +='<table cellpadding="0" cellspacing="0" width="240">';
    str +='<tr valign="top">';
    str +='<td width="18">';
    str +='<a href="#n">';
    str +='<img height="24" id="AntiXArrow0" onclick="js:toggleMenu(menuArray, subMenus, 14, \'AntiXArrow0\');" src="plus.gif" width="24" /></a>';
    str +='</td>';
    str +='<td class="navTab" width="164"> ';
    str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 14, \'AntiXArrow0\');">Anti-X</a>';
    str +='</td> ';
    str +='</tr> ';
    str +='</table> ';

    str +='<div align="left" class="navSubCatOff" id="AntiX" style="margin-left: 24px;"> ';
    str +='<table cellpadding="0" cellspacing="0"> ';

    str +='<tr> ';
    str +='<td width="18">';
    str +='<img height="24" src="nav_bullet.gif" width="24" />';
    str +='</td> ';
    str +='<td>';
    str +='<div class="subNavTab">No Package installed</div> ';
    str +='</td>'; 
    str +='</tr>';

    str +='</table>';
    str +='</div>';
    str +='</td>';
    str +='</tr>';
  }

}
/*========= AntiX Ends =========*/

/*========TM Starts=========*/

if(qosFlag==true)
	  {
		qosFun();
	  }
	  else
	  {


str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="qosnavArrow0" onclick="js:toggleMenu(menuArray, subMenus, 8, \'qosnavArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 8, \'qosnavArrow0\');">Traffic Management</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="QoSSettings" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab">No Package installed</div> ';
str +='</td>'; 
str +='</tr>';


str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

}
/*========TM Ends=========*/



/*======Certificate Manager Starts======*/
/*Security Vpn pki Starts*/



		str +='<tr>';
		str +='<td align="left">';

		str +='<table cellpadding="0" cellspacing="0" width="240">';
		str +='<tr valign="top">';
		str +='<td width="18">';
		str +='<a href="#n">';
		str +='<img height="24" id="CertMngrArrow0" onclick="js:toggleMenu(menuArray, subMenus, 9, \'CertMngrArrow0\');" src="plus.gif" width="24" /></a>';
		str +='</td>';
		str +='<td class="navTab" width="164"> ';
		str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 9, \'CertMngrArrow0\');">Certificate Manager</a>';
		str +='</td> ';
		str +='</tr> ';
		str +='</table> ';

		str +='<div align="left" class="navSubCatOff" id="CertMngrId" style="margin-left: 24px;"> ';
		str +='<table cellpadding="0" cellspacing="0"> ';


if(vpnFlag==true)
{
/*Security Vpn pki privatekey Starts*/

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'pkitcapp.xml\',true,\'mod=PKI-VPN\')" target="main">Trusted Certificates </a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="javascript:setLocationFire(\'pkiselft.xml\',true,\'mod=PKI-VPN\')" target="main">Self Certificates</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pkicrltl.htm" target="main">CRLs</a></div> ';
str +='</td>'; 
str +='</tr>';
/*Security Vpn pki privatekey Ends*/
}


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="pkiocsp.htm" target="main">OCSP</a></div> ';
str +='</td>'; 
str +='</tr>';



str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';


/*======Certificate Manager Ends======*/


/* Statistics */
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="systemStatsArrow0" onclick="js:toggleMenu(menuArray, subMenus, 10, \'systemStatsArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 10, \'systemStatsArrow0\');">Statistics</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="Statistics" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="stnetwrk.htm" target="main">Active Sessions</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="mcadstats.htm" target="main">Multicast Statistics</a></div> ';
str +='</td>'; 
str +='</tr>';

if(AntiXFlag == true)
{
  str +='<tr> ';
  str +='<td width="18">';
  str +='<img height="24" src="nav_bullet.gif" width="24" />';
  str +='</td> ';
  str +='<td>';
  str +='<div class="subNavTab"><a href="avas_stats.htm" target="main">Email Statistics</a></div> ';
  str +='</td>'; 
  str +='</tr>';
}

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';


/* Stats Ends */

/* Maintenance Settings */
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="maintnavArrow0" onclick="js:toggleMenu(menuArray, subMenus, 11, \'maintnavArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 11, \'maintnavArrow0\');">Maintenance</a>';
str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="Maintenance" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

if(vpnFlag == true && iipsFlag == true && qosFlag == true && AntiXFlag == true)
{
	str +='<tr> ';
	str +='<td width="18">';
	str +='<img height="24" src="nav_bullet.gif" width="24" />';
	str +='</td> ';
	str +='<td>';
	str +='<div class="subNavTab"><a href="ssfirmup.htm" target="main">Upgrade Firmware</a></div> ';
	str +='</td>';
	str +='</tr>';
}

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="sssave.htm" target="main">Save Configuration</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ssreboot.htm" target="main">Restart System</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ssimpexp.htm" target="main">Import/Export</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ssresfac.htm" target="main">Factory Defaults</a></div> ';
str +='</td>'; 
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';
/* maint Settings Ends*/



/* Diagnostics Starts */
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="systemDiagArrow0" onclick="js:toggleMenu(menuArray, subMenus, 12, \'systemDiagArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 12, \'systemDiagArrow0\');">Diagnostics</a>';

str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="Diagnostics" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="diping.htm" target="main">Ping Test</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ditrace.htm" target="main">Trace Route</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="didns.htm" target="main">Domain Name Reslover</a></div> ';
str +='</td>'; 
str +='</tr>';


/* Diagnostics Result Starts */

str +='<tr valign="top">';
str +='<td><a href="#n"><img height="24" id="systemDiagresArrow0" onclick="js:toggleMenu(Diag_menu_state, Diag_menu, 0, \'systemDiagresArrow0\');" src="plus.gif" width="24" /></a></td>';
str +='<td><div class="subNavTab"><a href="#n" onclick="js:toggleMenu(Diag_menu_state, Diag_menu, 0, \'systemDiagresArrow0\');">Historical Results</a></div>';
str +='<div align="left" class="navSubCatOff" id="DiagResult" style="margin-left: 2px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';

str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="dipingsh.htm" target="main">Ping</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="ditracsh.htm" target="main">Trace Route</a></div> ';
str +='</td>'; 
str +='</tr>';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="didnsrsh.htm" target="main">Domain Name Reslove</a></div> ';
str +='</td>'; 
str +='</tr>';

  
/* Diagnostics Result Ends */

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';



/* Diagnostics Ends */







/* Diagnostics Starts */
str +='<tr>';
str +='<td align="left">';

str +='<table cellpadding="0" cellspacing="0" width="240">';
str +='<tr valign="top">';
str +='<td width="18">';
str +='<a href="#n">';
str +='<img height="24" id="ShowLogMessagesArrow0" onclick="js:toggleMenu(menuArray, subMenus, 13, \'ShowLogMessagesArrow0\');" src="plus.gif" width="24" /></a>';
str +='</td>';
str +='<td class="navTab" width="164"> ';
str +='<a href="#n" onclick="js:toggleMenu(menuArray, subMenus, 13, \'ShowLogMessagesArrow0\');">Show Log Messages</a>';

str +='</td> ';
str +='</tr> ';
str +='</table> ';

str +='<div align="left" class="navSubCatOff" id="ShowLogMessages" style="margin-left: 24px;"> ';
str +='<table cellpadding="0" cellspacing="0"> ';


str +='<tr> ';
str +='<td width="18">';
str +='<img height="24" src="nav_bullet.gif" width="24" />';
str +='</td> ';
str +='<td>';
str +='<div class="subNavTab"><a href="showmsgfm.xml" target="main">Message Families</a></div> ';
str +='</td>';
str +='</tr>';




/* Net Access Ends */



str +='</table>';
str +='</div>';
str +='</td>';
str +='</tr>';

/* Diagnostics Ends */


}
return str;
}









function setLocation(lnk,hasSnet,params)
{
	var tHref = "";
	if(hasSnet)
	{
		if(params == '')
		{
			tHref = lnk+"?snet="+getCookieLocal("Name-SNET");
		}
		else
		{
			tHref = lnk+"?snet="+getCookieLocal("Name-SNET")+"&"+params;
		}
	}
	else
	{
		tHref = lnk+"?"+params;
	}
	location.href = tHref;
}

function setLocationFire(lnk,hasSnet,params)
{

        var tHref = "";
        if(hasSnet)
        {
                if(params == '')
                {
                        tHref = lnk+"?Snet="+getCookieLocal("Name-SNET");
                }
                else
                {
                        tHref = lnk+"?Snet="+getCookieLocal("Name-SNET")+"&"+params;
                }
        }
        else
        {
                tHref = lnk+"?"+params;
        }
       top.bottom.main.location.href = tHref;
}


function getCookieLocal(name)
{
 var cstr = "" + document.cookie;
 var index1 = cstr.indexOf(name);

 if (name=="" || index1== -1) return "";

 var index2 = cstr.indexOf( ';' , index1);
 if (index2 == -1) index2 = cstr.length;

 return unescape(cstr.substring(index1+name.length+1, index2));
}

function mmLoadMenus()
{
return 0;
}

function includeheader()
{
return 0;
}

function openHelpWindow(HelpInfo)
{
  remote = window.open(HelpInfo, 'helpwindow', 'width=' + 600 + ',height=' + 500 +', resizable=yes,scrollbars=yes,toolbar=yes,menubar=yes,copyhistory=yes');
  if (remote != null)
  {
    if (remote.opener == null)
      remote.opener = _self;
    remote.name = 'HelpWindow';
    remote.location.href = HelpInfo;
  }
  remote.focus();
  return 0;
}
/*License Manager script functions*/
function submitLicenseManager(){
  var frm = document.forms["xmlsubmitavas"];
  var xs = "<action>\n";
  xs += "<Browser successxsl='licenman' failurexsl='licenman' sredirect='sslcauth.xml' fredirect='failure.xml' />\n";
  xs += "<LICMGR_getServiceList>\n";
  xs += "<status>ALL</status>\n";
  xs += "</LICMGR_getServiceList>\n";
  xs += " </action>";
  frm.xmlreq.value = xs;
  frm.submit();
}

function submitLicenseManagerManageServicePage(){
  var frm = document.forms["xmlsubmitavas"];
  var xs = "<action>\n";
  xs += "<Browser successxsl='mgrserv' failurexsl='mgrserv' sredirect='sslcauth.xml' fredirect='failure.xml' />\n";
  xs += "<LICMGR_getServiceList>\n";
  xs += "<status>ALL</status>\n";
  xs += "</LICMGR_getServiceList>\n";
  xs += " </action>";
  frm.xmlreq.value = xs;
  frm.submit();
}
function submitLicenseManagerUploadLicensePage(serviceId){
  var frm = document.forms["xmlsubmitavas"];
  var xs = "<action>\n";
  xs += "<Browser successxsl='licnsupl' failurexsl='licnsupl' sredirect='sslcauth.xml' fredirect='failure.xml' />\n";
  xs += "<LICMGR_getPartnerServiceList>\n";
  xs += "<serviceId>"+serviceId+"</serviceId>\n";
  xs += "</LICMGR_getPartnerServiceList>\n";
  xs += " </action>";
  frm.xmlreq.value = xs;
  frm.submit();
}

function submitLicenseManagerRequestPage(serviceId)
{
  var frm = document.forms["xmlsubmitavas"];
  var xs = "<action>\n";
  xs += "<Browser successxsl='licgenrq' failurexsl='licgenrq' sredirect='sslcauth.xml' fredirect='failure.xml' />\n";
  xs += "<LICMGR_getPartnerServiceList>\n";
  xs += "<serviceId>"+serviceId+"</serviceId>\n";
  xs += "</LICMGR_getPartnerServiceList>\n";
  xs += " </action>";
  frm.xmlreq.value = xs;
  frm.submit();
}
/*License Manager script functions*/

function includefooter()
{
if(logoFlag==true)
	{	
	  document.write("<tr>");
	  document.write("<td class=\"right\">");
	  //document.write("<img src=\"logoicon.gif\"/>");
	  document.write("<img src=\"images/VortiqaLogo.gif\"/>");
	  document.write("</td>");
	  document.write("</tr>");

	}

}
