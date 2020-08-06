/*
 From EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
*/

var serverrevision = 0;

function doRequest(id='', value='', callback='') {
   console.log('Status - doRequest');
   console.log(serverrevision);
    var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ardujaxsh) window.ardujaxsh.out();
    req.onload = function() {
       doUpdates(JSON.parse(req.responseText));
       if(window.ardujaxsh) window.ardujaxsh.in();
       if(callback) callback();
    }
    if(id) {         // Why?
       req.onerror = req.ontimeout = function() {
          if(callback) callback();
       }
    }
    console.log('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
}

function doUpdates(response) {
   console.log('Status - doUpdates');
   console.log(serverrevision);
   serverrevision = response.revision;
   var updates = response.updates;
   for(i = 0; i < updates.length; i++) {
      element = document.getElementById(updates[i].id);
      changes = updates[i].changes;
      console.log(updates[i].id);
      //console.log(changes);
      for(j = 0; j < changes.length; ++j) {
         var spec = changes[j][0].split('.');
         var prop = element;
         //console.log(spec[0]);

         if( element ) {      //  Handle missing elements
            for(k = 0; k < (spec.length-1); ++k) {
               prop = prop[spec[k]];
            }
            prop[spec[spec.length-1]] = changes[j][1];
         }
         else if( spec[0]=="embajax_var" ) {      // Handle variable
            //console.log(changes[j][1]);
            window[updates[i].id] = changes[j][1];
         }
         else if( spec[0]=="embajax_func" ) {     // Handle function call
            console.log(serverrevision);
            console.log(changes);
            window[updates[i].id](changes[j][1]);
         }
      }
   }
}

function doPoll() {
   console.log("Poll");
   doRequest('','',updatePage);
}


initPage();
setInterval(doPoll,1000);
