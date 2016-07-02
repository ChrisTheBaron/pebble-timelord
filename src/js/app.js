var api_key = "6714984033428651727560238623082704224251296711076362946824083834883347645233279353289151659128276857";
var api_url = 'https://ury.org.uk/api/v2/';

function xhrRequest(url, type, success, error) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        success(this.responseText);
    };
    xhr.onerror = function () {
        if (typeof error == "function") {
            error();
        }
    };
    xhr.open(type, url);
    xhr.send();
}

function getStudio(success, error) {
    xhrRequest(
        api_url + 'selector/statusattime?api_key=' + api_key,
        'GET',
        function (response) {
            var data = JSON.parse(response);
            var studio;
            switch (data["payload"]["studio"]) {
                case 1:
                case 2:
                    studio = 'Studio ' + studio;
                    break;
                case 3:
                    studio = 'Jukebox';
                    break;
                case 4:
                    studio = 'OB';
                    break;
                default:
                    studio = 'Unknown';
                    break;
            }
            success(studio);
        },
        error
    );
}

function getShow(success, error) {
    xhrRequest(
        api_url + 'timeslot/currentandnext?n=10&filter[]=1&filter[]=2&api_key=' + api_key,
        'GET',
        function (response) {
            var data = JSON.parse(response);
            var name = data["payload"]["current"]["title"];
            var end = data["payload"]["current"]["end_time"];
            if (end == "The End of Time") {
                // @TODO: Check if this is the only time this happens
                end = 0;
            } else {
                // Just to make sure it's an integer
                end = parseInt(end)
            }
            success(name, end);
        },
        error
    );
}

function getData() {
    getStudio(
        function (studio) {
            getShow(
                function (show_name, show_end) {
                    // Assemble dictionary using our keys
                    var dictionary = {
                        "CURRENT_STUDIO": studio,
                        "CURRENT_SHOW_NAME": show_name,
                        "CURRENT_SHOW_END": show_end
                    };
                    // Send to Pebble
                    Pebble.sendAppMessage(dictionary,
                        function (e) {
                            console.log('Info sent to Pebble successfully!');
                        },
                        function (e) {
                            console.log('Error sending info to Pebble!');
                        }
                    );
                },
                function () {
                    console.log("Failed to call URY API");
                }
            );
        },
        function () {
            console.log("Failed to call URY API");
        }
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
    function (e) {
        console.log('PebbleKit JS ready!');
        getData();
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function (e) {
        console.log('AppMessage received!');
        getData();
    }
);
