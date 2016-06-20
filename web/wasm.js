
var loaded = false;
var Module = {
    preRun: [],
    postRun: [],
    print: (function() {
        var element = document.getElementById('output');
        if (element) element.value = '';
        return function(text) {
            text = Array.prototype.slice.call(arguments).join(' ');
            console.log(text);
            if (element) {
                element.value += text + '\n';
                element.scrollTop = element.scrollHeight;
            }
        };
    })(),
    printErr: function(text) {
        text = Array.prototype.slice.call(arguments).join(' ');
        console.error(text);
        if (text == 'no native wasm support detected') {
            text += '\n\nyou need the latest Firefox Nightly or Chrome Canary and manually enable WebAssembly support:\n\n';
            text += 'Firefox Nightly: about:config -> javascript.options.wasm -> true\n';
            text += 'Chrome Canary: chrome://flags/#enable-webassembly -> check\n';
        }
        var elm = document.getElementById('output');
        elm.style.display = 'block';
        elm.value += text + '\n';
        elm.scrollTop = elm.scrollHeight;
    },
    canvas: (function() {
        var canvas = document.getElementById('canvas');
        canvas.addEventListener("webglcontextlost", function(e) { alert('FIXME: WebGL context lost, please reload the page'); e.preventDefault(); }, false);
        return canvas;
    })(),
    setStatus: function(text) {
        console.log("status: " + text);
    },
    totalDependencies: 0,
    monitorRunDependencies: function(left) {
        console.log("monitor run deps: " + left);
        if (0 == left) { 
            document.getElementById('canvas').style.display = 'block';
            document.getElementById('output').style.display = 'block';
            loaded = true;
        }
    },
};

window.onerror = function(event) {
    console.log("onerror: " + event);
};

function callAsEventHandler(func_name) {
    // this is some hackery to make the browser module believe that it
    // is running in an event handler
    var eventHandler = { allowsDeferredCalls: true };
    ++JSEvents.inEventHandler;
    JSEvents.currentEventHandler = eventHandler;
    Module.cwrap(func_name)()
    --JSEvents.inEventHandler;
}

