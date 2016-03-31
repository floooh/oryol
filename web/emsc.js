/** emscripten wrapper page Javascript functions **/

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
            document.getElementById('canvas2d').style.display = 'none'; 
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

var spinnerFrameCount = 0;
function drawSpinner() {

    // only draw spinner after half a second
    spinnerFrameCount++;
    if (spinnerFrameCount > 30) {
        var spinnerAngle = spinnerFrameCount * 0.2;
        var ctx = document.getElementById('canvas2d').getContext("2d");
        var midx = 50.5;
        var midy = 50.5;

        ctx.setTransform(1, 0, 0, 1, midx, midy);
        ctx.clearRect(-50, -50, 100, 100);
        ctx.rotate(spinnerAngle);
        ctx.beginPath();
        colors = ['#ff0000', 
                  '#ff3F00',
                  '#ff7f00', 
                  '#ffbf00',
                  '#ffff00',
                  '#7fff00',
                  '#00ff00',
                  '#007fff',
                  '#0000ff', 
                  '#7f00ff',
                  '#ff00ff',
                  '#ff007f' ];
        var numSteps = 12;
        var step = (2.0 * Math.PI) / numSteps;
        for (i = 0; i < numSteps; i++) {
            var start = i * step + 0.2;
            var end = start + step - 0.2;
            ctx.beginPath();
            ctx.fillStyle = colors[i];
            ctx.moveTo(0, 0);
            ctx.arc(0, 0, 30, start, end); 
            ctx.closePath();
            ctx.fill();
        }
    }

    if (!loaded) {
        requestAnimationFrame(drawSpinner);
    }
}

drawSpinner();
