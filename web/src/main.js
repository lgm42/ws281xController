document.addEventListener("DOMContentLoaded", function() {
    document.getElementById("dashboardLink").onclick = function() {
        document.getElementById('configurationContent').classList.add('collapse');
        document.getElementById('dashboardContent').classList.remove('collapse');
        return false;
    };

    document.getElementById("configurationLink").onclick = function() {
        document.getElementById('configurationContent').classList.remove('collapse');
        document.getElementById('dashboardContent').classList.add('collapse');

        //request configuration
        var httpRequest = new XMLHttpRequest();
        httpRequest.onload = function () {
            var configuration = JSON.parse(httpRequest.responseText);
            if (httpRequest.readyState == 4 && httpRequest.status == "200") {
                console.log(configuration);
                document.getElementById('hostnameInput').value = configuration.hostname;
                document.getElementById('mqttServerIpInput').value = configuration.mqttServerIp;
                document.getElementById('mqttServerPortInput').value = configuration.mqttServerPort;
                document.getElementById('mqttLoginInput').value = configuration.mqttLogin;
                document.getElementById('mqttPasswordInput').value = configuration.mqttPassword;
                document.getElementById('colorLedOnInput').value = configuration.colorLedOn.toString(16);
                document.getElementById('numLedsInput').value = configuration.numLeds;

                for (const [key, value] of Object.entries(configuration.customCommands)) {
                    console.log(key, value);
                    checkbox = document.getElementById('activated' + key + 'Check');
                    nameField = document.getElementById('name' + key + 'Input');
                    commandField = document.getElementById('command' + key + 'Input');
                    if (checkbox) {
                        if (value.name != "") {
                            checkbox.checked = true;
                            document.getElementById(checkbox.getAttribute("aria-controls")).classList.add('show');
                            checkbox.setAttribute("aria-expanded", true);
                        } else {
                            checkbox.checked = false;
                            document.getElementById(checkbox.getAttribute("aria-controls")).classList.remove('show');
                            checkbox.setAttribute("aria-expanded", false);
                        }
                    }
                    if (value.name != "") {
                        //rule is active
                        if (nameField)
                            nameField.value = value.name;
                        if (commandField)
                            commandField.value = value.command;
                    } else {    
                        if (nameField) 
                            nameField.value = "";
                        if (commandField)
                            commandField.value = "";
                    }
                }
                
            } else {
                console.error(configuration);
            }
        }
        httpRequest.open("GET", baseUrl + "/getConfig");
        httpRequest.send();

        return false;
    };

    document.getElementById("switch-on-off").onclick = function() {
        if (document.getElementById('switch-on-off').checked) {
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("GET", baseUrl + "/cmd?bt_on");
            httpRequest.send();
        } else {
            var httpRequest = new XMLHttpRequest();
            httpRequest.open("GET", baseUrl + "/cmd?bt_off");
            httpRequest.send();
        }
    };

    var restoreDefaultConfigurationBtn = document.getElementById('restoreDefaultConfigurationBtn');

    restoreDefaultConfigurationBtn.onclick = function (event) {
        var httpRequest = new XMLHttpRequest();
            httpRequest.open("GET", baseUrl + "/resetConfig");
            httpRequest.send();
    };

    var saveConfigurationBtn = document.getElementById('saveConfigurationBtn');

    saveConfigurationBtn.onclick = function (event) {
        //we create object to send
        configuration = {};

        configuration.hostname = document.getElementById('hostnameInput').value;
        configuration.mqttServerIp = document.getElementById('mqttServerIpInput').value;
        configuration.mqttServerPort = document.getElementById('mqttServerPortInput').value;
        configuration.mqttLogin = document.getElementById('mqttLoginInput').value;
        configuration.mqttPassword = document.getElementById('mqttPasswordInput').value;
        configuration.colorLedOn = parseInt(document.getElementById('colorLedOnInput').value, 16);
        configuration.numLeds = document.getElementById('numLedsInput').value;
        configuration.customCommands = {};

        for (var key = 0; key < 14; ++key) {
            var item = {};

            checkbox = document.getElementById('activated' + key + 'Check');
            nameField = document.getElementById('name' + key + 'Input');
            commandField = document.getElementById('command' + key + 'Input');

            if ((!checkbox) || (checkbox.checked)) {
                if (nameField)
                    item["name"] = nameField.value;
                item["command"] = commandField.value;
                configuration.customCommands[key] = item;
            }
        }

        console.log(configuration);
        var httpRequest = new XMLHttpRequest();
        httpRequest.open("POST", baseUrl + "/setConfig");
        httpRequest.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

        httpRequest.onload = function () {
            new bootstrap.Toast(document.getElementById('configurationSavedToast')).show()
        };

        httpRequest.send(JSON.stringify(configuration));

        

    };

});

