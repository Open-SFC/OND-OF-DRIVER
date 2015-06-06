var logoutFlag="no";
var ucmReqObj;
var delRecord="";
// Detect if the browser is IE or not.
// If it is not IE, we assume that the browser is NS.
/*var IE = document.all?true:false
var cX = 0; var cY = 0; var rX = 0; var rY = 0;
// If NS -- that is, !IE -- then set up for mouse capture
if (!IE) document.captureEvents(Event.MOUSEMOVE)
// Set-up to use getMouseXY function onMouseMove
document.onmousemove = UpdateCursorPositionDocAll;
// Main function to retrieve mouse x-y pos.s
function UpdateCursorPosition(e){ cX = e.pageX; cY = e.pageY;}
function UpdateCursorPositionDocAll(e){ cX = event.clientX; cY = event.clientY;}
if(IE) { document.onmousemove = UpdateCursorPositionDocAll; }
else { document.onmousemove = UpdateCursorPosition; }*/
function AssignLeftPosition(doc) {
  if(self.pageYOffset) {
    rX = self.pageXOffset;
    rY = self.pageYOffset;
    //alert("self.pageXOffset= " +rX + "\t self.pageYOffset= " + rY);
  } else if(doc.documentElement && doc.documentElement.scrollTop) {
    rX = doc.documentElement.scrollLeft;
    rY = doc.documentElement.scrollTop;
    //alert("doc.documentElement.scrollLeft= " +rX + "\t doc.documentElement.scrollTop= " + rY);
  } else if(doc.body) {
    rX = doc.body.scrollLeft;
    rY = doc.body.scrollTop;
    //alert("doc.body.scrollLeft= " +rX + "\t doc.body.scrollTop= " + rY);
  }
  if(doc.all) {
    cX += rX; 
    cY += rY;
  }
  doc.getElementById('MouseX').innerHTML = cX+50;
  doc.getElementById('MouseY').innerHTML = cY+100; 
   
}
/*
function AssignRightPosition() {
if(self.pageYOffset) {
        rX = self.pageXOffset;
        rY = self.pageYOffset;
        }
else if(document.documentElement && document.documentElement.scrollTop) {
        rX = document.documentElement.scrollLeft;
        rY = document.documentElement.scrollTop;
        }
else if(document.body) {
        rX = document.body.scrollLeft;
        rY = document.body.scrollTop;
        }
if(document.all) {
        cX += rX; 
        cY += rY;
        }
  document.getElementById('MouseX').innerHTML = cX-700;
  document.getElementById('MouseY').innerHTML = cY+50;
  return true
}
*/
// Hash map to store properties.
var infoMap = {
  Set : function(key,val) {this[key] = val;},
  Get : function(key) {return this[key];}
}

// This function can be used to debug XSL 
// script by embedding the function within the script tag.
function echoMsg(str) {
  alert("echoMsg::>" + str );
}

// Hash map to store already loaded XML Document objects
var loadMap = {
  Set : function(key,val) {this[key] = val;},
  Get : function(key) {return this[key];}
}

// Function return loaded XML Document objects from the list or create new.
function loadXMLDocument(oFile) {
  var oXmlDoc;
  oXmlDoc = loadMap.Get(oFile);
  if(!oXmlDoc) {
    var oXmlDoc = loadXMLDoc(oFile);
    loadMap.Set(oFile, oXmlDoc);
  }
  return oXmlDoc;
}

// Function to limit Text Area Length
function txtAreaLimit(txtFld, maxlen) {
    if(txtFld.value.length > maxlen) {
        txtFld.value = txtFld.value.substring(0,maxlen);
    }
} //txtAreaLimit

function showHelp(url) {
  //alert("showHelp: docUrl = " + docUrl);
  if(url) {
    var docUrl = "/doc/" + url;
    //window.open (docUrl ,"helpwindow"); 
	var winWidth = 700, winHeight = 550;
	var winOpen = window.open(docUrl, "helpwindow", "toolbar=0, resize=1, scrollbars=1, status=0, location=0, menubar=0");
     winOpen.resizeTo(winWidth, winHeight);
     winOpen.moveTo(screen.width/2-winWidth/2, screen.height/2-winHeight/2);
  }
}

// Function to set host infromation
function setHostInfo(hostVal) {
  setCookie("hostInfo", hostVal, 1);
}
function delRec(recID) {
  //alert("delRec: recID= "+recID);
}
function getOpCode(tOpcode) {
  var opCode = "";
  switch(tOpcode) {
     case "ADD":
       opCode =1;
       break; 
     case "BYKEY":
       opCode = 2;
       break; 
     case "MORE":
       opCode = 3;
       break; 
     case "GETROLE":
       opCode = 4;
       break; 
     case "IGETROLE":
       opCode = 5;
       break; 
     case "RBYKEY":
       opCode = 6;
       break;
     case "IRBYKEY":
       opCode = 7;
       break;
     case "AGGR":
       opCode = 8;
       break;
     case "AVG":
       opCode = 9;
       break;
     case "PERDEV":
       opCode = 10;
       break;
  } // switch 
  return opCode;
}
var vrtlInstPath="";
var vrtlInstName="";
function trim (s){ return rtrim(ltrim(s));}
function setHostID(vHost) {
  if(vHost == "" || trim(vHost).length == 0) {
    setHostInfo(defaultHost);
  } else {
    setHostInfo(vHost);
  }
}
var hostNm = "";

function updateHostInfo() {
  var pdoc = parent.parent.frameTop.toolbar.document
  if(pdoc != null) {
    updateToolBar(pdoc);
  }
}
function updateToolBar(pDoc) {
   var loginName = getCookie("Name-Login"); 
   var msg = "<font color=\"orange\">";
   msg +="User:</font>&#160;" + loginName + "&#160;";
   hostNm = getHostID();
   //alert("updateToolBar::hostNm= " + hostNm);
   if(hostNm == "localhost") {
      hostNm = "Management Card";
      pDoc.getElementById("toolBar").innerHTML = pDoc.getElementById("mgmtDiv").innerHTML;
   } else if(hostNm == "MASTER") {
      hostNm = "Master Card";
      pDoc.getElementById("toolBar").innerHTML = pDoc.getElementById("masterDiv").innerHTML;
   }
   if( defaultHostType != "SingleDevice") {
   msg +="<font color=\"#c4cfd1\">|</font>&#160;<font color=\"orange\">Configure/View:</font>&#160;" + hostNm + "&#160;&#160;";
   }   
   pDoc.getElementById("loginDiv").innerHTML = msg;
}
function setProductName(pDoc) {
   pDoc.getElementById("productNameDiv").innerHTML="<font face=\"Verdana, Arial, Helvetica, sans-serif\" size=\"3\" color=\"#fff\"><i>" + productName + "&#160;</i></font>";
   /*var parTitle = pDoc.parent.parent.getElementsByTagName("title");
   parTitle[0].innerHTML = productName;*/
}
function setProductNameInBlackFont(pDoc) {
   pDoc.getElementById("productNameDiv").innerHTML="<font face=\"Verdana, Arial, Helvetica, sans-serif\" size=\"3\" color=\"#000\"><i>" + productName + "&#160;</i></font>";
}
function setProductNameAsTitle(pDoc) {   
   var titles = pDoc.getElementsByTagName("title");
   titles[0].innerHTML = productName;
}
function getHostID() {
  var hostID="";
  if((getCookie("hostInfo") != "localhost") 
             && (getSelectedHost() != "")) {
    hostID = getSelectedHost();
  } else if(getCookie("hostInfo") != "") {
    hostID = getCookie("hostInfo");
  } else {
    hostID = defaultHost;
  }
  //alert("getHostID()=" + hostID);
  return hostID;
}
function setVertualInstanceName(vName) {
  //alert("setVertualInstanceName= " + vName);
  vrtlInstName = vName;
}
function getVertualInstanceName() {
  return vrtlInstName;
}

function setVertualInstancePath(vPath) {
  //alert("setVertualInstancePath= " + vPath);
  vrtlInstPath = vPath;
}
function getVertualInstancePath() {
  return vrtlInstPath;
}

// This function is called by the menu.xsl, to populate the GET URL.
function showScreen(objRef) {
  //alert("Inside showScreen..." + objRef.getAttribute("urlRef"));
  var urlStr = objRef.getAttribute("urlRef");
  var mySplitResult = urlStr.split("&");
  var fqn="";
  var mtpath="";
  var spg="";
  var keys="";
  var cookie= "";
  var opcode= "";
  var vrtInstPath="";
  var vrInstName="";
  var host="";

  for(i=0; i<mySplitResult.length; i++) {
    //alert(mySplitResult[i]);
    var splRes = mySplitResult[i].split("=");
    switch(splRes[0]) {
      case "fqn":
        fqn = splRes[1];
        break; 
      case "mtpath":
        mtpath = splRes[1];
        break; 
      case "spg":
        spg = splRes[1];
        break; 
      case "keys":
        keys = splRes[1];
        break;
      case "cookie":
        cookie = splRes[1];
        break;
      case "opcode":
        opcode = splRes[1];
        break;
      case "vrtInstPath":
        vrtInstPath = splRes[1];
        setVertualInstancePath(vrtInstPath);
        break;
      case "vrtInstName":
        vrInstName = splRes[1];
        setVertualInstanceName(vrInstName);
        break;
      case "host":
        host = splRes[1];
        //alert("host= " + host);
        if(host == "")
         host = defaultHost;
        break;
      } // switch 
  } // for
  if(mtpath != 'AROLE_MetaData.xml' && mtpath != 'ROLE_MetaData.xml' ) {
    setHostID(host);
    var pdoc = parent.parent.frameTop.toolbar.document
    if(pdoc != null) {
      updateToolBar(pdoc);
    }
    setCookieValue(fqn, opcode, mtpath, spg, keys, cookie,'', '');
  }
}
//Function to escape XML string
function xmlEscape(s) {
    var result = s.replace(/&amp;/g, "&amp;")
                  .replace(/</g, "&lt;")
                  .replace(/>/g, "&gt;")
                  .replace(/"/g, "&quot;")
                  .replace(/'/g, "'");
    //alert("xmlEscape:: s= " + s + "  result= " + result);
    return result;
}
/**
 *  XPath API common for IE, Mozilla, Firefox, Opera, etc.
 */
/****************** XPath API Scripts START ***************/
// Function to return String value for a given XPath expression.
function selectNodeStrVal(exprStr, oXmlObj) {
  var strVal = "";
  if (window.ActiveXObject) {
     // code for IE
    strVal = getIEStrVal(oXmlObj.selectNodes(exprStr));

  } else if (document.implementation 
     && document.implementation.createDocument) {
    // code for Mozilla, Firefox, Opera, etc.
    var tmpData = oXmlObj.evaluate(exprStr, oXmlObj, null,
                                   XPathResult.STRING_TYPE, null);
    strVal = tmpData.stringValue;
  }
  strVal = xmlEscape(strVal);
  return strVal;
}
// Function to return Node array for a given XPath expression.
function selectNodeArry(exprStr, oXmlObjt) {
  var resArry; 
  if (window.ActiveXObject) {
     // code for IE
    resArry = oXmlObjt.selectNodes(exprStr);
  } else if (document.implementation 
     && document.implementation.createDocument) {
    // code for Mozilla, Firefox, Opera, etc.
    resArry = new Array();
    var evalRes = oXmlObjt.evaluate(exprStr, oXmlObjt, null,
                    XPathResult.ORDERED_NODE_ITERATOR_TYPE, null);
    if(evalRes) {
      var oElem = evalRes.iterateNext(); 
      while (oElem) {
        resArry.push(oElem);
        oElem = evalRes.iterateNext(); 
      }
    }
  }
  return resArry;
}
/****************** XPath API Scripts END   ***************/
/**
 * Scripts to populate references
 */
/****************** References Scripts START ***************/
// To create XML HTTP object
function getObject(handler,s) {
  state=s;
  //alert("getObject:: state= " + state);
  var objXmlHttp=null;
  if (navigator.userAgent.indexOf("MSIE")>=0) {
    var str;
    if (navigator.appVersion.indexOf("MSIE 5.5")>=0)
      str="Microsoft.XMLHTTP";
    else
      str="Msxml2.XMLHTTP";
    try {
        objXmlHttp=new ActiveXObject(str)
        objXmlHttp.onreadystatechange=handler;
        return objXmlHttp;
    } catch(e) {
      alert("Error. Scripting for ActiveX might be disabled")
      return
    }
  }
  if (navigator.userAgent.indexOf("Mozilla")>=0) {
    objXmlHttp=new XMLHttpRequest();
    objXmlHttp.onload=handler;
    objXmlHttp.onerror=handler;
    return objXmlHttp;
  }
}

//To populate div with dynamic list for the text box
function populate(ele) {
  var realElement = ele.replace('3','#');
  var array = realElement.split('|');
  var textFld = array[0];
  var textVal = array[1];
  for(i=0; i< document.forms[1].elements.length; i++) {
    if(document.forms[1].elements[i].name == textFld) {
      document.forms[1].elements[i].value = textVal;
    }
  }  
  return(true);
}

// Function to send GET/POST request to the server using AJAX
function sendRequestToServer(urlStr, method, params, respHandler) {
  /*
   * NOTE: For GET requests, POST method is prefered to overcome
   * GET URL length limitation on the server.
   */
  xmlobject=new getObject(respHandler,1);
  if(xmlobject==null) {
    alert("Browser does not support AJAX");
    return false;
  }
  xmlobject.open(method,urlStr,false);
  xmlobject.send(params);
}
function sendRequestToServerEx(urlStr, method, params, respHandler, tvar) {
  /*
   * NOTE: For GET requests, POST method is prefered to overcome
   * GET URL length limitation on the server.
   */
  xmlobject=new getObject(function(){respHandler(tvar)},1);
  if(xmlobject==null) {
    alert("Browser does not support AJAX");
    return false;
  }
  xmlobject.open(method,urlStr,false);
  xmlobject.send(params);
}

// To populate dynamic list for the drop down menu
function dynamiclist(cookieName) {
  //alert("Inside dynamiclist...");
  for(i=0; i< document.forms[1].elements.length; i++) {
    //alert("ele type= " + document.forms[1].elements[i].type);
    if((document.forms[1].elements[i].type == 'select-one') 
       || (document.forms[1].elements[i].type == 'select-multiple')) {
      if(document.forms[1].elements[i].getAttribute('depdmpath') != null
           && document.forms[1].elements[i].getAttribute('depends') != null) {
        if(document.forms[1].elements[i].getAttribute('id') 
                    != document.getElementById('test').innerHTML) {
          document.getElementById('test').innerHTML = 
                                        document.forms[1].elements[i].getAttribute('id');
          var dmpath = document.forms[1].elements[i].getAttribute('depdmpath');
          var depends = document.forms[1].elements[i].getAttribute('depends');          
          var keys = "";
          var keyInstanceStr = "";
          if (document.forms[1].elements[i].getAttribute('key') != null && document.forms[1].elements[i].getAttribute('key') != '') {
                var keyFieldName = document.forms[1].elements[i].getAttribute('key');
                //alert(keyFieldName);
                var keyValueSelObj = document.getElementById(keyFieldName);
                var keyFieldNameInSM = keyValueSelObj.getAttribute('depends');
                keyFieldNameInSM = keyFieldNameInSM.substring(0, keyFieldNameInSM.indexOf("+"));
                //alert(keyFieldNameInSM);
                //alert(keyValueSelObj.options[keyValueSelObj.selectedIndex].value);                
                keys = keyFieldNameInSM + "^STR!" + keyValueSelObj.options[keyValueSelObj.selectedIndex].value + "|";
                keyInstanceStr = "{" + keyValueSelObj.options[keyValueSelObj.selectedIndex].value + "}";
                //alert(keys);  
                if (dmpath.indexOf("{0}") != -1) {
                    //dmpath = procParentKeys(dmpath, keys);
                    dmpath = dmpath.replace("{0}", keyInstanceStr); 
                    //alert(dmpath);
                }             
          } else {
                keys = depends.replace('+','^')+"!nil|";
          }
          var params = setCookieInfo(dmpath,'', '', keys, cookieName, '');
          params += "&keyprms="+keys;
          params += "&reqtype=GET";
          params += "&host=" + getHostID();
          sendRequestToServer('ucmreq.igw', 'POST', params, showloadRec_stateChanged);
          //alert("sent request params: " + params);
          //alert(document.forms[1].elements[i].getAttribute("opcode"));
          if (document.forms[1].elements[i].type == 'select-multiple') 
          {//get selected list...
              dmpath = document.forms[1].elements[i].getAttribute('dmpath');            
              var parentKeys =  document.forms[1].elements[i].getAttribute("parentkeys");             
              if(parentKeys != null) 
              {
                 dmpath = procParentKeys(dmpath,parentKeys);
              }
              if(dmpath.indexOf("{0}") != -1) continue;
              params = setCookieInfo(dmpath,'', '', keys, cookieName, '');
              params += "&keyprms="+keys;
              params += "&reqtype=GET";
              params += "&host=" + getHostID();
              sendRequestToServer('ucmreq.igw', 'POST', params, list_getSelectedRecords);
              //alert("sent request2 params: " + params);
          }

        }
      }
    }
    if(document.forms[1].elements[i].getAttribute('dmpath') != null
           && document.forms[1].elements[i].getAttribute('dynamic') == 'true'){

          if(document.forms[1].elements[i].getAttribute('id') 
                    != document.getElementById('test').innerHTML) {
          document.getElementById('test').innerHTML = 
                                        document.forms[1].elements[i].getAttribute('id');
          var dmpath = document.forms[1].elements[i].getAttribute('dmpath');
                                   
          var keys =  document.forms[1].elements[i].name+"!nil|";
          var opCode = getOpCode("BYKEY");

          var base_url = setCookieInfo(dmpath,'','',keys,cookieName,opCode);
          base_url += "&reqtype=GET";
          base_url += "&host=" + getHostID();
          sendRequestToServer('ucmreq.igw', 'POST',base_url,showloadRec_stateChanged);
        }
    }
  }
}

function list_getSelectedRecords() {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert("data= " + data);
    var xml;
    if(window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    } else if(document.implementation 
              && document.implementation.createDocument) {
      // code for Mozilla, Firefox, Opera, etc.
      var parser = new DOMParser();
      xml = parser.parseFromString(data, 'text/xml');
    }
    //for(k=0; k< document.forms[1].elements.length; k++) {
    var list = document.getElementById('test').innerHTML;//alert(list);
    var resElement=document.getElementById(list);//alert(resElement);
    if(resElement == null){return;}
      if(resElement.type == 'select-multiple') {
        if (resElement.getAttribute('depends') != null) {
          var depends = list.substring(0,list.indexOf("#"));
          //resElement.getAttribute('depends').replace('+STR','');//alert(depends);alert(data);
          if(data.match(depends) != null) {
            var keys = list.replace('#','^');
            var dmpath = selectNodeStrVal("/root/dmpath", xml);
            var recs = xml.getElementsByTagName("record");
            var lastRec = recs[recs.length - 1];
            var tbl_records = 
                 selectNodeArry("//param[@name='"+depends+"']/@value", xml);
            var arrName = 
                 selectNodeStrVal("//param[@name='"+depends+"']/@name", xml);
            arrName += "SelArry";//alert(arrName);
            var oSelArray = infoMap.Get(arrName);
            if(!oSelArray) {
              infoMap.Set(arrName, tbl_records);
            } else {
              oSelArray = oSelArray.concat(tbl_records);
              infoMap.Set(arrName, oSelArray);
            }
            
            var parms = lastRec.getElementsByTagName("param");
            var keyVal = "";
            for(l = 0; l < parms.length; l++) {
              var pname =parms[l].getAttribute("name");
              if(pname == depends) {
                keyVal = parms[l].getAttribute("value");
                break;
              }
            }
            if((keyVal) && (keyVal != "")) {
              keys += "!"+ keyVal;
            } else {
              keys += "!nil";
            }
            keys += "|";
            var url = "dmpath=" + dmpath + "&mtpath=" + '' + "&sredirect=" 
                 + '' + "&opcode=" + getOpCode("MORE")  ;
            url = url + "&ucmreq=" + keys + "&reqtype=GET";
            url += "&host=" + getHostID();//alert("getNext url: " + url);
            sendRequestToServer('ucmreq.igw', 'POST', url, list_getSelectedRecords);
            return(true);
          }
        }
      }     
      if(resElement.getAttribute('dmpath') != null && 
          resElement.getAttribute('dynamic') == 'true') {
        var tbl_records = selectNodeArry("//param/@value",xml);
        var list = document.getElementById('test').innerHTML;
        //alert("name  "+ resElement.getAttribute('name'));
        if(resElement.getAttribute('id') == list) {
          for(j=0;j<tbl_records.length;j++) {
            if(resElement.type == 'checkbox'){
              if(tbl_records[j].nodeValue== 1) {
                resElement.checked = true; 
              }else{
                resElement.checked = false; 
              }
            }else {
              resElement.value = tbl_records[j].nodeValue;
            }
          }
          return(true);
        }
      }
    //} // for
  } // if
}

function showloadRec_stateChanged() {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert("data=" + data);
    var xml;
    if(window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    } else if(document.implementation 
              && document.implementation.createDocument) {
      // code for Mozilla, Firefox, Opera, etc.
      var parser = new DOMParser();
      xml = parser.parseFromString(data, 'text/xml');
    }
    //for(k=0; k< document.forms[1].elements.length; k++) {
    var list = document.getElementById('test').innerHTML;
    var resElement=document.getElementById(list);
    if(resElement == null){return;}
      if(resElement.type == 'select-one') {
        if((resElement.getAttribute('depdmpath') != null) && 
                   (resElement.getAttribute('depends') != null)) {
          var depends = resElement.getAttribute('depends').replace('+STR','');
          var depwhere = resElement.getAttribute('where');
          var dwhere = "";
          var dval = "";
          if (depwhere != null && depwhere != '') {
              if (depwhere.indexOf(",") != -1) {
                 depwhere = depwhere.substring(0, depwhere.indexOf(","));
              }
              dwhere = depwhere.substring(0, depwhere.indexOf("="));
              dval = depwhere.substring(depwhere.indexOf("=") + 1, depwhere.length);                
          }//alert("depwhere: "+depwhere);
          var depwhereMatched = false;
          if(data.match(depends) != null) {
            //var exp = "//param[@name='"+depends+"']/@value";            
            //var tbl_records = selectNodeArry(exp, xml);
            //var list = document.getElementById('test').innerHTML;
            if(resElement.getAttribute('id') == list) {
              var selIndex = 0;
              var curVal = resElement.getAttribute("curval");//alert(curVal);
              var selObj = resElement;
              if(selObj.options.length == 0) {
                 selObj.options[selObj.options.length] = 
                                         new Option("-Select Option-", "");
              }
              //alert("selIndex= " + selIndex);              
              if(selIndex > 0){
                selObj.selectedIndex = selIndex;
              }//alert("depends: " + document.forms[1].elements[i].getAttribute('depends'));
              var keys = document.forms[1].elements[i].getAttribute('depends').replace('+','^');
              //alert("keys: " + keys);
              var dmpath = selectNodeStrVal("/root/dmpath", xml);
              //alert("dmpath: " + dmpath);
              var recs = xml.getElementsByTagName("record");

              for (var cr=0; cr<recs.length; cr++) {  
               depwhereMatched = false;
               var depNameStr = "";
               var depValueStr = ""; 
               var recparams = recs[cr].getElementsByTagName("param");

               for(var cp=0; cp < recparams.length; cp++) {
                 var pn = recparams[cp].getAttribute("name");                 
                 var pv = recparams[cp].getAttribute("value");                 
                 var wv = "";
                 if (pn == depends) {
                     depNameStr = pn;
                     depValueStr = pv;
                 }
                 if (depwhere != null && depwhere != '') {
                   if (pn == dwhere) {                                   
                    var wv = pv;
                    if (wv == dval) {
                        //alert("where criteria met:" + wv + "  "+ depNameStr + "::" +depValueStr);
                        depwhereMatched = true;
                        break;
                    }
                   } 
                 }               
                
               }//end for params              
               
               //alert("curval= " +curVal);               
               if (depwhere != null && depwhere != '') {
                if (depwhereMatched) {
                  if(curVal != null && curVal == depValueStr) {                  
                   selObj.options[selObj.options.length] = 
                      new Option(depValueStr, depValueStr, 'TRUE');                 
                   selIndex = selObj.options.length - 1;
                  } else {
                   selObj.options[selObj.options.length] = 
                     new Option(depValueStr, depValueStr);
                  } 
                }
               } else {//alert("no where clause");
                 if(curVal != null && curVal == depValueStr) {                  
                  selObj.options[selObj.options.length] = 
                      new Option(depValueStr, depValueStr, 'TRUE');                 
                  selIndex = selObj.options.length - 1;
                 } else {
                  selObj.options[selObj.options.length] = 
                     new Option(depValueStr, depValueStr);
                 }
               }

              }//end for recs

              var lastRec = recs[recs.length - 1];             
              var parms = lastRec.getElementsByTagName("param");
               //alert("parms: " + parms);
               var keyVal = "";
               for(l = 0; l < parms.length; l++) {
                 var pname =parms[l].getAttribute("name");
                 if(pname == depends) {
                   keyVal = parms[l].getAttribute("value");
                   break;
                }
               }
               if((keyVal) && (keyVal != "")) {
                 keys += "!"+ keyVal;
               } else {
                 keys += "!nil";
               }
              keys += "|";//alert(keys);
              var url = "dmpath=" + dmpath + "&mtpath=" + '' + "&sredirect=" 
                 + '' + "&opcode=" + getOpCode("MORE")  ;
              url = url + "&ucmreq=" + keys + "&reqtype=GET";
              url += "&host=" + getHostID();//alert("showloadRec_stateChanged more: " + url);
              sendRequestToServer('ucmreq.igw', 'POST', url, showloadRec_stateChanged);
              return(true);
            }
          }
        }
      } else if(resElement.type == 'select-multiple') {
        if (resElement.getAttribute('depends') != null) {
          var depends =
            resElement.getAttribute('depends').replace(
                                                                   '+STR','');
          if(data.match(depends) != null) {
            var keys = resElement.getAttribute('depends').replace('+','^');
            var dmpath = selectNodeStrVal("/root/dmpath", xml);
            var recs = xml.getElementsByTagName("record");
            var lastRec = recs[recs.length - 1];
            var tbl_records = 
                 selectNodeArry("//param[@name='"+depends+"']/@value", xml);
            var arrName = 
                 selectNodeStrVal("//param[@name='"+depends+"']/@name", xml);
            arrName += "SelArry";
              //start populate data in the list
              var selObj = resElement;
              for(j=0;j<tbl_records.length;j++) {
                //alert("curval= " +curVal+"\ttbl_records[j]= " + tbl_records[j].nodeValue);
                if(curVal != null && curVal == tbl_records[j].nodeValue) {
                  //alert("set true...");
                  selObj.options[selObj.options.length] = 
                      new Option(tbl_records[j].nodeValue, tbl_records[j].nodeValue, 'TRUE');                 
                  selIndex = selObj.options.length - 1;
                } else {
                  selObj.options[selObj.options.length] = 
                     new Option(tbl_records[j].nodeValue, tbl_records[j].nodeValue);
                }
              }
              //end populate data in the list
            var parms = lastRec.getElementsByTagName("param");
            var keyVal = "";
            for(l = 0; l < parms.length; l++) {
              var pname =parms[l].getAttribute("name");
              if(pname == depends) {
                keyVal = parms[l].getAttribute("value");
                break;
              }
            }
            if((keyVal) && (keyVal != "")) {
              keys += "!"+ keyVal;
            } else {
              keys += "!nil";
            }
            keys += "|";
            var url = "dmpath=" + dmpath + "&mtpath=" + '' + "&sredirect=" 
                 + '' + "&opcode=" + getOpCode("MORE")  ;
            url = url + "&ucmreq=" + keys + "&reqtype=GET";
            url += "&host=" + getHostID();
            sendRequestToServer('ucmreq.igw', 'POST', url, showloadRec_stateChanged);
            return(true);
          }
        }
      }     
      if(resElement.getAttribute('dmpath') != null && 
          resElement.getAttribute('dynamic') == 'true') {
        var tbl_records = selectNodeArry("//param/@value",xml);        
        if(resElement.getAttribute('id') == list) {
          for(j=0;j<tbl_records.length;j++) {
            if(resElement.type == 'checkbox'){
              if(tbl_records[j].nodeValue== 1) {
                resElement.checked = true; 
              }else{
                resElement.checked = false; 
              }
            }else {
              resElement.value = tbl_records[j].nodeValue;
            }
          }
          return(true);
        }
      }
    //} // for
  } // if
}

// To populate request for dynamic list for the list box
function loadList(element, ndmpath, ndepends) {
  //alert("inside loadList: " + element);
  for(i=0; i<document.forms[1].elements.length; i++) {//alert(document.forms[1].elements[i].name);
    if(document.forms[1].elements[i].name == element) {
      if(document.forms[1].elements[i].getAttribute('depdmpath') != null 
          && document.forms[1].elements[i].getAttribute('depends') != null) {
        /*if(document.forms[1].elements[i].getAttribute('id') 
                      != document.getElementById('test').innerHTML) {*/
          document.getElementById('test').innerHTML = 
                                   document.forms[1].elements[i].getAttribute('id');
          var dmpath = document.forms[1].elements[i].getAttribute('depdmpath');          
          var depends = document.forms[1].elements[i].getAttribute('depends');
          if (ndmpath != null && ndepends != null) {
              dmpath = ndmpath;
              depends = ndepends;
              document.forms[1].elements[i].setAttribute('depdmpath', dmpath);
              document.forms[1].elements[i].setAttribute('depends', depends);
              //alert(document.forms[1].elements[i].getAttribute('depdmpath'));
              //alert(document.forms[1].elements[i].getAttribute('depends'));
          }
          //alert(dmpath); alert(depends);
          var keys="";
          var keyInstanceStr="";
          //alert("key attrib: " + document.forms[1].elements[i].getAttribute('key'));
          if (document.forms[1].elements[i].getAttribute('key') != null && document.forms[1].elements[i].getAttribute('key') != '') {
                var keyFieldName = document.forms[1].elements[i].getAttribute('key');
                //alert(keyFieldName);
                var keyValueSelObj = document.getElementById(keyFieldName);
                var keyFieldNameInSM = keyValueSelObj.getAttribute('depends');
                keyFieldNameInSM = keyFieldNameInSM.substring(0, keyFieldNameInSM.indexOf("+"));
                //alert(keyFieldNameInSM);
                //alert(keyValueSelObj.options[keyValueSelObj.selectedIndex].value);                
                keys = keyFieldNameInSM + "^STR!" + keyValueSelObj.options[keyValueSelObj.selectedIndex].value + "|";
                keyInstanceStr = "{" + keyValueSelObj.options[keyValueSelObj.selectedIndex].value + "}";
                //alert(keys);  
                if (dmpath.indexOf("{0}") != -1) {
                    //dmpath = procParentKeys(dmpath, keys);
                    dmpath = dmpath.replace("{0}", keyInstanceStr); 
                    //alert(dmpath);
                }              
          } else {
                keys = depends.replace('+','^')+"!nil|";//alert("here");
                dmpath = procParentKeys(dmpath, keys);
          }
          var params = setCookieInfo(dmpath,'', '', keys, '', '');
          params += "&reqtype=GET";
          params += "&host=" + getHostID();
          //alert("loadList:: params= " + params);
          sendRequestToServer('ucmreq.igw', 'POST', 
                               params, showloadRec_stateChanged);
        }
      }
    //}
  }
}

// To populate request for dynamic SelctrlList
function loadSelctrlList(element, ndmpath, ndepends) {
  //alert("loadSelcrtl List: " + element + " newdmpath: " + ndmpath + " newdepends: " +ndepends);
  for(i=0; i<document.forms[1].elements.length; i++) {//alert(document.forms[1].elements[i].name);
    if(document.forms[1].elements[i].name == element) {
      document.forms[1].elements[i].options.length = 0;
      var selectedMemberList = "targetList_" + element;
      document.getElementById(selectedMemberList).options.length = 0;
      if(document.forms[1].elements[i].getAttribute('depdmpath') != null 
          && document.forms[1].elements[i].getAttribute('depends') != null) {
        /*if(document.forms[1].elements[i].getAttribute('id') 
                      != document.getElementById('test').innerHTML) {*/
          document.getElementById('test').innerHTML = 
                                   document.forms[1].elements[i].getAttribute('id');
          var dmpath = ndmpath;//document.forms[1].elements[i].getAttribute('depdmpath');
          dmpath = procParentKeys(dmpath, document.forms[1].elements[i].getAttribute('parentkeys'));
          dmpath = dmpath.substring(0,dmpath.lastIndexOf("{"));
          document.forms[1].elements[i].setAttribute('dmpath', dmpath);
          var depends = ndepends; //document.forms[1].elements[i].getAttribute('depends');
           //alert(depends);
          document.forms[1].elements[i].setAttribute('depends', depends);
          var keys = depends.replace('+','^')+"!nil|";         
          var params = setCookieInfo(dmpath,'', '', keys, '', '');
          params += "&reqtype=GET";
          params += "&host=" + getHostID();
          //alert("loadSelctrlList:: params= " + params);
          sendRequestToServer('ucmreq.igw', 'POST', 
                               params, showloadSelctrlRec_stateChanged);
        }
      }    
  }
}

function showloadSelctrlRec_stateChanged() {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert("data=" + data);
    var xml;
    if(window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    } else if(document.implementation 
              && document.implementation.createDocument) {
      // code for Mozilla, Firefox, Opera, etc.
      var parser = new DOMParser();
      xml = parser.parseFromString(data, 'text/xml');
    }
    
    var list = document.getElementById('test').innerHTML;
    var resElement=document.getElementById(list);
    if(resElement == null){return;}
      if(resElement.type == 'select-multiple') {
        if (resElement.getAttribute('depends') != null) {
          var depends =
            resElement.getAttribute('depends').replace('+STR','');
          if(data.match(depends) != null) {//alert("here");
            var keys = resElement.getAttribute('depends').replace('+','^');
            var dmpath = selectNodeStrVal("/root/dmpath", xml);
            var recs = xml.getElementsByTagName("record");
            var lastRec = recs[recs.length - 1];
            var tbl_records = 
                 selectNodeArry("//param[@name='"+depends+"']/@value", xml);
            var arrName = 
                 selectNodeStrVal("//param[@name='"+depends+"']/@name", xml);
            arrName += "SelArry";
              //start populate data in the list
              var selObj = resElement;//alert(tbl_records.length);
              var curVal = resElement.getAttribute("curval");
              for(j=0;j<tbl_records.length;j++) {
                //alert("curval= " +curVal+"\ttbl_records[j]= " + tbl_records[j].nodeValue);
                if(curVal != null && curVal == tbl_records[j].nodeValue) {
                  //alert("set true...");
                  selObj.options[selObj.options.length] = 
                      new Option(tbl_records[j].nodeValue, tbl_records[j].nodeValue, 'TRUE');                 
                  selIndex = selObj.options.length - 1;
                } else {
                  selObj.options[selObj.options.length] = 
                     new Option(tbl_records[j].nodeValue, tbl_records[j].nodeValue);
                }
              }
              //end populate data in the list
            var parms = lastRec.getElementsByTagName("param");
            var keyVal = "";
            for(l = 0; l < parms.length; l++) {
              var pname =parms[l].getAttribute("name");
              if(pname == depends) {
                keyVal = parms[l].getAttribute("value");
                break;
              }
            }
            if((keyVal) && (keyVal != "")) {
              keys += "!"+ keyVal;
            } else {
              keys += "!nil";
            }
            keys += "|";
            var url = "dmpath=" + dmpath + "&mtpath=" + '' + "&sredirect=" 
                 + '' + "&opcode=" + getOpCode("MORE")  ;
            url = url + "&ucmreq=" + keys + "&reqtype=GET";
            url += "&host=" + getHostID();
            sendRequestToServer('ucmreq.igw', 'POST', url, showloadSelctrlRec_stateChanged);
            return(true);
          }
        }
      }     
      if(resElement.getAttribute('dmpath') != null && 
          resElement.getAttribute('dynamic') == 'true') {
        var tbl_records = selectNodeArry("//param/@value",xml);        
        if(resElement.getAttribute('id') == list) {
          for(j=0;j<tbl_records.length;j++) {
            if(resElement.type == 'checkbox'){
              if(tbl_records[j].nodeValue== 1) {
                resElement.checked = true; 
              }else{
                resElement.checked = false; 
              }
            }else {
              resElement.value = tbl_records[j].nodeValue;
            }
          }
          return(true);
        }
      }
  } // if
}

// To populate request for dynamic list for the text box
function dynamicList(element) {
  //alert("inside dynamicList...");
  for(i=0; i< document.forms[1].elements.length; i++) {
    if(document.forms[1].elements[i].name == element) {
      if(document.forms[1].elements[i].getAttribute('dmpath') != null 
          && document.forms[1].elements[i].getAttribute('depends') != null) {
        if(document.forms[1].elements[i].getAttribute('id') 
                      != document.getElementById('test').innerHTML) {
          document.getElementById('test').innerHTML = 
                                   document.forms[1].elements[i].getAttribute('id');
          var dmpath = 
             document.forms[1].elements[i].getAttribute('dmpath').replace('{0}',
                                                '{'+getCookie('Name-SNET')+'}');
          var depends = document.forms[1].elements[i].getAttribute('depends');
          var keys = depends.replace('+','^')+"!nil|";
          var params = setCookieInfo(dmpath,'', '', keys, '', '');
          params += "&reqtype=GET";
          params += "&host=" + getHostID();
          //alert("dynamicList:: params= " + params);
          sendRequestToServer('ucmreq.igw', 'POST', 
                               params, showloadList_stateChanged);
        }
      }
    }
  }
}

function sendSelctrlReq(component,cookieName,opcode,value){ 
  var metafile;
  var fredirect;
  var sredirect;
  f = document.forms[0];
  for(j=0;j<f.elements.length;j++){
     if(f.elements[j].name=='mtpath'){
       metafile = f.elements[j].value;
     }
   }
   if(component.getAttribute('depdmpath') != null 
          && component.getAttribute('depends') != null) {
          sredirect = component.getAttribute('sredirect');
          fredirect = component.getAttribute('fredirect');        
          var dmpath = component.getAttribute('depdmpath');
          var keys = component.getAttribute('name')+"="+value+"|";
          keys = "ucdm_" + dmpath + "^" + keys;
          var base_url = setCookieInfo(dmpath,metafile,sredirect,keys,cookieName,opcode);
          sendRequestToServer('ucmreq.igw', 'POST', base_url, proccessDynamicResp);                
  }
}
function sendDynamicReq(component,cookieName){
  var opcode = 'EDIT' ;
  var element = component.getAttribute('name');
  var value = 0;
  var metafile;
  var fredirect;
  var sredirect;
  var tokens;
  f = document.forms[0];
  for(j=0;j<f.elements.length;j++){
     if(f.elements[j].name=='mtpath'){
       metafile = f.elements[j].value;
     }
   }
   if(component.getAttribute("visible") == 'true')
   {
   if(component.getAttribute('dmpath') != null 
          && component.getAttribute('dynamic') != null) {

          sredirect = component.getAttribute('sredirect');
          fredirect = component.getAttribute('fredirect');

          if(component.type == 'checkbox'){
             if(component.checked){
                value = 1;
             }
          }else {
            value = component.value; 
          }
          var compPath = component.getAttribute('name');
          compPath = compPath.substring(0, compPath.indexOf("#"));
          var dmpath = document.forms[0].dmpath.value;//component.getAttribute('dmpath');
	  dmpath = dmpath + "." + compPath;
          /*if(dmpath.indexOf("{0}") > -1){
            tokens = dmpath.split("0");
            if((cookieName) && ('' != getCookie(cookieName))){
               dmpath = tokens[0] + getCookie(cookieName) + tokens[1];
             }
           }*/ 
          var keys = component.getAttribute('name')+"="+value+"|";
          keys = "ucdm_" + dmpath + "^" + keys;
          var base_url = setCookieInfo(dmpath,metafile,sredirect,keys,cookieName,opcode);
          sendRequestToServer('ucmreq.igw', 'POST', base_url, proccessDynamicResp);                
  }
  }
}
// To populate dynamic list for the text box.
function showloadList_stateChanged() {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert("showloadList_stateChanged:: data= " +data);
    if (window.ActiveXObject) {
      var xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
      for(i=0; i< document.forms[1].elements.length; i++) {
        if(document.forms[1].elements[i].type == 'text') {
          if(document.forms[1].elements[i].getAttribute('dmpath') != null) {
            var depends =  
                document.forms[1].elements[i].getAttribute('depends').replace('+STR','');
            if(data.match(depends) != null) {
              var tbl_records = xml.selectNodes("//param[@name='"+depends+"']/@value");
              var list = document.getElementById('test').innerHTML;
              if(document.forms[1].elements[i].getAttribute('id') == list) {
                var htmlStr = "";
                for(j=0;j<tbl_records.length;j++) {
                    var elename = list.replace('#','3');
                    htmlStr += 
                        "<a href='#' onclick=populate('"
                        + elename+"|"+tbl_records[j].nodeValue+"');>"
                        + tbl_records[j].nodeValue+"</a><br/>";
                }
                var lst  = list.replace('#','-');
                document.getElementById(lst).style.display="block";
                document.getElementById(lst).innerHTML=htmlStr;
                
                return(true);
              }
            }
          }
        }
      }
    }
    // code for Mozilla, Firefox, Opera, etc.
    else if(document.implementation && document.implementation.createDocument) {
      var list = document.getElementById('test').innerHTML;
      for(i=0; i< document.forms[1].elements.length; i++) {
        if(document.forms[1].elements[i].getAttribute('id') == list) {
          var depends =  
              document.forms[1].elements[i].getAttribute('depends').replace('^STR','');
                
          if(data.match(depends) != null) {
            var parser = new DOMParser();
            var xmldom = parser.parseFromString(data, 'text/xml');
            var tbl_records = 
                  xmldom.evaluate("//param[@name='"+depends+"']/@value", 
                                     xmldom, null,XPathResult.ANY_TYPE, null);
            var item;
            var html ='' ;
            while(item=tbl_records.iterateNext()) {
              var elename = list.replace('#','3');
              html += "<a href='#' onclick=populate('"
                       + elename+"|"+item.nodeValue+"');>"
                       + item.nodeValue+"</a><br/>";
            }
            var lst  = list.replace('#','-');
            document.getElementById(lst).innerHTML = html;
            document.getElementById(lst).style.display = "block";
                    
            return(true);
          }
        }
      }  
    }
  }
}
/****************** References Scripts END ***************/

/****************** Form related Scripts START ***************/

function getOPCodeStr(opcode) {
   var opCode = "";
   if('SETROL' == opcode) {
      opCode += getOpCode('GETROLE');
   } else if('ISETROL' == opcode) {
      opCode += getOpCode('IGETROLE');
   }
   return opCode;
}
function processMsgNode(mtpat, resXML) {
  var msgString="";
  var resStr="";
  var errMsg="";
  var dmPath="";
  var nvName="";
  var nvVal="";
  var metaxml = loadXMLDocument(mtpat); // Load metadata for lables
  resStr = selectNodeStrVal("/root/Msg/ResStr", resXML);
  errMsg = selectNodeStrVal("/root/Msg/ErrMsg", resXML);
  dmPath = selectNodeStrVal("/root/Msg/DMPath", resXML);
  msgString += "Result String: " + resStr;
  msgString += "\n";
  msgString += "Error Message: " + errMsg;
  nvName = selectNodeStrVal("/root/Msg/NvPair/@name", resXML);
  nvVal  = selectNodeStrVal("/root/Msg/NvPair/@value", resXML);
  msgString += "\n";
  msgString += "Name: ";
  if(nvName) {
    var expstr = "/root/obj[@name=\'" + nvName + "\']/@lbl";
    msgString += selectNodeStrVal(expstr, metaxml);
  }
  msgString += "\t";
  msgString += "Value: " + nvVal;

  msgString += "\n";
  msgString += "DM Path: " + dmPath;
  alert(msgString);
} //processMsgNode

// To get the Added or Modified record.
function getRecordFromServer(params, dmPath, mtPath, spg, opcode) {
  var opCode = getOpCode("BYKEY");
  var keys = "";
  var reqStr = params.substring(params.indexOf("ucmreq=") + 7, params.length);
  if(opcode == "SETROL") {
    opCode = getOpCode("RBYKEY");
  } else if(opcode == "ISETROL") {
    opCode = getOpCode("IRBYKEY");
  }//alert(params);
  //alert("getRecordFromServer::reqStr= " + reqStr);
  //alert("getRecordFromServer::dmPath= " + dmPath);
  if(reqStr.lastIndexOf("ucdm_") != 0) {//multi DM
    var tmpFirstReq = reqStr.split("ucdm_"); 
    for (var xy = 0; xy < tmpFirstReq.length; xy++) {
      //alert("tmpFirstReq["+xy+"]= " + tmpFirstReq[xy]);
      if (tmpFirstReq[xy].indexOf("!ADD^") != -1 
            && tmpFirstReq[xy].indexOf(dmPath) != -1) {
        reqStr = "ucdm_" + tmpFirstReq[xy];
        break;
      }
    }
  }
  var tmpToks = reqStr.split("^");
  var ptstr = "";
  for (var x = 0; x < tmpToks.length; x++) {
    if (tmpToks[x].indexOf("ucdm_") == -1) {//alert("t: " + tmpToks[x]);
      var kStr = tmpToks[x];
      if(kStr.indexOf("&") != -1) kStr = kStr.substring(0, kStr.indexOf("&"));
      if(kStr.indexOf("|") != -1) kStr = kStr.substring(0, kStr.indexOf("|"));
      //alert("kStr before replace: " +kStr);
      kStr = kStr.replace('#','^');
      kStr = kStr.replace('=','!');
      ptstr +=  kStr + "|"; break;
    }
  }
  keys = ptstr.substring(0, ptstr.length -1);
  //Exception for Zones
  //key can be modified to a new one. If key is changed, frame getExact on new key
  if (mtPath == "ZoneInfo_MetaData.xml") {
    var zoneName = "";
    var kvPairs = reqStr.split("|"); 
    for (var y = 0; y < kvPairs.length; y++) {
      if (kvPairs[y].indexOf("^zonename#") != -1) {
        zoneName = kvPairs[y].substring(kvPairs[y].indexOf("=") + 1, kvPairs[y].length);
        //alert("zone name: "+zoneName);
      }
      if (kvPairs[y].indexOf("newzonename#") != -1) {
        var newZoneName = kvPairs[y].substring(kvPairs[y].indexOf("=") + 1, kvPairs[y].length);
        //alert("new zone name: " + newZoneName);
        if (newZoneName != "") {
          if (newZoneName != zoneName) {
            keys = keys.replace(zoneName, newZoneName);
          }
        }
      }
    } //end for
  } //end if  
  else {
     var mdxml  = loadXMLDoc(mtPath);keys = "";
     var keyArr = selectNodeArry("/root/obj[@key='yes']", mdxml);
     for(var j=0; j<keyArr.length; j++) {
        var keyname = keyArr[j].getAttribute("name");
        var keytype = keyArr[j].getAttribute("type");
        //alert(keyname);        
        keys += keyname;
        keys += "^";
        keys += keytype;
        keys += "!";
        var keyFieldVal = params.substring(params.indexOf(keyname+"#"+keytype+"="), params.length);
        keyFieldVal = keyFieldVal.substring(keyFieldVal.indexOf("=") + 1, keyFieldVal.length);
        if (keyFieldVal.indexOf("|") != -1) {
            keyFieldVal = keyFieldVal.substring(0, keyFieldVal.indexOf("|"));
        } else {
            keyFieldVal = keyFieldVal.substring(0, keyFieldVal.indexOf("&"));
        }
        //alert(keyFieldVal);
        keys += keyFieldVal;            
        keys += "|";
     }
  }       
  //alert("params: " + reqStr);
  //alert("keys extracted : " +keys);
  if (params.indexOf("reqtype=DONE") != -1) {
    if (dmPath.charAt(dmPath.length - 1) == "}") {
      var index = dmPath.lastIndexOf("{");
      if (index != -1) {
        dmPath=dmPath.substring(0, index);
      }
    }
  }
  //alert("dmPath: " + dmPath);
  if (params.indexOf("reqtype=DONE") != -1) {
    keys += "bPassive^UINT!7|";
  }
  var b_url = setCookieInfo(dmPath,mtPath,spg,keys,'',opCode);         
  //b_url += "&keyprms="+keys;     
  if (params.indexOf("reqtype=DONE") != -1) {
    b_url += "&reqtype=GETPASSV";
  } else {
    b_url += "&reqtype=GET";
  }
  b_url += "&host=" + getHostID();
  //alert("getRecordFromServer:: b_url: " + b_url);
  sendRequestToServer('ucmreq.igw', 'POST', b_url, processResp);
  return;
} // getRecordFromServer

var actionFlag="";
var oldParams="";
function processRespAction(status, dmPath, mtPath, 
                            opcode, spg, fpg,
                            msgStr, keys, xml, params) {
  var delFlag='false';//alert(params);
  //alert("actionFlag= " + actionFlag);
  if('error' == status) {
    if('SAVE' == opcode) {
      removeSaveDiv();
      //alert("Save operation failed.\n" + msgStr);
      processMsgNode(mtPath,xml);
    } else if('FACRES' == opcode) {
      //alert("Reset to factory defaults operation failed.");
      processMsgNode(mtPath,xml);
    } else if('DELROL' == opcode || 'IDELROL' == opcode) {
      alert("Failed to delete the role.");
    } else {
      if('' != msgStr) {
        alert(msgStr);
      } else {
        processMsgNode(mtPath,xml);
      }
    }
    return;
  } else {
    if('STATS' == actionFlag) {
      actionFlag = "";
      return;
    }
    if (mtPath == "autosync_MetaData.xml") {
       if (params.indexOf("ucmreq=ucdm_igd.clstrgrp.autosync!EDIT^status#STR=enable") != -1) {
          // Here in logout page .."Save and Logout" button need to be hidden..
          logoutwithoutsave();
          return;          
       }
    }
    if('SAVE' == opcode) {
      removeSaveDiv();
      if (logoutFlag == "yes") { //logout
            logoutFlag = "no";            
            var form1 = document.getElementById('submit-form');
            form1.submit.value = "Logout Without Saving";
            //alert(form1.submit.value);                                 
      } else {
            alert("Successfully saved the configuration.");
      }      
      return;
    }else if('REBOOT' == opcode){
    alert("Rebooting the device. Please logout and re-login");
    // Here in logout page .."Save and Logout" button need to be hidden..
    logoutwithoutsave();
    return;
    } 
     else if('FACRES' == opcode) {
      alert("Successfully loaded the factory default configuration.");
      //delFlag = 'true';
    } else if('DELROL' == opcode || 'IDELROL' == opcode || 'DEL' == opcode
                     || ("MULTCMD" == opcode && params.indexOf("DEL") !=-1)) {
      delFlag = 'true';
      if(delRecord != "") {
          if(delRecord.indexOf(":") != -1) {
             var rowInd = delRecord.substring(0,delRecord.indexOf(":"));
             var tblId = delRecord.substring(delRecord.indexOf(":") + 1,delRecord.length);
             var tbl = document.getElementById(tblId);
             tbl.deleteRow(rowInd);
             delRecord="";
          }
      }
    } else {
      if('' != msgStr) {
        alert(msgStr);
      }
    }
    url = "ucm?dmpath=" + dmPath 
           + "&mtpath=" + mtPath
           + "&sredirect=" + spg;
    var opt = getOPCodeStr(opcode);
    if('' != opt) {
      url += "&opcode=";
      url += opt;
    }
    if('' != keys) {
      url += "&keys=" + keys;
    }
   //if target is self; otherwise show success/fail, close pop-up and render the record 
   //alert("params: " + params); 
   //alert("opcode: " + opcode);  
   //alert("keys: " + keys);  
   var isParentDiv="false";
   var prFrm = null;
   if(opener == null || opener.document == null)
     prFrm = parent.document.getElementById('main-form');
   else
     prFrm = null;
   var pFrame;
   var myTable;
   var tblId="";
   //alert("prFrm= " + prFrm + "delFlag= " + delFlag);
   if(prFrm && delFlag != 'true') {
     var pfrmName = prFrm.ifrmID.value;
     var pifrmObj = window.top.frameBottom.rightPanel.ifrmObj;
     if(pifrmObj) {
       //alert("PARENT::pifrmObj.name= " + pifrmObj.name);
       pFrame = pifrmObj;
       if(pifrmObj.document) {
         prFrm = pifrmObj.document.getElementById('main-form');
         tblId = prFrm.tblID.value;
         //alert("pifrmObj.document.tblID:= " + tblId);
         myTable = pifrmObj.document.getElementById(tblId);
       } else if(pifrmObj.contentDocument) {
         prFrm = pifrmObj.contentDocument.getElementById('main-form');
         tblId = prFrm.tblID.value;
         //alert("pifrmObj.contentDocument.tblID:= " + tblId);
         myTable = pifrmObj.contentDocument.getElementById(tblId);
       } else if(pifrmObj.contentWindow){
         prFrm = pifrmObj.contentWindow.getElementById('main-form');
         tblId = prFrm.tblID.value;
         //alert("pifrmObj.contentWindow.tblID:= " + tblId);
         myTable = pifrmObj.contentWindow.getElementById(tblId);
       }
     } else {
       prFrm = parent.document.getElementById('main-form');
       tblId = prFrm.tblID.value;
       myTable = parent.document.getElementById(tblId);
     } 
     isParentDiv="true";
   }

   if((this.window.name.indexOf("newW") != -1 || isParentDiv == "true")
                                               && (delFlag != 'true')) {
        //alert("opcode= " + opcode);
        if (opcode == "ADD" || opcode == "EDIT" || opcode == "MULTCMD" 
                            || opcode == "SETROL" || opcode == "ISETROL") {
            oldParams = params;
            getRecordFromServer(params, dmPath, mtPath, spg, opcode); 
            return;
        }
        var rowIndexVal="";
        //alert("render new rec val");
        if(this.window.name.indexOf("newW") != -1) {
	  prFrm = opener.document.getElementById('main-form');
        }
        tblId = prFrm.tblID.value;
        //alert("tblID= " +tblId);
        rowIndexVal = prFrm.rowEditIndex.value;
        var mtdata = selectNodeStrVal("/root/MetaData", xml);
        var records = selectNodeArry("/root/Config/record", xml);
        //alert("records:" +records.length);
        if ((records.length) == 0) {
          //alert("Render records from Params...");
           renderRecordFromParams(oldParams); 
           return;
        }
        var mtxml = loadXMLDocument(mtdata);
        //alert(opener.document.forms[0].rowEditIndex.value);
	//alert("No records Flag: " + opener.document.forms[0].noRecords.value);
        if(this.window.name.indexOf("newW") != -1) {
          opener.document.forms[0].modifiedRecords.value = "true";
	  //alert("modified records Flag: " + opener.document.forms[0].modifiedRecords.value);
	  myTable = opener.document.getElementById(tblId);
        }
	if (prFrm.noRecords.value == "true") {
          myTable.deleteRow(1);
	}
	updateMainTableRow(xml, xml, mtxml, tblId, myTable, rowIndexVal, pFrame);
	//updateMainTableRow(xml, xml, mtxml, tblId, rowIndexVal);
	prFrm.noRecords.value = "false";
	prFrm.renderRows.value = "yes";
	prFrm.rowEditIndex.value = "";
        var newHeight = 150;
        if(pFrame) {
          var f = parent.document.getElementById(pFrame.name);
          //newHeight = pFrame.document.getElementById("screenHeightDiv").offsetTop;
          //f.style.height = newHeight + 10 + "px";
          f.style.height = f.contentWindow.document.body.scrollHeight + 10 + "px";
          //alert("pFrame.name= " + pFrame.name + "\t NEW pFrame.height= " + pFrame.height);
        }
        if(this.window.name.indexOf("newW") != -1) {
          removeGreyDiv();
          window.open('','_parent','');
          window.close();
        } 
        else if(isParentDiv == "true") {
          //removeAllGreyDivs();
          var addEditDiv;
          if(pFrame) {
            //addEditDiv = this.name.substring(0, this.name.indexOf("_ifrm"));
            var frmName;
            if(this.name && this.name !="") {
              frmName = this.name;
            } else {
              frmName = tblId + "_addedit";
            }
            if(frmName.indexOf("_ifrm") != -1) {
              addEditDiv = frmName.substring(0, frmName.indexOf("_ifrm"));
            } else {
              addEditDiv = frmName;
            }
          } else {
            addEditDiv = tblId + "_addedit";
          }
          //alert("addEditDiv = " + addEditDiv);
          var aeDiv = parent.document.getElementById(addEditDiv);
          parent.document.body.removeChild(aeDiv);
        }

        if(mtPath == 'ROLE_MetaData.xml') {
          updateParentInstRole(dmPath, params, mtPath);
        }
   } else {
      if(opcode == "EDIT")  {
        opt = getOpCode("BYKEY");  
      }
      else if(opcode == "MULTCMD" && delFlag == 'true')  {
        //alert(fpg);
        if(this.window.name.indexOf("newW") != -1 && fpg == "frm_meta.xsl") {
          removeGreyDiv();
          window.open('','_parent','');
          window.close();
        }//to do ... close ldiv, if used?
      }
      if(delFlag != 'true') {        
        setCookieValue(dmPath, opt, mtPath, spg, keys, '', '', '');
      }
   }
  }
}

function renderRecordFromParams(params) {
	//alert("Render the row added/edited: " + params);
	var reqStr = params.substring(params.indexOf("ucmreq=") + 7, params.length);
        var pFrame;
        var prFrm = null;
	var tblId;
	var myTable;

        if(reqStr.indexOf("ucdm_") != -1) {
          var tmpToks = reqStr.split("^");
          var ptstr = "";
          for(var x = 0; x < tmpToks.length; x++) {
             if(tmpToks[x].indexOf("ucdm_") == -1) {
              ptstr += tmpToks[x] + "|";
            }
          }
          reqStr = ptstr;
        }
	//alert("after:: reqStr= " +reqStr);
	var paramStr = reqStr.split("|");
	var xmlRec = "<record>";
	for (var index=0; index < paramStr.length; index++) {
          var prm = paramStr[index];
          if (prm.indexOf("&reqtype") != -1) {
            prm = prm.substring(0, prm.indexOf("&reqtype"));
          }
          xmlRec += "<param name='" + prm.substring(0, prm.indexOf("#")) + "' value='" + prm.substring(prm.indexOf("=") + 1, prm.length) + "' />";
	}
	xmlRec += "</record>";
	//alert(xmlRec);
        //alert("row index: "+opener.document.forms[0].rowEditIndex.value);
	//Reload the values of the row with new values; High-light the row
	var fields = params.split("&");
	var xmlHdr = "<?xml version='1.0' encoding='UTF-8'?><root><MetaData>" + fields[0].substring(fields[0].indexOf("mtpath=") + 7, fields[0].length) + "</MetaData><dmpath>" + fields[1].substring(fields[1].indexOf("dmpath=") + 7, fields[1].length) + "</dmpath><Config>";
	var data = xmlHdr + xmlRec + "</Config></root>";
        //alert("local:= " + data);

         prFrm = opener.document.getElementById('main-form');
         tblId = prFrm.tblID.value;
         //alert("tblId" +tblId);
         myTable = opener.document.getElementById(tblId);
         //alert("myTable" +myTable);
        // code for IE
        if (window.ActiveXObject) {
          //alert(data);
          var xml = new ActiveXObject("Microsoft.XMLDOM");
          xml.loadXML(data);
        } // code for Mozilla, Firefox, Opera, etc.
        else if (document.implementation 
                  && document.implementation.createDocument) {
          var parser = new DOMParser();
          xml = parser.parseFromString(data, 'text/xml');
        }
        var mtdata = selectNodeStrVal("/root/MetaData", xml);
        var mtxml = loadXMLDocument(mtdata);
        //alert("rowEditIndex:" +opener.document.forms[0].rowEditIndex.value);
	//alert("No records Flag: " + opener.document.forms[0].noRecords.value);
	//var myTable = opener.document.getElementById(tblId);
	if (opener.document.forms[0].noRecords.value == "true") {
		myTable.deleteRow(1);
	}
	updateMainTableRow(xml, xmlRec, mtxml, tblId, myTable, opener.document.forms[0].rowEditIndex.value, pFrame);
	prFrm.noRecords.value = "false";
	prFrm.renderRows.value = "yes";
	prFrm.rowEditIndex.value = "";
        if(this.window.name.indexOf("newW") != -1) {
          removeGreyDiv();
          window.open('','_parent','');
          window.close();
        } 
}

function rsizeIfrmToFit(ifrobj) {
  ifrobj.style.height = ifrobj.contentWindow.document.body.scrollHeight + 10 + "px";
}

function removeSaveDiv() {
   var progressPanelDiv = this.parent.parent.frameTop.toolbar.document.getElementById("progressPanelDiv");
   if (progressPanelDiv != null) {
       this.parent.parent.frameTop.toolbar.document.body.removeChild(progressPanelDiv);        
   }
   var greyTopPanelDiv = this.parent.parent.frameTop.toolbar.document.getElementById("greyTopPanelDiv");
   if (greyTopPanelDiv != null) {
       this.parent.parent.frameTop.toolbar.document.body.removeChild(greyTopPanelDiv);        
   }

   var greyLeftPanelDiv = this.parent.parent.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv");
        if (greyLeftPanelDiv != null) {
            this.parent.parent.frameBottom.leftPanel.document.body.removeChild(greyLeftPanelDiv);        
        }


            var greyRightPanelDiv = this.parent.parent.frameBottom.rightPanel.document.getElementById("greyRightPanelDiv");
        if (greyRightPanelDiv != null) {
            this.parent.parent.frameBottom.rightPanel.document.body.removeChild(greyRightPanelDiv);        
        }

}

function removeAllGreyDivs() {
  //alert("removeAllGreyDivs:: window.name= " + window.name);
  var greyRightPanelDiv = null;
  var greyLeftPanelDiv = null;
  var greyTopPanelDiv = null;

  if (window.name == "newW") {
    greyLeftPanelDiv = opener.parent.parent.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv");
    //greyLeftPanelDiv = opener.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv");
    greyRightPanelDiv = opener.document.getElementById("greyRightPanelDiv");            
    greyTopPanelDiv = opener.parent.parent.frameTop.toolbar.document.getElementById("greyTopPanelDiv");
  } else {
    greyLeftPanelDiv = window.top.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv");
    greyRightPanelDiv = window.top.frameBottom.rightPanel.document.getElementById("greyRightPanelDiv");
  }
  if (greyRightPanelDiv != null) {
    window.top.frameBottom.rightPanel.document.body.removeChild(greyRightPanelDiv);        
  }
  if (greyLeftPanelDiv != null) {
    window.top.frameBottom.leftPanel.document.body.removeChild(greyLeftPanelDiv);        
  }
  if (greyTopPanelDiv != null) {
    window.top.frameBottom.leftPanel.document.body.removeChild(greyTopPanelDiv);        
  }
} // removeAllGreyDivs

function removeGreyDiv() {
        //alert("removeGreyDiv:: window.name= " + window.name);
        if (opener == null) return;
        var greyRightPanelDiv = null;
        var greyLeftPanelDiv = null;
        var greyTopPanelDiv = null;
        if (window.name == "newW") {
            greyLeftPanelDiv = opener.parent.parent.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv");
            greyRightPanelDiv = opener.document.getElementById("greyRightPanelDiv");            
            greyTopPanelDiv = opener.parent.parent.frameTop.toolbar.document.getElementById("greyTopPanelDiv");
        } else {
            greyRightPanelDiv = opener.document.getElementById("greyRightPanelDiv");            
        }
        if (greyRightPanelDiv != null) {
            opener.document.body.removeChild(greyRightPanelDiv);        
        }
        if (greyLeftPanelDiv != null) {
            opener.parent.parent.frameBottom.leftPanel.document.body.removeChild(greyLeftPanelDiv);        
        }
        if (greyTopPanelDiv != null) {
            opener.parent.parent.frameTop.toolbar.document.body.removeChild(greyTopPanelDiv);        
        }
}

// To update roles at parent instance level.
function updateParentInstRole(dmPath, params, mtPath) {
  if(dmPath.indexOf("{") != -1) {
    var pdmPath = dmPath.substring(0, dmPath.indexOf("{"));
    var msg = "Do you want to set the Role with " 
              + "\"Config Read Only\" Permissions "
              + "at dependent parent path \""
              + pdmPath + "\" level?";
    var cfmRole= confirm(msg);
    if(true == cfmRole) {
	  var sFields = params.split("&");
      var sParams = sFields[5];
      sParams = sParams.substring(sParams.indexOf("ucmreq=") + 7, sParams.length);
      sParams = sParams.substring(0, sParams.indexOf("|"));
      sParams += "|permiss#INT=1"
      //alert("sParams= " + sParams); 
      var setParams = setCookieInfo(pdmPath, mtPath, 'tbl_meta.xsl', 
                                               sParams, '', 'SETROL');
      setParams += "&host=" + getHostID(); 
      //alert("setParams= " + setParams); 
      sendRequestToServerEx('ucmreq.igw', 'POST', 
                             setParams, processResp, setParams);
    }
  }
}

function poplValue(mtxml, objID, objVal) {
  var retval;
  var expstr = "root/obj[@oid='" +objID +"']/list/item[@value='"
                              + objVal +"']/@name",mtxml;
  //alert("poplValue::expstr= " +expstr);
  val = selectNodeStrVal(expstr,mtxml);
  //alert("poplValue::val= " +val);
  if(val)
     retval = val;
  else
     retval = objVal;

  return retval;
}

function updateMainTableRow(xmlob, recs, mtxml, tblId, myTable, rowEditIndex, pFrame) {
  var tblelms =  selectNodeArry("root/tbl" ,mtxml);
  var htmTrStr = "";
  var htmTds= "<td align=\"center\" class=\"tdShowDiv\" width=\"10%\" height=\"1\">";
  var htmTde= "</td>";  
  var tBody = myTable.getElementsByTagName('tbody')[0];
  var cookieName = selectNodeStrVal("root/vrtlInst/@name" ,mtxml);
  var rindex;
  if (window.ActiveXObject) {
    rindex = 0;
  } else {
    rindex = 1;
  }

  if(!cookieName) {
    mtPat = selectNodeStrVal("root/depends/@mtpath", mtxml);
    var vtxml = loadXMLDocument(mtPat);
    cookieName = selectNodeStrVal("root/vrtlInst/@name", vtxml);
  }
  for(var x = 0; x < tblelms.length; x++) {
    var cols = tblelms[x].getElementsByTagName("col");
    var needRow = 'false';
    var row;
    if(this.window.name.indexOf("newW") != -1) {
      row = opener.document.createElement("tr");
    }  else {
      if(pFrame) {
        row = pFrame.document.createElement("tr");
      } else {
        row = parent.document.createElement("tr");
      }
    }
    for(var t = 0; t < cols.length; t++) {
        htmTrStr += htmTds;
        htmTrStr += "<img src='images/spacer.gif' align='absMiddle' height='25' width='1'/>"; //"<hr/>";
        //var cell1 = document.createElement("td");
        var cell1;
        var obrefs = cols[t].getElementsByTagName("objref");
        for(var o = 0; o < obrefs.length; o++) {
          var objID = obrefs[o].getAttribute("oid");
          var objName = "";
          var objVal = "";
          var isKey = "";
          var dlem = obrefs[o].parentNode.getAttribute("dlem");
          var nxtObj = obrefs[o+1];
          var nxtObjVal = "";
          var nxtObjName = "";

          if(nxtObj) {
            var nxtObjID = nxtObj.getAttribute("oid");
            if(nxtObjID) {
              var nxtObjName =
                 selectNodeStrVal("root/obj[@oid=" + nxtObjID +"]/@name", mtxml);
              if(nxtObjName) {
                nxtObjVal = selectNodeStrVal("/root/Config/record[" + rindex
                          + "]/param[@name='" + nxtObjName +"']/@value", xmlob);
              }
            }
          }

          if(objID) {
            isKey = selectNodeStrVal("root/obj[@oid=" + objID +"]/@key", mtxml);
            objName = 
                   selectNodeStrVal("root/obj[@oid=" + objID +"]/@name", mtxml);
            if(objName) {
              objVal = selectNodeStrVal("/root/Config/record[" + rindex
                          + "]/param[@name='" + objName +"']/@value", xmlob);
            }
            if(isKey == 'yes') {
              var keyType = 
                  selectNodeStrVal("root/obj[@oid=" + objID +"]/@type", mtxml);
              htmTrStr += "<keys ";
              htmTrStr += " value=\"" + objVal + "\"";
              htmTrStr += " type=\"" + keyType + "\"";
              htmTrStr += " name=\"" + objName + "\"";
              htmTrStr += " />";
            }
            if(objVal) {
              needRow = 'true';
              var mtFileName = selectNodeStrVal("root/MetaData", xmlob);
              if(mtFileName == "ROLE_MetaData.xml" && objName == "permiss" ) {
                if(objVal== '3' || objVal == '4') {
                  newVal = 
                     selectNodeStrVal("root/obj[@oid='2']/list/item[@value=2]"
                                       +"]/@name",mtxml);
                } else {
                  newVal = 
                     selectNodeStrVal("root/obj[@oid='2']/list/item[@value="
                              + objVal +"]/@name",mtxml);
                }
                htmTrStr += newVal;
              } else if(mtFileName == "AROLE_MetaData.xml" && objName == "permiss" ) {
                if(objVal== '1' || objVal == '2') {
                  newVal = 
                     selectNodeStrVal("root/obj[@oid='2']/list/item[@value=0]"
                                       +"]/@name",mtxml);
                } else {
                  newVal = 
                     selectNodeStrVal("root/obj[@oid='2']/list/item[@value="
                              + objVal +"]/@name",mtxml);
                }
                htmTrStr += newVal;
              } else {
                htmTrStr += poplValue(mtxml, objID, objVal);
              }
              if(dlem && (nxtObjVal != "")) {
                htmTrStr += dlem;
              }
              //htmTrStr += "&#160;";
            } else {
              htmTrStr += "";
            }
          }
        }
        htmTrStr += htmTde;
        htmTrStr += htmTds;
        var links = cols[t].getElementsByTagName("link");
        for(l = 0; l < links.length; l++) {
          var lnkName = links[l].getAttribute("name");
          var spg = links[l].getAttribute("spg");
          var lnkmtpath = links[l].getAttribute("mtpath");
          var opcode = links[l].getAttribute("opcode");
          var instancekey = links[l].getAttribute("instancekey");
          var subtbl = links[l].getAttribute("subtbl");//alert("subtbl: " + subtbl);
          var dmPat = selectNodeStrVal("root/dmpath", xmlob);
          if(!dmPat) {
            dmPat = selectNodeStrVal("root/dmpath", mtxml);
          }
          var lnxml = loadXMLDocument(lnkmtpath);
          var kExp = "/root/obj[@key='yes']";
          var kArr = selectNodeArry(kExp, lnxml);
          var lnkDmEx = "/root/dmpath";
          var lnkDmpath = selectNodeStrVal(lnkDmEx, lnxml);

          var keyPairs = "";
          for(k =0; k< kArr.length; k++) {
            var keyName = kArr[k].getAttribute("name");
            keyPairs += keyName;
            keyPairs += "^";
            keyPairs += kArr[k].getAttribute("type");
            keyPairs += "!";
            keyPairs += "nil";
            keyPairs += "|";
          }
          var lnStr = "[&#160;";
          var linkRef= lnStr;
          var tgt = links[l].getAttribute("target");
          var subDivId =  lnkmtpath.substring(0, lnkmtpath.indexOf('.'));
          //subDivId += "_" +tblId;
          //alert("subDivId= " + subDivId);
          linkRef += "<a href=\"#\"";
          if(tgt == "ldiv") {
            linkRef += " ldivid=\"" + subDivId + "\"";
          }
          linkRef += " tgt=\"" + tgt + "\"";
          linkRef += " onclick=\"setTblValueInstance(this,'";
          if (subtbl && subtbl == "no") { 
              //alert(lnkDmpath);
              linkRef += lnkDmpath + "', '";
          } else {
              linkRef += dmPat + "', '";
          }
          linkRef += cookieName + "', '";
          linkRef += lnkmtpath + "', '";
          linkRef += opcode + "', '";
          linkRef += spg + "', '";
          if (subtbl && subtbl == "no") { 
              linkRef += "', '";
          } else {
              linkRef += instancekey + "', '";
          }
          linkRef += keyPairs;
          linkRef += "','', 'new');\">";
          linkRef += lnkName; 
          linkRef += "</a>&#160;]";
          htmTrStr += linkRef;
          htmTrStr += "<br/>";
          //needRow = 'true';
        }
        htmTrStr += htmTde; // TD end for links
        htmTrStr += htmTds; // TD start for buttons
        var btns = cols[t].getElementsByTagName("btn");
        for(var b = 0; b < btns.length; b++) {
          var btnName = btns[b].getAttribute("name");
          var dmPath = selectNodeStrVal("root/dmpath", xmlob);
          if(!dmPath) {
            dmPath = selectNodeStrVal("root/dmpath", mtxml);
          }
          var mtPath = selectNodeStrVal("root/MetaData", xmlob);
          var opcode = selectNodeStrVal("root/opcode", mtxml);
          var isPassive = selectNodeStrVal("root/@passive", xmlob);
          switch(btnName) {
            case "Edit":
              var edbtn= "<input type=\"image\" src=\"images/prop.gif\" align=\"absMiddle\" title=\"Edit\"";
              edbtn += " onclick=\"popEditURL(this,'";
              edbtn += dmPath + "','";
              edbtn +=  mtPath + "','";
              edbtn += btns[b].getAttribute("spg") + "','";
              edbtn +=  opcode + "','";
              if(isPassive == "true") {
                edbtn +=  "GETPASSV" + "','";
              } else {
                edbtn +=  "GET" + "','";
              }
              edbtn +=  cookieName + "');\" />";
              htmTrStr += edbtn;
              break;
            case "Delete":
              var delbtn= "&#160;<input type=\"image\" src=\"images/delete.gif\" align=\"absMiddle\" title=\"Delete\"";
              var reqtyp = btns[b].getAttribute("reqtyp");
              if(reqtyp != null && reqtyp == "DONE") {
                delbtn += " reqtyp=\""+ reqtyp +"\"";
              }              
              delbtn += " onclick=\"deleteRecord(this,'";
              delbtn += dmPath + "','";
              delbtn +=  mtPath + "','";
              delbtn += btns[b].getAttribute("spg") + "','";
              delbtn += btns[b].getAttribute("fpg") + "','";
              delbtn +=  cookieName + "');\" />";
              htmTrStr += delbtn;
              break;
            case "Roles":
              var rolbtn= "&#160;<input type=\"image\" src=\"images/user.gif\" align=\"absMiddle\" title=\"Manage Permissions\"";
              rolbtn += " onclick=\"setRoleValueInstance(this,'";
              rolbtn += dmPath + "','";
              rolbtn +=  cookieName + "');\" />";
              htmTrStr += rolbtn;
              break;
            case "Up":
              var upbtn = "&#160;<input type=\"image\" src=\"images/uarrow.gif\" align=\"absMiddle\" title=\"Move Up\"";
              upbtn += " onclick=\"swapRowUp(this.parentNode.parentNode,'";
              upbtn +=  cookieName + "','"; 
              upbtn += dmPath + "','";
              upbtn +=  mtPath;
              upbtn += "');\" />";
              htmTrStr += upbtn ;
              break;
            case "Down":
              var dwbtn = "&#160;<input type=\"image\" src=\"images/darrow.gif\" align=\"absMiddle\" title=\"Move Down\"";
              dwbtn += " onclick=\"swapRowDown(this.parentNode.parentNode,'";
              dwbtn +=  cookieName + "','"; 
              dwbtn += dmPath + "','";
              dwbtn +=  mtPath; 
              dwbtn += "');\" />";
              htmTrStr += dwbtn ;
              break;
              case "radioBtn":
                var mexp=  "/root/obj[@key='yes']/@name";
                var kName = selectNodeStrVal(mexp, mtxml);
                var rexp=  "/root/Config/record/param[@name='" + kName+"']/@value";
                var rval = selectNodeStrVal(rexp, xmlob);
                var instName = selectNodeStrVal("/root/vrtlInst/@name", mtxml);
                var timOutVal = 
                      selectNodeStrVal("/root/vrtlInst/@timeout", mtxml);
                var radbtn = "<input type=\"radio\" name=\"";
                radbtn += btns[b].getAttribute("name") + "\" ";
                radbtn += " value=\"" + rval + "\" ";
                radbtn += " id=\"" + rval + "\" ";
                radbtn += " onclick=\"setInstance(this,'";
                radbtn += instName +"', '";
                radbtn += timOutVal +"');\" />";
                htmTrStr += radbtn;
                break;
          }
        }
        htmTrStr += htmTde;
        needRow = 'true';
        if(needRow == 'true') {
          if(this.window.name.indexOf("newW") != -1) {
            cell1 = opener.document.createElement("td");
          } else {
            if(pFrame) {
              cell1 = pFrame.document.createElement("td");
            } else {
              cell1 = parent.document.createElement("td");
            }
          }
          cell1.className="tabdata";
          cell1.height = 1;          

          cell1.innerHTML = htmTrStr;
	  row.className="glowRow";
	  row.height = 1;	  
          row.appendChild(cell1);
        }
        htmTrStr = "";
      }
      //alert("needRow: " + needRow);
      //alert("row = "+ row.innerHTML);
      if(needRow == 'true') {
	if (rowEditIndex == "") {//alert("add: " + rowEditIndex);
		//alert(myTable.getElementsByTagName('tr').length);
        	tBody.appendChild(row);
          if(this.window.name.indexOf("newW") != -1) {
            opener.window.scrollTo(0, opener.document.getElementById("addRecordDiv").offsetTop);
          } else {
            if(pFrame) {
              var offSetTopVal;
              if(pFrame.contentDocument) {
                offSetTopVal = pFrame.contentDocument.getElementById("addRecordDiv").offsetTop;
              } else if(pFrame.contentWindow) {
                offSetTopVal = pFrame.contentWindow.getElementById("addRecordDiv").offsetTop;
              } else if(pFrame.document) {
                offSetTopVal = pFrame.document.getElementById("addRecordDiv").offsetTop;
              }
              //parent.window.scrollTo(0, pFrame.contentDocument.getElementById("addRecordDiv").offsetTop);
              parent.window.scrollTo(0, offSetTopVal);
            } else {
              parent.window.scrollTo(0, parent.document.getElementById("addRecordDiv").offsetTop);
            }
          }
	} else {
	  tBody.appendChild(row);//alert(row.rowIndex);	  
	  //alert("Edit: " + rowEditIndex);
	  myTable.deleteRow(rowEditIndex);
	  var copyRow = myTable.insertRow(rowEditIndex);
	  copyRow.className="glowRow";
	  for (var i = 0; i < row.cells.length; i++) {
	    var fromCell = row.cells[i];
	    var toCell;
            if(this.window.name.indexOf("newW") != -1) {
	      toCell = opener.document.createElement("td");
            } else {
	      toCell = parent.document.createElement("td");
            }
	    toCell.className="tabdata";
	    toCell.innerHTML = fromCell.innerHTML;	    
	    copyRow.appendChild(toCell);
	    copyChildNodeValues(row.cells[i], toCell);	   
	  }
	  myTable.deleteRow(row.rowIndex);
	}
	
      }
      htmTrStr = "";
  }
} // updateTableRow

function getIEStrVal(xmlObj) {
  var dtstr= "";
  if(null == xmlObj) {
    return dtstr;
  }
  if((xmlObj[0]) && xmlObj[0].firstChild) {
    if(xmlObj[0].firstChild.nodeValue) {
      dtstr += xmlObj[0].firstChild.nodeValue;
    }
  } else {
    if((xmlObj[0]) && xmlObj[0].nodeValue) {
      dtstr += xmlObj[0].nodeValue;
    }
  }
  return dtstr;
}

function proccessDynamicResp(){
  var status = "";
  var dmPath = "";
  var mtPath = "";
  var opcode = "";
  var spg = "";
  var fpg = "";
  var msgStr = "";
  var keys = "";
  var xml;

  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    if (window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    }
    // code for Mozilla, Firefox, Opera, etc.
    else if(document.implementation && document.implementation.createDocument) {
       var parser = new DOMParser();
       var xml = parser.parseFromString(data, 'text/xml');
    }

    status = selectNodeStrVal("/root/@status", xml);
    dmPath = selectNodeStrVal("/root/dmpath", xml);
    mtPath = selectNodeStrVal("/root/MetaData", xml);
    opcode = selectNodeStrVal("/root/OPCode", xml);
    spg    = selectNodeStrVal("/root/Sredirect", xml);
    fpg    = selectNodeStrVal("/root/Fredirect", xml);
    if(status != 'ok') {
      msgStr = selectNodeStrVal("/root/Msg", xml);
      alert(msgStr);
    }

    var mtxml  = loadXMLDoc(mtPath);
    var index = dmPath.lastIndexOf(".");
    var name =  dmPath.substring(index+1,dmPath.length);
    var nodeArray = selectNodeArry("/root/obj", mtxml);
    var i =0;
    for(i=0;i<nodeArray.length; i++){
      if(nodeArray[i].getAttribute("name") == name){
         var key = name+"#"+nodeArray[i].getAttribute("type")+"!nil|";
         document.getElementById('test').innerHTML = name+"#"+nodeArray[i].getAttribute("type");

         var opCode = getOpCode("BYKEY");

         var base_url = setCookieInfo(dmPath,mtPath,spg,key,'',opCode);
         base_url += "&reqtype=GET";
         base_url += "&host=" + getHostID();
         sendRequestToServer('ucmreq.igw', 'POST',base_url,showloadRec_stateChanged);
         return(true);
       }
    }
  } // if readyState
}

function processResp(prms) {
  var status = "";
  var dmPath = "";
  var mtPath = "";
  var opcode = "";
  var spg = "";
  var fpg = "";
  var msgStr = "";
  var keys = "";
  var xml;

  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert(data);
    if (window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    }
    // code for Mozilla, Firefox, Opera, etc.
    else if(document.implementation && document.implementation.createDocument) {
       var parser = new DOMParser();
       var xml = parser.parseFromString(data, 'text/xml');
    }

    status = selectNodeStrVal("/root/@status", xml);
    dmPath = selectNodeStrVal("/root/dmpath", xml);
    mtPath = selectNodeStrVal("/root/MetaData", xml);
    opcode = selectNodeStrVal("/root/OPCode", xml);
    spg    = selectNodeStrVal("/root/Sredirect", xml);
    fpg    = selectNodeStrVal("/root/Fredirect", xml);
    if(status == 'ok') {
      msgStr = selectNodeStrVal("/root/Msg/ResStr", xml);
			if (msgStr != null && msgStr != '')
				  alert(msgStr);
    }    
    processRespAction(status, dmPath, mtPath, opcode, 
                      spg, fpg, msgStr, keys, xml, prms);
    return(true);
  } // if readyState
} // processResp

function submitFrmData(frmReq) {
  var params ="";
  //var frmReq = document.forms[0];
  //var  frmReq= parent.parent.frameBottom.rightPanel.document.forms[0];
  for(i = 0; i < frmReq.elements.length; i++){
    if(!(frmReq.elements[i].name == "keyprms" && frmReq.elements[i].value == "")) {
      params += frmReq.elements[i].name
               +  "=" + frmReq.elements[i].value + "&";
    }
  }
  if(params.charAt(params.length - 1) == '&') {
     params = params.substring(0,params.lastIndexOf("&"));
  }//alert(params);
  sendRequestToServerEx('ucmreq.igw', 'POST', params, processResp, params);
}
/****************** Form related Scripts END ***************/
function popValidateProp(fileName) {
  //alert("popValidateProp: fileName= "+fileName);
  try {
     req=createRequest();     
     url=fileName;
     req.onreadystatechange=function()
     {
       if(req.readyState==4) {
         resp=req.responseXML;
         prps=resp.getElementsByTagName("obj");
         if(prps) {
            for(i=0;i<prps.length;i++) {
              var oid = prps.item(i).getAttribute("oid");
              var objName = prps.item(i).getAttribute("name");
              var dtType = prps.item(i).getAttribute("type");

              /*alert("oid= "+ oid
                   + " name= "+ objName
                   + " type= "+ dtType);*/
              var rangeNode = prps.item(i).getElementsByTagName("range");
              var rangVal;
              if(rangeNode) {
                if(rangeNode.item(0)) {
                   rangVal = rangeNode.item(0).firstChild.nodeValue;
                  //alert("firstChild->Data= " + rangVal);
                }
              }
              infoMap.Set(objName, Array(oid,dtType,rangVal));
            }
         }
       }
    };
    req.open("GET", url, true);
    req.setRequestHeader("If-Modified-Since", "Sat, 1 Jan 2000 00:00:00 GMT");
    req.send(null);
    }
    catch(e)
    {
      //alert("Exception===> " +e.message);
    }
}
function displayInfo(key){
   alert(infoMap.Get(key));
}

//not in use
function popFrame(fileName) {
 // alert("popFrame: fileName= "+fileName);
  try {
     req=createRequest();     
     url=fileName;
     req.onreadystatechange=function()
     {
       if(req.readyState==4) {
        if(status ==200) {
          resp=req.responseXML;
          prps=resp.getElementsByTagName("MetaData").item(0).firstChild;
          prps.nodeValue="IPDB_Metadata.xml";
          alert("firstChild data= "+ prps.data);
       }
     }
    };
    req.open("GET", url, true);
    req.setRequestHeader("If-Modified-Since", "Sat, 1 Jan 2000 00:00:00 GMT");
    req.send(null);
    }
    catch(e)
    {
      alert("Exception===> " +e.message);
    }
   return req;
 }
function createRequest(){
  try{
    request = new XMLHttpRequest();
  } catch (trymicrosoft) {
    try {
      request = new ActiveXObject("Msxml2.XMLHTTP");
    } catch (othermicrosoft) {
      try {
        request = new ActiveXObject("Microsoft.XMLHTTP");
      } catch (failed) {
        request = false;
      }
    }
  }
  if (!request)
    alert("Error initializing XMLHttpRequest!");
  return request;
}

function loadXMLDoc(fname)
{
  var xmlDoc;
  // code for IE
  if (window.ActiveXObject)
  {
    xmlDoc=new ActiveXObject("Microsoft.XMLDOM");
  }
  // code for Mozilla, Firefox, Opera, etc.
  else if (document.implementation
  && document.implementation.createDocument)
  {
    xmlDoc=document.implementation.createDocument("","",null);
   }
  else
  {
    alert('Your browser cannot handle this script');
  }
  xmlDoc.async=false;
  xmlDoc.load(fname);
  return(xmlDoc);
}
function changeDisplay(id, status){
  //alert(" OBJECT ID = " + id);
  if (document.getElementById) {
    document.getElementById(id).style.display = status;
  }
}
function setContent(objId, obName) {
  //alert("setContent::objID= " + objId);
  var div1 = document.getElementById(objId);
  if(div1) {
    var inHtm = div1.innerHTML;
    var defId= "oDefault" + obName;
    //alert("defId=" + defId);
  
  //document.getElementById('oDefault').innerHTML = inHtm;
 
    if(document.getElementById(defId)) {
     //document.getElementById(defId).innerHTML = inHtm;
     var defDiv = document.getElementById(defId);
     defDiv.style.display = 'block';
     defDiv.innerHTML = inHtm;
    }
  }
}

function updateParentTxInfo(cookiename, ldmpath) {
  var  f = document.forms[0];
  for(var i = 0; i < f.elements.length; i++){
    if(f.elements[i].name=="useparenttx")
    {
      //alert("updateParentTxInfo:: found useparenttx");
      updateTxField(f.elements[i], cookiename, ldmpath);
    }
  }
}

function updateTxField(field, cookiename, ldmpath) {
  var fdmpat = "";
  var fopcode = "";
  var noRecs = "false";
  var ptxrecs = "false";
  if(opener) {
    noRecs = opener.document.forms[0].noRecords.value;
    ptxrecs = opener.document.forms[0].ptxRecords.value;
  }
  //alert("No records Flag: " + noRecs + " :: ptx.value= " + ptxrecs);
  if(ldmpath == null || ldmpath == "") {
    fdmpat = document.forms[0].dmpath.value;
  } else {
    fdmpat = ldmpath;
  }
  if (opener) {
    var prntopnr = opener.parent;
    if ((prntopnr != null) && (prntopnr.document) != null) {
       if ((prntopnr.document.forms[0] != null) && (prntopnr.document.forms[0].opcode) != null) {
        fopcode = prntopnr.document.forms[0].opcode.value;
      }
    }
  }
  //alert("fopcode=" +fopcode);
  var ppath = field.getAttribute('ppath');
  var fqn = field.getAttribute('dmpath');
  var ppathTok = "." + ppath + "{";                                          
  fqn = fdmpat.substring(0, fdmpat.indexOf(ppathTok) + ppath.length + 1);
  //alert("updateTxField::fdmpat= " + fdmpat);
  var dmPath="";
  var keyNvPair="";
  if(fqn.indexOf("{0}") > -1){
    var tokens = fqn.split("{0}");
    if((cookiename) && ('' != getCookie(cookiename))){
      dmPath = tokens[0] + "{" + getCookie(cookiename) + "}" + tokens[1];
    }
  }
  if("" == dmPath){
    dmPath = fqn;
  }

  var kname = field.value;
  if(kname.charAt(kname.length - 1) == ',') {
    kname = kname.substring( 0, kname.length - 1 );
  }
  //alert("kname= " + kname);
  var keyToks = kname.split(",");
 
  var ppathIndex = fdmpat.indexOf(ppath);
  if(ppathIndex != -1) {
    var keyPathIndex = ppathIndex + ppath.length ;
    var keypath =  fdmpat.substring(keyPathIndex,  fdmpat.length);

    keypath = keypath.substring(1, keypath.indexOf("}"));
    // alert("keypath = " + keypath);

    if(keyToks.length > 0 && keypath.indexOf("=") != -1) {
      var keyPairToks = keypath.split(","); 
      for(var x=0; x < keyPairToks.length; x++) {
        var tkeyName = "";
        var tkeyVal = "";
        var kindex = keyPairToks[x].indexOf("=");
        if(kindex != -1) {
          tkeyName = keyPairToks[x].substring(0,kindex);
          tkeyVal = keyPairToks[x].substring(kindex +1, keyPairToks[x].length);
        }
        for(var y = 0; y < keyToks.length; y++) {
          if(keyToks[y].indexOf(tkeyName) != -1) {
            keyNvPair += keyToks[y] + "=" + tkeyVal + "|";
          }
        }
      }
      if(keyNvPair.charAt(keyNvPair.length - 1) == '|') {
        keyNvPair = keyNvPair.substring( 0, keyNvPair.length - 1);
      }
    } else {
      keyNvPair += kname +"="+keypath;
    }
    //var dmpath = "ucdm_"+ dmPath +"!EDIT^"+ keyNvPair;
    var cmd = ""; 

    //alert("updateTxField:: noRecs= " + noRecs + " ptxrecs= " + ptxrecs);
    if(noRecs == "true" && ptxrecs == "true" && fopcode == "ADD") {
      cmd = "!ADD^"; 
    } else if(noRecs == "false" && ptxrecs == "true" && fopcode != "EDIT") {
      field.value="";
      return;
    } else if(noRecs == "false" && ptxrecs == "true" && fopcode == "EDIT") {
      cmd = "!EDIT^"; 
    }else {
      cmd = "!EDIT^"; 
    }
    var dmpath = "ucdm_"+ dmPath + cmd + keyNvPair;
    //alert("updateTxField:: dmpath= " + dmpath);
    field.value=dmpath;
  }
}

function updateUIContent(cookiename) {
  var  f = document.forms[1];
    for(i=0; i<f.elements.length; i++){
     if(f.elements[i].name=="useparenttx")
     {
       updateTxField(f.elements[i], cookiename);
     }
     if (f.elements[i].type == "text") {
         f.elements[i].id = f.elements[i].name;
     } 
     else if(f.elements[i].type == "radio") {
        if(f.elements[i].checked){         
           var nameStr = f.elements[i].id;           
           contStr = nameStr + "_" 
                     + f.elements[i].value;
           toggleFields(contStr);
        }
     }else if(f.elements[i].type == "select-one") {        
        var nameStr = f.elements[i].getAttribute("obid");       
        contStr = nameStr + "_" 
                    + f.elements[i].value;
           toggleFields(contStr);
     }else if(f.elements[i].type == "select-multiple") {//alert(f.elements[i].name);
        if(f.elements[i].getAttribute("depends")){
           updateMSelectionList(f.elements[i].name);
        }
     }else if(f.elements[i].type == "checkbox"){
        if(f.elements[i].checked){          
           var nameStr = f.elements[i].id;           
           contStr = nameStr + "_" + "checked";
           toggleFields(contStr);
       } else {
           var nameStr = f.elements[i].id;           
           contStr = nameStr + "_" + "unchecked";
           toggleFields(contStr);
       }
   }
  }
  //load the iframes with tables...
   var iframes = document.getElementsByTagName("iframe");
   for(var l = 0; l < iframes.length; l++) {
     var iName = iframes[l].getAttribute("name");
     var dmPathStr = iframes[l].getAttribute("dmpath");
     var mtpathStr = iframes[l].getAttribute("mtpath");
     var pkeyStr = iframes[l].getAttribute("pkeys");
     var dependsOn = iframes[l].getAttribute("depends");
     //alert("depends= " + dependsOn);
     var dstyle = iframes[l].getAttribute("disStyle");
     //alert("style= " + dstyle);
     //alert("pkeysStr= " + pkeysStr);
     if (document.forms[0].dmpath.value != "") {
       var gDmPath = document.forms[0].dmpath.value;
       //alert("gDmPath= " + gDmPath);
       if (gDmPath.indexOf("{") != -1) {
         var iPaths = gDmPath.split(".");
         for (var ipInd=0; ipInd < iPaths.length; ipInd++) {
           if (iPaths[ipInd].indexOf("{") != -1) {
             var ipathStr=iPaths[ipInd];
             gKeyArray[kCount] = ipathStr.substring(ipathStr.indexOf("{")+1,
                                                     ipathStr.indexOf("}"));
             kCount++;
           } // if
         } // for
       } // if
     } // if
     var dmpathResoved="";
     var pkeysStr = pkeyStr;
     if(pkeysStr != null) {
       pkeysStr = pkeysStr.substring(0, pkeysStr.lastIndexOf("|")); 
       pkeysStr = pkeysStr.substring(pkeysStr.lastIndexOf("|") + 1, pkeysStr.length);
       var keyArr = pkeysStr.split(",");
       var keyResolvedFinal = "";
       for (var k=0; k<keyArr.length; k++) {
           if (keyArr[k] != "") {
             var keyResolved = keyArr[k].substring(0, keyArr[k].indexOf("$$")) + "!0|";
             keyResolved = keyResolved.replace("+", "^");
             //alert(keyResolved);
             keyResolvedFinal += keyResolved;
           }
       }
      if(keyResolvedFinal.charAt(keyResolvedFinal.length -1) == "|") {
        keyResolvedFinal = keyResolvedFinal.substring(0,keyResolvedFinal.length -1);
      }
       //alert("final keys: " + keyResolvedFinal);
       dmpathResoved = procParentKeys(dmPathStr, pkeyStr);
       //alert("dmpathResoved= " + dmpathResoved);
     }
     var renderingXSL = "";
     if (dstyle == "form") {
            renderingXSL = "frm_meta.xsl";
     } else {
            renderingXSL = "tbl_meta.xsl"; 
     }
     ucmReqObj = new UCMReq(dmpathResoved, "", mtpathStr,
                         renderingXSL, renderingXSL, 
                         "", keyResolvedFinal, "GET",
                         getHostID());
     iframes[l].src = "redirectformtable.htm";
  }  
} // updateUIContent 

function isParentFormTblPage() {
   var iframes = parent.document.getElementsByTagName("iframe");
   //alert("iframes.length= " + iframes.length);
   if(iframes == null) return false;
   if(iframes.length == 0) return false;
   var normalIfrmFlag = true;
   for(var l = 0; l < iframes.length; l++) {
     var src = iframes[l].getAttribute("src");
     //alert("src= " + src);
     if (src == "redirectformtable.htm") {
       normalIfrmFlag = false;
       //return true;
     }
   } // for
   if(normalIfrmFlag) {
     return false;
   } else {
     return true;
   }
}

function selectedValue(selItem) {//alert("in selectedValue func");
  var selIndex = selItem.selectedIndex;
  var selNode = selItem.options[selItem.selectedIndex];
  var selName = selItem.getAttribute("name");
  var loadStr = selItem.getAttribute("load");//alert(loadStr);
  if (loadStr != null && loadStr != '') { //alert(document.getElementById(loadStr).type);
   if (document.getElementById(loadStr) != null && typeof document.getElementById(loadStr).type != "undefined") {
     //Empty the list...
     var loadListObj = document.getElementById(loadStr);
     loadListObj.options.length = 0;

     var mdmp = loadListObj.getAttribute("mdmpath");
     var mdep = loadListObj.getAttribute("mdepends");
     var whereStr = loadListObj.getAttribute("mwhere");
     if (mdmp != null && mdep != null) {                    
            var dmps = mdmp.split(",");            
            var deps = mdep.split(",");            
            var fieldName = loadListObj.name;
            var FirstEleSelOption = selItem.options[0];
            if (FirstEleSelOption.text == "-Select Option-") { selIndex--; }
            //alert(dmps[selIndex]);
            //alert(deps[selIndex]);
            //alert("name: " + fieldName);
            if (whereStr != null) {
                if (whereStr.indexOf(",") != -1) {                    
                    var whereStrings = whereStr.split(",");
                    whereStr = whereStrings[selIndex];
                    //alert("multiple where; use this: "+whereStr);
                    loadListObj.setAttribute('where', whereStr);
                }
            }
            loadList(fieldName, dmps[selIndex], deps[selIndex]);         
     } else {     
            //load the field with name in string 'loadStr'
            loadList(document.getElementById(loadStr).name, null, null);
     }
   } else { //alert("load iframe: "+ loadStr);alert(document.getElementById(loadStr).getAttribute("class"));
     if (document.getElementById(loadStr).getAttribute("class") == "grpDiv") {
        //alert("load selctrl in the div");
        var mtpathStr = document.getElementById(loadStr).getAttribute("mtpath");
        //alert(mtpathStr);
        var mtObj = loadXMLDoc(mtpathStr); 
        //alert(mtObj);
        var selctrlField = selectNodeArry("/root/form/field[@type='selcrtl']", mtObj)[0];                                   
        if(selctrlField) { 
            var oid = selctrlField.getAttribute("oid");
            var mdmp = selctrlField.getAttribute("mdmpath");
            var dmps = mdmp.split(",");
            var mdep = selctrlField.getAttribute("mdepends");
            var deps = mdep.split(",");
            var fieldObj = selectNodeArry("/root/obj[@oid='"+oid+"']", mtObj)[0];
            var fieldName = fieldObj.getAttribute("name") + "#STR";
            //alert(dmps[selIndex]);
            //alert(deps[selIndex]);           
            //alert("name: " + fieldName);
            var whereStr = selctrlField.getAttribute("mwhere");
            if (whereStr != null) {
                if (whereStr.indexOf(",") != -1) {
                    //alert("multiple where: "+whereStr);
                    var whereStrings = whereStr.split(",");
                    whereStr = whereStrings[selIndex];
                    selctrlField.setAttribute('where', whereStr);
                }
            }              
            loadSelctrlList(fieldName, dmps[selIndex], deps[selIndex]);
        }
     }
   }
  }
  var nodes = selNode.getElementsByTagName("key");
  if(typeof nodes[0] == "undefined") {
     //alert("select option");
     toggleFields(selItem.getAttribute("obid") + "_");
  }
  if(nodes != null && nodes[0] != null) {
    var idValue = nodes[0].getAttribute("idval");
    var selVal = selItem.getAttribute("obid") + "_" + idValue.substring(idValue.indexOf("_") + 1, idValue.length);     
      toggleFields(selVal);
      forms = document.forms[0];     
      for(var j=0;j<forms.elements.length;j++){
       if(forms.elements[j].name == "mtpath"){
        metaFile = forms.elements[j].value;
        break;
       }
      }
      var mtxml = loadXMLDoc(metaFile); 
      registerForVald(document.getElementById(selVal),mtxml);
  }
}

function toggleCheckBox(checkbox,cookieName)
{
  var id;
  var idStr = checkbox.getAttribute("id");
  if(checkbox.checked){
     id = idStr + "_checked";
  }else{
     id = idStr + "_unchecked";
  }
 toggleFields(id);
 forms = document.forms[0];
 for(var j=0;j<forms.elements.length;j++){
    if(forms.elements[j].name == "mtpath"){
      metaFile = forms.elements[j].value;
      //alert("Meta File name " + metaFile);
      break;
    }
 }
 var mtxml = loadXMLDoc(metaFile);
 registerForVald(document.getElementById(id),mtxml);
}

function toggleFields(objId) {
  //alert("toggleFields:: " + objId);
  var div1 = document.getElementById(objId);
  if(div1) {
    var inHtm = div1.parentNode.innerHTML;
    var divElems =  div1.parentNode.childNodes;
    div1.parentNode.style.display = 'block';
    for(var j = 0; j < divElems.length; j++) {
      elemId  =  divElems[j].getAttribute("id");
      if(elemId == objId) {
          //alert("divElems[j].innerHTML = " +divElems[j].innerHTML);
          divElems[j].style.display = 'block';
          var subDivs = divElems[j].getElementsByTagName("div");
          //alert("subDivs.length= " + subDivs.length);
          for(var x = 0; x < subDivs.length; x++) {
             var grppath = subDivs[x].getAttribute("grppath");
             if(grppath) {
                 subDivs[x].style.display = 'block';
             } 
          }
          setAttribute(divElems[j].getElementsByTagName("input"),'visible','true');
          setAttribute(divElems[j].getElementsByTagName("select"),'visible','true');
     }
     else {
	  //alert("divElems[j].innerHTML = " +divElems[j].innerHTML);
	  if(divElems[j].style.display == 'block'){
            deRegisterForVald(divElems[j]);
          }
	  divElems[j].style.display = 'none';
	  resetFields(divElems[j]);
          setAttribute(divElems[j].getElementsByTagName("input"),'visible','false');
          setAttribute(divElems[j].getElementsByTagName("select"),'visible','false');
          //setAttribute(divElems[j].getElementsByTagName("div"),'style','display:none');
     }
    }
  }
  return;
}
function toggleRadio(objId){
 toggleFields(objId);
 forms = document.forms[0];
 for(var j=0;j<forms.elements.length;j++){
    if(forms.elements[j].name == "mtpath"){
      metaFile = forms.elements[j].value;
      //alert("Meta File name " + metaFile);
      break;
    }
 }
 var mtxml = loadXMLDoc(metaFile); 
 registerForVald(document.getElementById(objId),mtxml);
}

function registerForVald(elem,mtxml){
 if(elem){
  var children = elem.childNodes;  
  for(var i =0 ;i<children.length;i++){
    if(children[i].type == 'text'){
      var index = children[i].name.indexOf('#');
      var name = children[i].name.substring(0,index);
      //alert(name + " is visible");
      var node = selectNodeArry("/root/obj[@name='"+name+"']", mtxml)[0];
      if(node) {
        var HTMLName = node.getAttribute("lbl");
        //var isMand =  node.getAttribute("mand");
        var rangeNode = selectNodeArry("/root/obj[@name='"+name+"']/range", mtxml)[0];
        var range;
        if(rangeNode)
           range= rangeNode.childNodes[0].nodeValue;

        var type = node.getAttribute("type");
        define(children[i].name,HTMLName,type,range,'true');
        //alert("HTML name " + HTMLName + "range " + range);
      }
    }else if((children[i].nodeName == 'DIV') && (children[i].style.display == 'none')){
      //alert(children[i].id + " has display style none ");
      continue;
    }else{
      //alert( i + " Fetching the children of " + children[i].nodeName + " for validation " + children[i].type);
      registerForVald(children[i],mtxml);
    }
  }
 }
 return;
}

function deRegisterForVald(elem){
 if(elem){
  var children = elem.childNodes;  
  for(var i =0 ;i<children.length;i++){
    if(children[i].type == 'text'){
      var index = children[i].name.indexOf('#');
      var name = children[i].name.substring(0,index);
      //alert(name + " is being deregistered");
      deRegister(children[i].name);
    }else if((children[i].nodeName == 'DIV') && (children[i].style.display == 'none')){
      //alert(children[i].id + " has display style none ");
      continue;
    }else{
      //alert( i + " Fetching the children of " + children[i].nodeName + " for validation " + children[i].type);
      deRegisterForVald(children[i]);
    }
  }
 }
 return;
}

function resetFields(div){
 var childNodes = div.getElementsByTagName("input");
 var i = 0;
 for(i=0;i<childNodes.length;i++){
     if(childNodes[i].type == 'text')
     {
        childNodes[i].value = "";
     }
     else if((childNodes[i].type == 'radio') || (childNodes[i].type == 'checkbox'))
     {
        childNodes[i].checked = false;
     }
 }
}

function setAttribute(nodes,attrib,value){
 var i = 0;
 for(i=0;i<nodes.length;i++){
	nodes[i].setAttribute(attrib,value);
 }
}

var toggleClicked = "no";
function processGrpFields(divObj) {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
     //alert("data= " + data);
    var xml;
    if(window.ActiveXObject) {
      xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(data);
    } else if(document.implementation 
              && document.implementation.createDocument) {
      // code for Mozilla, Firefox, Opera, etc.
      var parser = new DOMParser();
      xml = parser.parseFromString(data, 'text/xml');
    }
    var confNodes = selectNodeArry("/root/Config/record", xml);
    if(null != confNodes && confNodes.length > 0) {
      var childNodes = divObj.getElementsByTagName('*');
      for (var i=0;i<childNodes.length;i++) {
        //alert("childNodes[i].nodeName= " + childNodes[i].nodeName);
        if(childNodes[i].nodeName == 'INPUT' && childNodes[i].type == 'text')
        {
          var index = childNodes[i].name.indexOf("#");
          var nvName = childNodes[i].name.substring(0, index);
          var expstr = "/root/Config/record/param[@name=\'" + nvName + "\']/@value";
          var curVal = selectNodeStrVal(expstr, xml);
          //alert("childNodes[i].name= " + childNodes[i].name);
          childNodes[i].value = curVal;
          //alert("childNodes[i].value= " + childNodes[i].value);
        } else if(childNodes[i].nodeName == 'SELECT' && childNodes[i].type == 'select-one') {
          /*Need to verify for select-multiple*/
          var index = childNodes[i].name.indexOf("#");
          var nvName = childNodes[i].name.substring(0, index);
          var expstr = "/root/Config/record/param[@name=\'" + nvName + "\']/@value";
          var curVal = selectNodeStrVal(expstr, xml);
          //alert("curVal= " + curVal);
          childNodes[i].setAttribute('curval',curVal);
          /*alert("childNodes[i].name= " + childNodes[i].name 
                                       + " childNodes[i].type= " 
                                       + childNodes[i].type 
                                       + " childNodes[i].getAttribute('curval')= " 
                                       + childNodes[i].getAttribute('curval'));*/
          var opts = childNodes[i].options;
          for(var x = 0; x < opts.length; x++) {
            if(opts[x].value == curVal) {
              opts[x].selected = true;
              break;
            }
          } //for
        } else if((childNodes[i].nodeName == 'INPUT')
                    && ((childNodes[i].type == 'radio') 
                         || (childNodes[i].type == 'checkbox'))) {
          var index = childNodes[i].name.indexOf("#");
          var nvName = childNodes[i].name.substring(0, index);
          var expstr = "/root/Config/record/param[@name=\'" + nvName + "\']/@value";
          var curVal = selectNodeStrVal(expstr, xml);
          //alert("childNodes[i].name= " + childNodes[i].name + "\t childNodes[i].value= " + childNodes[i].value);
          //alert("curVal= " + curVal);
          if(curVal != '' && curVal == childNodes[i].value)
            childNodes[i].checked = true;
          else
            childNodes[i].checked = false;
        } // if radio or checkbox
      } //for
    } else {      
        divObj.style.display = "none";
        setAttribute(divObj.getElementsByTagName("input"),'visible','false');
        setAttribute(divObj.getElementsByTagName("select"),'visible','false');     
    } // if
    return(true);
  } // readyState==4
} //processGrpFields

function populateGrpFields(divObj, grppath, inst) {
  //alert("populateGrpFields:: grppath= " +grppath);
  var opCode = getOpCode("BYKEY");
  var base_url = setCookieInfo(grppath,'','','',inst,opCode);
  base_url += "&reqtype=GET";
  base_url += "&host=" + getHostID();
  //alert("populateGrpFields:: base_url= " +base_url);
  sendRequestToServerEx('ucmreq.igw', 'POST', base_url, processGrpFields, divObj);
}

function toggleIcon(id, displayFlag) {
  //alert("id=" + id);  
  var myIcon = get(id);  
  //alert(myIcon.src);  
  if (displayFlag == "block") {      
    myIcon.src = "images/Lminus.gif";
  } else {    
    myIcon.src = "images/Lplus.gif";
  } 
}

// Toggle Form div
function toggleContent(objId, isClicked) {
  toggleClicked = isClicked;
  var div1 = document.getElementById(objId);
  if (div1) {
    var grppath = div1.getAttribute("grppath");
    var pkeys  = div1.getAttribute("parentKeys");
    var inst  = div1.getAttribute("inst");
    var clr  = div1.getAttribute("clr");//alert(pkeys);

    //Added condition for not calling GetRecords while adding new record
    if((grppath != '') && (toggleClicked == "no") && pkeys != null) 
    {
      div1.style.display = "block";
      setAttribute(div1.getElementsByTagName("input"),'visible','true');
      setAttribute(div1.getElementsByTagName("select"),'visible','true');
      populateGrpFields(div1, procParentKeys(grppath,pkeys), inst);
    } //if 
    else {//manual click on the link; toggleClicked == "yes"
     if (div1.style.display == 'none') {
      div1.style.display = "block";
      setAttribute(div1.getElementsByTagName("input"),'visible','true');
      setAttribute(div1.getElementsByTagName("select"),'visible','true');
     } else {
      div1.style.display = "none"; 
      setAttribute(div1.getElementsByTagName("input"),'visible','false');
      setAttribute(div1.getElementsByTagName("select"),'visible','false');
     //setAttribute(div1.getElementsByTagName("div"),'style','display:none');
       if(clr != "no") {
        resetFields(div1); // clear all field values
       }
     }
    }
    if (objId != "" && pkeys != null) { toggleIcon("icon"+objId, div1.style.display); }
  }
}

function toggle(id, closed, opened, pre) {
    //alert(" id ===  " + id);
  var myChild = get(id);
  var myPIcon = get("icon" + id);
  var myIcon = get("icon" + id);

  if (myChild.style.display == "none") {
    myChild.style.display = "block";
    myIcon.src = opened;
    myPIcon.src = "images/" + pre + "minus.gif";
  } else {
    myChild.style.display = "none";
    myIcon.src = closed;
    myPIcon.src = "images/" + pre + "plus.gif";
  }
}

function get(id) {
  if (document.all) {
    return document.all[id];
  } else {
    return document.getElementById(id);
  }
}

function copyChildNodeValues(sourceNode, targetNode) {
  for ( var i = 0; i < sourceNode.childNodes.length; i++) {
        try{
                targetNode.childNodes[i].value = sourceNode.childNodes[i].value;
        }catch(e){
        }
  }
}

function validateString(val){
  alert("In validate " + val );
}
var confType = "";
function tblInit(cnfType) {
   //alert("cnfType ==>" +cnfType);
   confType = cnfType;  
}

// To populate the key value pairs to be sent to the server.
function populateKeyPairs(cell, delim, eopr) {
  var keyLstStr = "";
  var keyElements = cell.getElementsByTagName("keys");
  for(i = 0; i < keyElements.length; i++) {
    keyName  = keyElements[i].getAttribute("name");
    keyType  = keyElements[i].getAttribute("type");
    keyValue = keyElements[i].getAttribute("value");

    /*alert("Key Name = "+ keyName
                    + " \n Key Type = "+ keyType
                    + " \n Key Value = "+ keyValue);*/

    keyLstStr += keyName + delim + keyType + eopr + keyValue + "|";
  }
  return keyLstStr;
} // populateKeyPairs

var gdmPath="";
function deleteRecord(cell, dmpath, mtpath, spg, fpg, cookie) {
  var dmPath="";    
  var opcode ="DEL";
  var reqtyp = cell.getAttribute("reqtyp");
  //alert("deleteRecord::reqtyp= " + reqtyp);
  var cell = cell.parentNode.parentNode;
  var parentTxStr = "";
  var links = cell.getElementsByTagName("a");
  delRecord = cell.rowIndex + ":" + cell.parentNode.parentNode.id;
  for(var i = 0; i < links.length; i++) {
    var ldivatrb = links[i].getAttribute("ldivid");
    if(ldivatrb) {
      //alert("ldivatrb = " +ldivatrb);
      var chldDiv = window.top.frameBottom.rightPanel.document.getElementById(ldivatrb); 
      if(chldDiv) {
        var exp = "//div[@id='" +ldivatrb+ "']//div[contains(@id,'MetaData')]";
        //alert("exp= " + exp);
        var childDivs = selectNodeArry(exp, window.top.frameBottom.rightPanel.document);
        //alert("childDivs.length= " + childDivs.length);
        for(var j = 0; j < childDivs.length; j++) {
          var tcdiv = childDivs[j].getAttribute("id");
          //alert("child ldivid= " + tcdiv);
          window.top.frameBottom.rightPanel.document.getElementById(tcdiv).style.display= "none";
        } // for
        chldDiv.style.display = "none";
      } // if
    } // if
  } // for
  var keys = populateKeyPairs(cell, "#", '=');
  var temp = dmpath;
  if(temp.indexOf("{0}") > -1){
    var tokens = temp.split("{0}");
    if('' != getCookie(cookie)){
      dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
    }
  }
  if("" == dmPath){
    dmPath = temp;
  } 
  gdmPath = dmPath;
  //alert("confType = " + confType);
  //alert("gdmPath = " + gdmPath);
  if(gdmPath.charAt(gdmPath.length - 1) == '}' && confType == 'roleset'){
    opcode = "IDELROL";
  }
  if(confType == 'roleset') {
    if(gdmPath.charAt(gdmPath.length - 1) == '}' && confType == 'roleset'){
      opcode = "IDELROL";
    } else {
      opcode = "DELROL";
    }
  }
  var f = document.forms[0];
  for(var y = 0; y < f.elements.length; y++){
    if(f.elements[y].name=="useparenttx")
    {
      parentTxStr = f.elements[y].value;
    }
  }
  if(reqtyp != null && reqtyp == "DONE") {    
    var instanceStr="";
    var keyElements = cell.getElementsByTagName("keys");//alert(keyElements.length);
    for(var k=0; k<keyElements.length; k++) {
       kName  = keyElements[k].getAttribute("name");
       kType  = keyElements[k].getAttribute("type");
       kValue = keyElements[k].getAttribute("value");
       /*alert("Key Name = "+ kName
                    + " \n Key Type = "+ kType
                    + " \n Key Value = "+ kValue);*/
       if(keyElements.length>1) {
          instanceStr += kName + "=" + kValue + ",";
          continue;
       } else {
          instanceStr = kValue + ",";
          break;
       }       
    } 
    dmPath +="{" + instanceStr.substring(0,instanceStr.length - 1) + "}";   
    //alert("dmPath= " +dmPath);    
  }
  if(confType != 'roleset') {
   // Here '!' and '^' are used for delemeters
   // to separete opcode and keys
   keys = "ucdm_" + dmPath + "!DEL^" + keys;
   if(parentTxStr != '') {
     //alert("parentTxStr= " + parentTxStr);
     keys = parentTxStr + "^" + keys;
     opcode = "MULTCMD";
   }
  }
  var params = setCookieInfo(dmPath,mtpath, spg, keys, '',opcode);
  params += "&host=" + getHostID();
  if(reqtyp != null && reqtyp == "DONE") {
    params += "&reqtype=" + reqtyp;
  }
  //alert("deleteRecord:: params= " +params);
  sendRequestToServerEx('ucmreq.igw', 'POST', params, processResp, params);
}

function greyForSave() {
  var greyTopPanelDiv = this.parent.parent.frameTop.toolbar.document.createElement("div");
  greyTopPanelDiv.className = "greyOutDiv";
  greyTopPanelDiv.id = "greyTopPanelDiv";
  this.parent.parent.frameTop.toolbar.document.body.appendChild(greyTopPanelDiv);

  var greyLeftPanelDiv = parent.parent.frameBottom.leftPanel.document.createElement("div");
                 greyLeftPanelDiv.className = "greyOutDiv";
                 greyLeftPanelDiv.id = "greyLeftPanelDiv";
                 parent.parent.frameBottom.leftPanel.document.body.appendChild(greyLeftPanelDiv);
                 parent.parent.frameBottom.leftPanel.document.getElementById("greyLeftPanelDiv").style.height = parent.parent.frameBottom.leftPanel.document.getElementById("screenHeightDiv").offsetTop + 30 + "px"; 

              var greyRightPanelDiv = parent.parent.frameBottom.rightPanel.document.createElement("div");
              greyRightPanelDiv.className = "greyOutDiv";
              greyRightPanelDiv.id = "greyRightPanelDiv";              
              parent.parent.frameBottom.rightPanel.document.body.appendChild(greyRightPanelDiv);
              parent.parent.frameBottom.rightPanel.document.getElementById("greyRightPanelDiv").style.height = parent.parent.frameBottom.rightPanel.document.getElementById("screenHeightDiv").offsetTop + 50 + "px";

  var pDiv = this.parent.parent.frameTop.toolbar.document.createElement("div");
  pDiv.className = "progressDiv";
  pDiv.id = "progressPanelDiv";  
  this.parent.parent.frameTop.toolbar.document.body.appendChild(pDiv);
pDiv.innerHTML = "<table border='0'><tr><td height='40' valign='middle'>&nbsp;&nbsp;<img src='images/loading.gif' align='absmiddle'><font face='Verdana, Arial, Helvetica, sans-serif' size='2' color='#000'>&nbsp;&nbsp;<b>Please wait while 'Save' is in progress...</b></font></td></tr></table>";
}

function saveAndLogout(flg) {
   logoutFlag = flg;
   greyForSave();
    if (defaultHostType != null && defaultHostType == "SingleDevice") {
        setTimeout("submitFormRequest('', 'SAVE', 'igd');", 250);
    } else {
      var hostname = getHostID();
      if(hostname == "localhost") {
          submitFormRequest('', 'SAVE', 'mgmt');
      } else {
          submitFormRequest('', 'SAVE', 'igd');
      } 
    }  
}

function save() {
  var cfmSave = confirm("Do you wish to save the configuration");
  if(true == cfmSave){     
    greyForSave();
    if (defaultHostType != null && defaultHostType == "SingleDevice") {
        setTimeout("submitFormRequest('', 'SAVE', 'igd');", 250);
    } else {
       var hostname = getHostID();
       if (hostname == "localhost") {
          setTimeout("submitFormRequest('', 'SAVE', 'mgmt');", 250); 
       } else {
          setTimeout("submitFormRequest('', 'SAVE', 'igd');", 250);
       }   
    }
  }else{
      return;
  }
}

function factoryReset(){
  submitFormRequest('', 'FACRES', 'igd');
}
//function to get selected host
function  getSelectedHost () {
  return getCookie("Name-DIVMIP");
}

function setInstance(cell,snetName, timeout){
  //FIXME : if host is different may require to reset VSG cookie
  //alert("setInstance:: snetName= " +snetName);
  var keyValue = "";
  cell = cell.parentNode.parentNode;
  
  var keyElements = cell.getElementsByTagName("keys");
  if(keyElements.length > 1) {
    for(i = 0; i < keyElements.length; i++) {
      keyValue += keyElements[i].getAttribute("name");
      keyValue += "=";
      keyValue += keyElements[i].getAttribute("value");
      keyValue += ",";
    }
  } else {
    keyValue += keyElements[0].getAttribute("value");
  }
  if(keyValue.charAt(keyValue.length - 1) == ",") {
    keyValue = keyValue.substring( 0, keyValue.length - 1 );
  }
  if (snetName == "Name-SNET") {
     alert("VSG '" + keyValue + "' is selected");
  }
  if (snetName == "Name-DIVMIP") {
     alert("Device '" + keyValue + "' is selected");
  }
  setCookie(snetName,keyValue,timeout);
  //parent.rightPanel.location.reload();
   parent.parent.frameTop.location.reload();
}

function selectMasterRadioBtn(cookie, radioBtn) { 
  /*alert("radioBtn= " + radioBtn 
                     + "\t cookie= " + cookie);*/
  var radBtn = document.getElementsByName(radioBtn);
  if('' != cookie){
    for(i = 0; i < radBtn.length; i++) {
      var val = radBtn[i].value;
      if(val.charAt(val.length -1) == ",") {
        val = val.substring(0, val.length - 1);
      }
      //alert("radBtn[i].value" + val);
      if(val == cookie)
        radBtn[i].checked = true;
      else
        radBtn[i].checked = false;
    }
  }
}

function updateRadioBtn(cookie, radioBtn){
  var cookie = getCookie(cookie);
  /*alert("radioBtn= " + radioBtn 
                     + "\t cookie= " + cookie);*/
  var radBtn = document.getElementsByName(radioBtn);
  if('' != cookie){
    for(i = 0; i < radBtn.length; i++) {
      var val = radBtn[i].value;
      if(val.charAt(val.length -1) == ",") {
        val = val.substring(0, val.length - 1);
      }
      //alert("radBtn[i].value" + val);
      if(val == cookie)
        radBtn[i].checked = true;
      else
        radBtn[i].checked = false;
    }
  }
}

function processModifiedRecords() {
  var iframes = document.getElementsByTagName("iframe");
  var isModified = "";
  //alert(iframes.length);
  for(var l = 0; l < iframes.length; l++) {
    var frm;
    if(iframes[l].contentDocument) {
      frm = iframes[l].contentDocument.getElementById("main-form");
      //alert("iframes[l].contentDocument::frm= " + frm.modifiedRecords.value);
    } else if(iframes[l].contentWindow){
      frm = iframes[l].contentWindow.document.getElementById("main-form");
      //alert("iframes[l].contentDocument::frm= " + frm.modifiedRecords.value);
    } // else if
    isModified = frm.modifiedRecords.value;
    if(isModified == "true") {
       break;
    }
  } // for
  //alert("processModifiedRecords:: isModified = " + isModified);
  return isModified;
} // processModifiedRecords

/**
*  Back button functionality is implemented using history object.
*/
function back(){
  var parntFrm;
  var applyOpt;
  var applyBtn = document.getElementById("ApplyBtn");
  if(applyBtn != null) {
    var applyOpt = applyBtn.getAttribute("applyOpt");
    //alert("applyOpt= " + applyOpt);
    if(applyOpt == "COMMIT" ) {
      var modRec = processModifiedRecords();
      if(modRec == "true") {
        var cnfMsg = "Record(s) has been modified," 
                     + " are you sure to Cancel the changes?"
                     + " Click on OK to undo/ignore the changes.";
        var cfm = confirm(cnfMsg);
        if (true == cfm) {
           var f = document.forms[0];
           //alert("REVOKE for = " + f.dmpath.value);
           submitFormRequest('', 'REVOKE', f.dmpath.value);
        } else {
           return;
        }
      } //if
    } //if COMMIT
  }
  if (this.window.name.indexOf("newW") == -1) {
    if(window.top.frameBottom.rightPanel.ifrmObj) {
      parntFrm =  window.top.frameBottom.rightPanel.ifrmObj.document.getElementById('main-form');
    } else {
      parntFrm =  parent.document.getElementById("main-form");
    }
  }
  if (this.window.name.indexOf("newW") != -1) {
    //window.close(); IE... throwing confirmation box; using below code to avoid it
    /*var winObj = window.self;
    winObj.opener = window.self;
    winObj.close();*/
    removeGreyDiv();
    window.open('','_parent','');
    window.close();
  } else if(null != parntFrm) {
    removeAllGreyDivs();
    var tblid = parntFrm.tblID.value;
    //alert("parntFrm.tblID.value = " + tblid 
    // + " \t parntFrm.ifrmID.value= " + parntFrm.ifrmID.value);
    var frmName;
    if(this.name && this.name !="") {
      frmName = this.name;
    } else {
      frmName = tblid + "_addedit";
    }
    var addEditDiv;
    if(frmName.indexOf("_ifrm") != -1) {
      addEditDiv = frmName.substring(0, frmName.indexOf("_ifrm"));
    } else {
      addEditDiv = frmName;
    }
    //alert("addEditDiv= " + addEditDiv);
    var aeDiv = parent.document.getElementById(addEditDiv);
    if(!aeDiv) {
      aeDiv = parent.document.getElementById(parntFrm.ifrmID.value);
    }
    parent.document.body.removeChild(aeDiv);
  } else {
    history.back();
  }
}

function breadcrumb(path,cookie, dmpath){    
    var dmPath=""; 
    if(dmpath.indexOf("{0}") > -1){
      var tokens = dmpath.split("{0}");    
      if(('' != cookie) || ('' != getCookie(cookie))){
        dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
      }else{
        dmPath = dmpath; 
      }
      document.getElementById('ovsg').innerHTML = dmPath;
    }else{
       document.getElementById('ovsg').innerHTML = dmpath;
    }
} 

function applName(brdcmb){
   
   var tokens = brdcmb.split(">");
   document.getElementById('appln').innerHTML = tokens[tokens.length - 1];
}

function getCheckedValue(inputName)
{
 var  fo = document.forms[0];
 var checkedVal;
 for(k = 0; k < fo.elements.length; k++){
    if(fo.elements[k].type == "radio" && fo.elements[k].name == inputName) {
      if(fo.elements[k].checked){
        checkedVal = fo.elements[k].value;
        break;
      }
    }
 }
 return checkedVal;
}

function setRoleValue(dmpath, cookie,rolemtfile){
  var tOpcode = getOpCode("GETROLE");
    var dmPath = "";
    if(dmpath.indexOf("{0}") > -1){
      var tokens = dmpath.split("{0}");    
      if('' != cookie && '' != getCookie(cookie)){
        dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
      }
    }
    if("" == dmPath){
        dmPath = dmpath; 
    }
    if(dmPath.indexOf("{0}") > -1 &&  dmPath.charAt(dmPath.length - 1) == '}')
    {
      dmPath = dmPath.substring(0, dmPath.length - 3);
    }
    //url = "ucm?dmpath=" + dmPath + "&mtpath="+rolemtfile;     
    if(dmPath.charAt(dmPath.length - 1) == '}'){
      var tOpcode = getOpCode("IGETROLE");
      //url = url + "&opcode=" + tOpcode + "&sredirect=tbl_meta.xsl";
    }else {
      var tOpcode = getOpCode("GETROLE");
      //url = url + "&opcode=" + tOpcode + "&sredirect=tbl_meta.xsl";
    }
    //alert("setRoleValue:: url=" + url);

    /*if(dmpath == 'igd' && '' == cookie){ 
      parent.parent.frameBottom.rightPanel.location.href = url;
    }else{
      location.href = url;  
    }*/
    sumbitFrmReq(dmPath, tOpcode, rolemtfile, 'tbl_meta.xsl', '', '', "GET", '', '');
   return;
}

function setCookieValue(fqn, opcode, mtpath, spg, keys, cookie, keyid, target) {
    //alert("setCookieValue::fqn= " + fqn + " ::keys= " + keys);
    //alert("setCookieValue::getParentTxKeys= " + getParentTxKeys());
    var keyPair = "";
    var sParentTxKeys = getParentTxKeys();
    var vrInstPath = getVertualInstancePath();
    if(errorFlag == "TRUE") {
       return;
    }
    if(sParentTxKeys && sParentTxKeys != "") {
      fqn  = processParentKeys(fqn, sParentTxKeys);
      //fqn = sParentTxKeys;
    }
    //alert("setCookieValue::getVertualInstancePath()= " + vrInstPath);
    var hostVal = getHostID();
    //alert("setCookieValue::getHostID()= " + hostVal);

    if("" == fqn) {
      return;
    }
    var dmPath="";
    var tOpcode = "";
    if(fqn.indexOf("{0}") != -1){
      var tokens = fqn.split("{0}");
      if('' != getCookie(cookie)){
        dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
      }
    }
    if("" == dmPath){
        dmPath = fqn;
    }     
    if(dmPath.indexOf("{0}") > -1 &&  dmPath.charAt(dmPath.length - 1) == '}')
    {
      dmPath = dmPath.substring(0, dmPath.length - 3);
    }
    if(keys) {
      if(keys.charAt(keys.length - 1) == '|') {
        keys = keys.substring(0, keys.length - 1);
      }
      var currentTokens = keys.split("|");
      if(currentTokens.length > 1) {
        for ( var i = 0; i < currentTokens.length; i++ ) {
          var curTok = currentTokens[ i ];
          if((curTok.indexOf("^") != -1) 
                       && (curTok.indexOf("nil") == -1)) {
            keyPair += curTok.substring( 0, curTok.indexOf('^')) 
                   + "=" 
                   + curTok.substring( curTok.indexOf('!') +1, curTok.length)
                   + ",";
          }
        }
      } else {
        var curTok = keys;
        keyPair = curTok.substring( curTok.indexOf('!') +1, curTok.length);
        if(keyPair == 'nil') {
          keyPair = '';
        }
       
      }
      if(keyPair.charAt(keyPair.length -1) == ',')
      {
         keyPair = keyPair.substring(0,keyPair.lastIndexOf(","));
      }
      if (keyid != "") {
        if(dmPath.charAt(dmPath.length - 1) != '}' && keyPair!=""){
          dmPath = dmPath + '{' +  keyPair  +'}';
        }
      }
    } else {
      keys = "";
    }
    //alert("keyid = " + keyid + "\t keyid.length= " +keyid.length);
    if(keyid && keyid.length != 0) {//alert("here");
      dmPath +="."+keyid;
    }
    if(opcode != '') {
      if(opcode == 'ADD' || opcode== 'LOGS') {
        tOpcode = 1;
      }else if(opcode== 'BYKEY') {
        tOpcode = getOpCode(opcode);
      
      }else {
        tOpcode = opcode;
      }
    } 
    if((mtpath == 'ROLE_MetaData.xml' || mtpath == 'AROLE_MetaData.xml') 
                                                        && (opcode != 1)){
      if(dmPath.charAt(dmPath.length - 1) == '}'){
        tOpcode = getOpCode("IGETROLE");
      }else {
        tOpcode = getOpCode("GETROLE");
      }
    }
    // Add virtual instance value for Anchor paths if applicable.
    if(dmPath.indexOf("{")== -1 && dmPath.indexOf("}")== -1 && mtpath != 'VSG_MetaData.xml') {
      var vrIndex = dmPath.indexOf(vrInstPath);
      if(vrIndex != -1 && getCookie(cookie) != "" && vrInstPath !="" &&  dmPath != vrInstPath) {
         var tmpDmp = dmPath.substring(0, vrInstPath.length);
         tmpDmp += "{" + getCookie(cookie) + "}" 
                   +  dmPath.substring(vrInstPath.length, dmPath.length);
         dmPath = tmpDmp;
      }
    }
    if((dmPath.indexOf("{") != -1) 
           && (vrInstPath != "" && getCookie(cookie) == "")) {
      alert("Please select a \"" + getVertualInstanceName() +"\" instance.");
      return;
    }
    opcode = tOpcode;
    //alert("finally dmPath= " + dmPath);
    //if (dmPath.indexOf("mesgclnt.throttling.") != -1) keys = "family" + keys;//alert(keys);
    sumbitFrmReq(dmPath, opcode, mtpath, spg, '', keys, "GET", target, keys);
}

function UCMReq(dmpath, opcode, mtpath, sredirect, fredirect, ucmreq, keyprms, reqtype, host) {
  this.dmpath = dmpath;
  this.opcode = opcode;
  this.mtpath = mtpath;
  this.sredirect = sredirect;
  this.fredirect = fredirect;
  this.ucmreq = ucmreq;
  this.keyprms = keyprms;
  this.reqtype = reqtype;
  this.host = host;
}

function getUCMReqObj(frmObj) {
  ucmReqObj = new UCMReq(frmObj.dmpath.value, frmObj.opcode.value, frmObj.mtpath.value,
                         frmObj.sredirect.value, frmObj.fredirect.value, 
                         frmObj.ucmreq.value, frmObj.keyprms.value, frmObj.reqtype.value,
                         frmObj.host.value);
  return ucmReqObj;
}

function getUCMReqObjDirect() { 
  return ucmReqObj;
}

padedfrm = "no";
//pAdEdiFrm = this;
var pAdEdiFrm = this;

function sumbitFrmReq(dmPath, opcode, mtpath, spg, fpg, 
                          keys, reqType, tgt, keyprms) {
  var frm;
  if(!parent.parent.frameBottom) {
    frm = document.getElementById('tbl-form');
  } else {
    frm = 
         parent.parent.frameBottom.rightPanel.document.getElementById('tbl-form');
  }
  frm.dmpath.value = dmPath;
  frm.opcode.value = opcode;
  frm.mtpath.value = mtpath;
  frm.sredirect.value = spg;
  frm.fredirect.value = fpg;
  frm.ucmreq.value = keys;
  frm.keyprms.value = keyprms;
  frm.reqtype.value = reqType;
  frm.host.value = getHostID();

  //alert("tgt= " + tgt + " window.name= "+ window.name);
  if(tgt == "ldiv") {
    var url = "redirectifrm.htm";
    var ifrmid = getIfrmID();
    //alert("opcode = " + opcode);
    var ifrm;
    /*if(ifrmid.indexOf("_addedit") != -1) {
      greyOutFrames(window.top.frameBottom.rightPanel.document);
    }*/
    if(ifrmid.indexOf("_addedit") != -1 && parent.document.getElementById('MouseY')) {
      //ifrm = parent.document.getElementById(ifrmid);
      ifrm = window.top.frameBottom.rightPanel.document.getElementById(ifrmid);
      var pform = parent.document.getElementById("main-form");
      //alert("GETTING ::pform.ifrmID.value= " 
       //+ pform.ifrmID.value + "\t CUR ifrm ID= " + this.name);
      window.top.frameBottom.rightPanel.padedfrm = this.name;
      window.top.frameBottom.rightPanel.ifrmObj = this;
      window.top.frameBottom.rightPanel.pAdEdiFrm = getUCMReqObj(frm);
    } else {
      ifrm = window.top.frameBottom.rightPanel.document.getElementById(ifrmid);
      var pform = document.getElementById("main-form");
      pform.ifrmID.value = ifrmid;
      window.top.frameBottom.rightPanel.ifrmID = ifrmid;
      window.top.frameBottom.rightPanel.padedfrm = "no";
      window.top.frameBottom.rightPanel.pAdEdiFrm = getUCMReqObj(frm);
      //alert("SETTING ::pform.ifrmID.value= " + pform.ifrmID.value);
    }
    ifrm.src = url;
  } else if(tgt == "new") {
    var url = "redirect.htm";
    var greyRightPanelDiv = null;
    var greyLeftPanelDiv = null;
    var greyTopPanelDiv = null;
    var winWidth = 680, winHeight = 550;
    var relScreenX = 0, relScreenY = 0;
    var newWindowName = "";
    if (window.name == "rightPanel") {
      newWindowName = "newW";
      greyLeftPanelDiv = parent.parent.frameBottom.leftPanel.document.createElement("div");
      greyLeftPanelDiv.className = "greyOutDiv";
      greyLeftPanelDiv.id = "greyLeftPanelDiv";
      parent.parent.frameBottom.leftPanel.document.body.appendChild(greyLeftPanelDiv);

      greyTopPanelDiv = parent.parent.frameTop.toolbar.document.createElement("div");
      greyTopPanelDiv.className = "greyOutDiv";
      greyTopPanelDiv.id = "greyTopPanelDiv";
      parent.parent.frameTop.toolbar.document.body.appendChild(greyTopPanelDiv);
      parent.parent.frameBottom.
           leftPanel.document.getElementById("greyLeftPanelDiv").style.height =
                     parent.parent.frameBottom.leftPanel.document.
                            getElementById("screenHeightDiv").offsetTop 
                                                              + 30 + "px";
    } else if (window.name == "newW") {
      newWindowName = "newW1";
      relScreenX = 10, relScreenY = 24;
    } else {
      var parentWinName = window.name;//alert("window.name: " + window.name)
      if(parentWinName.indexOf("ifrm") != -1) {
         parentWinName = parent.window.name;
         //alert("parentWinName= " + parentWinName);
      }
      if (parentWinName.indexOf("newW") != -1 && parentWinName.length >= 4) {
        var indexStr;
        if (parentWinName == "newW") {
            indexStr = "0";
        } else {
            indexStr = parentWinName.substring(4, parentWinName.length);
        }
        var index = parseInt(indexStr);//alert("indexStr: " + indexStr);
        index++;
        newWindowName = "newW" + index;
        relScreenX = index * 10, relScreenY = index * 24;
      }
    } //alert("newWindowName= " + newWindowName);
      greyRightPanelDiv = document.createElement("div");
      greyRightPanelDiv.className = "greyOutDiv";
      greyRightPanelDiv.id = "greyRightPanelDiv";
      document.body.appendChild(greyRightPanelDiv);
      document.getElementById("greyRightPanelDiv").style.height = 
              document.getElementById("screenHeightDiv").offsetTop + 50 + "px";
      var winOpen = window.open(url, newWindowName, 
          "width=680, height=550, toolbar=0, resize=1, scrollbars=1, status=0, location=0, menubar=0");
      //resizable=yes,menubar=yes,scrollbars=yes,toolbar=no,status=yes,location=yes
      //winOpen.resizeTo(winWidth, winHeight);
      winOpen.moveTo((screen.width/2-winWidth/2) + relScreenX, (screen.height/2-winHeight/2) + relScreenY);
      return;
    } else {
      frm.submit();
    }
} // sumbitFrmReq

var prevRowIndex = 0;
function toggleLdivs(cell) {
  //alert("toggleLdivs::cell.rowIndex= " +cell.rowIndex + " \t prevRowIndex= " + prevRowIndex);
  if(prevRowIndex == 0 || prevRowIndex != cell.rowIndex) {
     prevRowIndex = cell.rowIndex;
  } else {
    return;
  }
  var links = cell.getElementsByTagName("a");

  for(var i = 0; i < links.length; i++) {
    var ldivatrb = links[i].getAttribute("ldivid");
    if(ldivatrb) {
      //alert("ldivatrb = " +ldivatrb);
      var chldDiv = window.top.frameBottom.rightPanel.document.getElementById(ldivatrb); 
      if(chldDiv) {
        var exp = "//div[@id='" +ldivatrb+ "']//div[contains(@id,'MetaData')]";
        //alert("exp= " + exp);
        var childDivs = selectNodeArry(exp, window.top.frameBottom.rightPanel.document);
        //alert("childDivs.length= " + childDivs.length);
        for(var j = 0; j < childDivs.length; j++) {
          var tcdiv = childDivs[j].getAttribute("id");
          //alert("child ldivid= " + tcdiv);
          window.top.frameBottom.rightPanel.document.getElementById(tcdiv).style.display= "none";
        } // for
        chldDiv.style.display = "none";
      } // if
    } // if
  } // for
} // toggleLdivs

function greyOutFrames(doc) {
  var greyRightPanelDiv = null;

  // Disable Right Frame.
  greyRightPanelDiv = doc.createElement("div");
  greyRightPanelDiv.className = "greyOutDiv";
  greyRightPanelDiv.id = "greyRightPanelDiv";
  doc.body.appendChild(greyRightPanelDiv);
  doc.getElementById("greyRightPanelDiv").style.height = 
              doc.getElementById("screenHeightDiv").offsetTop + "px";
              //doc.getElementById("screenHeightDiv").offsetTop + 50 + "px";
} // greyOutFrames

function setCookieInfo(fqn,mtpath,spg,keys,cookiename,opcode) {
    var dmPath="";
    if(fqn.indexOf("{0}") > -1){
      var tokens = fqn.split("{0}");    
      if((cookiename) && ('' != getCookie(cookiename))){
        dmPath = tokens[0] + "{" + getCookie(cookiename) + "}" + tokens[1];
      }
    }
    if("" == dmPath){
        dmPath = fqn;
    }
    var url = "dmpath=" + dmPath + "&mtpath=" + mtpath + "&sredirect=" 
              + spg + "&opcode=" + opcode;
    if(keys) {
      url = url + "&ucmreq=" + keys;
      //alert("url:" + url);
    }
    return(url); 
}

function submitFormRequest(cookie, cmd, rtPath) {
  var ucmreq = "";
  //alert("submitFormRequest:: rtPath= " + rtPath);
  var f;
  if (document.getElementById('tbl-form')) {
    f= document.getElementById('tbl-form');
  } else {
    f= parent.parent.frameBottom.rightPanel.document.getElementById('tbl-form');
  }
  //var  f= document.getElementById('tbl-form');
  for(i = 0; i < f.elements.length; i++){
   if(f.elements[i].name == "dmpath") {
       var temp = rtPath;
       if(temp.indexOf("{0}") > -1) {
         var tokens = temp.split("{0}");
         if('' != getCookie(cookie)){
           var dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
           f.elements[i].value = dmPath;
         }
       } else {
         f.elements[i].value = rtPath;
       }
     //alert("dmpath== " + f.elements[i].value);
   }
   if(f.elements[i].name == "ucmreq"){
     f.elements[i].value = ucmreq;
   }
   if(f.elements[i].name == "sredirect"){
     f.elements[i].value = 'tbl_meta.xsl';
   }
   if(f.elements[i].name == "fredirect"){
     f.elements[i].value = 'resp_meta.xsl';
   }
   if(f.elements[i].name == "opcode"){
     f.elements[i].value = cmd;
   }
   if(f.elements[i].name == "host"){
     f.elements[i].value = getHostID();
   }
  }
  if(cmd == "REVOKE") {
    f.reqtype.value = cmd;
  } else {
    f.reqtype.value = "";
  }
  submitFrmData(f);
}
// function to sumbit form request from the link or button
function submitLinkOrBtnReq(linkObj, cookie, tblID) {
   var dmpath = linkObj.getAttribute("dmpath");
   var key = linkObj.getAttribute("key");
   var value = linkObj.getAttribute("val");
   var mtpath = linkObj.getAttribute("mtpath");
   var opt = linkObj.getAttribute("opcode");
   actionFlag = linkObj.getAttribute("action");
   var frm;
   var pkeys; 
   var bStats = "FALSE";
   var  f= document.getElementById('tbl-form');
   //alert("submitLinkOrBtnReq:: opt = " + opt + "  tblID= " + tblID + " actionFlag= " + actionFlag);
   if (opt == "AGGR" || opt == "AVG" || opt == "PERDEV") {
     bStats = "TRUE";
   }
   if (bStats == "TRUE" && actionFlag == "STATS") {
     //getStatsData(tblID, opt);
     refreshTable(tblID, opt);
     return;
   }

   if (window.name.indexOf("newW") != -1) {
     frm = window.opener.document.getElementById('tbl-form');
   } else {
     frm = f;
   }
   pkeys = frm.keyprms.value;
   //alert("pkeys= " + pkeys);
   if(pkeys) {
   dmpath = processParentKeys(dmpath, pkeys);
   }
   //alert("dmpath= " + dmpath + " key= " + key + " value= " + value);
   var temp = dmpath;
   if(temp.indexOf("{0}") != -1) {
     var tokens = temp.split("{0}");
     if('' != getCookie(cookie)){
       var dmPath = tokens[0] + "{" + getCookie(cookie) + "}" + tokens[1];
       f.dmpath.value = dmPath;
     }
   } else {
     f.dmpath.value = dmpath;
   }
   //alert("f.dmpath.value= " + f.dmpath.value);
   //f.ucmreq.value = "ucdm_" + f.dmpath.value +"!EDIT^" +key.replace('+','#') + "=" + value;
   if(null != opt && opt != '') {
     f.opcode.value = opt;
   } else {
   f.opcode.value = "EDIT";
   }
   //alert(opt);
   if (f.opcode.value == "AGGR" || f.opcode.value == "AVG" || f.opcode.value == "PERDEV") {
     f.opcode.value = getOpCode(opt);
     f.ucmreq.value = "";
   } else {
     f.ucmreq.value = "ucdm_" + f.dmpath.value + "!" + f.opcode.value 
                    + "^" +key.replace('+','#') + "=" + value;
   }
   f.sredirect.value = "tbl_meta.xsl";
   f.fredirect.value = "tbl_meta.xsl";
   f.mtpath.value = mtpath;
   f.keyprms.value = "";
   if (f.opcode.value == "8" || f.opcode.value == "9" || f.opcode.value == "10")
     f.reqtype.value = "STATS";
   else
     f.reqtype.value = "";
   f.host.value = getHostID();
  if (bStats == "FALSE") {
   submitFrmData(f);
   //alert("END:: submitLinkOrBtnReq");
   refreshTable(tblID, '');
  } else {
    f.submit();
  }
  
}

// To save at the instance level.
function instanceSave(cookie, dmpath, mtpath) {
  var params = setCookieInfo(dmpath, mtpath, 'tbl_meta.xsl', '', '', 'SAVE');
  sendRequestToServerEx('ucmreq.igw', 'POST', params, processResp, params);
}
// To factory reset at the instance level.
function instanceFactoryReset(cookie, dmpath, mtpath){
  var params = setCookieInfo(dmpath, mtpath, 'tbl_meta.xsl', '', '', 'FACRES');
  sendRequestToServerEx('ucmreq.igw', 'POST', params, processResp, params);
}

// To show home page
function home()
{
  parent.parent.frameBottom.rightPanel.location.href = "welcome.htm";
}
// To logout session.
function logout()
{
  parent.parent.frameBottom.rightPanel.location.href = "logout.htm";
}

function logoutwithoutsave()
{
  parent.parent.frameBottom.rightPanel.location.href = "logoutwithoutsave.htm";
}

function rebootAction(dmpat, hostnm) {
  var cfmReboot = confirm("Are you sure you wish to reboot the device: " + hostnm);
  if (true == cfmReboot) {
    setHostID(defaultHost);
   submitFormRequest('', 'REBOOT', dmpat);
    return;
  } else {
     return;
  }
} //rebootAction

function setRoleValueInstance(cell, dmpath, cookie){
  //var frm = document.getElementById("main-form");
  //frm.roleopt.value = "ISETROL"; 
  document.forms[0].roleopt.value = "ISETROL";
  var cellnode = cell.parentNode.parentNode;
  var keyElements = cellnode.getElementsByTagName("keys");
  var keys = "";
  var keyName = "";
  var keyValue = "";

  if(keyElements.length  == 1) {
      keyValue = keyElements[0].getAttribute("value");
      keys = keyValue;
  } else {
    for(i = 0; i < keyElements.length; i++) {
      keyName  = keyElements[i].getAttribute("name");
      keyValue = keyElements[i].getAttribute("value");
      keys += keyName + "=" + keyValue + ",";
    }
    if(keys.charAt(keys.length -1) == ",") {
      keys = keys.substring(0,keys.length -1);
    }
  } // else
  dmpath = dmpath + "{" + keys + "}";
  document.forms[0].instPath.value = dmpath;
  setTblValueInstance(cell, dmpath, cookie, 'ROLE_MetaData.xml', 
                                       '', 'tbl_meta.xsl','', '', 'new');
}

var ifrmID;
var ifrmObj;
function getIfrmID() {
  return ifrmID;
}
function setIfrmID(idval) {
  ifrmID = idval;
}
// Creates Add\Edit Popup divs
function createAddEditPop(doc, name, iFrmOb) {
  var popDiv;
  var iFrm;
  var tmpDiv = doc.getElementById(name);

  if(tmpDiv) {
    if (tmpDiv.style.display == 'none') {      
      //set Position
      AssignLeftPosition(doc);
      tmpDiv.style.top = doc.getElementById('MouseY').innerHTML + "px";
      tmpDiv.style.left = doc.getElementById('MouseX').innerHTML + "px";
      tmpDiv.style.display = "block";
    }
    return;
  }
  //alert("createAddEditPop::name= " + name);
  popDiv = doc.createElement("div");
  popDiv.className = "white_content";
  popDiv.id = name;
  AssignLeftPosition(doc);
  //alert("createAddEditPop::AssignLeftPosition...done ");
  var iFrmY = 0;
  if (iFrmOb != null) {
      var ifrmDivId = iFrmOb.id;
      ifrmDivId = ifrmDivId.substring(0, ifrmDivId.indexOf("_ifrm"));
      //alert("ifrmDivId= " + ifrmDivId);
      iFrmY = doc.getElementById(ifrmDivId).offsetTop;
      var addButtonY = "";
       if(iFrmOb.document) {
         addButtonY = iFrmOb.document.getElementById('screenHeightDiv').offsetTop;         
       } else if(iFrmOb.contentDocument) {
         addButtonY = iFrmOb.contentDocument.getElementById('screenHeightDiv').offsetTop;
       } else if(iFrmOb.contentWindow){
         addButtonY = iFrmOb.contentWindow.getElementById('screenHeightDiv').offsetTop;
       }      
       //alert("iFrmY: " + iFrmY + "; addButtonY: " + addButtonY);
       iFrmY = iFrmY + addButtonY;
  }
  if (iFrmOb != null) {
      popDiv.style.top = iFrmY + 70 + "px";//alert(iFrmY);
  } else {
      popDiv.style.top = doc.getElementById('MouseY').innerHTML + "px";
  }
  popDiv.style.left = doc.getElementById('MouseX').innerHTML + "px";
  popDiv.style.display = "block";
  iFrm = doc.createElement("iframe");
  iFrm.name = name + "_ifrm";
  iFrm.id = name + "_ifrm";
  iFrm.width = "600";
  iFrm.height = "400";
  iFrm.src = "";
  popDiv.appendChild(iFrm);
  doc.body.appendChild(popDiv);      
  //alert("createAddEditPop::done ");
}

var errorFlag = "FALSE";

var parentTxKeys;
function getParentTxKeys() {
  return parentTxKeys;
}

function setTargetView(cell) {
  var target = "new";
  var tgt = "";
  tgt = cell.getAttribute("tgt");
  tgtID = cell.getAttribute("ldivid");
  var dmpath = cell.getAttribute("dmpath");
  var iFrmId = cell.getAttribute("ifrm");
  var ptx = cell.getAttribute("ptx");
  var pkeys = cell.getAttribute("pkeys");
  var iFrmOb = parent.document.getElementById(iFrmId);
  var pTxRec;
  if(parent.opener) {
    pTxRec = parent.opener.document.getElementById("ptxRecords");
    pTxRec.value = "true";
  }
  if(ptx == 'n') {
     document.getElementById("ptxRecords").value = "true";
  }
  //alert("setTargetView::ptxRecords= " + pTxRec.value);
  //alert("setTargetView::isParentFormTblPage= " + isParentFormTblPage());
  //alert("setTargetView:: ptx= " + ptx + " pkeys= " + pkeys + " iFrmId= " +iFrmId);
  if(/*ptx == 'y' &&*/ pkeys != '' && isParentFormTblPage()) {
    var pkeysStr = pkeys;
    if(pkeysStr != null) {
      pkeysStr = pkeysStr.substring(0, pkeysStr.lastIndexOf("|")); 
      pkeysStr = pkeysStr.substring(pkeysStr.lastIndexOf("|") + 1, pkeysStr.length);
      var keyArr = pkeysStr.split(",");
      var keyResolvedFinal = "";
      for (var k=0; k<keyArr.length; k++) {
          if (keyArr[k] != "") {
            //alert("keyArr[k]= " + keyArr[k]);
            var tkeys = keyArr[k].replace(/\+/g, '#');
            var f = parent.document.getElementById(tkeys);
            if (f == null) f = parent.document.getElementById(tkeys.substring(0,tkeys.indexOf("#")));
            if(f.value =="") {
              errorFlag = "TRUE";
              alert("Key field(s) cannot be empty.");
              return;
            } else {
              errorFlag = "FALSE";
            }
            var keyResolved = keyArr[k].substring(0, keyArr[k].indexOf("$$")) 
                                                         + "!" + f.value + "|";
            keyResolved = keyResolved.replace("+", "^");
            //alert("keyResolved= " + keyResolved);
            keyResolvedFinal += keyResolved;
          }
      }
     if(keyResolvedFinal.charAt(keyResolvedFinal.length -1) == "|") {
       keyResolvedFinal = keyResolvedFinal.substring(0,keyResolvedFinal.length -1);
     }
     //alert("final keys: " + keyResolvedFinal);
     parentTxKeys = keyResolvedFinal;
   }
  } else {
    parentTxKeys = "";
  }

  if(tgt == "ldiv") {
    var keys = "";
    //alert("setTargetView::tgt= " + tgt + "\t tgtID= " + tgtID );
    setIfrmID(tgtID +"_ifrm");
    if(window.top.frameBottom) {
     createAddEditPop(window.top.frameBottom.rightPanel.document, tgtID, iFrmOb);
    } else {
     createAddEditPop(window.top.document, tgtID, iFrmOb);
    }
    return;
  } else {
    ifrmID = "";
  }
}

function slideDown(obj, offset, full,px) {
  if(offset < full) {
    obj.style.height = offset+ "px";
    offset = offset + "px";
    setTimeout((function(){slideDown(obj, offset, full, px);}),1);
  } else {
    obj.style.height = full + "px";
  }
}

function setTblValueInstance(cell, dmpath, cookie, mtpath, opcode, 
                                        spg,keyid, keysArr, target) {
  var target = "new";
  var tgt = "";
  tgt = cell.getAttribute("tgt");
  tgtID = cell.getAttribute("ldivid");
  cell = cell.parentNode.parentNode;
  var keyElements = cell.getElementsByTagName("keys");
  var keys = "";

  for(i = 0; i < keyElements.length; i++) {
    keyName  = keyElements[i].getAttribute("name");
    keyType  = keyElements[i].getAttribute("type");
    keyValue = keyElements[i].getAttribute("value");
    keys += keyName + "^" + keyType + "!" + keyValue + "|";
  }
  if(tgt == "ldiv") {
    //alert("Clicked on frame: " +tgtID +"_ifrm");
    setIfrmID(tgtID +"_ifrm");
    target = tgt;
    var div1 = window.top.frameBottom.rightPanel.document.getElementById(tgtID);
    toggleLdivs(cell);
    if (div1.style.display == 'none') {
      div1.style.display = "block";
      //alert("div1.style.paddingLeft= " + pad + "\t new pad= " +pad+10);
      //div1.style.paddingLeft = "10px";
      div1.style.paddingLeft = "5px";     
    } 
  } else {
    ifrmID = "";
    target = "new"
  }
  setCookieValue(dmpath,opcode, mtpath, spg, keys, cookie, keyid, target);
}

//start... Functions for selection control
function updateMSelectionList(sellst) {
  //alert("sellst = " + sellst);
  var index = sellst.lastIndexOf("#");
  var nameStr =  sellst.substring(0, index);
  var selArrName = nameStr+"SelArry";
  //alert("selArrName= " + selArrName);
  //var cf = document.forms[1];
    var arrFltCat = infoMap.Get(selArrName);
    //alert(arrFltCat);
    var srcLst = document.getElementById(sellst);//eval("cf."+ sellst);
    //alert(srcLst);
    if (arrFltCat) {
      for (var i=0; i<arrFltCat.length; i++) {
        var val = arrFltCat[i].nodeValue;
        //alert("val= " +val);
        for(var j=0; j< srcLst.length; j++) {
          if(srcLst.options[j].value == val) {
            //alert("yes val= " +val);
            srcLst.options[j].selected = true;
            break;
          }
        }
      }
      if(document.getElementById("targetList_"+sellst) != null)
      {//for selection control component...
         addEntry(sellst, 'targetList_'+sellst);
      }
    }
}
function addEntry(list, selectedlist) {
  //alert("inside addEntry");
  //var cf = document.forms[1];
  //alert(eval("cf."+list));
  var selval = multipleSelectComboByText(document.getElementById(list));//multipleSelectComboByText(eval("cf."+list));
  //alert(selval);
  //addListByText(eval("cf."+list), eval("cf."+selectedlist), selval);
  addListByText(document.getElementById(list), document.getElementById(selectedlist), selval);          
}

function removeEntry(list, selectedlist) {
  //var cf = document.forms[1];
  var selval = multipleSelectComboByText(document.getElementById(selectedlist));
  //alert(selval);
  addListByText(document.getElementById(selectedlist), document.getElementById(list), selval);
}

function multipleSelectComboByText(obj) {
	var selval="";
	for(var i=0; i<obj.options.length; i++)
	{
		if(obj.options[i].selected) {
			selval += obj.options[i].text + ","; 
		}
	}	
	selval=selval.substring(0,selval.length-1)
	return selval;
}

function addListByText(obj1, obj2, val) {
  if(val == "" ) return;
  var list = val.split(",");       
  for(var i=0; i<list.length; i++){
    obj2.options[obj2.options.length] = new Option(list[i], list[i]);
    // adding to list

    for(count = obj1.options.length-1; count>= 0; count--){
      // removing from list
      if(obj1.options[count].text == list[i])
        obj1.remove(count);              
    }
  }        
}
//end... Functions for selection control

//var rowToRender;
function renderRecord() { 
 //alert("renderRecord:: document.forms[0].renderRows.value=" + document.forms[0].renderRows.value); 
 if(typeof document.forms[0].renderRows == "undefined" || document.forms[0].renderRows.value != "yes") {
     setTimeout("renderRecord();", 5000);
     return;
 }
 var tbl;
 var tblElems = document.getElementsByTagName("table");
 //alert("tblElems.length= " + tblElems.length);
 for(var t = 0 ; t < tblElems.length; t++) {
  tbl = tblElems[t];
  var titlAttr = tbl.getAttribute("titl");
  if(titlAttr == null) {
    continue;
 }
 var rows = tbl.getElementsByTagName('tr');
 for (var i=0; i < rows.length; i++) {
   if (i % 2 == 0) {
     rows[i].style.backgroundColor = "#dddddd";
   }
   else {
      rows[i].style.backgroundColor = "#f9f6f4";
   }
   rows[i].className = "tabdata";
  } // for
 } // for
 if(typeof document.forms[0].renderRows == "undefined") {
    document.forms[0].renderRows.value = "";
 }
 setTimeout("renderRecord();", 5000);
}

function hideCloseButton() {
if (this.window.name.indexOf("newW") == -1) {
   if (document.getElementById("bClose") != null) {
      document.getElementById("bClose").style.display = "none";
   }
}
}

function resetEditIndex() {
    document.forms[0].rowEditIndex.value = "";
}

function renderRowsAlternateColors(tblId) {
 var tbl = document.getElementById(tblId);
 if (!tbl) return;
 var rows = tbl.getElementsByTagName('tr'); 
 for (i=0; i < rows.length; i++) {
	/*rows[i].onclick = function() {
		alert(this.rowIndex);
        }*/	
        if (i % 2 == 0) {
		rows[i].style.backgroundColor = "#dddddd";
	}
	else {
		rows[i].style.backgroundColor = "#f9f6f4";
	}
	rows[i].className = "tabdata";
 } 
}
