


// Are you sure? Action
var sureAction;

// Wifi Station Dialog
var wifiStationID;

// Available station
var availWifiStn;

// Last revision the list was loaded on
var wifiListRevision = 0;




function addWifiStationEntry() {
    //console.log("Add station")

    // Remove loader
    if( window.wifi_stn_id == window.wifi_stn_count-1 ) clearLoader();


    // If there are any emtpy, then allow add
    if( window.wifi_stn_name == "" ) {
        document.getElementById("wifi_stn_add").hidden = false;
        availWifiStn = window.wifi_stn_id;
        return;
    }

    var id = window.wifi_stn_id;
    var already = document.getElementById("wifi_stn_entry" + id.toString());

    if( already ) return;

    var wifistnelement = document.getElementById("wifi_stn_entry");
    var newwifistn = wifistnelement.cloneNode(true);
    
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

    wifistnelement.parentNode.insertBefore(newwifistn, wifistnelement.nextSibling);

}

function initPage() {
    // console.log("Status - Initialize Page");

    if( !document.getElementById("dns_mode").checked ) disableElmt("dns_mdns","dns_mdnselmt");
    
    wifiListRevision = 0;
    doRequest("","",loadWifiList);

}

function loadWifiList() {

    // console.log("Status - Load WiFi list");

    // Clear entries
    for( var i = 0; i < window.wifi_stn_count; i++) {
        var wifistn = document.getElementById("wifi_stn_entry" + i.toString());
        if( wifistn ) wifistn.remove();
    }
    document.getElementById("wifi_stn_add").hidden = true;

    showLoader(document.getElementById("wifi_saved_box"));

    if( wifiListRevision == serverrevision ) return;
    for( var i = 0; i < window.wifi_stn_count; i++) doRequest("wifi_stn_id",i.toString(),addWifiStationEntry);
    wifiListRevision = serverrevision;

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
    // console.log("Status - Update Page");

    // DHCP Mode
    var x = document.getElementById("wifi_stn_statics");
    var y = document.getElementById("wifi_stn_dhcp");
    if( y.checked ) x.style.display = "none";
    else x.style.display = "block";

    // Update AP icon
    document.getElementById("wifi_mode_ap").checked ? 
        document.getElementById("wifi_ap_icon").style = "opacity: 1.0;"
        : document.getElementById("wifi_ap_icon").style = "opacity: 0.2;"
   
}

function loadWifiDialog(value) {
    // console.log('Status - Load Wifi Station');

    wifiStationID = value;
    doRequest("wifi_stn_btn",value,updatePage);
    disableElmt("wifi_stn_save");
    document.getElementById('wifi_stn_forget').style.display='block';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function connectWifi(value) {
    // console.log('Status - Connect Wifi Station');

    wifiStationID = value;
    SureDlg('wifi_stn_cnct');
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
    disableElmt("wifi_stn_save");
    document.getElementById('wifi_stn_forget').style.display='none';
    document.getElementById('wifi_stn_dlg').style.display='block';
}

function SureNo() {

    clearSure();

    if( sureAction == "wifi_mode_stn" || sureAction == "wifi_mode_ap" ) {
        document.getElementById(sureAction).checked = !document.getElementById(sureAction).checked;
        clearLoader();
        return;
    }

    if( sureAction == "wifi_ap_save" ) disableElmt("wifi_ap_save","wifi_ap_cancel");
    if( sureAction == "net_ck_save" ) disableElmt("net_ck_save","net_ck_cancel");
    if( sureAction == "dns_save" ) disableElmt("dns_save","dns_cancel");
    if( sureAction == "tlo_save" ) disableElmt("tlo_save","tlo_cancel");

    doRequestAll();
}

function SureYes() {

    clearSure();
        
    var x = document.getElementById('wifi_stn_ssid');

    // Are we forgetting this one?
    if( sureAction == "wifi_stn_forget" ) {
        x.value = "";
        sureAction = "wifi_stn_save";
    }

    if( sureAction == "wifi_stn_save") {

        // Send updates data
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_pwd');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_dhcp');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('wifi_stn_ip');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_snet');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_gtwy');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_dns1');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_stn_dns2');
        doRequestWait(x.id, x.value);

        showLoader(document.getElementById('wifi_saved_box'));

        // Send Save command then reload list
        doRequest(sureAction,wifiStationID);

        // Hide dialog
        document.getElementById('wifi_stn_dlg').style.display='none';
    }

    if( sureAction == "wifi_ap_save" ) {
        // Send updates data
        x = document.getElementById('wifi_ap_ssid');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_ap_pwd');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_ap_ip');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_ap_snet');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_ap_gtwy');
        doRequestWait(x.id, x.value);
        x = document.getElementById('wifi_ap_ch');
        doRequestWait(x.id, x.value);

        showLoader(document.getElementById('wifi_ap_box'));

        // Send Save command and update AP
        doRequest(sureAction,1);

        disableElmt("wifi_ap_save","wifi_ap_cancel");
        
    }

    if( sureAction == "wifi_mode_stn" || sureAction == "wifi_mode_ap" ) {

        var stn = document.getElementById('wifi_mode_stn').checked;
        var ap = document.getElementById('wifi_mode_ap').checked;

        var mode = 0;                   // WIFI_OFF
        if( stn && ap ) mode = 3;       // WIFI_AP_STA
        else if( ap ) mode = 2;         // WIFI_AP
        else if( stn ) mode = 1;        // WIFI_STN

        if( mode == 0 ) if( !confirm('This will disable network access. Are you really sure?') ) {

            wifiSureNo();
            return;
        }

        showLoader(document.getElementById('wifi_mode_box'));
  
        doRequest('wifi_mode_save',mode);
    }

    if( sureAction == "wifi_stn_cnct" ) {

        showLoader(document.getElementById('wifi_saved_box'));

        doRequest('wifi_stn_cnct',wifiStationID);
    }

    if( sureAction == "net_ck_save" ) {

        // Send updates data
        x = document.getElementById('net_ck_mode');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('net_ck_int');
        doRequestWait(x.id, x.value);
        x = document.getElementById('net_ck_url');
        doRequestWait(x.id, x.value);
        
        showLoader(document.getElementById('net_ck_box'));

        // Send Save command and update AP
        doRequest(sureAction,1,clearLoader);

        disableElmt("net_ck_save","net_ck_cancel");
    }

    if( sureAction == "dns_save" ) {

        // Send updates data
        x = document.getElementById('dns_mode');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('dns_name');
        doRequestWait(x.id, x.value);
        x = document.getElementById('dns_mdns');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        
        showLoader(document.getElementById('dns_box'));

        // Send Save command and update AP
        doRequest(sureAction,1,clearLoader);

        disableElmt("dns_save","dns_cancel");
    }

    if( sureAction == "tlo_save" ) {

        // Send updates data
        x = document.getElementById('tlo_token');
        doRequestWait(x.id, x.value);
        x = document.getElementById('tlo_ntp');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        
        showLoader(document.getElementById('tlo_box'));

        // Send Save command and update AP
        doRequest(sureAction,1,clearLoader);

        disableElmt("tlo_save","tlo_cancel");
    }

}


function DetectLocation() {
    showLoader(document.getElementById('tlo_box'));
    enableElmt("tlo_save","tlo_cancel");
    doRequest("tlo_detect",1,clearLoader);
}
