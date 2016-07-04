var S = require('string');

var api_key = "6714984033428651727560238623082704224251296711076362946824083834883347645233279353289151659128276857";
var api_url = 'https://ury.org.uk/api/v2/';

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

function getStudio(success, error) {
    function parseStudio(payload) {
        var studio;
        switch (payload["studio"]) {
            case 1:
            case 2:
                studio = 'Studio ' + payload["studio"];
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
        return studio;
    }

    apiRequest(
        'selector/statusattime',
        {},
        30,
        function (payload) {
            success(parseStudio(payload));
        },
        error
    );
}

function getShow(success, error) {
    function parseShow(payload) {
        var name = payload["current"]["title"];
        var end = payload["current"]["end_time"];
        var desc = htmlToPlainText(payload["current"]["desc"]);
        if (end == "The End of Time") {
            // @TODO: Check if this is the only time this happens
            end = 0;
        } else {
            // Just to make sure it's an integer
            end = parseInt(end)
        }
        return {
            name: name,
            end: end,
            desc: desc
        };
    }

    apiRequest(
        'timeslot/currentandnext',
        {
            n: 10,
            filter: [1, 2]
        },
        30,
        function (payload) {
            success(parseShow(payload));
        },
        error
    );
}

function getData() {
    getStudio(
        function (studio) {
            getShow(
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

/**
 * Converts html to plain text by stripping all tags from it.
 * **Only use this from trusted sources as it will run <script>s**

 * @param {string} html
 * @returns {string}
 */
function htmlToPlainText(html) {
    return S(html)
        .stripTags()
        .decodeHTMLEntities()
        .trim()
        .toString();
}

/**
 * Makes a request to the URY API
 * @param endpoint to call relative to root. Don't include leading slash
 * @param options to pass in as GET parameters
 * @param cache {Number|boolean} either number of seconds to cache the request for, or false to not cache it
 * @param success {function}
 * @param error {function}
 */
function apiRequest(endpoint, options, cache, success, error) {
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

    function serialize(obj) {
        // http://stackoverflow.com/a/1714899
        var str = [];
        for (var p in obj)
            if (obj.hasOwnProperty(p)) {
                str.push(encodeURIComponent(p) + "=" + encodeURIComponent(obj[p]));
            }
        return str.join("&");
    }

    if (cache !== false && validLSValue(endpoint)) {
        var value = getLSValue(endpoint);
        if (value !== null) {
            success(value);
            return;
        }
    }

    // If we get here then we have no cached data
    options = options || {};
    options["api_key"] = options["api_key"] || api_key;
    options = serialize(options);

    xhrRequest(api_url + endpoint + "?" + options, "GET",
        function (response) {
            try {
                var data = JSON.parse(response);
                if (cache !== false) {
                    setLSValue(endpoint, data["payload"], cache);
                }
                success(data["payload"]);
            } catch (error) {
                error();
            }
        },
        error
    );

}

/**
 * To help reduce data transmission we're going to store some stuff
 * in local storage. When an endpoint is called successfully we'll
 * insert into local store a couple of values like this:
 *
 * {
 *      "selector/statusattime": "{
 *          \"ready\": true,
 *          \"studio\": 3,
 *          \"lock\": 0,
 *      }"
 *      "@selector/statusattime": 1467497111
 *  }
 *
 *  Where one key is the endpoint called and the value is the
 *  JSON encoded payload returned and the other key is `@endpoint`
 *  and the value is the timestamp of when the endpoint should be
 *  called again to refresh the data.
 *
 *  If the endpoints get too long then one solution would be
 *  to MD5 the name or something similar.
 */

/**
 * Gets a value from Local Storage
 * @param key to retrieve
 * @return value if key is set
 * @return null if key is not set, or data has expired
 */
function getLSValue(key) {
    console.log("Looking for key '" + key + "'");
    var value = localStorage.getItem(key);
    if (value === null) {
        console.log("Cannot find key '" + key + "'");
        return null;
    }
    console.log("Found key '" + key + "'");
    if (!validLSValue(key)) {
        console.log("Key '" + key + "' has expired");
        return null;
    }
    try {
        console.log("Returning cached value!");
        return JSON.parse(value);
    } catch (error) {
        console.error(error);
        // Do nothing, break nothing
        return null;
    }
}

/**
 * Sets a value in Local Storage
 * @param key to store against
 * @param value to store
 * @param validFor seconds
 */
function setLSValue(key, value, validFor) {
    console.log("Storing value under '" + key + "' for " + validFor + "secs");
    var expires = Date.now() + (validFor * 1000);
    console.log("Value valid till: " + expires);
    localStorage.setItem(key, JSON.stringify(value));
    localStorage.setItem("@" + key, expires);
}

/**
 * Checks to see if a key in Local Storage exists and hasn't expired
 * @param key
 * @return boolean
 */
function validLSValue(key) {
    console.log("Looking for expiry key '@" + key + "'");
    var expires = localStorage.getItem("@" + key);
    if (expires === null) {
        console.log("Expiry key '@" + key + "' not found");
        return false;
    }
    if (expires < Date.now()) {
        console.log("Expiry key '@" + key + "' expired at: " + expires);
        return false;
    }
    return true;
}