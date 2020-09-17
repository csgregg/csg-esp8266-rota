
// Are you sure? Action
var sureAction;


function initPage() {
}

function updatePage() {
}



function SureNo() {
    document.getElementById('sure_dlg').style.display='none';
 }



function SureYes() {

    clearSure();
    showLoader();

    if( sureAction == "log_save" ) {

        x = document.getElementById('log_srl');
        doRequest(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_baud');
        doRequest(x.id, x.value);
        x = document.getElementById('log_ser');
        doRequest(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_url');
        doRequest(x.id, x.value);
        x = document.getElementById('log_key');
        doRequest(x.id, x.value);
        x = document.getElementById('log_tick');
        doRequest(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_tick_int');
        doRequest(x.id, x.value);
        x = document.getElementById('log_tags');
        doRequest(x.id, x.value);
        x = document.getElementById('log_level');
        doRequest(x.id, x.value);

        doRequest(sureAction,1,clearLoader);
    }
    else doRequest(sureAction,1,reloadPage);

}


