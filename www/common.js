
// Available station
var net_status;
var loader_element=document.getElementById('device_box');


// Helper function to get an element's exact position
function getPosition(el) {
   var xPos = 0;
   var yPos = 0;
  
   while (el) {
     if (el.tagName == "BODY") {
       // deal with browser quirks with body/window/document and page scroll
       var xScroll = el.scrollLeft || document.documentElement.scrollLeft;
       var yScroll = el.scrollTop || document.documentElement.scrollTop;
  
       xPos += (el.offsetLeft - xScroll + el.clientLeft);
       yPos += (el.offsetTop - yScroll + el.clientTop);
     } else {
       // for all other non-BODY elements
       xPos += (el.offsetLeft - el.scrollLeft + el.clientLeft);
       yPos += (el.offsetTop - el.scrollTop + el.clientTop);
     }
  
     el = el.offsetParent;
   }
   return {
     x: xPos,
     y: yPos
   };
 }


  
 // deal with the page getting resized or scrolled
 window.addEventListener("scroll", updateLoaderPosition, false);
 window.addEventListener("resize", updateLoaderPosition, false);
 document.getElementById('mySidebar').addEventListener("DOMAttrModified", updateLoaderPosition, false);

 function updateLoaderPosition() {
   if( loader_element ) {
      var loader = document.getElementById('loader');
      var pos = getPosition(loader_element);
      var width = loader_element.offsetWidth;
      var display = loader.style.display;

      loader.style.left = (pos.x+width-60)+"px";
      loader.style.top = (pos.y-22)+"px";
      loader.style.display = display;
   }
 }



function ValidateIPaddress(inputText,error) {
   var ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
   if(inputText.value.match(ipformat)) {
      return true;
   }
   else
   {
      alert(error);
      return false;
   }
}


// Common messsage popup
function post_message(value) {
   if(value != "") {
      document.getElementById('message_text').innerText = value;
      document.getElementById('message_bar').classList.remove("w3-hide");
   }
   else {
      document.getElementById('message_bar').classList.add("w3-hide");
   }
}
function message_ack() {
   doRequest("post_message","");     // Acknowledge done
}



function SureDlg(action) {
   if( typeof action === "string" ) sureAction = action;
   else sureAction = action.id;
   document.getElementById('sure_dlg').style.display='block';
}

function reloadPage() {
   location.reload();
}

function ClearLoader() {
   document.getElementById('loader').style.display='none';
}

function showLoader(element) {
   loader_element = element;
   updateLoaderPosition();
   document.getElementById('loader').style.display='block';
}

function clearSure() {
   document.getElementById('sure_dlg').style.display='none';
}

function enableElmt(...elmts) {
   for( var i=0; i < elmts.length; i++ ) {
      document.getElementById(elmts[i]).disabled = false;
      document.getElementById(elmts[i]).classList.remove('w3-disabled');
   }
}

function disableElmt(...elmts) {
   for( var i=0; i < elmts.length; i++ ) {
      document.getElementById(elmts[i]).disabled = true;
      document.getElementById(elmts[i]).classList.add('w3-disabled');
   }
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
    req.timeout = 5000;
    if(window.ajaxstatus) window.ajaxstatus.out();
    req.onreadystatechange = function() {
      if (req.readyState == 4){   //if complete
          if(req.status == 200){  //check if "OK"
            // console.log(req.responseText);
            doUpdates(JSON.parse(req.responseText));
            if(window.ajaxstatus) window.ajaxstatus.in();
            if( id=='' ) window.ajaxstatus.intStatus();
         }
         if(callback) callback();
      } 
    }
    // console.log('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
}


function doRequestWait(id='', value='') {
   // console.log('Status - doRequestWait');

   var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ajaxstatus) window.ajaxstatus.out();
    req.onreadystatechange = function() {
      if (req.readyState == 4){   //if complete
          if(req.status == 200){  //check if "OK"
            // console.log(req.responseText);
            if(window.ajaxstatus) window.ajaxstatus.in();
         }
      } 
    }
    // console.log('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);

}


function doRequestAll(callback) {
   serverrevision = 50000;       // Force reload all
   doRequest('','',callback);
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
       this.flash = false;
   }
   out() {
       if( this.misses < 5 ) if(++(this.misses) >= 5) this.div.style.background = "red";
   }
   in() {
       if( this.misses > 4 ) {
          this.div.style.background = "orange";
          initPage();
       }
       this.misses=0;
   }
   intStatus() {
      if( this.misses <= 1 ) {
         switch(Number(window.net_status)) {
            case 2:        // WiFi connected, no internet
               if( this.flash ) this.div.style.background = "orange";
               else this.div.style.background = "darkorange";
               break;
            case 1:        // Connected to internet
               if( this.flash ) this.div.style.background = "green";
               else this.div.style.background = "rgb(0, 150, 0)";
               break;
            default:       // Network disconnected
               this.div.style.background = "red";
          }
          this.flash = !this.flash;
      }
   }
} 



function doPoll() {
   //console.log("Status - Poll");
   
   doRequest('','',updatePage);  
}

 
///// Page initialization /////

// Each page should have updatePage and initPage JS functions

ajaxstatus = new embajaxstatus(document.getElementById('EmbAjaxStatusInd'));
initPage();
menu_highlight();
setInterval(doPoll,1000);
