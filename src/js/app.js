var Q = require('q');
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
        getUpdateData();
    }
);

function getInitialData() {

    Q.all([ury.getStudio(), ury.getShow()])
        .spread(function (studio, show) {
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
        })
        .catch(function (err) {
            console.log("Failed to call URY API", err);
        });

}

function getUpdateData() {

    Q.all([ury.getStudio(), ury.getShow()])
        .spread(function (studio, show) {
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
        })
        .catch(function (err) {
            console.log("Failed to call URY API", err);
        });

}