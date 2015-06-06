function validateIPAddress(object,comment)
{
	if(!comment)
	{
		var comment ="";
	}else
		comment +=" is";

  if(object.value!="")
  {
   var ipStr=object.value;
   var tempIPArr = ipStr.split(".");
   var ipFlag = false ;
   if((tempIPArr.length!=4))
   {
      alert(comment+" Invalid IP Address");
      object.focus();
      return ipFlag;
   }
  else if((tempIPArr[0]==0)&&(tempIPArr[1]==0)&&(tempIPArr[2]==0)&&(tempIPArr[3]==0))
  {
     alert(comment +" Invalid IP Address.");
     object.focus();
     return ipFlag;
  }
else if((tempIPArr[0]==255)&&(tempIPArr[1]==255)&&(tempIPArr[2]==255)&&(tempIPArr[3]==255))
  {
      alert(comment +" Invalid IP Address.");
      object.focus();
      return ipFlag;
  }
  else if((tempIPArr[0]>0) && (tempIPArr[0]<256))
  {
     if((tempIPArr[1]>=0) && (tempIPArr[1]<256))
     {
       if((tempIPArr[2]>=0) && (tempIPArr[2]<256))
       {
          if((tempIPArr[3]>0)&&(tempIPArr[3]<256)&&(tempIPArr[3] != 0))
          {
                  ipFlag = true;
                  return ipFlag;
           }
       }
     }
  }
  if(!(ipFlag))
  {
      alert(comment+" Invalid IP Address");
      object.focus();
     //alert(" Valid range for each octct in IP Address is 0 to 255");
      return ipFlag;
  }
}
}



function validSubnet(object,comment)
{
  if(object.value!="")
  {
   var ipStr=object.value;
   var tempIPArr = ipStr.split(".");
   var ipFlag = false ;
   if((tempIPArr.length!=4))
   {
      alert(comment+" is Invalid.");
      object.focus();
      return ipFlag;
   }
  else if((tempIPArr[0]==0)&&(tempIPArr[1]==0)&&(tempIPArr[2]==0)&&(tempIPArr[3]==0))
  {
     alert(comment +" is invalid.");
     object.focus();
     return ipFlag;
  }
else if((tempIPArr[0]==255)&&(tempIPArr[1]==255)&&(tempIPArr[2]==255)&&(tempIPArr[3]==255))
 {
      alert(comment +" is invalid.");
      object.focus();
      return ipFlag;
  }
  else if((tempIPArr[0]>0) && (tempIPArr[0]<256))
  {
     if((tempIPArr[1]>=0) && (tempIPArr[1]<256) && tempIPArr[1] != "" )
     {
      if((tempIPArr[2]>=0) && (tempIPArr[2]<256)  && tempIPArr[2] != "")
      {
       if( (tempIPArr[3]>=0) && (tempIPArr[3]<256)  && tempIPArr[3] != "" )
        {        
                ipFlag =  true;
                  return ipFlag;
         }
       }
     }
  }
  if(!(ipFlag))
  {
      alert(comment+"is invalid.");
     //alert(" Valid range for each octct in IP Address is 0 to 255");
      return ipFlag;
  }
}
}



function validIP(object,comment)
{       
  var ipStr=object.value;
  if(ipStr.length==0)
      return true;
   if(ipStr != "")
   {
     var tempIPArr = ipStr.split(".");
     var ipFlag = false ;
     if(tempIPArr.length>4)
     {
       alert(comment + " is invalid IP Address.");
       object.focus();
        return ipFlag;
      }
       else
       if((tempIPArr[0]>0) && (tempIPArr[0]<256))
       {
        if((tempIPArr[1]>=0) && (tempIPArr[1]<256) && tempIPArr[1] != "" )
         {
         if((tempIPArr[2]>=0) && (tempIPArr[2]<256)  && tempIPArr[2] != "")
           {
         if( (tempIPArr[3]>=0) && (tempIPArr[3]<256)  && tempIPArr[3] != "" )
             {        
                ipFlag =  true;
                  return ipFlag;
               }
              }
             }
           }
         if(!(ipFlag))
         {        
           alert(comment+" is invalid IP Address");
           object.focus();
           return ipFlag;
         }
   }
}

function changeDisplay(id, status){ 
  if (document.getElementById) {
    document.getElementById(id).style.display = status;
  }
}


function selectComboByVal(obj, val)
{
	for(var i=0; i<obj.options.length; i++)
	{
		if(val == obj.options[i].value) {
			obj.options[i].selected = true; break; }
	}	
}

function selectComboByText(obj, val)
{
	for(var i=0; i<obj.options.length; i++)
	{
		if(val == obj.options[i].text) {
			obj.options[i].selected = true; break; }
	}	
}

function selectListByText(obj, val)
{
	var list = val.split(",");
	for(var i=0; i<obj.options.length; i++)
	{
		for(j=0;j<list.length;j++)
		{
			if(list[j] == obj.options[i].text) {
				obj.options[i].selected = true; 
				break;
			}
		}
	}	
}

function multipleSelectComboByText(obj)
{
	var selval="";
	for(var i=0; i<obj.options.length; i++)
	{
		if(obj.options[i].selected) {
			selval +=obj.options[i].text+","; 
		}
	}	
	selval=selval.substring(0,selval.length-1)
	return selval;
}


// AJAX methods =========================================

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


/*IP Range start  upto End*/
function bitStrToInt(str)
{
 x=0;
 c=0;
 for(i=str.length-1;i>=0;i--)
 {
  val=parseInt(str.charAt(i).toString());
  x+=Math.pow(2,c)*val;
  c++;
 }
 return x;
}

function bitsToMask(bits)
{
  var cnt=31;
  var x=0;
  for(cnt=0;cnt<bits;cnt++)
  {
    x = x | (1<<cnt);
  }
  mask = padTextPostfix(toBin(x), "0", 32);
  x = bitStrToInt(mask);
  return x;
}

function decimalToDotNotaionIP(ip)
{
  return ((ip>>24)&255).toString() + "." + ((ip>>16)&255).toString() + "." +
((ip>>8)&255).toString() + "." + ((ip)&255).toString(); 
}

function dotNotationToDecimalIP(str)
{
  ip = (parseInt(str.split(".")[0])<<24)&0xff000000 |
(parseInt(str.split(".")[1])<<16)&0xff0000 | (parseInt(str.split(".")[2])<<8)&0xff00 | parseInt(str.split(".")[3]) ;
  
return ip;
}


function toBin(inVal) {
   base = 2 ;
   num = parseInt(inVal);
   binNum = num.toString(base);
   // pad leading spaces with "0"
   binNum = padTextPrefix(binNum, "0", 8) ;
   return binNum
}


function padTextPostfix (InString, PadChar, DefLength)  {
   if (InString.length >= DefLength)
      return (InString);
   OutString = InString
   for (Count = InString.length; Count < DefLength; Count++)  {
      OutString = OutString + PadChar;
   }
   return (OutString);
}


function padTextPrefix (InString, PadChar, DefLength)  {
   if (InString.length >= DefLength)
      return (InString);
   OutString = InString
   for (Count = InString.length; Count < DefLength; Count++)  {
      OutString = PadChar + OutString;
   }
   return (OutString);
}


function allDone(mask) 
{
  var doneVal=0;
  mask = parseInt(mask); 
  mask = bitsToMask(mask);
  return mask;
}

//common function to load a file from a url
function loadFromUrl(url){
  var xmlhttp = new XMLHttpRequest();
  xmlhttp.open("GET", url, false);
  xmlhttp.send('');
  var responseText = xmlhttp.responseText;
  var oDomDoc = Sarissa.getDomDocument();
  oDomDoc = (new DOMParser()).parseFromString(responseText, "text/xml");
  return oDomDoc;
};

//shows status div. requires div to be named as statusdiv (see the same in form.css)
function showstatus()
{
  changeDisplay('statusdiv','block');
  setTimeout("changeDisplay('statusdiv','none')", 3000);
}

//common function for getting response message translated using XSLT
function showTranslatedResponse(){
  if(req.responseText.search('Timed Out') > -1){
    //alert("Your session has timed out. Please re-login.");
    return "Your session has timed out. Please re-login by clicking the home button above.";
  }
  if(req.responseXML){
    var root = req.responseXML.documentElement;
    if(root.getElementsByTagName('smsg')[0]){
      return "<span class='ok-msg'>" +root.getElementsByTagName('smsg')[0].firstChild.data+ "</span>";
    }
    if(root.getElementsByTagName('ErrorString')[0]){
      return "<span class='error-msg'>"+ root.getElementsByTagName('ErrorString')[0].firstChild.data+"</span>" ;
    }
    //NOT USING XSLT processing
    /*using sarissa : cross-browser xslt processing
    // get a serializer object which will be used later to convert from xml to text
    var xmlSerializer = new XMLSerializer();
    // create an instance of XSLTProcessor
    var processor = new XSLTProcessor();
    // create a DOM Document containing an XSLT stylesheet
    var xslDoc = loadFromUrl("xsl_gen.xsl");

    // make the stylesheet reusable by importing it in the
    // XSLTProcessor
    processor.importStylesheet(xslDoc);

    //alert("got xsl"+xmlSerializer.serializeToString(xslDoc));
    //processor.setParameter(null, "XML_CONTENT", '<b>Hello</b>');

    // transform the response
    var newDocument = Sarissa.getDomDocument();
    newDocument = processor.transformToDocument(req.responseXML);
    //alert("xml is"+xmlSerializer.serializeToString(req.responseXML));
    return xmlSerializer.serializeToString(newDocument);
     */
  }
}

/* Function to check if a string starts with a particular character sequence */ 
String.prototype.startsWith = function(str)
{return (this.match("^"+str)==str)}

/* Function to check if a string ends with a particular character sequence */ 
String.prototype.endsWith = function(str)
{return (this.match(str+"$")==str)}

/* Function to trim all leading and trailing spaces */
String.prototype.trim = function()
{return(this.replace(/^[\s\xA0]+/, "").replace(/[\s\xA0]+$/, ""))}
