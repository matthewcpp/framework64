const fs = require("fs");

function sourceStringFromFile(path) {
    const text = fs.readFileSync(path, {encoding:"utf-8"});
    const charSet = new Set(text);

    let result = "";

    charSet.forEach((ch) => {
       if (ch !== '\r' && ch !== '\n')
           result += ch;
    });

    return result;
}

module.exports = {
    sourceStringFromFile: sourceStringFromFile
};
