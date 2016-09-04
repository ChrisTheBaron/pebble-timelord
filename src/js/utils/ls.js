/**
 * To help reduce data used we're going to store some stuff
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
 *  JSON encoded payload returned and the other key is `@{endpoint}`
 *  and the value is the timestamp of when the endpoint should be
 *  called again to refresh the data.
 *
 *  If the endpoints get too long then one solution would be
 *  to MD5 the name or something similar.
 */

var ls = module.exports = {

    /**
     * Gets a value from Local Storage
     * @param key to retrieve
     * @return value if key is set
     * @return null if key is not set, or data has expired
     */
    get: function (key) {
        console.log("Looking for key '" + key + "'");
        var value = localStorage.getItem(key);
        if (value === null) {
            console.log("Cannot find key '" + key + "'");
            return null;
        }
        console.log("Found key '" + key + "'");
        if (!ls.isValid(key)) {
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
    },

    /**
     * Sets a value in Local Storage
     * @param key to store against
     * @param value to store
     * @param validFor seconds
     */
    set: function (key, value, validFor) {
        console.log("Storing value under '" + key + "' for " + validFor + " secs");
        var expires = Date.now() + (validFor * 1000);
        console.log("Value valid till: " + expires);
        localStorage.setItem(key, JSON.stringify(value));
        localStorage.setItem("@" + key, expires);
    },

    /**
     * Checks to see if a key in Local Storage exists and hasn't expired
     * @param key
     * @return boolean
     */
    isValid: function (key) {
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

};
