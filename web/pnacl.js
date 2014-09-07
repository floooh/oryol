/** pnacl wrapper page Javascript functions **/

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
  putText(msg.data);
}
var naclModule = document.getElementById('pnacl-module');
naclModule.addEventListener('progress', handleProgress, true);
naclModule.addEventListener('load', handleLoad, true);
naclModule.addEventListener('error', handleError, true);
naclModule.addEventListener('crash', handleCrash, true);
naclModule.addEventListener('message', handleMessage, true);
