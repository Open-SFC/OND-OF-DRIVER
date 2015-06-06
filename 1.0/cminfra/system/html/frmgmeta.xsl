<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
  <xsl:include href="radio_meta.xsl"/>
  <xsl:include href="list_meta.xsl"/>
  <xsl:include href="mlist_meta.xsl"/>
  <xsl:include href="checkbox_meta.xsl"/>
  <xsl:include href="password_meta.xsl"/>
  <xsl:include href="tblfrm.xsl"/>
  <xsl:include href="label_meta.xsl"/>
  <xsl:include href="selcrtl.xsl"/>
  <xsl:include href="textarea_meta.xsl"/>
  <xsl:include href="file_meta.xsl"/>
  <xsl:include href="com_tmpl.xsl"/>

  <xsl:template match="/">
      <html>
       <head>
        <title></title>
        <link rel="stylesheet" type="text/css" href="form.css"/>
        <script language="JavaScript1.2" src="cookie.js"></script>
        <script language="JavaScript1.2" src="host.js"/>
        <script language="JavaScript1.2" src="com_utlils.js"/>
        <script language="JavaScript1.2" src="validate.js"/>
        <script language="JavaScript1.2" src="productinfo.js"></script>
        <script type="text/JavaScript" language="JavaScript">
        <![CDATA[        
          var confType="";
          var metaFile="";
          var enableJSVal=false;
          function init(status, msg, cookie, cnfType) {
              confType=cnfType;   
              //alert("frm meta :: cnfType = " + cnfType);
              if(status == "error") {
                //alert("status = " +status);
                //cookieForms('myform', 'load');
                document.getElementById("oError").innerHTML
                           = "<html> <font color='red'> <b>"
                             + msg  +" </b> </font> </html>";
                document.getElementById("oError").style.display="block";
              }
              delFCookie();
           }
           var dmArray = new Array();
           var gKeyArray = new Array();
           var kCount=0;
           var count=0;
           var gopcode=""; 

           function sendReq(cookie, vrtInstVar, applyOpt, keys) {//alert(applyOpt);             
             var f = document.forms[1];
             var i = 0;
             var postStr="";
             var parentTxStr="";
             var isInstReq="true";
             var gdmPath="";
             var keyValArray = new Array(); 
             dmArray = new Array();
             var finalPostStr="";
             var nameValStr="";
             count = 0;
             var multiCmdStr="";
             if (document.forms[0].opcode.value != "MULTCMD") {
                 gopcode = document.forms[0].opcode.value; 
             } else {
                 document.forms[0].opcode.value = gopcode;
             }
             if(enableJSVal){
               if(!validate())
                  return;
             }

             for(i = 0; i < f.elements.length; i++){
               var name = f.elements[i].name;
               var index1 = f.elements[i].name.lastIndexOf("$$");
               if (index1 != -1)
               {
                 name = f.elements[i].name.substring(0, index1);
               }
               var val;
               if(f.elements[i].name=="useparenttx")
               {
                 parentTxStr = f.elements[i].value;
                 //alert("sendReq:: parentTxStr= " + parentTxStr);
               }
               if ((f.elements[i].type == "select-multiple") && 
                   (f.elements[i].getAttribute("dmpath") != null) && 
                   (f.elements[i].getAttribute("depends") != null)){                   
                   var componentUsed="";
                   var parentKeys =  f.elements[i].getAttribute("parentkeys");
                   //alert("parentKeys " + parentKeys);                   
                   var eleOptCode = f.elements[i].getAttribute("opcode");
                   if(null == eleOptCode) {
                     eleOptCode = document.forms[0].opcode.value;
                   }                   
                   var selCtrl;
                   if(document.getElementById("targetList_"+f.elements[i].name) != null)
                   {
                     selCtrl=document.getElementById("targetList_"+f.elements[i].name);
                     componentUsed="selcrtl";
                   } else
                   {
                     selCtrl=f.elements[i];
                     componentUsed="mlist";
                   }
                   var dmpath = selCtrl.getAttribute("dmpath");
                   if(parentKeys != null) 
                   {
                     dmpath = procParentKeys(dmpath, parentKeys);
                   }

                   if ((f.elements[i].getAttribute("action") != null) && 
                      (f.elements[i].getAttribute("action") == 'multiReq') &&                      
                      (f.elements[i].getAttribute("opcode") == 'ADD')){
                        if(componentUsed=="selcrtl")
                        { //for select control component...                           
                          for (var optInd=0; optInd<selCtrl.length; optInd++) {                           
                               var dmpathTemp = dmpath + "{" + selCtrl[optInd].value + "}";
                               dmpathTemp += "!" + eleOptCode;
                               dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                               multiCmdStr += dmpathTemp + f.elements[i].name + "=" + selCtrl[optInd].value;
                          }
                        } else { //for mlist component...                         
                          for (var optInd=0; optInd<selCtrl.length; optInd++) {
                            if (selCtrl[optInd].selected) { 
                               var dmpathTemp = dmpath + "{" + selCtrl[optInd].value + "}";
                               dmpathTemp += "!" + eleOptCode;
                               dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                               multiCmdStr += dmpathTemp + f.elements[i].name + "=" + selCtrl[optInd].value;
                            }
                          }
                        }//alert(multiCmdStr);                     
                   }
                   else if ((f.elements[i].getAttribute("action") != null) && 
                      (f.elements[i].getAttribute("action") == 'multiReq') && 
                      (f.elements[i].getAttribute("opcode") == 'EDIT')){

		         var index = f.elements[i].name.lastIndexOf("#");
		         var nameStr =  f.elements[i].name.substring(0,index);
		         var selArrName = nameStr+"SelArry";
		         var arrFltCat = infoMap.Get(selArrName);
                         var opcodeElem = selCtrl.getAttribute("opcode");

		         if (arrFltCat == undefined) arrFltCat = new Array();
		         if (arrFltCat) {
                          if(componentUsed=="selcrtl")
                          { //for select control component... 
		           //Remove List...
		           for (var ind=0; ind<arrFltCat.length; ind++) {
		             var val = arrFltCat[ind].nodeValue;
		             //alert("val= " +val);
		             //Check whether this val is in the right select box; 
		             //if not, send delete command
		             var present = "false";
		             for (var optInd=0; optInd < selCtrl.length; optInd++) {
		               if (val == selCtrl[optInd].value) {present = "true"; break;}
		             }
		             if (present == "false") {		               
		               //alert("Send delete command for: " + val);
		               //sendSelctrlReq(f.elements[i],cookie,"DEL",val);
                               var dmpathTemp = dmpath + "{" + val + "}";
                               dmpathTemp += "!DEL";
                               dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                               multiCmdStr += dmpathTemp + f.elements[i].name + "=" + val;                               
		             }
		           }
		           //Add List...                            
		           for (var optInd=0; optInd < selCtrl.length; optInd++) {
		             var added = "true";        
		             for (var ind=0; ind<arrFltCat.length; ind++) {
		               if (selCtrl[optInd].value == arrFltCat[ind].nodeValue) {
		                 added = "false"; break;
		               }
		             }
		             if (added == "true") {		               
		               //alert("Send add command for: " + selCtrl[optInd].value);
		               //sendSelctrlReq(f.elements[i],cookie,"ADD",selCtrl[optInd].value);
                               var dmpathTemp = dmpath + "{" + selCtrl[optInd].value + "}";
                               dmpathTemp += "!ADD";
                               dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                               multiCmdStr += dmpathTemp + f.elements[i].name + "=" + selCtrl[optInd].value;
		             }
		           }
                         } else { //for mlist component...
 		           //Add List...                            
		           for (var optInd=0; optInd < selCtrl.length; optInd++) {		              
                             if (selCtrl[optInd].selected) {
                               var added = "true";       
		               for (var ind=0; ind<arrFltCat.length; ind++) {
		                 if (selCtrl[optInd].value == arrFltCat[ind].nodeValue) {
		                    added = "false"; break;
		                 }
		               }
		               if(added == "true") {
                                  var dmpathTemp = dmpath + "{" + selCtrl[optInd].value + "}";
                                  dmpathTemp += "!ADD";
                                  dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                                  multiCmdStr += dmpathTemp + f.elements[i].name + "=" + selCtrl[optInd].value;                                   
                               }
                            }
                           }
 		           //Removed List...                            
		           for (var optInd=0; optInd < selCtrl.length; optInd++) {		              
                             if (!selCtrl[optInd].selected) {
                               var removed = "false";       
		               for (var ind=0; ind<arrFltCat.length; ind++) {
		                 if (selCtrl[optInd].value == arrFltCat[ind].nodeValue) {
		                    removed = "true"; break;
		                 }
		               }
		               if(removed == "true") {
                                  var dmpathTemp = dmpath + "{" + selCtrl[optInd].value + "}";
                                  dmpathTemp += "!DEL";
                                  dmpathTemp = "^ucdm_" + dmpathTemp + "^";
                                  multiCmdStr += dmpathTemp + f.elements[i].name + "=" + selCtrl[optInd].value;           
                               }
                            }
                           }
                                  
                         }
                      }//end if (arrFltCat)                    

                  }//end if action=multiReq
		  else //if action=singleReq or action tag not given
                  {//alert("singleReq");                       
                      var csVal = "";
                      var selCtrl = document.getElementById("targetList_"+f.elements[i].name);
                       if (selCtrl == null) {
                           selCtrl = document.getElementById(f.elements[i].name);
                             for (var optInd=0; optInd < selCtrl.length; optInd++) {
                                 if (selCtrl[optInd].selected) { 
                                        csVal += selCtrl[optInd].value + ",";
                                 }
                             }
                       } else {
                             for (var optInd=0; optInd < selCtrl.length; optInd++) {
                                 csVal += selCtrl[optInd].value + ",";
                             }
                       }
                       if (csVal.charAt(csVal.length - 1) == ',') {
                              csVal = csVal.substring(0,csVal.lastIndexOf(","));
                       }                       
                       postStr = postStr + f.elements[i].name + "="+ csVal+"|";                      
                  }

               } //end if select-multiple
               else if ((f.elements[i].getAttribute("dynamic")!= null) && 
                      (f.elements[i].getAttribute("dynamic")=='true')){
                         sendDynamicReq(f.elements[i],cookie);
               }
               else {
                  if(f.elements[i].type == "radio") {
                    if(!f.elements[i].checked){
                       continue;
                    }
                  }
                  if(f.elements[i].type == "checkbox") {
                    if(f.elements[i].checked){
                      val="1";
                    }else {
                      val="0";
                    }
                  }else {
                    val = f.elements[i].value;
                    val = encodeURIComponent(val);
                  }
      
                  //alert("name = " + name + "  value = " + val);
                  if(name == "vsgname#STR" || cookie == '') {
                     isInstReq = "false";
                  } 

                  if(name != "" && val != "" ){
                    var tempPostStr ="";
                     if(applyOpt != '' && applyOpt == "FRMTBL") {
                        if (name.indexOf("operator#STR") != -1) {
                           var tmpname = name.replace("operator","");
                     if (document.getElementById(tmpname).value == "") {
                                  //no need to send the operator when the value is empty
                           } else {
                                  nameValStr = name + "="+ val+"|";
                           }
                        } else {
                          nameValStr = name + "="+ val+"|";
                        }
                     } else {
                        nameValStr = name + "="+ val+"|";
                     }
                     //alert("visible attribute for " + f.elements[i].name + "is " + f.elements[i].getAttribute("visible"));
                     /* if ((f.elements[i].getAttribute("dmpath") == null ||  
                          f.elements[i].getAttribute("depends") != null) 
                          &&(f.elements[i].getAttribute("visible") =='true')){
                        postStr = postStr + nameValStr;
                     } */
                     if((f.elements[i].getAttribute("dmpath") != null) 
                         && (f.elements[i].getAttribute("parentkeys") != null)
                         && (f.elements[i].getAttribute("visible") =='true'))
                     {
                        var parentKeys =  f.elements[i].getAttribute("parentkeys");
                       //alert("parentKeys " + parentKeys);
                        var dmpath = f.elements[i].getAttribute("dmpath");
                        var eleOptCode = f.elements[i].getAttribute("opcode");
                        
                        if(parentKeys != null) 
                        {
                          dmpath = procParentKeys(dmpath,parentKeys);
                        }
                        if(null == eleOptCode) {
                          eleOptCode = document.forms[0].opcode.value;
                        }
                        dmpath += "!" + eleOptCode;
                        tempPostStr = keyValArray[dmpath];
                        //alert("keyValArray[dmpath] " + keyValArray[dmpath]);
                        if(!tempPostStr) {
                             keyValArray[dmpath] = nameValStr;
                             dmArray[count] = dmpath;
                             count = count +1 ;
                            
                        } else {
                             tempPostStr = tempPostStr.concat(nameValStr);
                             keyValArray[dmpath]= tempPostStr;
                        }
                     }
                     else if(f.elements[i].getAttribute("visible") =='true'){
                        postStr = postStr + nameValStr;
                     }

                  }
                }
            } // end for

            if (document.forms[0].dmpath.value.indexOf(".vsg") != -1){
            if('' == getCookie(cookie) && confType != 'roleset') { 
               if(isInstReq == "true") {
                alert("Please select the VSG");
                return;
               }
            }
            }
            if(postStr.charAt(postStr.length - 1) == '|') {
               postStr = postStr.substring(0,postStr.lastIndexOf("|"));
            }

            var optVal = document.forms[0].opcode.value;
            for(k=0;k<dmArray.length;k++)
            {        
               var dmPath="";
               var temp = dmArray[k];

               if(temp.indexOf("{0}") > -1){
                 var tokens = temp.split("0");
                 if('' != cookie && '' != getCookie(cookie)) {
                    dmPath = tokens[0] + getCookie(cookie) + tokens[1];
                 }
               }
               if("" == dmPath){
                  dmPath = temp;
               }
               // dmpath is of type ex: 'igd.vsg'.
               if(vrtInstVar == 'yes' 
                     && dmPath.charAt(dmPath.length - 1) == '}' ) {
                 dmPath = temp.substring(0, temp.indexOf('{'));
               }

               //alert("finally dmpath= " + dmPath);

               nameValStr = keyValArray[dmArray[k]];
               if(nameValStr.charAt(nameValStr.length - 1) == '|') {
                  nameValStr = nameValStr.substring(0,nameValStr.lastIndexOf("|"));
               }
               // + "!" + optVal
               finalPostStr = finalPostStr + "ucdm_" + dmPath 
                              + "^" + nameValStr + "^"; 
            }

            f = document.forms[0];
            for(i = 0; i < f.elements.length; i++) {
              if(f.elements[i].name == "mtpath"){
                 metaFile = f.elements[i].value;
              }
              if(f.elements[i].name == "ucmreq") {
                 if(applyOpt != '' && applyOpt == "FRMTBL") {
                    postStr = postStr.replace(/#/g, '^');
                    postStr = postStr.replace(/=/g, '!');                        
                 }
                 //finalPostStr = postStr + "^" + finalPostStr; 
                 //f.elements[i].value = postStr;
              }
              if(f.elements[i].name == "dmpath") {
                 var dmPath="";
                 var temp = f.elements[i].value;

                 if(temp.indexOf("{0}") > -1){
                   var tokens = temp.split("0");
                   if('' != cookie && '' != getCookie(cookie)) {
                      dmPath = tokens[0] + getCookie(cookie) + tokens[1];
                   }
                 }
                 if("" == dmPath){
                      dmPath = temp;
                 }
                 // dmpath is of type ex: 'igd.vsg'.
                 if(vrtInstVar == 'yes' 
                      && dmPath.charAt(dmPath.length - 1) == '}' ) {
                 dmPath = temp.substring(0, temp.indexOf('{'));
                }

                 f.elements[i].value = dmPath; 
                 gdmPath = f.elements[i].value;
                // alert("finally dmpath= " + f.elements[i].value);
             }
             if(f.elements[i].name == "opcode") {
               if(window.opener != null) {
                 var opn = window.opener;
                 if(opn.opener != null) {
                   var roleopt = opn.opener.document.forms[0].roleopt.value;
                   var instpath = opn.opener.document.forms[0].instPath.value;
                   //alert("roleopt= " + roleopt + "  instpath= " + instpath);
                   if(roleopt != "none") {
                     f.elements[i].value = roleopt;
                     if(instpath != " none") 
                       f.dmpath.value = instpath;
                     //alert("after role dmpath= " + f.dmpath.value);
                   }
                 } // if
               } // if
               /*if(gdmPath.charAt(gdmPath.length - 1) == '}'
                    && confType == 'roleset'){
                 f.elements[i].value = "ISETROL";
               } */
               if(applyOpt != '' && applyOpt == "FRMTBL") {
                     f.elements[i].value = "";
               }
             }
          }
          var roleSettings = "false";
          if(f.mtpath.value =="AROLE_MetaData.xml"
                         || f.mtpath.value =="ROLE_MetaData.xml") {
            roleSettings = "true";
          }
          if(postStr != "")
          {
            if(roleSettings == "false") {
              finalPostStr = postStr + "^" + finalPostStr;
              if(applyOpt == "DONE") {
               var noRecs;
               var ptxrecs;
               if(opener) {
                 noRecs = opener.document.forms[0].noRecords.value;
                 ptxrecs = opener.document.forms[0].ptxRecords.value;
               }
               if (noRecs == "false") {
                 gdmPath += "{0}";
                 //alert("keys= " + keys);
                 gdmPath = procParentKeys(gdmPath,keys);
               }
              }
              finalPostStr = "ucdm_" + gdmPath +"!"+optVal+ "^" + finalPostStr;
              if(finalPostStr.charAt(finalPostStr.length - 1) == '^') {
                finalPostStr = finalPostStr.substring(0,finalPostStr.lastIndexOf("^"));
              }
            
            } else {
              finalPostStr = postStr;
            }
          }
          if (multiCmdStr != "") {
              finalPostStr += multiCmdStr;
              f.opcode.value = "MULTCMD";
          }
          document.getElementsByName("ucmreq")[0].value = finalPostStr;
          if(dmArray.length >= 1)
          {
            f.opcode.value = "MULTCMD";
            //alert("f.opcode.value " + f.opcode.value);
          }
          //alert("finally ucmreq:  " + document.getElementsByName("ucmreq")[0].value);
          
          if(applyOpt != '' && applyOpt == "FRMTBL") {
            setCookie("AuditFilterStr", postStr, 1);
            if(postStr == '' && keys != '') {
                postStr = keys;
            }
            //alert(postStr);
            f.host.value = getCookie("hostInfo");
            sumbitFrmReq(f.dmpath.value, '', f.mtpath.value, 
            f.sredirect.value, f.fredirect.value, postStr, "GET", '', keys);
            return;
          }
          //alert(parentTxStr);
          if(finalPostStr != '') {
            if(parentTxStr != '') {
              //alert("parentTxStr= " + parentTxStr);
              finalPostStr = parentTxStr + "^" + finalPostStr;
              f.opcode.value = "MULTCMD";
              document.getElementsByName("ucmreq")[0].value = finalPostStr;
            }
            var ptxrecs = "false";
            if(opener) {
              ptxrecs = opener.document.forms[0].ptxRecords.value;
            }
            //alert("frmgmeta:: ptxrecs= " + ptxrecs);
            //alert("finalPostStr= " + finalPostStr);
            var f1 = document.forms[1];
            //alert("current iframe name= " + this.name);
            //alert("f1.action= " + f1.action);
            if (document.forms[0].mtpath.value == "autosync_MetaData.xml") {
               if (finalPostStr.indexOf("ucdm_igd.clstrgrp.autosync!EDIT^status#STR=enable") != -1) {
                   //alert("mesg");
                   var cfm = confirm("Auto Sync enable reboots the iTCM card '" + getHostID() + "'\nDo you wish to proceed?");
                   if (false == cfm) {                        
                         return;
                   }
               }
            }
            
            if(f1.action != "" && f1.action.indexOf("ucm") == -1 ) {
              //alert("f1.enctype= " + f1.enctype);
              // f1.host.value = getCookie("hostInfo");
              if(applyOpt != '' && (applyOpt == "DONE"
                                    || applyOpt == "COMMIT"
                                    || applyOpt == "CANCEL"
                                    || applyOpt == "REVOKE")) { 
                                    //&& (ptxrecs == "true")) {
               var noRecs;
               var ptxrecs;
               if(opener) {
                 noRecs = opener.document.forms[0].noRecords.value;
                 ptxrecs = opener.document.forms[0].ptxRecords.value;
               }
               
                if(applyOpt == "DONE" && noRecs == "false") {
                  var tdmpt = f1.dmpath.value;
                  //alert("tdmpt= " + tdmpt + "  keys="+keys);
                 if(tdmpt.charAt(tdmpt.length - 1) != '}') {
                   f1.dmpath.value = procDmPath(tdmpt, keys);
                 }
                }
                f1.reqtype.value = applyOpt;
                //alert("f1.reqtype.value= " + f1.reqtype.value);
              }
              f1.submit();
            } else {
              if(applyOpt != '' && (applyOpt == "DONE"
                                    || applyOpt == "COMMIT"
                                    || applyOpt == "CANCEL"
                                    || applyOpt == "REVOKE")) {
                                    //&& (ptxrecs == "true")) {
                f.reqtype.value = applyOpt;
                   var noRecs;
                   var ptxrecs;
                   if(opener) {
                     noRecs = opener.document.forms[0].noRecords.value;
                     ptxrecs = opener.document.forms[0].ptxRecords.value;
                   }
                if(applyOpt == "DONE" && noRecs == "false") {
                  var tdmpt = f.dmpath.value;
                  //alert("tdmpt= " + tdmpt + "  keys="+keys);
                 if(tdmpt.charAt(tdmpt.length - 1) != '}') {
                   f.dmpath.value = procDmPath(tdmpt, keys);
                 }
                }
                //alert("f.reqtype.value= " + f.reqtype.value);
              }
              f.host.value =  getHostID();
              submitFrmData(f);
            }
          } else {
            back();
          }
       } // sendReq

       function procDmPath(dmpath, keys) {
         dmpath += "{0}";
         dmpath = procParentKeys(dmpath, keys);
         return dmpath;
       }
       function procParentKeys(dmpath,parentKeys) {
           var pKeys;
           var index;
           var keys ;
           var myPath ="";
           //alert("procParentKeys:: parentKeys= " + parentKeys);
           var tkeys = parentKeys.replace(/\+/g, '#');
           parentKeys = tkeys;
           if(parentKeys.charAt(parentKeys.length - 1) == ',') {
               parentKeys = parentKeys.substring(0,parentKeys.lastIndexOf(","));
           }
           f = document.forms[1];
           //alert("doc dm: " + document.forms[0].dmpath.value + " dm: "+ dmpath + " prev dmArray[count]: " + dmArray[count-1]);
           pKeys =  parentKeys.split('|');
           pathTokens = dmpath.split('{0}');
           for(x=0;x<pathTokens.length;x++)
           {
              //alert("pathTokens " + x + " " + pathTokens[x]);
              if(pKeys[x] != null && pKeys[x] != '')
              {
                temp = pKeys[x];
                if(temp.charAt(temp.length - 1) == ',') {
                   temp = temp.substring(0,temp.lastIndexOf(","));
                }

                keys = temp.split(',');
                if(keys.length == 1 && (document.getElementsByName(keys[0]).length > 0))
                {//alert("a");
                  var keyValStr = document.getElementsByName(keys[0])[0].value;
                  if (keyValStr == '') keyValStr = 0;
                  myPath = myPath + pathTokens[x] + '{' + keyValStr + '}';
                }
                else if( keys.length == 1 && (document.getElementsByName(keys[0]).length == 0))
                {//alert("b");
                   //search in gKeyArray, which uses global dmpath
                   if (gKeyArray[x]) {
                       myPath = myPath + pathTokens[x] + '{' + gKeyArray[x] + '}';
                   } else {
                           myPath = myPath + pathTokens[x] + '{0}';
                   }
                }
                else if(keys.length > 1)
                {//alert("c");
                   var val;
                   var keyName ;
                   var keyValStr = "";

                  for(var y=0;y<keys.length;y++)
                  {
                      //alert("keys[y]" + keys[y]);
                      if((keys[y] != '') &&  (document.getElementsByName(keys[y]).length > 0))
                      {
                        val =  document.getElementsByName(keys[y])[0].value;
                        if (val == "") val = "0";
                        //keyName = keys[y].split('#')[0];
                        keyName = keys[y].substring(0, keys[y].indexOf('#'));
                        keyValStr = keyValStr + keyName + '=' + val + ',';
                        //alert("***keyValStr= " + keyValStr);
                      } else {
                        if(f.useparenttx) {
                         var parentTx = f.useparenttx.value;
                         if(null != parentTx) {
                           if(parentTx.indexOf(keys[y]) != -1) {
                              //alert("keys[y] " + keys[y]);
                              var tlen = keys[y].length;
                              var tstr =  parentTx.substring(parentTx.indexOf(keys[y]) + tlen +1, parentTx.length);
                              if(tstr.indexOf("|") != -1)
                              {
                                 tstr = tstr.substring(0, tstr.indexOf("|"));
                              }
                              keyName = keys[y].split('#')[0];
                              keyValStr = keyValStr + keyName + '=' + tstr + ',';
                              //alert("keyValStr= " + keyValStr);
                           }
                         }
                        }
                      } // else
                  } // for
                  if(keyValStr.charAt(keyValStr.length - 1) == ',') {
                      keyValStr = keyValStr.substring(0,keyValStr.lastIndexOf(","));
                  }
                  if(keyValStr != '')
                  {
                    myPath = myPath + pathTokens[x] + '{' +  keyValStr + '}';
                  }
                } 
              }
              else
              {
                myPath = myPath + pathTokens[x];
              }
           }
           //alert("myPath  " + myPath);
           return myPath;
       } // procParentKeys

       function loadParentKeys() {
             if (document.forms[0].dmpath.value != "") {
                 var gDmPath = document.forms[0].dmpath.value;
                 if (gDmPath.indexOf("{") != -1) {
                    var iPaths = gDmPath.split(".");
                    for (var ipInd=0; ipInd < iPaths.length; ipInd++) {
                        if (iPaths[ipInd].indexOf("{") != -1) {
                           var ipathStr=iPaths[ipInd];
                           gKeyArray[kCount] = 
                                    ipathStr.substring(ipathStr.indexOf("{")+1,
                                                       ipathStr.indexOf("}"));
                           kCount++;
                        } // if
                    } // for
                 } // if
             } // if 
       }      

       function greyDisabled() {//alert("here");
         f = document.forms[1];
         for (i = 0; i < f.elements.length; i++) {
           if (f.elements[i].disabled) {        
             f.elements[i].style.backgroundColor = "#CCCCCC";              
           }
         }
         //alert(this.window.name);
         var parntFrm =  parent.document.getElementById("main-form");
         //alert("parntFrm.tblID.value" +parntFrm.tblID.value);
         if (this.window.name.indexOf("newW") != -1 || null != parntFrm) {
           document.getElementById("back").value = "Close";
           if (document.forms[0].dmpath.value == "igd.logmesg") {
             document.getElementById("ApplyBtn").style.display = "none";
           }
         }
       } // greyDisabled
        ]]>  
        </script>
       </head>
      <body onunload="removeGreyDiv();">
       <xsl:variable name="mtDataFil" select="document(/root/MetaData)/root" /> 
       <xsl:variable name="cookieName">
         <xsl:choose>
           <xsl:when test="not(document(/root/MetaData)/root/depends)">
             <xsl:value-of select="document(/root/MetaData)/root/vrtlInst/@name" />
           </xsl:when>
           <xsl:otherwise>
             <xsl:variable name="mtPath">
               <xsl:value-of select="document(/root/MetaData)/root/depends/@mtpath" />
             </xsl:variable>
             <xsl:value-of select="document($mtPath)/root/vrtlInst/@name" />
           </xsl:otherwise>
         </xsl:choose>
       </xsl:variable>
       <!-- Onload function calls -->
       <xsl:attribute name="onload">
         <xsl:text>setProductNameAsTitle(this.document);greyDisabled();loadParentKeys();</xsl:text>
         <xsl:text>dynamiclist('</xsl:text>
           <xsl:value-of select="$cookieName" />
         <xsl:text>');updateUIContent('</xsl:text>
           <xsl:value-of select="$cookieName" />
         <xsl:text>');</xsl:text>
       </xsl:attribute>
       <p id="test" style="display:none;" ></p>
       
      <script language="JavaScript">
           <xsl:text>init('</xsl:text>
           <xsl:choose>
             <xsl:when test="not(/root/Config/*)">
                   <xsl:value-of select="/root/@status" />
             </xsl:when>
             <xsl:otherwise>
               <xsl:text>ok</xsl:text>
             </xsl:otherwise>
           </xsl:choose>
           <xsl:text>','</xsl:text>
             <xsl:value-of select="/root/Msg" />
           <xsl:text>','</xsl:text>
             <xsl:value-of select="$cookieName"/>
           <xsl:text>','</xsl:text>
           <xsl:choose>
             <xsl:when test="$mtDataFil/opcode='ROLESET'" >
                <xsl:text>roleset</xsl:text>
             </xsl:when>
             <xsl:otherwise>
                <xsl:text>config</xsl:text>
             </xsl:otherwise>
           </xsl:choose>  
           <xsl:text>');</xsl:text>
         </script>
         <!--
           Get Metadata file name from the response XML 
         -->
         <xsl:variable name="tmpVar">
           <xsl:value-of select="/root/MetaData"/>
         </xsl:variable>
         <table class="pagetitle" border="0" cellspacing="0" cellpadding="0" width="99%" bordercolor="#cccccc" bordercolorlight="#cccccc" bordercolordark="#ffffff" bgcolor="#ffffff">         
         <tr class="toolbar">
            <td width="85%" height="20">            
               <!--img class="helpImg" src="images/blueball.gif"/-->&#160;
               <span id="appln" class="titlefont">
                 <script language="JavaScript">
                    applName('<xsl:value-of select="document($tmpVar)/root/breadcrumb"/>');
                 </script>
               </span><br/>&#160;
               <span id="ovsg" class="breadcrumb">
                 <script language="JavaScript">
                    breadcrumb('<xsl:value-of select="document($tmpVar)/root/dmpath" />', 
                               '<xsl:value-of select="$cookieName"/>',
                               '<xsl:value-of select="/root/dmpath" />');
                    <!--breadcrumb('<xsl:value-of select="document($tmpVar)/root/dmpath" />', 
                               '<xsl:value-of select="$cookieName"/>',
                               '<xsl:value-of select="document($tmpVar)/root/dmpath" />');-->
                 </script>
               </span>
            </td>
             <td width="10%">
          <span class="breadcrumb" style="float:right; clear:right;">
                 &#160;&#160;<a href="#">
                   <xsl:attribute name="onclick">
                      <xsl:text>showHelp('</xsl:text>
                      <xsl:value-of select="document($tmpVar)/root/help/@src"/>
                      <xsl:text>');</xsl:text>
                   </xsl:attribute>                   
                    <xsl:text>Help</xsl:text>                   
                 </a>&#160;<font color="#c4cfd1">|</font>&#160;<a href="#" onclick="back();">Close</a></span>
                
    <!-- Save and Factory reset button will not appear in Add and 
       Edit page -->
                <!-- <a>
                   <xsl:attribute name="onclick">
                      <xsl:text>save('</xsl:text>
                      <xsl:value-of select="$cookieName"/>
                      <xsl:text>');</xsl:text>
                   </xsl:attribute>
                   <span class="breadcrumb" style="float:right; clear:right;">
                <xsl:text>Save</xsl:text>
                  </span>  
                 </a>--> 
            </td>
            <td width="5%"> 
                <!--  <a>
                   <xsl:attribute name="onclick">
                      <xsl:text>factoryReset('</xsl:text>
                      <xsl:value-of select="$cookieName"/>
                      <xsl:text>');</xsl:text>
                   </xsl:attribute>
                   <span class="breadcrumb" style="float:right; clear:right;">
                    <xsl:text>&#160;|&#160;Factory Reset</xsl:text>
                   </span>
                 </a> -->                  
            </td>
          </tr>       
         </table>

         <table border="0" cellspacing="0" cellpadding="0" width="100%" height="1">
         <tr>
             <td>
               <img src="images/spacer.gif" align="absMiddle" height="1" width="1"/>
             </td>
         </tr>
         <tr>
           <td>         
           <form name="main-form" action="ucmreq.igw"  method="post">
           
             <xsl:variable name="mtData" select="document($tmpVar)/root"/>
             <xsl:variable name="dmPath" select="$mtData/dmpath"/>             
             <input type="hidden" name="mtpath">
               <xsl:attribute name="value">
                 <xsl:value-of select="$mtData/form/btn[@type='submit']/@mtpath" />
               </xsl:attribute>
             </input>
             <input type="hidden" name="dmpath" >
                   <xsl:attribute name="value">
                     <xsl:choose>
                       <!-- 
                          * If metadata file does not contain dmpath
                          * pick the dmpath from related XML form page
                          * received from the server.
                          * this happens only in Roles and Permission
                          * page context.
                        -->
                       <xsl:when test="/root/dmpath=''">
                         <xsl:value-of select="$mtData/dmpath"/>
                       </xsl:when>
                       <xsl:when test="/root/dmpath=''">
                         <xsl:value-of select="$mtData/dmpath"/>
                       </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="/root/dmpath"/>
                      </xsl:otherwise>
                     </xsl:choose>
                   </xsl:attribute>
             </input>

             <xsl:choose>
               <xsl:when test="$mtData/opcode='ROLESET'">
                 <input type="hidden" name="opcode" >
                   <xsl:attribute name="value">
                     <xsl:text>SETROL</xsl:text>
                   </xsl:attribute>
                 </input>
               </xsl:when>
               <xsl:when test="$mtData/form[@frmType='grpscalar']">
                 <input type="hidden" name="opcode" >
                   <xsl:attribute name="value">
                     <xsl:text>EDIT</xsl:text>
                   </xsl:attribute>
                 </input>
               </xsl:when>
               <xsl:when test="not(/root/Config/*)">
                 <input type="hidden" name="opcode" >
                   <xsl:attribute name="value">
                     <xsl:text>ADD</xsl:text>
                   </xsl:attribute>
                 </input>
               </xsl:when>
               <xsl:otherwise>
                 <input type="hidden" name="opcode" >
                   <xsl:attribute name="value">
                     <xsl:text>EDIT</xsl:text>
                   </xsl:attribute>
                 </input>
               </xsl:otherwise>
             </xsl:choose>

             <input type="hidden" name="sredirect">
               <xsl:attribute name="value">
                 <xsl:choose>
                   <xsl:when test="$mtData/form/btn[@type='submit']/@spg">
                     <xsl:value-of select="$mtData/form/btn[@type='submit']/@spg" />
                   </xsl:when>
                   <xsl:otherwise>
                     <xsl:text>""</xsl:text>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:attribute>
             </input>
             <input type="hidden" name="fredirect">
               <xsl:attribute name="value">
                 <xsl:choose>
                   <xsl:when test="$mtData/form/btn[@type='submit']/@fpg">
                     <xsl:value-of select="$mtData/form/btn[@type='submit']/@fpg" />
                   </xsl:when>
                   <xsl:otherwise>
                     <xsl:text>""</xsl:text>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:attribute>
             </input>
             <input type="hidden" name="ucmreq" value="" />
             <input type="hidden" name="keyprms" />
             <input type="hidden" name="reqtype" />
             <input type="hidden" name="host" />
           </form>           
           <!--form name="myform" action="ucm"  method="post"-->
           <form id="myform" name="myform" method="post">
             <xsl:variable name="mtdat" select="document($tmpVar)/root"/> 
             <xsl:if test="$mtdat/form//field[@type='file']">
               <xsl:attribute name="enctype">
                 <xsl:text>multipart/form-data</xsl:text>
               </xsl:attribute>
               <xsl:choose>
                 <xsl:when test="$mtdat/form/btn[@action!='']">
                   <xsl:attribute name="action">
                     <xsl:value-of select="$mtdat/form/btn/@action"/>
                   </xsl:attribute>
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:attribute name="action">
                     <xsl:text>ucm</xsl:text>
                   </xsl:attribute>
                 </xsl:otherwise>
               </xsl:choose>
             </xsl:if>
             <xsl:call-template name="gen_Form">
               <xsl:with-param name="parvar" select="$tmpVar" />
               <xsl:with-param name="cookieName" select="$cookieName" />
             </xsl:call-template>
           </form>           
           </td>
         </tr>
         <xsl:variable name="metaData" select="document($tmpVar)/root"/>
         <xsl:if test="$metaData/form[@note != '']">
           <tr>
             <td>
               <img src="images/spacer.gif" align="absMiddle" height="1" width="4"/><i><xsl:value-of select="$metaData/form/@note"/></i>
             </td>
           </tr>                        
         </xsl:if>
         <tr>
             <td>
               <img src="images/spacer.gif" align="absMiddle" height="1" width="1"/>
             </td>
         </tr>
         <tr>
            <td>
               <xsl:variable name="mtDat" select="document($tmpVar)/root"/>
                <xsl:variable name="vrtInstVar">
                  <xsl:choose>
                    <xsl:when test="$mtDat/vrtlInst">
                    <xsl:text>yes</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                    <xsl:text>no</xsl:text>
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:variable>
                <xsl:variable name="applyOpt">
                  <xsl:choose>
                    <xsl:when test="$mtDat/form/btn[@type='submit']/@action">
                      <xsl:value-of select="$mtDat/form/btn[@type='submit']/@action" />
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:text></xsl:text>
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:variable>
                <!--<xsl:variable name="frmType">
                  <xsl:choose>
                    <xsl:when test="not(/root/Config/*)">
                      <xsl:text>ADD</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                      <xsl:text>EDIT</xsl:text>
                    </xsl:otherwise>
                  </xsl:choose>
                </xsl:variable>-->
                <input type="submit" id="ApplyBtn" name="Submit" value="Apply">
                      <xsl:attribute name="applyOpt">
                        <xsl:value-of select="$applyOpt" />
                      </xsl:attribute>
                      <xsl:attribute name="onclick">
                        <xsl:text>sendReq('</xsl:text> 
                        <xsl:value-of select="$cookieName"/>
                        <xsl:text>','</xsl:text>
                        <xsl:value-of select="$vrtInstVar"/>
                        <xsl:text>','</xsl:text>
                        <xsl:value-of select="$applyOpt" />
                        <xsl:text>','</xsl:text>
                        <xsl:choose> 
                          <xsl:when test="$applyOpt = 'DONE'"> 
                            <xsl:for-each select="$mtDat/obj[@key='yes']">
                              <xsl:variable name="keyName" select="@name"/>
                              <xsl:value-of select="$keyName"/>
                              <xsl:text>+</xsl:text>
                              <xsl:value-of select="@type"/>
                              <xsl:text>$$</xsl:text>
                              <xsl:value-of select="@oid"/>
                              <xsl:text>,</xsl:text>
                            </xsl:for-each>
                          </xsl:when>
                          <xsl:otherwise>
                            <xsl:for-each select="$mtDat/obj[@key='yes']">
                              <xsl:variable name="keyName" select="@name"/>
                              <xsl:value-of select="$keyName"/>
                              <xsl:text>^</xsl:text>
                              <xsl:value-of select="@type"/>
                              <xsl:text>!nil</xsl:text>
                              <xsl:text>|</xsl:text>
                            </xsl:for-each>
                          </xsl:otherwise>
                        </xsl:choose> 
                        <xsl:text>')</xsl:text>
                      </xsl:attribute>
                </input>&#160;  
            <input type="button" name="back" onclick="back();" value="Back" id="back"/>            
            </td>
          </tr>   
         </table>
         <xsl:call-template name="tbl-frm" />
      <span id="screenHeightDiv" style="position: relative;">&#160;<img src="images/spacer.gif" align="absMiddle" height="1" width="1"/></span>
       </body>
      </html>
    </xsl:template>
    
    <xsl:template name="gen_Form">
      <xsl:param name="parvar" select="'Not Available'" />
      <xsl:param name="cookieName" select="'Not Available'" />
      <!-- <xsl:value-of select="$parvar" /> -->
      <xsl:variable name="srcData" select="/root"/>
      <xsl:variable name="metaData" select="document($parvar)/root"/>
      <xsl:if test="$metaData/useparenttx/@mtpath">
        <xsl:variable name="pMeta" select="document($metaData/useparenttx/@mtpath)/root"/>
        <input id="useparenttx" type="hidden" name="useparenttx">
          <!--xsl:attribute name="dynamic">
            <xsl:text>true</xsl:text>
          </xsl:attribute-->
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
      </xsl:if>
      <xsl:choose>
        <xsl:when test="not(/root/Config/*)">
          <xsl:variable name="frTyp">ADD</xsl:variable>
          <xsl:call-template name="popl_frm">
             <xsl:with-param name="sData" select="$srcData" />
             <xsl:with-param name="mData" select="$metaData" />
             <xsl:with-param name="frmType" select="$frTyp" />
             <xsl:with-param name="cookieName" select="$cookieName" />
             <xsl:with-param name="isVisible" select="'true'" />
             <xsl:with-param name="isExtFrmRef" select="'false'" />
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:variable name="frTyp">EDIT</xsl:variable>
          <xsl:call-template name="popl_frm">
             <xsl:with-param name="sData" select="$srcData" />
             <xsl:with-param name="mData" select="$metaData" />
             <xsl:with-param name="frmType" select="$frTyp" />
             <xsl:with-param name="cookieName" select="$cookieName" />
             <xsl:with-param name="isVisible" select="'true'" />
             <xsl:with-param name="isExtFrmRef" select="'false'" />
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
