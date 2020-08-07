/*
 Based on EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
*/


///// Ajax Handlers /////

var serverrevision = 0;

function doRequest(id='', value='', callback='') {
   // console.log('Status - doRequest');
   
   var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ajaxstatus) window.ajaxstatus.out();
    req.onload = function() {
       doUpdates(JSON.parse(req.responseText));
       if(window.ajaxstatus) window.ajaxstatus.in();
       if(callback) callback();
    }
    if(id) {         // Why?
       req.onerror = req.ontimeout = function() {
          if(callback) callback();
       }
    }
    // console.log('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);


}

function doUpdates(response) {
   // console.log('Status - doUpdates');

   serverrevision = response.revision;
   var updates = response.updates;
   for(i = 0; i < updates.length; i++) {
      element = document.getElementById(updates[i].id);
      changes = updates[i].changes;

      for(j = 0; j < changes.length; ++j) {
         var spec = changes[j][0].split('.');
         var prop = element;

         if( element ) {      //  Handle missing elements
            for(k = 0; k < (spec.length-1); ++k) {
               prop = prop[spec[k]];
            }
            prop[spec[spec.length-1]] = changes[j][1];
         }
         else if( spec[0]=="embajax_var" ) {      // Handle variable
            window[updates[i].id] = changes[j][1];
         }
         else if( spec[0]=="embajax_func" ) {     // Handle function call
            window[updates[i].id](changes[j][1]);
         }
      }
   }
}

///// Status Indicator /////

class embajaxstatus {
   constructor(div) {
       this.misses = 0;
       this.div = div;
   }
   out() {
       if( this.misses < 5 ) if(++(this.misses) >= 5) this.div.style.background = "red";
   }
   in() {
       if( this.misses > 4 ) this.div.style.background = "green";
       this.misses=0;
   }
} 

ajaxstatus = new embajaxstatus(document.getElementById('EmbAjaxStatusInd'));



///// Page initialization and update /////

// Each page should have updatePage and initPage JS functions

function doPoll() {
   // console.log("Status - Poll");
   doRequest('','',updatePage);
}

initPage();
setInterval(doPoll,1000);
