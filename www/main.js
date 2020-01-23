/*
   Page Include Support
   From w3.css
*/

// Handle page includes
function includeHTML() {
   var z, i, elmnt, file, xhttp;
   /* Loop through a collection of all HTML elements: */
   z = document.getElementsByTagName("*");
   for (i = 0; i < z.length; i++) {
       elmnt = z[i];
       /*search for elements with a certain atrribute:*/
       file = elmnt.getAttribute("w3-include-html");
       if (file) {
           /* Make an HTTP request using the attribute value as the file name: */
           xhttp = new XMLHttpRequest();
           xhttp.onreadystatechange = function() {
               if (this.readyState == 4) {
                   if (this.status == 200) {elmnt.innerHTML = this.responseText;}
                   if (this.status == 404) {elmnt.innerHTML = "Page not found.";}
                   /* Remove the attribute, and call this function once more: */
                   elmnt.removeAttribute("w3-include-html");
                   includeHTML();
               }
           }
           xhttp.open("GET", file, true);
           xhttp.send();
           /* Exit the function: */
           return;
       }
   }
}



/*
   Menu Support
   From w3.css
*/

// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
   if (mySidebar.style.display === 'block') {
       mySidebar.style.display = 'none';
       overlayBg.style.display = "none";
   } else {
       mySidebar.style.display = 'block';
       overlayBg.style.display = "block";
   }
}

// Close the sidebar with the close button
function w3_close() {
   mySidebar.style.display = "none";
   overlayBg.style.display = "none";
}

/*
   AJAX Support
   From EmbAJAX Library - https://github.com/tfry-git/EmbAJAX
*/

// Send requests back to AJAX handers
var serverrevision = 0;
function doRequest(id='', value='') {
    var req = new XMLHttpRequest();
    req.timeout = 10000;
    if(window.ardujaxsh) window.ardujaxsh.out();
    req.onload = function() {
       doUpdates(JSON.parse(req.responseText));
       if(window.ardujaxsh) window.ardujaxsh.in();
    }
    if(id) {
       req.onerror = req.ontimeout = function() {
          serverrevision = 0;
       }
    }
    req.open('POST', document.URL, true);
    req.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    req.send('id=' + id + '&value=' + encodeURIComponent(value) + '&revision=' + serverrevision);
}

// Get updates from AJAX handers
function doUpdates(response) {
    serverrevision = response.revision;
    var updates = response.updates;
    for(i = 0; i < updates.length; i++) {
       element = document.getElementById(updates[i].id);
       changes = updates[i].changes;
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

// Set up regular updates
function doPoll() {
    doRequest();
}



/*
   Main page load function to be called on all pages
*/


function mainpageload() {

   // Handle includes
   includeHTML();

   // Setup request handling
   setInterval(doPoll,1000);

}

