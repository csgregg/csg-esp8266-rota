// LED status
var thing_led = false;

function initPage() {
    // console.log("Status - Initialize Page");

    updatePage();
}

function updatePage() {
    // console.log("Status - Update Page");
    if( thing_led=='t' ) document.getElementById('led').style.background = "green";
    else document.getElementById('led').style.background = "red";
}
