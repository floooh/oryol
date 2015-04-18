/** pnacl wrapper page Javascript functions **/
var origWidth = 0;
var origHeight = 0;
var progressCount = 0;
function updateProgress(percent) {
    var progress = document.getElementById('progress');
    progress.style.width = percent + '%';
}
function putText(text) {
  var element = document.getElementById('output');
  element.value += text;
  element.scrollTop = element.scrollHeight;
}

function handleProgress(event) {
  var loadPercent = 0.0;
  var loadPercentString;
  if (event.lengthComputable && event.total > 0) {
    loadPercent = (event.loaded / event.total) * 100.0;
  } else {
    loadPercent = 100;
  }
  updateProgress(loadPercent);
}
function handleLoad() {
  updateProgress(100);
}
function handleError() {
  var msg = 'PNACL ERROR: ' + document.getElementById('pnacl-module').lastError;
  console.log(msg); 
  putText(msg);
}
function handleCrash() {
  var msg = 'PNACL APPLICATION CRASHED!';
  console.log(msg); 
  putText(msg);
}
function handleMessage(msg) {
  var msg_json = JSON.parse(msg.data);
  if (msg_json.msg == 'log') {
    putText(msg_json.val + '\n');
  }
  else if (msg_json.msg == 'resize') {
    console.log(msg_json);
    var canvas = document.getElementById('pnacl-module');
    canvas.width = msg_json.w;
    canvas.height = msg_json.h;
  }
  else {
      putText('unknown msg received: ' + msg_json.msg + '\n');
  }
}
function switchToFullscreen() {
    // don't need to care about non-Chrome browsers
    var canvas = document.getElementById("pnacl-module");
    origWidth = canvas.width;
    origHeight = canvas.height;
    canvas.webkitRequestFullscreen();
}
function fullscreenChanged() {
    canvas = document.getElementById("pnacl-module");
    if (document.webkitIsFullscreen) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = origWidth;
        canvas.height = origHeight;
    }
    console.log("fullscreen change event received, new size: " + canvas.width + " " + canvas.height + "\n");
}
var naclModule = document.getElementById('pnacl-module');
naclModule.addEventListener('progress', handleProgress, true);
naclModule.addEventListener('load', handleLoad, true);
naclModule.addEventListener('error', handleError, true);
naclModule.addEventListener('crash', handleCrash, true);
naclModule.addEventListener('message', handleMessage, true);
document.addEventListener('webkitfullscreenchange', fullscreenChanged);
