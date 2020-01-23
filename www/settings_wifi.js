/*
   Page Specific Support

   ( Must include call to mainpageload(); )
*/


// Show Wifi settings tab
function openCity(evt, cityName) {
    var i;
    var x = document.getElementsByClassName("city");
    for (i = 0; i < x.length; i++) {
        x[i].style.display = "none";
    }
    var activebtn = document.getElementsByClassName("testbtn");
    for (i = 0; i < x.length; i++) {
        activebtn[i].className = activebtn[i].className.replace(" w3-dark-grey", "");
    }
    document.getElementById(cityName).style.display = "block";
    evt.currentTarget.className += " w3-dark-grey";
}


function onpageload() {

    // Call main page support
    mainpageload();

    // Get updates
    doRequest();

    // Page specific calls
    var mybtn = document.getElementById("myLink");
    mybtn.click();

}



