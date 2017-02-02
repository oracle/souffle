

function clean_percentages(data) {
    if (data < 1) {
        return data.toFixed(3);
    }
    return data.toPrecision(3);
}

function humanize_time(time) {
    if (precision) return time.toString();
    if (time < 1e-9) {
        return '0';
    }
    if (time < 1) {
        milli = time * 1000.0;
        if (milli > 1) {
            return time.toFixed(3) + "s"
        }
        micro = milli * 1000.0;
        if (micro >= 1) {
            return micro.toFixed(1) + "µs"
        }
        //return "-";
        return (micro*1000).toFixed(1) + "ns"
    } else {
        minutes = (time / 60.0);
        if (minutes < 3) return time.toPrecision(3) + "s";
        hours = (minutes / 60);
        if (hours < 3) return minutes.toFixed(2) + "m";
        days = (hours / 24);
        if (days < 3) return hours.toFixed(2) + "H";
        weeks = (days / 7);
        if (weeks < 3) return days.toFixed(2) + "D";
        year = (days / 365);
        if (year < 3) return weeks.toFixed(2) + "W";
        return year.toFixed(2) + "Y"
    }
}

function minify_numbers(num) {
    if (precision) return num.toString();
    kilo = (num / 1000);
    if (kilo < 3) return num;
    mil = (kilo / 1000);
    if (mil < 3) return kilo.toFixed(1) + "K";
    bil = (mil / 1000);
    if (bil < 3) return mil.toFixed(1) + "M";
    tril = (bil / 1000);
    if (tril < 3) return days.toFixed(1) + "B";
    quad = (tril / 1000);
    if (quad < 3) return weeks.toFixed(1) + "T";
    quin = (quad / 1000);
    if (quin < 3) return weeks.toFixed(1) + "q";
    sex = (quin / 1000);
    if (sex < 3) return weeks.toFixed(1) + "Q";
    sept = (quin / 1000);
    if (sept < 3) return weeks.toFixed(1) + "s";
    return sept.toFixed(1) + "S"
}



(function () {
    var cleanNumber = function (x) {
        if (x.slice(-1) == 'K') {
            return parseFloat(x.slice(0, -1)) * 1e3;
        } else if (x.slice(-1) == 'M') {
            return parseFloat(x.slice(0, -1)) * 1e6;
        } else if (x.slice(-1) == "B") {
            return parseFloat(x.slice(0, -1)) * 1e9;
        } else if (x.slice(-1) == "T") {
            return parseFloat(x.slice(0, -1)) * 1e12;
        } else if (x.slice(-1) == "q") {
            return parseFloat(x.slice(0, -1)) * 1e15;
        } else if (x.slice(-1) == "Q") {
            return parseFloat(x.slice(0, -1)) * 1e18;
        } else if (x.slice(-1) == "s") {
            return parseFloat(x.slice(0, -1)) * 1e21;
        }
        return parseFloat(x);
    };
    var a = function (a) {
        return a;
    }, b = function (a, b) {
        return a = cleanNumber(a), b = cleanNumber(b), a = isNaN(a) ? 0 : a, b = isNaN(b) ? 0 : b, a - b
    };
    Tablesort.extend("number", function (a) {
        return a.match(/.*/)
    }, function (c, d) {
        return c = a(c), d = a(d), b(d, c)
    })
})();

(function () {
    var compare = function (a, b) {
        // TODO: regex for numbers and sort them? (for IDs and src location)
        return a.localeCompare(b);
    };
    Tablesort.extend("text", function (a) {
        return a.match(/.*/)
    }, function (c, d) {
        return compare(d, c)
    })
})();

(function () {
    var cleanNumber = function (x) {
            if (x.slice(-1) == 'Y') {
                return parseFloat(x.slice(0, -1)) * 365 * 24 * 60 * 60;
            } else if (x.slice(-1) == 'W') {
                return parseFloat(x.slice(0, -1)) * 7 * 24 * 60 * 60;
            } else if (x.slice(-1) == "D") {
                return parseFloat(x.slice(0, -1)) * 24 * 60 * 60;
            } else if (x.slice(-1) == "H") {
                return parseFloat(x.slice(0, -1)) * 60 * 60;
            } else if (x.slice(-1) == "m") {
                return parseFloat(x.slice(0, -1)) * 60;
            } else if (x.slice(-2) == "µs") {
                return parseFloat(x.slice(0, -2)) / 1e6;
            } else if (x.slice(-2) == "ns") {
                return parseFloat(x.slice(0, -2)) / 1e9;
            } else if (x.slice(-1) == "s") {
                return parseFloat(x.slice(0, -1));
            }
            return parseFloat(x);
        },
        compareNumber = function (a, b) {
            a = isNaN(a) ? 0 : a;
            b = isNaN(b) ? 0 : b;
            return a - b;
        };
    Tablesort.extend('time', function (item) {
        return true;
    }, function (a, b) {
        a = cleanNumber(a);
        b = cleanNumber(b);
        return compareNumber(b, a);
    });
}());
