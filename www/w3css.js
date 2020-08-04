// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");

// Are you sure? Action
var sureAction;

// Wifi Station Dialog
var wifiStationID;

// Available station
var availWifiStn;

// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
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
    mySidebar.style.display = "none";
    overlayBg.style.display = "none";
}


function addWifiStationEntry() {
    if( window.wifi_stn_name == "" ) {
        document.getElementById("wifi_add_stn_btn").style.display = "";
        document.getElementById("wifi_add_stn_icon").style.display = "";
        return;
    }

    var wifistnelement = document.getElementById("wifi_stn_entry");
    var newwifistn = wifistnelement.cloneNode(true);
    var id = window.wifi_stn_id
    newwifistn.id = wifistnelement.id + id.toString();
    newwifistn.hidden = false;
    
    for (var i = 0; i < newwifistn.children.length; i++) {
        if(newwifistn.children[i].nodeName == "IMG")
            newwifistn.children[i].style = (window.wifi_stn_on=="1") ? "opacity: 1.0" : "opacity: 0.2";
        if(newwifistn.children[i].nodeName == "BUTTON") {
            newwifistn.children[i].textContent = window.wifi_stn_name;
            newwifistn.children[i].setAttribute("onclick","loadWifiDialog("+id.toString()+")");
        }
    }

    wifistnelement.parentElement.appendChild(newwifistn);


}


function initPage() {
    console.log("Init Page");

    loadWifiList();
}


function loadWifiList() {
    for( var i = 0; i < window.wifi_stn_count; i++) {
        doRequest("wifi_stn_id",i.toString(),addWifiStationEntry);
    }
}

function clearWifiList() {
    for( var i = 0; i < window.wifi_stn_count; i++) {
        var wifistn = document.getElementById("wifi_stn_entry" + i.toString());
        if(wifistn ) wifistn.remove();
    }
    document.getElementById("wifi_add_stn_btn").style.display = "none";
    document.getElementById("wifi_add_stn_icon").style.display = "none";
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

    var spare = 0;
    var isspare = false;

    // Wifi visibility
    x = document.getElementById("wifi_stn1_btn");
    y = document.getElementById("wifi_stn1_icon");
    isspare = (x.innerHTML == "");
    x.style.display = isspare ? "none" : "";
    y.style.display = isspare ? "none" : "";
    if( isspare ) {
        spare++;
        availWifiStn = 0;
    }

    x = document.getElementById("wifi_stn2_btn");
    y = document.getElementById("wifi_stn2_icon");
    isspare = (x.innerHTML == "");
    x.style.display = isspare ? "none" : "";
    y.style.display = isspare ? "none" : "";
    if( isspare ) {
        spare++;
        availWifiStn = 1;
    }

    x = document.getElementById("wifi_stn3_btn");
    y = document.getElementById("wifi_stn3_icon");
    isspare = (x.innerHTML == "");
    x.style.display = isspare ? "none" : "";
    y.style.display = isspare ? "none" : "";
    if( isspare ) {
        spare++;
        availWifiStn = 2;
    }

    // Do we show the add button

    
}

function setWifiIcon(id,state) {
    if( state ) id.style = "opacity: 1.0"
    else id.style = "opacity: 0.2"
}

function loadWifiDialog(value) {
    console.log('Status - Load Wifi Station');
    console.log(value);
    wifiStationID = value;
    doRequest("wifi_stn_btn",value,updateCSS);
    document.getElementById('wifi_stn_forget').style.display='block';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function addWifiDialog() {
    sureAction='wifi_stn_save';
    document.getElementById('wifi_stn_ssid').value = '';
    document.getElementById('wifi_stn_pwd').value = '';
    document.getElementById('wifi_stn_dhcp').checked = true;
    document.getElementById('wifi_stn_ip').value = '';
    document.getElementById('wifi_stn_subnet').value = '';
    document.getElementById('wifi_stn_gateway').value = '';
    document.getElementById('wifi_stn_dns1').value = '';
    document.getElementById('wifi_stn_dns2').value = '';
    wifiStationID = availWifiStn;
    updateCSS();
    document.getElementById('wifi_stn_forget').style.display='none';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function wifiSureYes() {

    // Send updates data

    var x = document.getElementById('wifi_stn_ssid');
    // Are we forgetting this one?
    if( sureAction == "wifi_stn_forget" ) x.value = "";
    doRequest(x.id, x.value);

    x = document.getElementById('wifi_stn_pwd');
    doRequest(x.id, x.value);
    x = document.getElementById('wifi_stn_dhcp');
    doRequest(x.id, x.checked ? 't' : 'f');
    x = document.getElementById('wifi_stn_ip');
    doRequest(x.id, x.value);
    x = document.getElementById('wifi_stn_subnet');
    doRequest(x.id, x.value);
    x = document.getElementById('wifi_stn_gateway');
    doRequest(x.id, x.value);
    x = document.getElementById('wifi_stn_dns1');
    doRequest(x.id, x.value);
    x = document.getElementById('wifi_stn_dns2');
    doRequest(x.id, x.value);

    clearWifiList();

    // Send Save command then reload list
    doRequest(sureAction,wifiStationID,loadWifiList);

    // Hide dialog
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



