
var checkObjects	= new Array();
var errorMsg		= new Array();
errorMsg["header"]	= "The following error(s) occured:"
errorMsg["start"]	= "->";
errorMsg["field"]	= " Field ";
errorMsg["mandatory"]	= " is mandatory";
errorMsg["min"]		= " must consist of at least ";
errorMsg["max"]		= " and must not contain more than ";
errorMsg["minmaxnum"]      = " should have a value between "
errorMsg["minmax"]	= " and no more than ";
errorMsg["chars"]	= " characters";
errorMsg["num"]		= " must contain a number";
errorMsg["email"]	= " must contain a valid e-mail address";
errorMsg["ip"]		= " has invalid ip value";

function validate(){
  //alert("checkObjects.length " + checkObjects.length);
  if (checkObjects.length > 0) {
    errorObject = "";
    for (i = 0; i < checkObjects.length; i++) {
       var comp ; 
       comp = document.getElementsByName(checkObjects[i].name);

         validateObject = new Object();
         validateObject.name = checkObjects[i].name;
         validateObject.HTMLName = checkObjects[i].HTMLName;
         //alert("LENGTH " + comp[0].value.length);
         validateObject.comp = comp[0];
         validateObject.val =  comp[0].value;
         validateObject.len = comp[0].value.length;
         validateObject.min = eval(checkObjects[i].min);
         validateObject.max = eval(checkObjects[i].max);
         validateObject.type = checkObjects[i].type;
         validateObject.isMand = checkObjects[i].isMand;

         //alert(validateObject.HTMLName + " is visible "+ isVis); 

         //alert("validating " +validateObject.name);
         if((validateObject.isMand == 'true') && (validateObject.len <= 0)){
            alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['mandatory'] + "\n");
            return false;
         }
        if((validateObject.val != '')&&((validateObject.type == 'STR') || (validateObject.type == 'INT'))){
         //alert(validateObject.name + " field is number" + " " + isNaN(validateObject.val));
         if ((validateObject.type == "INT") && isNaN(validateObject.val)) {
               alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['num'] + "\n");
               return false;
         }else if(validateObject.type =='STR' && validateObject.min && validateObject.max && 
            (validateObject.len < validateObject.min || validateObject.len > validateObject.max)){
             alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['min'] + validateObject.min + errorMsg['minmax'] + 
                   validateObject.max+errorMsg['chars'] + "\n"); 
             return false;
         }else if (validateObject.type == 'INT' && validateObject.min && validateObject.max &&                                                             (validateObject.val < validateObject.min || validateObject.val > validateObject.max)) {
             alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['minmaxnum'] + validateObject.min + 
                 "-" + validateObject.max + "\n");
             return false;
        }
      }else if((validateObject.type == "IPADDR") && (validateObject.val != '')) {
	if((validateObject.val=="0.0.0.0") || (validateObject.val=="255.255.255.255")){
	   alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['ip'] + "\n"); 
           return false;
	}else{
		var ipStr=validateObject.val;
		var tempIPArr = ipStr.split(".");
		var ipFlag = false ;
		if(tempIPArr.length !=4){
			ipFlag = false ;
		}else if( (tempIPArr[0]>0) && (tempIPArr[0]<256) ){
			if( (tempIPArr[1]>=0) && (tempIPArr[1]<256) )
			{
				if( (tempIPArr[2]>=0) && (tempIPArr[2]<256) )
				{
					if( (tempIPArr[3]>=0) && (tempIPArr[3]<256) )
					{
						ipFlag=true;
					}
				}
			}
		}

		if(!(ipFlag)){
                  alert(errorMsg['field'] + validateObject.HTMLName + errorMsg['ip'] + "\n"); 
                  return false;
	    }
	}
      }
    }
  } 
  return true;
}

function define(name,HTMLName,type,range,isMand) {
  var min;
  var max;
  var type;
  //alert("define method is invoked " + range);
  if (range){
   min = range.substring(0,range.indexOf('-')); 
   max = range.substring(range.indexOf('-') +1 ,range.length);
  }
  //alert(name + " field type is : " + type);
  //alert("type : " + type);
  
  eval("V_"+name.replace('#','_')+" = new formResult(name,HTMLName,type, min, max,isMand);");
  checkObjects[eval(checkObjects.length)] = eval("V_"+name.replace('#','_'));
  //alert("checkObjects.length " + checkObjects.length);
}

function formResult(name,HTMLName,type, min, max,isMand) {
this.name = name;
this.HTMLName = HTMLName;
this.type = type;
this.min  = min;
this.max  = max;
this.isMand = isMand;
}

function deRegister(name){
   //alert("checkObjects.length " + checkObjects.length);
   for(var i=0;i<checkObjects.length;i++){
      if(checkObjects[i].name == name)
      {
        //alert("removing " +name + " From registration");
        checkObjects.splice(i,1);
        break;
      }
   }
   //alert("checkObjects.length " + checkObjects.length);
   return;
}
