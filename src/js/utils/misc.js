var S = require('string');

var misc = module.exports = {

    /**
     * Converts html to plain text by stripping all tags from it.
     * **Only use this from trusted sources as it will run <script>'s**

     * @param {string} html
     * @returns {string}
     */
    htmlToText: function (html) {
        return S(html)
            .stripTags()
            .decodeHTMLEntities()
            .trim()
            .toString();
    }

};