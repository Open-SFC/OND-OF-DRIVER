
function delFCookie()
{
  if(getCookie("File-Name")!='')
  {
    delCookie("saved_myform");
    delCookie("File-Name");
  }
}

function ltrim ( s ){   return s.replace( /^\s*/, "" );}
function rtrim ( s ){   return s.replace( /\s*$/, "" );}
       function setFName()
      {
        TextSpaceCheck("myform");
        var url = document.location.href;
        var fname = url.split("/")[(url.split("/").length)-1];
        setCookie("File-Name",fname,0);
        cookieForms("myform","save");
      }
      function delFName()
      {
        if(getCookie("File-Name")!='')
        {
          cookieForms("myform","load");
          delCookie("saved_myform");
          delCookie("File-Name");
        }
      }
function getexpirydateUTC(nodays)
{
  Today = new Date();
  milli = Date.parse(Today);
  Today.setTime(milli+ nodays*24*60*60*1000);
  return Today.toUTCString();
}

function setCookie(name, value, days)
{
  cstr = value;


if(cstr.length < 2600){  // As cookies max value for I.E. is 2667 and for Mozilla 4084 
	cstr = name+"="+cstr;
  if (days != 0)  // 0 means valid for current session only
     cstr += ";EXPIRES=" + getexpirydateUTC(days);	
	document.cookie = cstr;
	}
	else{  // As cookie max value for I.E. is 2667 and for Mozilla 4084 
 	 var noOfFormParts = 0; 
	 var cstrPart = '';
	 for(var i=0; i<= cstr.length; i=i+2600)
	 {		
	  if(cstr.length < i+2600)
	  cstrPart = cstr.substring(i);
	   else {
	   cstrPart = cstr.substring(i,i+2600); }
			noOfFormParts = noOfFormParts + 1;
			cstrPart = name+noOfFormParts+"="+cstrPart;
  if (days != 0)  // 0 means valid for current session only
     cstr += ";EXPIRES=" + getexpirydateUTC(days);	
			document.cookie = cstrPart;		
		}	
		cstrPart =  "NoOfFormParts="+noOfFormParts; //In how many parts the form data is divided
		document.cookie = cstrPart;		
	}  // end of else		
	 if (getCookie(name)=="") return false;
	return true;
 
}
function delCookie(name)
{
  var cstr = "" + document.cookie;
  var index1 = cstr.indexOf(name);
  if (name=="" || index1== -1) return;
  setCookie(name, "", -1); // set expiry date in the past
}
var hostCookieStr = getCookie('hostInfo');
var snetCookieStr = getCookie('Name-SNET');
//alert("hostCookieStr: " + hostCookieStr);
//alert("snetCookieStr: " + snetCookieStr);
if (hostCookieStr == "") {
    setCookie("hostInfo", "MASTER", 1);
}
if (snetCookieStr == "") {
    setCookie("Name-SNET", "general", 1);
}

function getCookie(name)
{
 var cstr = "" + document.cookie;
 var index1 = cstr.indexOf(name);

 if (name=="" || index1== -1) return "";

 var index2 = cstr.indexOf( ';' , index1);
 if (index2 == -1) index2 = cstr.length;

 return unescape(cstr.substring(index1+name.length+1, index2));
}


// cookieForms saves form content of a page.
// use the following code to call it:
//  <body onLoad="cookieForms('form_1', 'load')" onUnLoad="cookieForms('form1','save')">
// It works on text fields and dropdowns in IE 5+
// It only works on text fields in Netscape 4.5


function cookieForms(formName, mode) 
{  
  mfrm = eval("document." + formName);
  len = mfrm.elements.length;
  var cookieValue = '';
  if(mode == 'load') {		
   //alert(" MOAD IS LOAD...");
  if(getCookie("NoOfFormParts") != ""){  
   //alert("NoOfFormParts = "+getCookie("NoOfFormParts"));
 // if cookie max value exceed its limit get remaing value from "saved_formName1"			
   for(var i=1; i<= getCookie("NoOfFormParts"); i=i+1)
	cookieValue += getCookie('saved_'+formName + i);
	}
	else
	{
	   cookieValue = getCookie('saved_'+formName);
	}		

	if(cookieValue != null) {
        var cookieArray = cookieValue.split('#cf#');
	if(cookieArray.length == mfrm.elements.length) {
	for(i=0; i<mfrm.elements.length; i++) {
        if(cookieArray[i].substring(0,6) == 'select') {
             mfrm.elements[i].options.selectedIndex = parseInt(cookieArray[i].substring(6, cookieArray[i].length),10); 
        }
        else  if(cookieArray[i].substring(0,6) == 'selmul') {
         cookieArray[i] = cookieArray[i].substring(6,cookieArray[i].length); 
         if(cookieArray[i].substring(cookieArray[i].length,cookieArray[i].length-1)=="*")
         {
          cookieArray[i] = cookieArray[i].substring(0,cookieArray[i].length-1); 
          for(var j=0;j<cookieArray[i].split('*').length;j++)
          {
          /*
           if(j<=mfrm.elements[i].options.length)
           {
             break;
           }
           */
           var x =parseInt(cookieArray[i].split('*')[j],10); 
           if(mfrm.elements[i].options[x])
             mfrm.elements[i].options[x].selected = true; 
          }   
        }  
   } 
   else if((cookieArray[i] == 'cbtrue') || (cookieArray[i] == 'rbtrue')) { mfrm.elements[i].checked = true; }

   else if((cookieArray[i] == 'cbfalse') || (cookieArray[i] == 'rbfalse')) { mfrm.elements[i].checked = false; }
					
	else { if(mfrm.elements[i].type != "file") { mfrm.elements[i].value = (cookieArray[i]) ? cookieArray[i] : ''; } }

      }
     }
   }
} // load

    if(mode == 'save') {	
      cookieValue = '';	
      for(i=0; i<mfrm.elements.length; i++) {
	   fieldType = mfrm.elements[i].type;
      if(fieldType == 'password') { passValue = ''; }

      else if(fieldType == 'checkbox') { passValue = 'cb'+mfrm.elements[i].checked; }

      else if(fieldType == 'radio') { passValue = 'rb'+mfrm.elements[i].checked; }

      else if(fieldType == 'select-one') { passValue = 'select'+mfrm.elements[i].options.selectedIndex; }
    
     else if(fieldType == 'select-multiple'){
           passValue='selmul';
      for(var k=0;k<mfrm.elements[i].options.length;k++){ 
          if(mfrm.elements[i].options[k].selected)
          {
               passValue+=k+'*';
           }
       }      
      }  
      else { passValue = mfrm.elements[i].value; }

		cookieValue = cookieValue + passValue + '#cf#';
      } // for

   cookieValue = cookieValue.substring(0, cookieValue.length-4); // Remove last delimiter
    setCookie('saved_'+formName, cookieValue, 0);		
   } // save

}

function TextSpaceCheck(formName)
{
    mfrm = eval("document." + formName);
    for(i=0; i<mfrm.elements.length; i++) {
    fieldType = mfrm.elements[i].type;
    if(fieldType == 'text'){
      if(mfrm.elements[i].value.substring(mfrm.elements[i].value.length-1) == ' ')
      {
        mfrm.elements[i].value = rtrim(mfrm.elements[i].value);
      }
      if(mfrm.elements[i].value.substring(0,1) == ' ') { 
     mfrm.elements[i].value = ltrim(mfrm.elements[i].value);
     }
     var flag = mfrm.elements[i].value.indexOf('&amp;');
     if(flag==-1)
	mfrm.elements[i].value = mfrm.elements[i].value.replace(/&/gi,'&amp;');
	mfrm.elements[i].value = mfrm.elements[i].value.replace(/</gi,'&lt;');
        mfrm.elements[i].value = mfrm.elements[i].value.replace(/>/gi,'&gt;');

    }

	
    }
}
//  End -->
