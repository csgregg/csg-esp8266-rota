
// Are you sure? Action
var sureAction;


function initPage() {
}

function updatePage() {
}


function reloadPage() {
    location.reload();
}


function SureNo() {
    document.getElementById('sure_dlg').style.display='none';
}


function SureDlg(action) {
    sureAction = action.id;
    document.getElementById('sure_dlg').style.display='block';
}


function SureYes() {

    document.getElementById('sure_dlg').style.display='none';
    document.getElementById('loader').style.display='block';

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

function clearLoader(value) {
    document.getElementById('loader').style.display='none';
}
