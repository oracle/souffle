/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2017, The Souffle Developers. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


function select_for_graph(a) {
    tabcontent = document.getElementById("tabcontent");
    tablinks = document.getElementsByClassName("rul_ver_row");
    for (i = 0; i < tablinks.length; i++)
        tablinks[i].style.background = "rgba(255,255,0,0)";
    tablinks = document.getElementsByClassName("rulesofrel_row");
    for (i = 0; i < tablinks.length; i++)
        tablinks[i].style.background = "rgba(255,255,0,0)";
    a.style.background = "rgba(255,255,0,0.2)";
}

function graph_rul_ver() {
    var a, b, c, d, j;
    if (!selected.rul) return void alert("please select a recursive rule to graph (ID starts with C)");
    if ("C" != selected.rul[0])
        return void alert("Please select a recursive rule (ID starts with C)");
    document.getElementById("chart_tab").style.display = "block";
    c = document.getElementsByClassName("tabcontent");
    for (b = 0; b < c.length; b++)
        c[b].style.display = "none";
    d = document.getElementsByClassName("tablinks");
    for (b = 0; b < d.length; b++)
        d[b].className = d[b].className.replace(" active", "");
    document.getElementById("chart_tab").style.display = "block";
    document.getElementById("chart_tab").className += " active";
    document.getElementById("chart").style.display = "block";
    var e = [];
    f = [];
    g = [];
    for (j = 0; j < data.rul[selected.rul][10].tot_t.length; j++) e.push(j.toString()), f.push({
        value: data.rul[selected.rul][10].tot_t[j]
    }), g.push({
        value: data.rul[selected.rul][10].tuples[j]
    });
    graph_vals.labels = e;
    graph_vals.tot_t = f;
    graph_vals.tuples = g;
    draw_graph()
}

function graph_rul() {
    var a, b, c, d, j;
    if (!selected.rul) return void alert("please select a recursive rule to graph (ID starts with C)");
    if ("C" != selected.rul[0])
        return void alert("Please select a recursive rule (ID starts with C)");
    document.getElementById("chart_tab").style.display = "block";
    c = document.getElementsByClassName("tabcontent");
    for (b = 0; b < c.length; b++)
        c[b].style.display = "none";
    d = document.getElementsByClassName("tablinks");
    for (b = 0; b < d.length; b++)
        d[b].className = d[b].className.replace(" active", "");
    document.getElementById("chart_tab").style.display = "block";
    document.getElementById("chart_tab").className += " active";
    document.getElementById("chart").style.display = "block";
    var e = [];
    f = [];
    g = [];
    for (j = 0; j < data.rul[selected.rul][9].tot_t.length; j++) e.push(j.toString()), f.push({
        value: data.rul[selected.rul][9].tot_t[j]
    }), g.push({
        value: data.rul[selected.rul][9].tuples[j]
    });
    graph_vals.labels = e;
    graph_vals.tot_t = f;
    graph_vals.tuples = g;
    draw_graph()
}

function graph_rel() {
    var a, b, c, d, j;
    if (!selected.rel) return void alert("please select a relation to graph");

    document.getElementById("chart_tab").style.display = "block";
    c = document.getElementsByClassName("tabcontent");
    for (b = 0; b < c.length; b++)
        c[b].style.display = "none";
    d = document.getElementsByClassName("tablinks");
    for (b = 0; b < d.length; b++)
        d[b].className = d[b].className.replace(" active", "");
    document.getElementById("chart_tab").style.display = "block";
    document.getElementById("chart_tab").className += " active";
    document.getElementById("chart").style.display = "block";
    var e = [];
    f = [];
    g = [];
    for (j = 0; j < data.rel[selected.rel][9].tot_t.length; j++) e.push(j.toString()), f.push({
        value: data.rel[selected.rel][9].tot_t[j]
    }), g.push({
        value: data.rel[selected.rel][9].tuples[j]
    });
    graph_vals.labels = e;
    graph_vals.tot_t = f;
    graph_vals.tuples = g;
    draw_graph()
}


function ctBarLabels(a) {
    a instanceof Chartist.Bar && a.on("draw", function (a) {
        var b, c, d, e;
        if ("bar" === a.type &&
            (b = a.x1 + .5 * a.element.width(), c = a.y1 + a.element.height() * -1 - 2, e = a.element.attr("ct:value"), "0" !== e))
            return d = new Chartist.Svg("text"), d.text(e), d.addClass("ct-barlabel"), d.attr({
                x: b,
                y: c,
                "text-anchor": "middle"
            }), a.group.append(d)
    })
}

function draw_graph() {
    var d = {
        height: "37vh",
        axisY: {
            labelInterpolationFnc: function (value) {
                return humanize_time(value);
            }
        },
        axisX: {
            labelInterpolationFnc: function (value) {
                var n = Math.floor(graph_vals.labels.length/15);
                if (n>1) {
                    if (value%n == 0) {
                        return value;
                    }
                    return null;
                }
                return value;
            }
        }
    };

    show_graph_vals && (d.plugins = [ctBarLabels]);

    new Chartist.Bar(".ct-chart1", {
        labels: graph_vals.labels,
        series: [graph_vals.tot_t],
    }, d);
    d.axisY = {
        labelInterpolationFnc: function (value) {
            return minify_numbers(value);
        }
    };
    new Chartist.Bar(".ct-chart2", {
        labels: graph_vals.labels,
        series: [graph_vals.tuples],
    }, d)
}


function changeRowRel(a) {
    selected_for_chart = !1;
    selected.rel = a;
    var b, c, d, e, f, g, h, p;
    for (d = document.getElementsByClassName("rel_row"), b = 0; b < d.length; b++) d[b].style.background = "rgba(255,255,0,0)";
    document.getElementById(a).style.background = "rgba(255,255,0,0.2)";
    document.getElementById("rulesofrel").style.display = "block";
    c = document.getElementById("rulesofrel_body");
    c.innerHTML = "";
    e = data.rel[a];
    f = document.createElement("tr");
    f.style.position = "relative";
    h = document.createElement("td");
    h.className = "text_cell";
    h.style.position = "relative";
    p = document.createElement("span");
    p.innerHTML = e[0];
    h.appendChild(p);
    h.appendChild(p);
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = e[1];
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = humanize_time(e[2]);
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = humanize_time(e[3]);
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = humanize_time(e[4]);
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = humanize_time(e[5]);
    f.appendChild(h);
    h = document.createElement("td");
    h.innerHTML = minify_numbers(e[6]);
    f.appendChild(h);
    h = document.createElement("td");
    g = document.createElement("div");
    g.className = "perc_time";
    g.style.width = "100%";
    g.innerHTML = "100";
    h.appendChild(g);
    f.appendChild(h);
    h = document.createElement("td");
    g = document.createElement("div");
    g.className = "perc_time";
    g.style.width = "100%";
    g.innerHTML = "100";
    h.appendChild(g);
    f.appendChild(h);
    h = document.createElement("td");
    h.className = "text_cell";
    p = document.createElement("span");
    p.innerHTML = e[7];
    f.appendChild(h);
    f.className = "rulesofrel_row";

    c.appendChild(f);
    for (b = 0; b < e[8].length; b++) {
        f = document.createElement("tr");
        f.style.position = "relative";
        h = document.createElement("td");
        h.style.position = "relative";
        h.className = "text_cell";

        p = document.createElement("span");
        p.innerHTML = data.rul[e[8][b]][0];
        p.style.cssText = "word-break:break-all; position: absolute;height:100%;width: 100%;text-overflow: ellipsis;white-space: nowrap;overflow: hidden;";
        h.appendChild(p);
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = data.rul[e[8][b]][1];
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = humanize_time(data.rul[e[8][b]][2]);
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = humanize_time(data.rul[e[8][b]][3]);
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = humanize_time(data.rul[e[8][b]][4]);
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = humanize_time(data.rul[e[8][b]][5]);
        f.appendChild(h);
        h = document.createElement("td");
        h.innerHTML = minify_numbers(data.rul[e[8][b]][6]);
        f.appendChild(h);
        h = document.createElement("td");
        g = document.createElement("div");
        g.className = "perc_time";
        0 == e[2] ? (g.style.width = "0%", g.innerHTML = "0") : (
            g.style.width = 100 * data.rul[e[8][b]][2] / e[2] + "%",
                g.innerHTML = (100 * data.rul[e[8][b]][2] / e[2]).toPrecision(4)
        );
        h.appendChild(g);
        f.appendChild(h);
        h = document.createElement("td");
        g = document.createElement("div");
        g.className = "perc_time", 0 == e[6] ? (g.style.width = "0%", g.innerHTML = "0") : (
            g.style.width = 100 * data.rul[e[8][b]][6] / e[6] + "%",
                g.innerHTML = (100 * data.rul[e[8][b]][6] / e[6]).toPrecision(4)
        );
        h.appendChild(g);
        f.appendChild(h);

        h = document.createElement("td");
        h.className = "text_cell";
        p = document.createElement("span");
        p.innerHTML = data.rul[e[8][b]][7];
        h.appendChild(p);
        f.appendChild(h);
        f.onclick = function () {
            select_for_graph(this)
        }, f.className = "rulesofrel_row";
        c.appendChild(f)
    }
    created_relrul || (new Tablesort(document.getElementById("rulesofrel_table")), created_relrul = !0);
}

function changeRowRul(a) {
    selected.rul = a;

    var b, c, d, e, f, g, h, p;
    for (d = document.getElementsByClassName("rul_row"), b = 0; b < d.length; b++) d[b].style.background = "rgba(255,255,0,0)";
    document.getElementById(a).style.background = "rgba(255,255,0,0.2)";
    document.getElementById("rulver").style.display = "block";
    c = document.getElementById("rulver_body"), c.innerHTML = "";
    e = data.rul[a];
    f = document.createElement("tr");
    f.style.position = "relative";
    g = document.createElement("td");
    g.className = "text_cell";

    p = document.createElement("span");
    p.innerHTML = e[0];
    g.appendChild(p);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = e[1];
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = humanize_time(e[2]);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = humanize_time(e[3]);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = humanize_time(e[4]);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = humanize_time(e[5]);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = minify_numbers(e[6]);
    f.appendChild(g);
    g = document.createElement("td");
    g.innerHTML = "-";
    f.appendChild(g);
    g = document.createElement("td");
    h = document.createElement("div");
    h.className = "perc_time";
    h.style.width = "100%";
    h.innerHTML = "100";
    g.appendChild(h);
    f.appendChild(g);
    g = document.createElement("td");
    h = document.createElement("div");
    h.className = "perc_time";
    h.style.width = "100%";
    h.innerHTML = "100";
    g.appendChild(h);
    f.appendChild(g);
    g = document.createElement("td");
    g.className = "text_cell";
    p = document.createElement("span");
    p.innerHTML = e[7];
    g.appendChild(p);


    f.appendChild(g);
    f.className = "rul_ver_row";

    for (b = 0; b < e[8].length; b++) {
        f = document.createElement("tr");

        g = document.createElement("td");
        g.style.position = "relative";
        g.className = "text_cell";
        p = document.createElement("span");
        p.innerHTML = e[8][b][0];


        g.appendChild(p);
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = e[8][b][1];
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = humanize_time(e[8][b][2]);
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = humanize_time(e[8][b][3]);
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = humanize_time(e[8][b][4]);
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = humanize_time(e[8][b][5]);
        f.appendChild(g);
        g = document.createElement("td");
        g.innerHTML = minify_numbers(e[8][b][6]);
        f.appendChild(g);

        g = document.createElement("td");
        g.innerHTML = e[8][b][8];
        f.appendChild(g);
        g = document.createElement("td");
        h = document.createElement("div");
        h.className = "perc_time";
        0 == e[2] ? (h.style.width = "0%", h.innerHTML = "0") : (h.style.width = 100 * e[8][b][2] / e[2] + "%",
            h.innerHTML = (100 * e[8][b][2] / e[2]).toPrecision(4));
        g.appendChild(h);
        f.appendChild(g);
        g = document.createElement("td");
        h = document.createElement("div");
        h.className = "perc_time";
        0 == e[6] ? (h.style.width = "0%", h.innerHTML = "0") : (h.style.width = 100 * e[8][b][6] / e[6] + "%",
            h.innerHTML = (100 * e[8][b][6] / e[6]).toPrecision(4));
        g.appendChild(h);
        f.appendChild(g);
        g = document.createElement("td");
        g.className = "text_cell";
        p = document.createElement("span");

        p.innerHTML = e[8][b][7];
        g.appendChild(p);
        f.appendChild(g);
        f.className = "rul_ver_row";
        f.onclick = function () {
            select_for_graph(this)
        }, c.appendChild(f)
    }
    created_rulver || (new Tablesort(document.getElementById("rulvertable")), created_rulver = !0);

}

function changeTab(a, b) {
    document.getElementById("chart_tab").style.display = "none";
    var c, d, e;
    for (d = document.getElementsByClassName("tabcontent"), c = 0; c < d.length; c++)
        d[c].style.display = "none";
    for (e = document.getElementsByClassName("tablinks"), c = 0; c < e.length; c++)
        e[c].className = e[c].className.replace(" active", "");
    document.getElementById(b).style.display = "block", a.currentTarget.className += " active"
}

function gen_top() {
    var x, line1, line2;
    x = document.getElementById("Top");
    line1 = document.createElement("p");
    line1.textContent = "Total runtime: " + humanize_time(data.top[0]) + " (" + data.top[0] + " seconds)";
    line2 = document.createElement("p");
    line2.textContent = "Total tuples: " + minify_numbers(data.top[1]) + " (" + data.top[1] + ")";
    x.appendChild(line1);
    x.appendChild(line2)
}

function gen_rel_table() {
    var a, b, c, d, p, f = 0;
    g = .01;
    for (d in data.rel)
        data.rel.hasOwnProperty(d) && !isNaN(data.rel[d][2]) && (f += data.rel[d][2], g += parseFloat(data.rel[d][6]));
    a = document.getElementById("Rel_table_body");
    a.innerHTML = "";
    for (d in data.rel)
        if (data.rel.hasOwnProperty(d)) {
            b = document.createElement("tr"), b.id = data.rel[d][1], b.className = "rel_row", b.onclick = function () {
                changeRowRel(this.id)
            }, c = document.createElement("td");
            c.className = "text_cell";
            c.style.position = "relative";
            p = document.createElement("span");
            p.innerHTML = data.rel[d][0];
            c.appendChild(p);
            c.style.wordBreak = "break-all";
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = data.rel[d][1];
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rel[d][2]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rel[d][3]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rel[d][4]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rel[d][5]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = minify_numbers(data.rel[d][6]);
            b.appendChild(c);
            c = document.createElement("td");
            div = document.createElement("div");
            div.className = "perc_time";
            div.style.width = data.rel[d][2] / f * 100 + "%";
            div.innerHTML = clean_percentages(data.rel[d][2] / f * 100);
            c.appendChild(div);
            b.appendChild(c);
            c = document.createElement("td");
            div = document.createElement("div");
            div.className = "perc_time";
            div.style.width = 100 * parseFloat(data.rel[d][6]) / g + "%";
            div.innerHTML = clean_percentages(100 * parseFloat(data.rel[d][6]) / g);
            c.appendChild(div);
            b.appendChild(c);
            c = document.createElement("td");
            c.className = "text_cell";
            p = document.createElement("span");
            p.innerHTML = data.rel[d][7];
            c.style.width = '95px';
            c.appendChild(p);
            b.appendChild(c);
            a.appendChild(b)
        }
}

function gen_rul_table() {
    var a, b, c, d, p, f = 0;
    g = .01;
    for (d in data.rul)
        data.rul.hasOwnProperty(d) && !isNaN(data.rul[d][2]) && (f += data.rul[d][2], g += parseFloat(data.rul[d][6]));
    a = document.getElementById("Rul_table_body");
    a.innerHTML = "";
    for (d in data.rul)
        if (data.rul.hasOwnProperty(d)) {
            b = document.createElement("tr"), b.id = data.rul[d][1], b.className = "rul_row", b.onclick = function () {
                changeRowRul(this.id)
            };
            c = document.createElement("td");
            c.style.position = "relative";
            c.className = "text_cell";

            p = document.createElement("span");
            p.innerHTML = data.rul[d][0];
            c.appendChild(p);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = data.rul[d][1];
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rul[d][2]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rul[d][3]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rul[d][4]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = humanize_time(data.rul[d][5]);
            b.appendChild(c);
            c = document.createElement("td");
            c.innerHTML = minify_numbers(data.rul[d][6]);
            b.appendChild(c);

            c = document.createElement("td");
            div = document.createElement("div");
            div.className = "perc_time";
            div.style.width = data.rul[d][2] / f * 100 + "%";
            div.innerHTML = clean_percentages(data.rul[d][2] / f * 100);
            c.appendChild(div);
            b.appendChild(c);
            c = document.createElement("td");
            div = document.createElement("div");
            div.className = "perc_time";
            div.style.width = 100 * parseFloat(data.rul[d][6]) / g + "%";
            div.innerHTML = clean_percentages(100 * parseFloat(data.rul[d][6]) / g);
            c.appendChild(div);
            b.appendChild(c);
            c = document.createElement("td");
            c.className = "text_cell";
            p = document.createElement("span");
            p.innerHTML = data.rul[d][7];
            c.appendChild(p);

            b.appendChild(c);
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
    precision = !precision;
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


var created_relrul = !1;
var created_rulver = !1;
var show_graph_vals = !1;
var precision = !1;
var selected = {rel: !1, rul: !1};
var graph_vals = {tot_t:[],
    tuples:[],
    copy_t:[]};
init();