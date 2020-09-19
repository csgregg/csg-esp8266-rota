
// Are you sure? Action
var sureAction;


function initPage() {
    doRequest("","");
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
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_baud');
        doRequestWait(x.id, x.value);
        x = document.getElementById('log_ser');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_url');
        doRequestWait(x.id, x.value);
        x = document.getElementById('log_key');
        doRequestWait(x.id, x.value);
        x = document.getElementById('log_tick');
        doRequestWait(x.id, x.checked ? 't' : 'f');
        x = document.getElementById('log_tick_int');
        doRequestWait(x.id, x.value);
        x = document.getElementById('log_tags');
        doRequestWait(x.id, x.value);
        x = document.getElementById('log_level');
        doRequestWait(x.id, x.value);

        doRequest(sureAction,1,clearLoader);

        disableSave("log_save","log_cancel");
    }
    else {
        doRequest(sureAction,1,reloadPage);
    }

}


