
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

    doRequest(sureAction,1,reloadPage);

}

function clearLoader(value) {
    document.getElementById('loader').style.display='none';
}
