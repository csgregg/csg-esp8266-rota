// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");

// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");


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



// TODO - Classify this 

// Are you sure? Action
var sureAction;

// Wifi Station Dialog
var wifiStationID;

// Available station
var availWifiStn;

var maxWifiStns = 0;
var wifiListInit = false;

var wifiListLoaded = false;
var wifiListRevision = 0;


function addWifiStationEntry() {

    console.log("Adder");
    console.log(serverrevision);

    // If there are any emtpy, then allow add
    if( window.wifi_stn_name == "" ) {
        document.getElementById("wifi_stn_add").hidden = false;
        availWifiStn = window.wifi_stn_id;
        return;
    }

    var wifistnelement = document.getElementById("wifi_stn_entry");
    var newwifistn = wifistnelement.cloneNode(true);
    var id = window.wifi_stn_id;
    newwifistn.id = wifistnelement.id + id.toString();
    newwifistn.hidden = false;
    
    for (var i = 0; i < newwifistn.children.length; i++) {
        if(newwifistn.children[i].nodeName == "IMG") {
            newwifistn.children[i].style = (window.wifi_stn_on=="t") ? "opacity: 1.0; cursor: auto" : "opacity: 0.2; cursor: pointer";
            newwifistn.children[i].setAttribute("onclick","connectWifi("+id.toString()+")");
        }
        if(newwifistn.children[i].nodeName == "BUTTON") {
            newwifistn.children[i].textContent = window.wifi_stn_name;
            newwifistn.children[i].setAttribute("onclick","loadWifiDialog("+id.toString()+")");
        }
    }

    wifistnelement.parentElement.appendChild(newwifistn);

    
 //   console.log(window.wifi_stn_id);
 //   console.log(maxWifiStns);

    // Remove loader
    if( window.wifi_stn_id == window.wifi_stn_count-1 ) {

     //   doRequest("wifi_stn_count",maxWifiStns);
        
        document.getElementById('loader').style.display='none';
    }
}

function initPage() {
    console.log("Init Page");

    document.getElementById('loader').style.display='block';

    doRequest("","",loadWifiList);

}

function loadWifiList(arg) {

    console.log("Status - Load WiFi list");
    console.log(serverrevision);
    console.log(wifiListRevision);
    console.log(arg);

    if( wifiListRevision == serverrevision ) return;

    clearWifiList(window.wifi_stn_count);

    // Show loader
    document.getElementById('loader').style.display='block';

    console.log(window.wifi_stn_count);
    for( var i = 0; i < window.wifi_stn_count; i++) {
        console.log(window.wifi_stn_count);
        console.log(i);
        doRequest("wifi_stn_id",i.toString(),addWifiStationEntry);
    }
    wifiListLoaded = true;
    wifiListRevision = serverrevision;
}

function clearWifiList(number) {
    for( var i = 0; i < number; i++) { // Change to dynamic calc of number
        var wifistn = document.getElementById("wifi_stn_entry" + i.toString());
        if( wifistn ) wifistn.remove();
    }
    document.getElementById("wifi_stn_add").hidden = true;
}



function visiblePwd(element) {
    var x = document.getElementById(element);
    if (x.type == "password") {
      x.type = "text";
    } else {
      x.type = "password";
    }
}

function updatePage() {
    console.log("Status - Update Page");

    // DHCP Mode
    var x = document.getElementById("wifi_stn_statics");
    var y = document.getElementById("wifi_stn_dhcp");
    if( y.checked ) x.style.display = "none";
    else x.style.display = "block";
   
}

function loadWifiDialog(value) {
    console.log('Status - Load Wifi Station');
    wifiStationID = value;
    doRequest("wifi_stn_btn",value,updatePage);
    document.getElementById('wifi_stn_forget').style.display='block';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function connectWifi(value) {
    console.log('Status - Connect Wifi Station');

    wifiStationID = value;
    sureAction="wifi_stn_cnct";

    document.getElementById('wifi_stn_sure').style.display='block';
}

function addWifiDialog() {
    sureAction='wifi_stn_save';
    document.getElementById('wifi_stn_ssid').value = '';
    document.getElementById('wifi_stn_pwd').value = '';
    document.getElementById('wifi_stn_dhcp').checked = true;
    document.getElementById('wifi_stn_ip').value = '';
    document.getElementById('wifi_stn_snet').value = '';
    document.getElementById('wifi_stn_gtwy').value = '';
    document.getElementById('wifi_stn_dns1').value = '';
    document.getElementById('wifi_stn_dns2').value = '';
    wifiStationID = availWifiStn;
    updatePage();
    document.getElementById('wifi_stn_forget').style.display='none';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function wifiSureYes() {

    document.getElementById('loader').style.display='block';

    var x = document.getElementById('wifi_stn_ssid');
    // Are we forgetting this one?
    if( sureAction == "wifi_stn_forget" ) {
        x.value = "";
        sureAction = "wifi_stn_save";
    }

    if( sureAction == "wifi_stn_save") {

        // Send updates data
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_pwd');
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_dhcp');
        doRequest(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('wifi_stn_ip');
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_snet');
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_gtwy');
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_dns1');
        doRequest(x.id, x.value);
        x = document.getElementById('wifi_stn_dns2');
        doRequest(x.id, x.value);
    }

    // Send Save command then reload list
    //reloadWifiList = true;
    //doRequest("wifi_stn_reld", "t");
    console.log("save");
    //doRequest(sureAction,wifiStationID,loadWifiList);
    doRequest(sureAction,wifiStationID);

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



