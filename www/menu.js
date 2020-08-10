// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
    // Get the Sidebar
    var mySidebar = document.getElementById("mySidebar");

    // Highlight menu item
    var url = window.location.pathname;
    var hrefname = "a[href='" + url.substring(url.lastIndexOf('/')+1) + "']";   
    var menuitem = mySidebar.querySelectorAll(hrefname);
    menuitem[0].classList.add("w3-blue");

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
function w3_close() {
    // Get the Sidebar
    var mySidebar = document.getElementById("mySidebar");

    // Get the DIV with overlay effect
    var overlayBg = document.getElementById("myOverlay");
    
    mySidebar.style.display = "none";
    overlayBg.style.display = "none";
}

