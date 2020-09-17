


function SureDlg(action) {
   sureAction = action.id;
   document.getElementById('sure_dlg').style.display='block';
}



function reloadPage() {
   location.reload();
}


function clearLoader() {
   document.getElementById('loader').style.display='none';
}

function showLoader() {
   document.getElementById('loader').style.display='block';
}


function clearSure() {
   document.getElementById('sure_dlg').style.display='none';
}

function showSure() {
   document.getElementById('sure_dlg').style.display='block';
}

///// Menu functions /////


function menu_highlight() {
   var mySidebar = document.getElementById("mySidebar");
   // Highlight menu item
   var url = window.location.pathname;
   var hrefname = "a[href='" + url.substring(url.lastIndexOf('/')+1) + "']";   
   if( hrefname == "a[href='']" ) hrefname = "a[href='index.html']";     // Catch error if / 
   var menuitem = mySidebar.querySelectorAll(hrefname);
   menuitem[0].classList.add("w3-blue");
}



// Toggle between showing and hiding the sidebar, and add overlay effect
function menu_open() {
   // Get the Sidebar
   var mySidebar = document.getElementById("mySidebar");

   menu_highlight();

   // Get the DIV with overlay effect
   var overlayBg = document.getElementById("myOverlay");

   if (mySidebar.style.display == 'block') {
       mySidebar.style.display = 'none';
       overlayBg.style.display = "none";
   } else {
       mySidebar.style.display = 'block';
       overlayBg.style.display = "block";
   }
   doPoll();
}

// Close the sidebar with the close button
function menu_close() {
   // Get the Sidebar
   var mySidebar = document.getElementById("mySidebar");

   // Get the DIV with overlay effect
   var overlayBg = document.getElementById("myOverlay");
   
   mySidebar.style.display = "none";
   overlayBg.style.display = "none";
}



///// Ajax Handlers /////

/*
 Based on EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
*/

var serverrevision = 0;

function doRequest(id='', value='', callback='') {
   // console.log('Status - doRequest');
   
   var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ajaxstatus) window.ajaxstatus.out();
    req.onload = function() {
       // console.log(req.responseText);
       doUpdates(JSON.parse(req.responseText));
       if(window.ajaxstatus) window.ajaxstatus.in();
       if(callback) callback();
    }
    if(id) {         // TODO - Why?
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
       if( this.misses > 4 ) {
          this.div.style.background = "green";
          initPage();
       }
       this.misses=0;
   }
} 


function doPoll() {
   // console.log("Status - Poll");
   doRequest('','',updatePage);
}

 
///// Page initialization /////

// Each page should have updatePage and initPage JS functions

ajaxstatus = new embajaxstatus(document.getElementById('EmbAjaxStatusInd'));
initPage();
menu_highlight();
doPoll();
setInterval(doPoll,1000);
