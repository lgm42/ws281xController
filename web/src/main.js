document.addEventListener("DOMContentLoaded", function() {
    document.getElementById("dashboardLink").onclick = function() {
        document.getElementById('configurationContent').classList.add('collapse');
        document.getElementById('dashboardContent').classList.remove('collapse');
        return false;
    };

    document.getElementById("configurationLink").onclick = function() {
        document.getElementById('configurationContent').classList.remove('collapse');
        document.getElementById('dashboardContent').classList.add('collapse');
        return false;
    };

    document.getElementById("switch-on-off").onclick = function() {
        if (document.getElementById('switch-on-off').checked) {
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("GET", baseUrl + "/cmd?bt_on");
            httpRequest.send();
        } else 
        {
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("GET", baseUrl + "/cmd?bt_off");
            httpRequest.send();
        }
    };
});

