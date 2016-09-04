var ury = require('./services/ury');

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
    function (e) {
        console.log('PebbleKit JS ready!');
        getInitialData();
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function (e) {
        console.log('AppMessage received!');
        getInitialData();
    }
);

function getInitialData() {
    ury.getStudio(
        function (studio) {
            ury.getShow(
                function (show) {
                    // Assemble dictionary using our keys
                    var dictionary = {
                        "CURRENT_STUDIO": studio,
                        "CURRENT_SHOW_NAME": show.name,
                        "CURRENT_SHOW_END": show.end,
                        "CURRENT_SHOW_DESC": show.desc
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
