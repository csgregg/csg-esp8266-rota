// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");

// Are you sure? Action
var sureAction;

// Wifi Station Dialog
var wifiStationID;

// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
    if (mySidebar.style.display == 'block') {
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




function visiblePwd(element) {
    var x = document.getElementById(element);
    if (x.type == "password") {
      x.type = "text";
    } else {
      x.type = "password";
    }
}

function updateCSS() {
    console.log("Update CSS");

    // DHCP Mode
    var x = document.getElementById("wifi_static_show");
    var y = document.getElementById("wifi_dhcp_mode");
    if( y.checked ) x.style.display = "none";
    else x.style.display = "block";

    // Wifi Icons
    x = document.getElementById("wifi_stn1_icon");
    y = document.getElementById("wifi_icon1");
    setWifiIcon(y,(x.innerText=="On"));

    x = document.getElementById("wifi_stn2_icon");
    y = document.getElementById("wifi_icon2");
    setWifiIcon(y,x.innerText=="On");

    x = document.getElementById("wifi_stn3_icon");
    y = document.getElementById("wifi_icon3");
    setWifiIcon(y,x.innerText=="On");

}

function setWifiIcon(id,state) {
    if( state ) id.style = "opacity: 1.0"
    else id.style = "opacity: 0.2"
}

function loadWifiDialog(wifi_id,id) {
    wifiStationID = id;
    console.log('Load');
    doRequest(wifi_id,id,updateCSS);
    document.getElementById('wifi_station').style.display='block';}

function wifiSureYes() {
    console.log('Sure');
    console.log(sureAction);
    doRequest(sureAction,wifiStationID);
    document.getElementById('wifi_station_sure').style.display='none';
    document.getElementById('wifi_station').style.display='none';
}
