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
    console.log("Status - Update CSS");

    // DHCP Mode
    var x = document.getElementById("wifi_stn_statics");
    var y = document.getElementById("wifi_stn_dhcp");
    if( y.checked ) x.style.display = "none";
    else x.style.display = "block";

    // Wifi Icons
    x = document.getElementById("wifi_stn1_ctrl");
    y = document.getElementById("wifi_stn1_icon");
    setWifiIcon(y,(x.innerText=="On"));

    x = document.getElementById("wifi_stn2_ctrl");
    y = document.getElementById("wifi_stn2_icon");
    setWifiIcon(y,x.innerText=="On");

    x = document.getElementById("wifi_stn3_ctrl");
    y = document.getElementById("wifi_stn3_icon");
    setWifiIcon(y,x.innerText=="On");

    // Wifi visibility
    x = document.getElementById("wifi_stn1_btn");
    y = document.getElementById("wifi_stn1_icon");
    x.style.display = ( x.innerHTML == "" ) ? "none" : "";
    y.style.display = ( x.innerHTML == "" ) ? "none" : "";

    x = document.getElementById("wifi_stn2_btn");
    y = document.getElementById("wifi_stn2_icon");
    x.style.display = ( x.innerHTML == "" ) ? "none" : "";
    y.style.display = ( x.innerHTML == "" ) ? "none" : "";

    x = document.getElementById("wifi_stn3_btn");
    y = document.getElementById("wifi_stn3_icon");
    x.style.display = ( x.innerHTML == "" ) ? "none" : "";
    y.style.display = ( x.innerHTML == "" ) ? "none" : "";
}

function setWifiIcon(id,state) {
    if( state ) id.style = "opacity: 1.0"
    else id.style = "opacity: 0.2"
}

function loadWifiDialog(wifi_id,id) {
    wifiStationID = id;
    console.log('Status - Load');
    var x = document.getElementById("wifi_stn_pwd");
    doRequest(wifi_id,id,updateCSS);
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function wifiSureYes() {
    console.log('Status - Sure');
    doRequest(sureAction,wifiStationID);
    document.getElementById('wifi_stn_sure').style.display='none';
    document.getElementById('wifi_stn_dlg').style.display='none';
}


class ardujaxshc {
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

ardujaxsh = new ardujaxshc(document.getElementById('EmbAjaxStatus'));



