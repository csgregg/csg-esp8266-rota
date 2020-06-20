// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");

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

// Show Wifi settings tab
function selectWiFiStation(evt, wifiName) {
    var i;
    var x = document.getElementsByClassName("wifistn");
    for (i = 0; i < x.length; i++) {
        x[i].style.display = "none";
    }
    var activebtn = document.getElementsByClassName("testbtn");
    for (i = 0; i < x.length; i++) {
        activebtn[i].className = activebtn[i].className.replace(" w3-dark-grey", "");
    }
    document.getElementById(wifiName).style.display = "block";
    evt.currentTarget.className += " w3-dark-grey";
}

function visiblePwd(wifiPwd) {
    var x = document.getElementById(wifiPwd);
    if (x.type === "password") {
      x.type = "text";
    } else {
      x.type = "password";
    }
}

function visibleIPs(show,label) {
    var dhcpshow = document.getElementById(show);
    var dhcplabel = document.getElementById(label);
    if(dhcpshow.style.display === "block") {
        dhcpshow.style.display = "none";
        dhcplabel.innerHTML = "Dynamic IP";
    }
    else {
        dhcpshow.style.display = "block";
        dhcplabel.innerHTML = "Static IP";
    }
}

var mybtn = document.getElementById("wifi1_ssid");
mybtn.click();
