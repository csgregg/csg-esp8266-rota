/*
 From EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
*/

var serverrevision = 0;

function doRequest(id='', value='', callback='') {
   console.log('Status - doRequest');
    var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ardujaxsh) window.ardujaxsh.out();
    req.onload = function() {
       doUpdates(JSON.parse(req.responseText));
       if(window.ardujaxsh) window.ardujaxsh.in();
       if(callback) callback();
    }
    if(id) {
       req.onerror = req.ontimeout = function() {
          serverrevision = 0;
       }
    }
//    console.log('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
}

function doUpdates(response) {
   console.log('Status - doUpdates');
    serverrevision = response.revision;
    var updates = response.updates;
    for(i = 0; i < updates.length; i++) {
       element = document.getElementById(updates[i].id);
       changes = updates[i].changes;
//       console.log(changes);
       for(j = 0; j < changes.length; ++j) {
          var spec = changes[j][0].split('.');
          var prop = element;
          for(k = 0; k < (spec.length-1); ++k) {
              prop = prop[spec[k]];
          }
          prop[spec[spec.length-1]] = changes[j][1];
       }
    }
}

function doPoll() {
    doRequest('','',updateCSS);
}

doPoll();
setInterval(doPoll,1000);
