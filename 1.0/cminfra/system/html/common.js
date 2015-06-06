var stack = new Array();

function more(loc){
   var url = loc;
   stack.push(url);
}

function previous(loc){
   var popVal = this.stack.pop();
   if (undefined != popVal && '' != popVal){
     parent.parent.frameBottom.rightPanel.location.href = popVal;
   }else{
     return;
   }  
}