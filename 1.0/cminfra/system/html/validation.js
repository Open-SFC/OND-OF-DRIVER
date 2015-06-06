
function ltrim ( s ){   return s.replace( /^\s*/, "" );}
function rtrim ( s ){   return s.replace( /\s*$/, "" );}
function trim ( s ){    return rtrim(ltrim(s));}


// Generic Form Validation
// Jacob Hage (jacob@hage.dk)
var checkObjects	= new Array();
var errors		= "";
var returnVal		= false;
var language		= new Array();
language["header"]	= "The following error(s) occured:"
language["start"]	= "->";
language["field"]	= " Field ";
language["require"]	= " is required";
language["min"]		= " and must consist of at least ";
language["max"]		= " and must not contain more than ";
language["minmax"]	= " and no more than ";
language["chars"]	= " characters";
language["num"]		= " and must contain a number";
language["email"]	= " must contain a valid e-mail address";
language["ip"]		= " invalid address ";
// -----------------------------------------------------------------------------
// define - Call this function in the beginning of the page. I.e. onLoad.
// n = name of the input field (Required)
// type= string, num, email (Required)
// min = the value must have at least [min] characters (Optional)
// max = the value must have maximum [max] characters (Optional)
// d = (Optional)
// -----------------------------------------------------------------------------
function define(n, type, HTMLname, min, max, d) {
var p;
var i;
var x;
if (!d) d = document;
if ((p=n.indexOf("?"))>0&&parent.frames.length) {
d = parent.frames[n.substring(p+1)].document;
n = n.substring(0,p);
}
if (!(x = d[n]) && d.all) x = d.all[n];
for (i = 0; !x && i < d.forms.length; i++) {
x = d.forms[i][n];
}
for (i = 0; !x && d.layers && i < d.layers.length; i++) {
x = define(n, type, HTMLname, min, max, d.layers[i].document);
return x;       
}
eval("V_"+n+" = new formResult(x, type, HTMLname, min, max);");
checkObjects[eval(checkObjects.length)] = eval("V_"+n);
}
function formResult(form, type, HTMLname, min, max) {
this.form = form;
this.type = type;
this.HTMLname = HTMLname;
this.min  = min;
this.max  = max;
}
function validate() {
if (checkObjects.length > 0) {
errorObject = "";
for (i = 0; i < checkObjects.length; i++) {
validateObject = new Object();
validateObject.form = checkObjects[i].form;
validateObject.HTMLname = checkObjects[i].HTMLname;
validateObject.val = trim(checkObjects[i].form.value);
validateObject.len = checkObjects[i].form.value.length;
validateObject.min = checkObjects[i].min;
validateObject.max = checkObjects[i].max;
validateObject.type = checkObjects[i].type;
if (validateObject.type == "num" || validateObject.type == "string") {
if ((validateObject.type == "num" && validateObject.len <= 0) || (validateObject.type == "num" && isNaN(validateObject.val))) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['require'] + language['num'] + "\n";
} else if (validateObject.min && validateObject.max && (validateObject.len < validateObject.min || validateObject.len > validateObject.max)) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['require'] + language['min'] + validateObject.min + language['minmax'] + validateObject.max+language['chars'] + "\n";
} else if (validateObject.min && !validateObject.max && (validateObject.len < validateObject.min)) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['require'] + language['min'] + validateObject.min + language['chars'] + "\n";
} else if (validateObject.max && !validateObject.min &&(validateObject.len > validateObject.max)) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['require'] + language['max'] + validateObject.max + language['chars'] + "\n";
} else if (!validateObject.min && !validateObject.max && validateObject.len <= 0) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['require'] + "\n";
   }
} 
else if(validateObject.type == "ip") {
	if((validateObject.val=="0.0.0.0") || (validateObject.val=="255.255.255.255")){
		errors += language['start'] + language['field'] + validateObject.HTMLname + language['ip'] + "\n"; 
	}
	else{
		var ipStr=validateObject.val;
		var tempIPArr = ipStr.split(".");
		var ipFlag = false ;
		if(tempIPArr.length !=4){
			ipFlag = false ;
		}
		else
		if( (tempIPArr[0]>0) && (tempIPArr[0]<256) )
		{
			if( (tempIPArr[1]>=0) && (tempIPArr[1]<256) )
			{
				if( (tempIPArr[2]>=0) && (tempIPArr[2]<256) )
				{
					if( (tempIPArr[3]>=0) && (tempIPArr[3]<256) )
					{
						ipFlag=true;
						break;
					}
				}
			}
		}

		if(!(ipFlag)){
			errors += language['start'] + language['field'] + validateObject.HTMLname + language['ip'] + "\n"; 
	    }
	}
}
else if(validateObject.type == "email") {
// Checking existense of "@" and ".". 
// Length of must >= 5 and the "." must 
// not directly precede or follow the "@"
if ((validateObject.val.indexOf("@") == -1) || (validateObject.val.charAt(0) == ".") || (validateObject.val.charAt(0) == "@") || (validateObject.len < 6) || (validateObject.val.indexOf(".") == -1) || (validateObject.val.charAt(validateObject.val.indexOf("@")+1) == ".") || (validateObject.val.charAt(validateObject.val.indexOf("@")-1) == ".")) { errors += language['start'] + language['field'] + validateObject.HTMLname + language['email'] + "\n"; }
      }
   }
}
if (errors) {
alert(language["header"].concat("\n" + errors));
errors = "";
returnVal = false;
} else {
returnVal = true;
   }
return returnVal;
}

function extractNumber(obj, decimalPlaces, allowNegative)
{
	var temp = obj.value;
	
	// avoid changing things if already formatted correctly
	var reg0Str = '[0-9]*';
	if (decimalPlaces > 0) {
		reg0Str += '\\.?[0-9]{0,' + decimalPlaces + '}';
	} else if (decimalPlaces < 0) {
		reg0Str += '\\.?[0-9]*';
	}
	reg0Str = allowNegative ? '^-?' + reg0Str : '^' + reg0Str;
	reg0Str = reg0Str + '$';
	var reg0 = new RegExp(reg0Str);
	if (reg0.test(temp)) return true;

	// first replace all non numbers
	var reg1Str = '[^0-9' + (decimalPlaces != 0 ? '.' : '') + (allowNegative ? '-' : '') + ']';
	var reg1 = new RegExp(reg1Str, 'g');
	temp = temp.replace(reg1, '');

	if (allowNegative) {
		// replace extra negative
		var hasNegative = temp.length > 0 && temp.charAt(0) == '-';
		var reg2 = /-/g;
		temp = temp.replace(reg2, '');
		if (hasNegative) temp = '-' + temp;
	}
	
	if (decimalPlaces != 0) {
		var reg3 = /\./g;
		var reg3Array = reg3.exec(temp);
		if (reg3Array != null) {
			// keep only first occurrence of .
			//  and the number of places specified by decimalPlaces or the entire string if decimalPlaces < 0
			var reg3Right = temp.substring(reg3Array.index + reg3Array[0].length);
			reg3Right = reg3Right.replace(reg3, '');
			reg3Right = decimalPlaces > 0 ? reg3Right.substring(0, decimalPlaces) : reg3Right;
			temp = temp.substring(0,reg3Array.index) + '.' + reg3Right;
		}
	}
	
	obj.value = temp;
}
function blockNonNumbers(obj, e, allowDecimal, allowNegative)
{
	var key;
	var isCtrl = false;
	var keychar;
	var reg;
		
	if(window.event) {
		key = e.keyCode;
		isCtrl = window.event.ctrlKey
	}
	else if(e.which) {
		key = e.which;
		isCtrl = e.ctrlKey;
	}
	
	if (isNaN(key)) return true;
	
	keychar = String.fromCharCode(key);
	
	// check for backspace or delete, or if Ctrl was pressed
	if (key == 8 || isCtrl)
	{
		return true;
	}

	reg = /\d/;
	var isFirstN = allowNegative ? keychar == '-' && obj.value.indexOf('-') == -1 : false;
	var isFirstD = allowDecimal ? keychar == '.' && obj.value.indexOf('.') == -1 : false;
	
	return isFirstN || isFirstD || reg.test(keychar);
}
function checkDomain(nname)
{
var arr = new Array(
'.com','.net','.org','.biz','.coop','.info','.museum','.name',
'.pro','.edu','.gov','.int','.mil','.ac','.ad','.ae','.af','.ag',
'.ai','.al','.am','.an','.ao','.aq','.ar','.as','.at','.au','.aw',
'.az','.ba','.bb','.bd','.be','.bf','.bg','.bh','.bi','.bj','.bm',
'.bn','.bo','.br','.bs','.bt','.bv','.bw','.by','.bz','.ca','.cc',
'.cd','.cf','.cg','.ch','.ci','.ck','.cl','.cm','.cn','.co','.cr',
'.cu','.cv','.cx','.cy','.cz','.de','.dj','.dk','.dm','.do','.dz',
'.ec','.ee','.eg','.eh','.er','.es','.et','.fi','.fj','.fk','.fm',
'.fo','.fr','.ga','.gd','.ge','.gf','.gg','.gh','.gi','.gl','.gm',
'.gn','.gp','.gq','.gr','.gs','.gt','.gu','.gv','.gy','.hk','.hm',
'.hn','.hr','.ht','.hu','.id','.ie','.il','.im','.in','.io','.iq',
'.ir','.is','.it','.je','.jm','.jo','.jp','.ke','.kg','.kh','.ki',
'.km','.kn','.kp','.kr','.kw','.ky','.kz','.la','.lb','.lc','.li',
'.lk','.lr','.ls','.lt','.lu','.lv','.ly','.ma','.mc','.md','.mg',
'.mh','.mk','.ml','.mm','.mn','.mo','.mp','.mq','.mr','.ms','.mt',
'.mu','.mv','.mw','.mx','.my','.mz','.na','.nc','.ne','.nf','.ng',
'.ni','.nl','.no','.np','.nr','.nu','.nz','.om','.pa','.pe','.pf',
'.pg','.ph','.pk','.pl','.pm','.pn','.pr','.ps','.pt','.pw','.py',
'.qa','.re','.ro','.rw','.ru','.sa','.sb','.sc','.sd','.se','.sg',
'.sh','.si','.sj','.sk','.sl','.sm','.sn','.so','.sr','.st','.sv',
'.sy','.sz','.tc','.td','.tf','.tg','.th','.tj','.tk','.tm','.tn',
'.to','.tp','.tr','.tt','.tv','.tw','.tz','.ua','.ug','.uk','.um',
'.us','.uy','.uz','.va','.vc','.ve','.vg','.vi','.vn','.vu','.ws',
'.wf','.ye','.yt','.yu','.za','.zm','.zw');

var mai = nname;
var val = true;

var dot = mai.lastIndexOf(".");
var dname = mai.substring(0,dot);
var ext = mai.substring(dot,mai.length);
//alert(ext);
if(mai=='')
{
  alert("Domain Name field should not be empty!");
  return false;
}
	
if(dot>2 && dot<57)
{
	for(var i=0; i<arr.length; i++)
	{
	  if(ext == arr[i])
	  {
	 	val = true;
		break;
	  }	
	  else
	  {
	 	val = false;
	  }
	}
	if(val == false)
	{
	  	 alert("Your domain extension "+ext+" is not correct");
		 return false;
	}
	else
	{
		for(var j=0; j<dname.length; j++)
		{
		  var dh = dname.charAt(j);
		  var hh = dh.charCodeAt(0);
		  if((hh > 47 && hh<59) || (hh > 64 && hh<91) || (hh > 96 && hh<123) || hh==45 || hh==46)
		  {
			 if((j==0 || j==dname.length-1) && hh == 45)	
		  	 {
		 	  	 alert("Domain name should not begin are end with '-'");
			      return false;
		 	 }
		  }
		else	{
		  	 alert("Your domain name should not have special characters");
			 return false;
		  }
		}
	}
}
else
{
  if(dot==-1){
    alert("Domain Name is Invalid!");
    return false;
  }
  else{
      alert("Your Domain name is too long");
      return false;
  }
}	

return true;
}


/*
function isValidIPv6Address(ipaddr) {

// check if there is a '::' in the field
var parts = ipaddr.split("::");

if(parts.length == 1) {

// there is NO occurence of '::'
var re11 = /^([0]\:){6}\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}(\/\d{1,2})?$/;
if (re11.test(ipaddr)) {
// IPv4 address in IPv6 address 0:0:0:0:0:0:n.n.n.n/nn e.g. 0:0:0:0:0:0:10.10.10.10/24
var ipv4parts = ipaddr.split(":");
return isValidIPv4Address(ipv4parts[6]);
}

// Pure IPv6 address
var re12 = /^[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}\:[A-Fa-f0-9]{1,4}(\/\d{1,2})?$/;
if (re12.test(ipaddr)) return true;
else return false;
}

if(parts.length == 2) {

// there is one occurence of '::'
if (parts[0] == "") {
// IPv4 address in IPv6 address ::n.n.n.n/nn e.g. :10.10.27.127/24
return isValidIPv4Address(parts[1]);
}

// Pure IPv6 address

// check first part
var re21 = /^[A-Fa-f0-9]{1,4}(\:[A-Fa-f0-9]{1x,4}){0,5}$/;
if(!re21.test(parts[0])) {
return false
}

// check second part
var re22 = /^([A-Fa-f0-9]{1,4}\:){0,5}[A-Fa-f0-9]{1,4}(\/\d{1,2})?$/
if(!re22.test(parts[1])) {
return false;
}
return true;
}

// there is more that one occurence of '::'
return false;

} // end function
*/
function specialChar(id,cmt)
{
  var iChars = "!@#$%^&*()+=-[]\\\';,./{}|\":<>?";

    for (var i = 0; i < id.value.length; i++) {
              if (iChars.indexOf(id.value.charAt(i)) != -1) {        alert ("Your "+cmt+" has special characters. \nThese are not allowed.\n Please remove them and try again.");
                        return false;
                                }
                }
    return true;
} 
            function checkInt(id)
		{	
		var testresult=false;	
		var x=id.value;
			var anum=/(^\d+$)|(^\d+\.\d+$)/

			if(id.value!="")
			{
			
				if (anum.test(x))
				{
				testresult=true;
				}
				if(!testresult)
				{
					alert("Please enter a valid number!");
					id.value="";
					this.focus();
					return false;
				}
			}
		}

   function checkEmail(f)
   {
	 if (f.value.length >0) {
	 i=f.value.indexOf("@")
	 j=f.value.indexOf(".",i)
	 k=f.value.indexOf(",")
	 kk=f.value.indexOf(" ")
	 jj=f.value.lastIndexOf(".")+1
	 len=f.value.length

 	if ((i>0) && (j>(1+1)) && (k==-1) && (kk==-1) && (len-jj >=2) && (len-jj<=3)) {
		return true;
 	}
 	else {
 		alert("Please enter an exact email address.\n" +
		f.value + " is invalid.");
		return false;
 	}

 }
   }
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


