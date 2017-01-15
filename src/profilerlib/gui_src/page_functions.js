/*
* Souffle - A Datalog Compiler
* Copyright (c) 2017, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

(function () {
    var a = function (a) {
        return a;
    }, b = function (a, b) {
        return a = parseFloat(a), b = parseFloat(b), a = isNaN(a) ? 0 : a, b = isNaN(b) ? 0 : b, a - b
    };
    Tablesort.extend("number", function (a) {
        return a.match(/.*/)
    }, function (c, d) {
        return c = a(c), d = a(d), b(d, c)
    })
})();

(function () {
    var b = function (a, b) {
        return a.localeCompare(b);
    };
    Tablesort.extend("text", function (a) {
        return a.match(/.*/)
    }, function (c, d) {
        return b(d, c)
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
                return parseFloat(x.slice(0, -2)) / 1000000;
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


function select_for_graph(a) {
    for (tabcontent = document.getElementById("tabcontent"), tablinks = document.getElementsByClassName("rul_ver_row"), i = 0; i < tablinks.length; i++) tablinks[i].style.background = "rgba(255,255,0,0)";
    for (tablinks = document.getElementsByClassName("rulesofrel_row"), i = 0; i < tablinks.length; i++) tablinks[i].style.background = "rgba(255,255,0,0)";
    a.style.background = "rgba(255,255,0,0.2)", selected_for_chart = a
}

function showChart() {
    var a, b, c, d, j;
    if (!selected_for_chart) return void alert("please select a recursive rule to graph");
    if (a = selected_for_chart.cells[1].innerHTML, "C" != a[0]) return void alert("Please select a recursive rule (ID starts with C)");
    for (document.getElementById("chart_tab").style.display = "block", c = document.getElementsByClassName("tabcontent"), b = 0; b < c.length; b++) c[b].style.display = "none";
    for (d = document.getElementsByClassName("tablinks"), b = 0; b < d.length; b++) d[b].className = d[b].className.replace(" active", "");
    document.getElementById("chart_tab").style.display = "block", document.getElementById("chart_tab").className += " active", document.getElementById("chart").style.display = "block";
    var e = [],
        f = [],
        g = [];
    for (j = 0; j < data.rul[a][10].tot_t.length; j++) e.push(j.toString()), f.push({
        value: data.rul[a][10].tot_t[j]
    }), g.push({
        value: data.rul[a][10].tuples[j]
    });
    draw_graph(e, f, g)
}

function ctBarLabels(a) {
    a instanceof Chartist.Bar && a.on("draw", function (a) {
        var b, c, d, e;
        if ("bar" === a.type && (b = a.x1 + .5 * a.element.width(), c = a.y1 + a.element.height() * -1 - 2, e = a.element.attr("ct:value"), "0" !== e)) return d = new Chartist.Svg("text"), d.text(e), d.addClass("ct-barlabel"), d.attr({
            x: b,
            y: c,
            "text-anchor": "middle"
        }), a.group.append(d)
    })
}

function draw_graph(a, b, c) {
    var d = {
        height: "37vh"
    };
    show_graph_vals && (d.plugins = [ctBarLabels]), new Chartist.Bar(".ct-chart1", {
        labels: a,
        series: [b]
    }, d), new Chartist.Bar(".ct-chart2", {
        labels: a,
        series: [c]
    }, d)
}

function clean_percentages(data) {
    if (data < 1) {
        return data.toFixed(3);
    }
    return data.toPrecision(3);
}

function humanize_time(data) {
    if (precision) return data;
    if (data < 1) {
        milli = data * 1000;
        if (milli > 1) {
            return data.toFixed(3) + "s"
        }
        micro = milli * 1000;
        if (micro > 1) {
            return micro.toPrecision(3) + "µs"
        }
        return micro
    } else {
        minutes = (data / 60);
        if (minutes < 3) return data.toPrecision(3) + "s";
        hours = (minutes / 60);
        if (hours < 3) return minutes.toFixed(1) + "m";
        days = (hours / 24);
        if (days < 3) return hours.toFixed(1) + "H";
        weeks = (days / 7);
        if (weeks < 3) return days.toFixed(1) + "D";
        year = (days / 365);
        if (year < 3) return weeks.toFixed(1) + "W";
        return year.toFixed(1) + "Y"
    }
}

function minify_numbers(data) {
    if (precision) return data;
    if (data < 1e6) return data;
    return data.toExponential(2);
}

function changeRowRel(a) {
    selected_for_chart = !1;
    selected.rel = a;
    var b, c, d, e, f, g, h;
    for (d = document.getElementsByClassName("rel_row"), b = 0; b < d.length; b++) d[b].style.background = "rgba(255,255,0,0)";
    document.getElementById(a).style.background = "rgba(255,255,0,0.2)", document.getElementById("rulesofrel").style.display = "block", c = document.getElementById("rulesofrel_body"), c.innerHTML = "", e = data.rel[a],
             f = document.createElement("tr"),

        h = document.createElement("td"),
        h.innerHTML = e[0],
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = e[1],
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = humanize_time(e[2]),
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = humanize_time(e[3]),
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = humanize_time(e[4]),
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = humanize_time(e[5]),
        f.appendChild(h);
    h = document.createElement("td"),
        h.innerHTML = minify_numbers(e[6]),
        f.appendChild(h);
    for (h = document.createElement("td"), g = document.createElement("div"), g.className = "perc_time", g.style.width = "100%", g.innerHTML = "100", h.appendChild(g), f.appendChild(h), h = document.createElement("td"), g = document.createElement("div"), g.className = "perc_time", g.style.width = "100%", g.innerHTML = "100", h.appendChild(g), f.appendChild(h), h = document.createElement("td"), h.innerHTML = e[7], f.appendChild(h), f.className = "rulesofrel_row", f.onclick = function () {
        select_for_graph(this)
    }, c.appendChild(f), b = 0; b < e[8].length; b++) {
        f = document.createElement("tr");
        h = document.createElement("td"),
            h.innerHTML = data.rul[e[8][b]][0],
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = data.rul[e[8][b]][1],
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = humanize_time(data.rul[e[8][b]][2]),
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = humanize_time(data.rul[e[8][b]][3]),
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = humanize_time(data.rul[e[8][b]][4]),
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = humanize_time(data.rul[e[8][b]][5]),
            f.appendChild(h);
        h = document.createElement("td"),
            h.innerHTML = minify_numbers(data.rul[e[8][b]][6]),
            f.appendChild(h);
        h = document.createElement("td"),
            g = document.createElement("div"),
            g.className = "perc_time",
            0 == e[2] ? (g.style.width = "0%", g.innerHTML = "0") : (
                    g.style.width = 100 * data.rul[e[8][b]][2] / e[2] + "%",
                    g.innerHTML = (100 * data.rul[e[8][b]][2] / e[2]).toPrecision(4)
                ),
            h.appendChild(g),
            f.appendChild(h),
            h = document.createElement("td"),
            g = document.createElement("div"),
            g.className = "perc_time", 0 == e[6] ? (g.style.width = "0%", g.innerHTML = "0") : (
                g.style.width = 100 * data.rul[e[8][b]][6] / e[6] + "%",
                g.innerHTML = (100 * data.rul[e[8][b]][6] / e[6]).toPrecision(4)
            ),
            h.appendChild(g), f.appendChild(h), h = document.createElement("td"),
            h.innerHTML = data.rul[e[8][b]][7],
            f.appendChild(h),
            f.onclick = function () {
            select_for_graph(this)
        }, f.className = "rulesofrel_row",
            c.appendChild(f)
    }
    created_relrul || (new Tablesort(document.getElementById("rulesofrel_table")), created_relrul = !0)
}

function changeRowRul(a) {
    selected_for_chart = !1;
    selected.rul = a;

    var b, c, d, e, f, g, h;
    for (d = document.getElementsByClassName("rul_row"), b = 0; b < d.length; b++) d[b].style.background = "rgba(255,255,0,0)";
    document.getElementById(a).style.background = "rgba(255,255,0,0.2)",
        document.getElementById("rulver").style.display = "block",
        c = document.getElementById("rulver_body"), c.innerHTML = "",
        e = data.rul[a], f = document.createElement("tr"),
        g = document.createElement("td"),
        g.innerHTML = e[0],
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = e[1],
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = humanize_time(e[2]),
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = humanize_time(e[3]),
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = humanize_time(e[4]),
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = humanize_time(e[5]),
        f.appendChild(g);
    g = document.createElement("td"),
        g.innerHTML = minify_numbers(e[6]),
        f.appendChild(g);
    for (g = document.createElement("td"), g.innerHTML = "-", f.appendChild(g), g = document.createElement("td"), h = document.createElement("div"), h.className = "perc_time", h.style.width = "100%", h.innerHTML = "100", g.appendChild(h), f.appendChild(g), g = document.createElement("td"), h = document.createElement("div"), h.className = "perc_time", h.style.width = "100%", h.innerHTML = "100", g.appendChild(h), f.appendChild(g), g = document.createElement("td"), g.innerHTML = e[7], f.appendChild(g), f.className = "rul_ver_row", f.onclick = function () {
        select_for_graph(this)
    }, c.appendChild(f), b = 0; b < e[8].length; b++) {
        f = document.createElement("tr");
        g = document.createElement("td"),
            g.innerHTML = data.rul[e[8][b][1]][0],
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = data.rul[e[8][b][1]][1],
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = humanize_time(data.rul[e[8][b][1]][2]),
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = humanize_time(data.rul[e[8][b][1]][3]),
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = humanize_time(data.rul[e[8][b][1]][4]),
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = humanize_time(data.rul[e[8][b][1]][5]),
            f.appendChild(g);
        g = document.createElement("td"),
            g.innerHTML = minify_numbers(data.rul[e[8][b][1]][6]),
            f.appendChild(g);

        g = document.createElement("td"), g.innerHTML = e[8][b][0], f.appendChild(g), g = document.createElement("td"), h = document.createElement("div"), h.className = "perc_time", 0 == e[2] ? (h.style.width = "0%", h.innerHTML = "0") : (h.style.width = 100 * data.rul[e[8][b][1]][2] / e[2] + "%", h.innerHTML = (100 * data.rul[e[8][b][1]][2] / e[2]).toPrecision(4)), g.appendChild(h), f.appendChild(g), g = document.createElement("td"), h = document.createElement("div"), h.className = "perc_time", 0 == e[6] ? (h.style.width = "0%", h.innerHTML = "0") : (h.style.width = 100 * data.rul[e[8][b][1]][6] / e[6] + "%", h.innerHTML = (100 * data.rul[e[8][b][1]][6] / e[6]).toPrecision(4)), g.appendChild(h), f.appendChild(g), g = document.createElement("td"), g.innerHTML = data.rul[e[8][b][1]][7], f.appendChild(g), f.className = "rul_ver_row", f.onclick = function () {
            select_for_graph(this)
        }, c.appendChild(f)
    }
    created_rulver || (new Tablesort(document.getElementById("rulvertable")), created_rulver = !0)
}

function changeTab(a, b) {
    document.getElementById("chart_tab").style.display = "none";
    var c, d, e;
    for (d = document.getElementsByClassName("tabcontent"), c = 0; c < d.length; c++) d[c].style.display = "none";
    for (e = document.getElementsByClassName("tablinks"), c = 0; c < e.length; c++) e[c].className = e[c].className.replace(" active", "");
    document.getElementById(b).style.display = "block", a.currentTarget.className += " active"
}

function gen_top() {
    x = document.getElementById("Top");
    line1 = document.createElement("p");
    line1.textContent = "Total runtime: " + data.top[0],
        line2 = document.createElement("p"),
        line2.textContent = "Total tuples: " + data.top[1],
        x.appendChild(line1),
        x.appendChild(line2)
}

function gen_rel_table() {
    var a, b, c, d, f = 0,
        g = .01;
    for (d in data.rel) data.rel.hasOwnProperty(d) && (f += data.rel[d][2], g += parseFloat(data.rel[d][6]));
    a = document.getElementById("Rel_table_body");
    a.innerHTML="";
    for (d in data.rel)
        if (data.rel.hasOwnProperty(d)) {
            b = document.createElement("tr"), b.id = data.rel[d][1], b.className = "rel_row", b.onclick = function () {
                changeRowRel(this.id)
            }, c = document.createElement("td"),
                c.innerHTML = data.rel[d][0],
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = data.rel[d][1],
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rel[d][2]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rel[d][3]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rel[d][4]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rel[d][5]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = minify_numbers(data.rel[d][6]),
                b.appendChild(c);
            c = document.createElement("td"),
                div = document.createElement("div"),
                div.className = "perc_time",
                div.style.width = data.rel[d][2] / f * 100 + "%",
                div.innerHTML = clean_percentages(data.rel[d][2] / f * 100),
                c.appendChild(div),
                b.appendChild(c),
                c = document.createElement("td"),
                div = document.createElement("div"),
                div.className = "perc_time",
                div.style.width = 100 * parseFloat(data.rel[d][6]) / g + "%",
                div.innerHTML = clean_percentages(100 * parseFloat(data.rel[d][6]) / g),
                c.appendChild(div),
                b.appendChild(c),
                c = document.createElement("td"),
                c.innerHTML = data.rel[d][7],
                b.appendChild(c),
                a.appendChild(b)
        }
}

function gen_rul_table() {
    var a, b, c, d, f = 0,
        g = .01;
    for (d in data.rul) data.rul.hasOwnProperty(d) && (f += data.rul[d][2], g += parseFloat(data.rul[d][6]));
    a = document.getElementById("Rul_table_body");
    a.innerHTML="";
    for (d in data.rul)
        if (data.rul.hasOwnProperty(d)) {
            b = document.createElement("tr"), b.id = data.rul[d][1], b.className = "rul_row", b.onclick = function () {
                changeRowRul(this.id)
            };
            c = document.createElement("td"),
                c.innerHTML = data.rul[d][0],
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = data.rul[d][1],
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rul[d][2]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rul[d][3]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rul[d][4]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = humanize_time(data.rul[d][5]),
                b.appendChild(c);
            c = document.createElement("td"),
                c.innerHTML = minify_numbers(data.rul[d][6]),
                b.appendChild(c);

            c = document.createElement("td"),
                div = document.createElement("div"),
                div.className = "perc_time",
                div.style.width = data.rul[d][2] / f * 100 + "%",
                div.innerHTML = clean_percentages(data.rul[d][2] / f * 100),
                c.appendChild(div),
                b.appendChild(c),
                c = document.createElement("td"),
                div = document.createElement("div"),
                div.className = "perc_time",
                div.style.width = 100 * parseFloat(data.rul[d][6]) / g + "%",
                div.innerHTML = clean_percentages(100 * parseFloat(data.rul[d][6]) / g),
                c.appendChild(div),
                b.appendChild(c),
                c = document.createElement("td"),
                c.innerHTML = data.rul[d][7],
                b.appendChild(c),
                a.appendChild(b)
        }
}

function init() {
    gen_top();
    gen_rel_table();
    gen_rul_table();
    document.getElementById("default").click();
    new Tablesort(document.getElementById("Rel_table"));
    new Tablesort(document.getElementById("Rul_table"));
}

function toggle_precision() {
    precision=!precision;
    gen_rel_table();
    gen_rul_table();
    if (selected) {
        if (selected.rul) {
            changeRowRul(selected.rul);
        }
        if (selected.rel) {
            changeRowRel(selected.rel);
        }
    }
}


var created_relrul = !1,
    created_rulver = !1,
    selected_for_chart = !1,
    show_graph_vals = !1,
    precision = !1,
    selected = {rel:!1,rul:!1};
init();