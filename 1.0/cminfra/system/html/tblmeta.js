/*
 * Java Script file for common Table related functions.
 */

// Function to populate EDIT URL
function popEditURL(cell, dmpath, mtpath, spg, opcode, reqtype, cookie) {
  //alert(cell.parentNode.parentNode.rowIndex);
  document.forms[0].rowEditIndex.value = cell.parentNode.parentNode.rowIndex;
  //alert("popEditURL:: dmpath = " + dmpath);
  var dmPath = "";
  if(dmpath.indexOf("{0}") > -1){
    var tokens = dmpath.split("0");
    dmPath = tokens[0] + getCookie(cookie) + tokens[1];
  } else {
    dmPath = dmpath;
  }
  var keys= "";
  cell = cell.parentNode.parentNode;
  //alert("cell = "+ cell.innerHTML);
  //alert("popEditURL::reqtype = "+ reqtype);

  keys = populateKeyPairs(cell, "^", "!");
  if(keys.charAt(keys.length - 1) == '|') {
     keys = keys.substring(0,keys.lastIndexOf("|"));
  }
  var opCode = "";
  if(opcode == "ROLESET") {
    // alert("popEditURL::ROLESET dmPath = " + dmPath);
    if(dmPath.charAt(dmPath.length - 1) == '}') {
      opCode = "IRBYKEY";
    } else {
      opCode = "RBYKEY";
    }
  } else {
    opCode = "BYKEY";
  }
  if(reqtype == "GETPASSV") {
    keys += "|bPassive^UINT!7";
  }
  tOpcode = getOpCode(opCode);
  //sumbitFrmReq(dmPath, tOpcode, mtpath, spg, '', keys, "GET", 'new', '');
  sumbitFrmReq(dmPath, tOpcode, mtpath, spg, '', keys, reqtype, 'new', '');

} // popEditURL

// Function to swap the table rows upwards or downwards.
function processSwapResp(prms) {
  var status = "";
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
    //alert(status);
    if('error' == status) {
      alert("Failed to move the record.");
      return;
    }
    var curRow = prms[0];
    //alert("prms[1]= " +prms[1]);
    if(prms[1] == 'up') {
      moveRow(curRow, curRow.rowIndex - 1);
    } else {
      moveRow(curRow, curRow.rowIndex + 1);
    }
    document.forms[0].renderRows.value = 'yes';
    renderRecord();
    document.forms[0].renderRows.value = '';
    return;
  }
} // processSwapResp

// Function to swap the table rows upwards.
function swapRowUp(chosenRow, cookie, dmPath, mtpath) {
  var tblid = chosenRow.parentNode.parentNode.id;
  
  var keyNam, prevType, prevKeyID;
  if((!cookie) || '' == getCookie(cookie)){
    alert("Please select the VSG");
    return;
  }

  var curType, curKeyID;
  //alert("swapRowUp::chosenRow.innerHTML= " + chosenRow.innerHTML);
  var mainTable = document.getElementById(tblid); 
  var rowArr = mainTable.rows;
  var curIndex = chosenRow.rowIndex;
  var prmArry = new Array();

  if((curIndex - 1) < 0) {
   return;
  }
  var prevRow = rowArr[chosenRow.rowIndex - 1];
  var cRow = rowArr[chosenRow.rowIndex];
  var cKeyElem = cRow.getElementsByTagName("keys")[0];//Need handle multiple keys
  keyNam = cKeyElem.getAttribute("name");
  curType = cKeyElem.getAttribute("type");
  curKeyID = cKeyElem.getAttribute("value");
  
  //alert("swapRowUp::prevRow.innerHTML= " + prevRow.innerHTML);
  var keyElem = prevRow.getElementsByTagName("keys")[0];//Need handle multiple keys
  if(!keyElem) {
    //alert("No keys...");
    return;
  }
  prevType   = keyElem.getAttribute("type");
  prevKeyID = keyElem.getAttribute("value");

  var ucmreq="";
  if((keyNam) && (prevType) && (prevKeyID) && (curKeyID) && (curType)) {
    ucmreq += keyNam + '#' + curType + '=' + curKeyID + "|relativity#STR=" + "before" 
                   + '|' + 'relativekey' + '#' +prevType + '=' +prevKeyID;

    //alert("swapRowUp::ucmreq= " + ucmreq);
    if (chosenRow.rowIndex >= 2) {
      prmArry.push(chosenRow);
      prmArry.push('up');
      ucmreq = "ucdm_" + dmPath + "^" + ucmreq;
      var params = setCookieInfo(dmPath, mtpath, 'tbl_meta.xsl', ucmreq, '', 'EDIT');
      sendRequestToServerEx('ucmreq.igw', 'POST', params, processSwapResp, prmArry);
    }else{
      return;  
    }
  }
} // swapRowUp

// Function to swap the table rows downwards.
function swapRowDown(chosenRow, cookie, dmPath, mtpath) {
           
  var prmArry = new Array();
  var tblid = chosenRow.parentNode.parentNode.id;
  var mainTable = document.getElementById(tblid);
           
  if('' == getCookie(cookie)){
    alert("Please select the VSG");
    return;
  }
           
  var curType, curKeyID;
  //alert("swapRowDown::chosenRow.innerHTML= " + chosenRow.innerHTML);
  var rowArr = mainTable.rows;
  var nextRow = rowArr[chosenRow.rowIndex + 1];
  if(!nextRow) {
   //alert("No more rows below to move.");
   return;
  }
  var cRow = rowArr[chosenRow.rowIndex];
  var cKeyElem = cRow.getElementsByTagName("keys")[0];//Need handle multiple keys
  keyNam = cKeyElem.getAttribute("name");
  curType = cKeyElem.getAttribute("type");
  curKeyID = cKeyElem.getAttribute("value");

  //alert("swapRowDown::nextRow.innerHTML= " + nextRow.innerHTML);
  var keyElem = nextRow.getElementsByTagName("keys")[0];//Need handle multiple keys
  prevType   = keyElem.getAttribute("type");
  prevKeyID = keyElem.getAttribute("value");

  var ucmreq="";
  if((keyNam) && (prevType) && (prevKeyID) && (curKeyID) && (curType)) {
    ucmreq += keyNam + '#' + curType + '=' + curKeyID
                   + "|relativity#STR="
                   + "after" + '|' + 'relativekey' + '#'
                   + prevType + '=' +prevKeyID;
    //alert("swapRowDown::ucmreq= " + ucmreq);
    if (chosenRow.rowIndex != mainTable.rows.length - 1) {
      prmArry.push(chosenRow);
      prmArry.push('down');
      ucmreq = "ucdm_" + dmPath + "^" + ucmreq;
      var params = setCookieInfo(dmPath, mtpath, 'tbl_meta.xsl', ucmreq, '', 'EDIT');
      sendRequestToServerEx('ucmreq.igw', 'POST', params, processSwapResp, prmArry);
    }
     
  }
} // swapRowDown
    
// Function to move the table rows.
function moveRow(chosenRow, newIndex) {
          
  if (newIndex > chosenRow.rowIndex) {
    newIndex++;
  }
  var tblid = chosenRow.parentNode.parentNode.id;
  var mainTable = document.getElementById(tblid);

  var theCopiedRow = mainTable.insertRow(newIndex);
          
  for ( var i = 0; i < chosenRow.cells.length; i++) {
    var oldCell = chosenRow.cells[i];
    var newCell = document.createElement("td");
    newCell.className="tabdata";
    newCell.innerHTML = oldCell.innerHTML;
    theCopiedRow.appendChild(newCell);
    copyChildNodeValues(chosenRow.cells[i], newCell);
  }
  mainTable.deleteRow(chosenRow.rowIndex);
} // moveRow
    
var moreKeys= "";
function setMoreKeys(keypair) {
  moreKeys = keypair;
}
function getMoreKeys() {
  return moreKeys;
}

function autoRefreshTable(tblid,tInterval) {
   refreshTable(tblid, '');
   setTimeout(function () {autoRefreshTable(tblid,tInterval)}, tInterval);
}
function autoRefreshPage(dmPat, opcode, mtpath, spg, keys, cookie, tInterval, tblid){
   //alert("tblid= " + tblid);
   //setTimeout(function () {setCookieValue(dmPat, opcode, mtpath, spg, keys, cookie,'', '')}, tInterval);
   setTimeout(function () {autoRefreshTable(tblid,tInterval)}, tInterval);
}

// Function to refresh table rows using AJAX.
function refreshTable(tblId, opcod) {
  var tTable = document.getElementById(tblId);
  var tblType = tTable.getAttribute("typ"); 
  // First remove all the rows
  while(tTable.rows.length >1) {
    tTable.deleteRow(tTable.rows.length-1);
  }
  //alert(tblType);
  if (tblType == null) {
     getFirstNRecords(tblId); // For multiple rows table
  }
  else if (tblType == 'scalar') {
    if (opcod == "AGGR" || opcod == "AVG" || opcod == "PERDEV") {
      getStatsData(tblId, opcod); // For scalar or statistics tables
    } else {
      getScalarData(tblId); // For scalar or statistics tables
    }
  }
} //refreshTable

// Function to get scalar using AJAX.
function getScalarData(tblId) {
  var divElem =document.getElementById(tblId);
  var fqn = "";
  var mtpath = "";
  var spg  = "";
  var keys = "";
  var pkeys = "";
  var cookiename = "";
  var opcode = "";
  var frm;
  if(divElem) {
    fqn = divElem.getAttribute("dmpath");
    mtpath = divElem.getAttribute("mtpath");
    spg  = divElem.getAttribute("spg");
    var  f= document.getElementById('tbl-form');

    if (window.name.indexOf("newW") != -1) {
      frm = window.opener.document.getElementById('tbl-form');
    } else {
      frm = f;
    }
    keys = "";
    //pkeys = divElem.getAttribute("parentKeys");
    pkeys = frm.keyprms.value;
    cookiename = divElem.getAttribute("cookiename");
    opcode = divElem.getAttribute("opcode"); 
  }
  if(pkeys != '') {
   fqn = processParentKeys(fqn,pkeys);
  }
  var topt = getOpCode(opcode);
  //alert("getScalarData:: opcode= " + opcode + " topt= " + topt);
  var base_url = setCookieInfo(fqn, mtpath, spg, keys, cookiename, topt);
  if(base_url) {
     base_url += "&reqtype=GET";
     base_url += "&host=" + getHostID();
     //alert("getScalarData::base_url= " + base_url);
     sendRequestToServerEx('ucmreq.igw', 'POST', base_url, 
                             parseStatsData, tblId);
  } //if
} //getScalarData

function getStatsData(tblId, opcode) {
  var divElem =document.getElementById(tblId);
  var fqn = "";
  var mtpath = "";
  var spg  = "";
  var keys = "";
  var pkeys = "";
  var cookiename = "";
  var frm;
  if(divElem) {
    fqn = divElem.getAttribute("dmpath");
    mtpath = divElem.getAttribute("mtpath");
    spg  = divElem.getAttribute("spg");
    var  f= document.getElementById('tbl-form');

    if (window.name.indexOf("newW") != -1) {
      frm = window.opener.document.getElementById('tbl-form');
    } else {
      frm = f;
    }
    keys = "";
    //pkeys = divElem.getAttribute("parentKeys");
    pkeys = frm.keyprms.value;
    cookiename = divElem.getAttribute("cookiename");
    //opcode = divElem.getAttribute("opcode"); 
  }
  if(pkeys != '') {
   fqn = processParentKeys(fqn,pkeys);
  }
  var topt = getOpCode(opcode);
  //alert("getStatsData:: opcode= " + opcode + " topt= " + topt);
  var base_url = setCookieInfo(fqn, mtpath, spg, keys, cookiename, topt);
  if(base_url) {
     base_url += "&reqtype=STATS";
     base_url += "&host=" + getHostID();
     //alert("getStatsData::base_url= " + base_url);
     sendRequestToServerEx('ucmreq.igw', 'POST', base_url, 
                             parseStatsData, tblId);
  } //if
} //getStatsData

function processParentKeys(dmPath, pKeys) {
  var fqn = dmPath;
  var pkeystmp =  pKeys;
  var tpath = fqn.substring(0,fqn.lastIndexOf("{") +1);
  var tkeys = pkeystmp.split('|');
  //alert("tkeys.length"+tkeys.length);
  if(tkeys.length > 1) {
    for(var i=0;i<tkeys.length;i++) {
      tpath += tkeys[i].substring(0,tkeys[i].indexOf("^"));
      tpath += "=" + tkeys[i].substring(tkeys[i].indexOf("!")+1, tkeys[i].length);
      tpath += ",";
    } // for
  } else {
    tpath += tkeys[0].substring(tkeys[0].indexOf("!")+1, tkeys[0].length);
  }

  if(tpath.charAt(tpath.length -1) == ',') {
     tpath = tpath.substring(0,tpath.lastIndexOf(","));
  }
  tpath += "}" + fqn.substring(fqn.lastIndexOf("}") +1, fqn.length);
  //alert("tpath= " + tpath);
  return tpath;
} //processParentKeys

// Function to get first N records using AJAX.
function getFirstNRecords(tblId) {
  var morelnk = "more_link" + tblId;
  var mlId = "ml" + tblId;
  //alert(" getFirstNRecords::tblId = " + tblId);
  //alert("morelnk= " + morelnk + "\tmlId= " +mlId);
  var moreElem =document.getElementById(morelnk);
  var fqn = "";
  var mtpath = "";
  var spg  = "";
  var keys = "";
  var cookiename = "";
  var opcode = "";

  if(moreElem) {
    fqn = moreElem.getAttribute("dmpath");
    mtpath = moreElem.getAttribute("mtpath");
    spg  = moreElem.getAttribute("spg");
    keys = getMoreKeys();
    cookiename = moreElem.getAttribute("cookiename");
    opcode =  ""; // For first N records opcode is empty/default
  }
  var base_url = setCookieInfo(fqn, mtpath, spg, keys, cookiename,opcode);
  if(base_url) {
     if(fqn == "igd.logmesg") {// To be fixed
       base_url += "&keyprms=" + getCookie("AuditFilterStr");
     }
     base_url += "&reqtype=GET";
     base_url += "&host=" + getHostID();

    if(document.getElementById(mlId).value != base_url) {
      document.getElementById(mlId).value=base_url;
      //alert("base_url= " + base_url);
      sendRequestToServerEx('ucmreq.igw', 'POST', base_url, 
                             showcont_stateChanged_head, tblId);
    }
  }
} // getFirstNRecords

// Function to get next N records using AJAX.
function showcont(tblId) {
  var morelnk = "more_link" + tblId;
  var mlId = "ml" + tblId;
  //alert("morelnk= " + morelnk + "\tmlId= " +mlId);
  var moreElem =document.getElementById(morelnk);
  var fqn = "";
  var mtpath = "";
  var spg  = "";
  var keys = "";
  var cookiename = "";
  var opcode = "";

  if(moreElem) {
    fqn = moreElem.getAttribute("dmpath");
    mtpath = moreElem.getAttribute("mtpath");
    spg  = moreElem.getAttribute("spg");
    keys = getMoreKeys();
    cookiename = moreElem.getAttribute("cookiename");
    opcode =  moreElem.getAttribute("opcode");
  }
  var base_url = setCookieInfo(fqn, mtpath, spg, keys, cookiename,opcode);
  if(base_url) {
     if(fqn == "igd.logmesg") {// To be fixed
       base_url += "&keyprms=" + getCookie("AuditFilterStr");
     }
     base_url += "&reqtype=GET";
     base_url += "&host=" + getHostID();

    //alert("base_url= "+ base_url
     //      +"\t ml.value= "+document.getElementById('ml').value);
    /*if(document.getElementById('ml').value != base_url) {
      document.getElementById('ml').value=base_url;
      sendRequestToServerEx('ucmreq.igw', 'POST', base_url, 
                             showcont_stateChanged_head, tblId);
    }*/
    if(document.getElementById(mlId).value != base_url) {
      document.getElementById(mlId).value=base_url;
      sendRequestToServerEx('ucmreq.igw', 'POST', base_url, 
                             showcont_stateChanged_head, tblId);
    }
  }
} // showcont

// AJAX handler to process get Stats records.
function parseStatsData(tblId) {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;
    //alert("parseStatsData:: data= " + data);
    if((data) && (data.search(/\<param/)!='-1')) {        
      // code for IE
      if (window.ActiveXObject) {
        var xml = new ActiveXObject("Microsoft.XMLDOM");
        xml.loadXML(data);
      } // code for Mozilla, Firefox, Opera, etc.
      else if (document.implementation 
                  && document.implementation.createDocument) {
        var parser = new DOMParser();
        xml = parser.parseFromString(data, 'text/xml');
      }
      var recs = xml.getElementsByTagName("record");
      var lastRec = recs[recs.length - 1];
      var mtdata = selectNodeStrVal("/root/MetaData", xml);
      var mtxml = loadXMLDocument(mtdata);
      updateScalarTableRows(xml, recs, mtxml, tblId);
    } // if param
  } // xmlobject.readyStateif
} // parseStatsData

// Function to update scalar table rows.
function updateScalarTableRows(xmlob, recs, mtxml, tblId) {
  var tblelms =  selectNodeArry("root/tbl" ,mtxml);
  var htmTds= "<td align=\"center\" class=\"tabdata\" width=\"10%\" height=\"1\">";
  var htmTde= "</td>";
  var myTbl = document.getElementById(tblId);
  var tBody = myTbl.getElementsByTagName('tbody')[0];
  //alert("after tbody in :: updateScalarTableRows");
  for(var x = 0; x < tblelms.length; x++) {
    var childElems = tblelms[x].childNodes;
    //alert("childElems.length= " + childElems.length);

    for(var i = 0; i < childElems.length; i++ ) {
        //alert("childElems[i].nodeName= " + childElems[i].nodeName);
      if(childElems[i].nodeName == 'objref') {
        var cell1;
        var cell2;
        var row = document.createElement("tr");
        var objID = childElems[i].getAttribute("oid");
        //alert("objID= " + objID);
        var objName = "";
        var objVal = "";
        var objLbl = "";
        if(objID) {
          objName = 
                   selectNodeStrVal("root/obj[@oid=" + objID +"]/@name", mtxml);
          if(objName) {
            objVal = selectNodeStrVal("/root/Config/record"
                          + "/param[@name='" + objName +"']/@value", xmlob);
          }
          objLbl = selectNodeStrVal("root/obj[@oid=" + objID +"]/@lbl", mtxml);
          if(!objLbl) {
            objLbl = objName;
          }
          if(objVal) {
                objVal = poplValue(mtxml, objID, objVal);
            } else {
              objVal = "";
          }
        } // if objID
          cell1 = document.createElement("td");
          cell2 = document.createElement("td");
	  cell2.className="tdformlabel";
	  cell1.className="tdformlabel";
	  cell1.height = 1;
	  cell2.height = 1;
	  cell1.width = "25%";
	  cell2.width = "75%";
          cell1.innerHTML = objLbl;
          cell2.innerHTML = objVal;

	  row.className="trform";
	  row.height = 1;	  
          row.appendChild(cell1);
          row.appendChild(cell2);
          //alert(row.innerHTML);
          tBody.appendChild(row);
      } // if objref
    } // for each childElems
  }
} // updateScalarTableRows

// AJAX handler to process get next N records.
function showcont_stateChanged_head(tblId) {
  if(xmlobject.readyState==4 || xmlobject.readyState=='complete') {
    var data=xmlobject.responseText;//alert(data);
    if((data) && (data.search(/\<param/)!='-1')) {        
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
      }//alert(data);
      var recs = xml.getElementsByTagName("record");
      var lastRec = recs[recs.length - 1];
      var mtdata = selectNodeStrVal("/root/MetaData", xml);
      var mtxml = loadXMLDocument(mtdata);//alert(xml);
      //alert("update table: " + tblId);alert("with recs: " + recs.length);
      updateTableRows(xml, recs, mtxml, tblId);
      //alert("updated table");
      var keysExp = "/root/obj[@key='yes']";
      var keyArr = selectNodeArry(keysExp, mtxml);
      var keys = "";
      for(j =0; j< keyArr.length; j++) {
        var keyName = keyArr[0].getAttribute("name");
        keys += keyName;
        keys += "^";
        keys += keyArr[0].getAttribute("type");
        keys += "!";
        var parms = lastRec.getElementsByTagName("param");
        var keyVal = "";
        for(l = 0; l < parms.length; l++) {
          var pname =parms[l].getAttribute("name");
          if(pname == keyName) {
            keyVal = parms[l].getAttribute("value");
            break;
          }
        }
        if((keyVal) && (keyVal != "")) {
          keys += keyVal;
        } else {
          keys += "nil";
        }
        keys += "|";
      }
      setMoreKeys(keys);
    }
    renderRowsAlternateColors(tblId);
    showcont(tblId);    
    //setTimeout("showcont('" + tblId + "');", 100);    
  }
} // showcont_stateChanged_head

// Function to update table rows with get next N records.
function updateTableRows(xmlob, recs, mtxml, tblId) {
  //alert("tblmeta:: updateTableRows");
  var tblelms =  selectNodeArry("root/tbl" ,mtxml);
  var htmTrStr = "";
  var htmTds= "<td align=\"center\" class=\"tabdata\" width=\"10%\" height=\"1\">";
  var htmTde= "</td>";
  //var myTable = document.getElementById("mainTable");
  var myTable = document.getElementById(tblId);
  var tBody = myTable.getElementsByTagName('tbody')[0];
  var cookieName = selectNodeStrVal("root/vrtlInst/@name", mtxml);  
  if(!cookieName) {
    mtPat = selectNodeStrVal("root/depends/@mtpath", mtxml);
    var vtxml = loadXMLDocument(mtPat);
    cookieName = selectNodeStrVal("root/vrtlInst/@name", vtxml);   
  }
  for(x = 0; x < tblelms.length; x++) {
    var cols = tblelms[x].getElementsByTagName("col");
    for(r = 0; r <= recs.length; r++) {
      var needRow = 'false';
      var row = document.createElement("tr");
      for(t = 0; t < cols.length; t++) {
        htmTrStr += htmTds;
        htmTrStr += "<img src='images/spacer.gif' align='absMiddle' height='25' width='1'/>"; //"<hr/>";
        //var cell1 = document.createElement("td");
        var cell1;
        var obrefs = cols[t].getElementsByTagName("objref");
        for(o = 0; o < obrefs.length; o++) {
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
                var rindex = r;
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
              var rindex = r;
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
              var newVal = "";
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
          var ldmpatatrib = links[l].getAttribute("dmpath");
          var lnStr = "[";
          var linkRef= lnStr;
          linkRef += "<a href=\"#\"";
          if(lnkName == 'Reboot') {
            var lkeyName = selectNodeStrVal("root/obj[@key='yes']/@name", mtxml);
            var rindex = r;
            var lkeyVal = selectNodeStrVal("/root/Config/record[" + rindex
                        + "]/param[@name='" + lkeyName +"']/@value", xmlob);
            linkRef += " onclick=\"rebootAction('";
            linkRef += ldmpatatrib + "', '";
            linkRef += lkeyVal + "');\">";
            linkRef += lnkName; 
            linkRef += "</a>]";
            htmTrStr += linkRef;
            htmTrStr += "<br/>"; 
            continue;           
          }

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
          var tgt = links[l].getAttribute("target");
          var subDivId =  lnkmtpath.substring(0, lnkmtpath.indexOf('.'));        
            if(tgt == "ldiv") {
              linkRef += " ldivid=\"" + subDivId + "\"";
            }
            linkRef += " tgt=\"" + tgt + "\"";
            linkRef += " onclick=\"setTblValueInstance(this,'";
            if (subtbl && subtbl == "no") {                
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
          linkRef += "</a>]";
          htmTrStr += linkRef;
          htmTrStr += "<br/>";          
        }
        htmTrStr += htmTde; // TD end for links
        htmTrStr += htmTds; // TD start for buttons
        var btns = cols[t].getElementsByTagName("btn");
        for(b = 0; b < btns.length; b++) {
          var btnName = btns[b].getAttribute("name");
          var btnDmpatAtrib = btns[b].getAttribute("dmpath");
          var dmPath = selectNodeStrVal("root/dmpath", xmlob);
          if(!dmPath) {
            dmPath = selectNodeStrVal("root/dmpath", mtxml);
          }
          var mtPath = selectNodeStrVal("root/MetaData", xmlob);
          switch(btnName) {
            case "Edit":
              var opcode = selectNodeStrVal("root/opcode", mtxml);
              var isPassive = selectNodeStrVal("root/@passive", xmlob);
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
            case "Reboot":
              var bkeyName = selectNodeStrVal("root/obj[@key='yes']/@name", mtxml);
              var bindex = r;
              var bkeyVal = selectNodeStrVal("/root/Config/record[" + bindex
                        + "]/param[@name='" + bkeyName +"']/@value", xmlob);
              var rebootbtn= "&#160;<input type=\"image\" src=\"images/delete.gif\" align=\"absMiddle\" title=\"Reboot\"";
              rebootbtn += " onclick=\"rebootAction('";
              rebootbtn += btnDmpatAtrib + "','";
              rebootbtn +=  bkeyVal + "');\" />";
              htmTrStr += rebootbtn;
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
                //var rexp=  "/root/Config/record/param[@name='" + kName+"']/@value";
                var rexp=  "/root/Config/record[" + r
                          + "]/param[@name='" + kName+"']/@value";//alert(r);
                var rval = selectNodeStrVal(rexp, xmlob);
                var instName = selectNodeStrVal("/root/vrtlInst/@name", mtxml);
                var timOutVal = 
                      selectNodeStrVal("/root/vrtlInst/@timeout", mtxml);
                if (rval != "") {
                  var vsgCookie = getCookie(instName);//alert(cookie);
                  var checkedStr = "";//alert(dmPath);
                  if (dmPath == "igd.vsg" && vsgCookie == rval) {
                      checkedStr = "checked='true'";
                  } else if (dmPath == "mgmt.devinfo") {
                      var exp1="/root/Config/record[" + r
                          + "]/param[@name='cardstatus']/@value";
                      var cardStatus=selectNodeStrVal(exp1, xmlob);
                      if (cardStatus == "Master") {
                          checkedStr = "checked='true'";
                      }
                  }
                  var radbtn = "<input type=\"radio\" name=\"";
                  radbtn += btns[b].getAttribute("name") + "\" ";
                  radbtn += " value=\"" + rval + "\" ";
                  radbtn += " id=\"" + rval + "\" " + checkedStr;
                  radbtn += " onclick=\"setInstance(this,'";
                  radbtn += instName +"', '";
                  radbtn += timOutVal +"');\" />";
                  htmTrStr += radbtn;
                  needRow = 'true';
                }
                break;
          }
        }
        htmTrStr += htmTde;
        if(needRow == 'true') {
          cell1 = document.createElement("td");
	  cell1.className="tabdata";
	  cell1.height = 1;          

          cell1.innerHTML = htmTrStr;
	  row.className="tabdata";
	  row.height = 1;	  
          row.appendChild(cell1);
        }
        htmTrStr = "";
      }
      //alert("row = "+ row.innerHTML);
      if(needRow == 'true') {
        tBody.appendChild(row);
      }
      htmTrStr = "";
    }
  }
} // updateTableRows

// Function to update table with next N records on Scroll
function updateTable() {
      
  var test = document.getElementById('more_link').href;
  var hgt = document.getElementById('load').scrollHeight;
  var top = document.getElementById('load').scrollTop;
  var pos=hgt-500;

  //document.getElementById('ht').innerHTML = pos;
  //document.getElementById('tp').innerHTML = top;
      
  if(top>=pos)
  {
    if(document.getElementById('ika').value!=pos)
    {
      showcont();
    }
    document.getElementById('ika').value=pos;
  }  
} // updateTable

