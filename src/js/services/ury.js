var Q = require('q');
var ls = require('./../utils/ls');
var misc = require('./../utils/misc');

const api_key = "6714984033428651727560238623082704224251296711076362946824083834883347645233279353289151659128276857";
const api_url = 'https://ury.org.uk/api/v2/';

/**
 * Makes a request to the URY API
 * @param endpoint to call relative to root. Don't include leading slash
 * @param options to pass in as GET parameters
 * @param cache {Number|boolean} either number of seconds to cache the request for, or false to not cache it
 * @return Promise
 */
var apiRequest = function (endpoint, options, cache) {

    var deferred = Q.defer();

    var xhrRequest = function (url, type) {

        var deferred = Q.defer();

        var xhr = new XMLHttpRequest();
        xhr.onload = function () {
            deferred.resolve(this.responseText);
        };
        xhr.onerror = deferred.reject;
        xhr.open(type, url);
        xhr.send();

        return deferred.promise;

    };

    var serialize = function (obj) {
        // http://stackoverflow.com/a/1714899
        var str = [];
        for (var p in obj)
            if (obj.hasOwnProperty(p)) {
                str.push(encodeURIComponent(p) + "=" + encodeURIComponent(obj[p]));
            }
        return str.join("&");
    };

    if (cache !== false && ls.isValid(endpoint)) {
        var value = ls.get(endpoint);
        deferred.resolve(value);
    } else { // No data in cache
        options = options || {};
        options["api_key"] = options["api_key"] || api_key;
        options = serialize(options);

        xhrRequest(api_url + endpoint + "?" + options, "GET")
            .then(function (response) {
                try {
                    var data = JSON.parse(response);
                    if (cache !== false) {
                        ls.set(endpoint, data["payload"], cache);
                    }
                    deferred.resolve(data["payload"]);
                } catch (error) {
                    deferred.reject(error);
                }
            })
            .catch(deferred.reject)
            .done();
    }

    return deferred.promise;

};

var ury = module.exports = {

    /**
     * @return Promise
     */
    getStudio: function () {
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

        return apiRequest(
            'selector/statusattime',
            {},
            30
        ).then(parseStudio);
    },

    /**
     * @return Promise
     */
    getShow: function () {
        function parseShow(payload) {
            var name = payload["current"]["title"];
            var end = payload["current"]["end_time"];
            var desc = misc.htmlToText(payload["current"]["desc"]);
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

        return apiRequest(
            'timeslot/currentandnext',
            {
                n: 10,
                filter: [1, 2]
            },
            30
        ).then(parseShow);
    },

    /**
     * @return Promise
     */
    getSeasons: function () {
        return apiRequest(
            'season/allseasonsinlatestterm',
            null,
            // It is unlikely that the schedule will change often,
            // so we can cache this for a while
            60 * 60 * 24
        );
    }

};