var x = null;

function byId(key) {
    return document.getElementById(key);
}

function statesUpdate() {
    if (x != null) {
        x.abort();
    }
    x = new XMLHttpRequest();
    x.onreadystatechange = function () {
        if (x.readyState == 4 && x.status == 200) {
            var obj = JSON.parse(x.responseText);
            for (var key in obj) {
                var span = byId(key);
                if (span != null) {
                    span.innerHTML = obj[key];
                }
            }
        }
    };

    x.open('GET', '/rest/states', true);
    x.send();
}