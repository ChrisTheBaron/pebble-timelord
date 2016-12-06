var Q = require('q');
var ury = require('./services/ury');
var keys = require('message_keys');
const config = require("./config");

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

    Q.all([ury.getStudio(), ury.getShows()])
        .spread(function (studio, shows) {
            // Assemble dictionary using our keys
            var dictionary = assembleDictionary(studio, shows);
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
    getInitialData();
}

function assembleDictionary(studio, shows) {
    var dictionary = {};
    dictionary[keys["CURRENT_STUDIO"]] = studio;
    // Do a check for the actual length just in case the api
    // didn't return as many shows as we were expecting.
    var num_shows = Math.min(shows.length, config.NUM_SHOWS);
    dictionary[keys["NUM_SHOWS"]] = num_shows;
    for (var i = 0; i < num_shows; i++) {
        dictionary[keys["SHOW_NAME"] + i] = shows[i].name;
        dictionary[keys["SHOW_START"] + i] = shows[i].start;
        dictionary[keys["SHOW_END"] + i] = shows[i].end;
        dictionary[keys["SHOW_DESC"] + i] = shows[i].desc;
    }
    return dictionary;
}