/** pnacl wrapper page Javascript functions **/

var progressCount = 0;
function updateProgress(percent) {
    progress = document.getElementById('progress');
    progress.style.width = percent + '%';
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
  console.log('PNACL ERROR: ' + document.getElementById('pnacl-module').lastError); 
}
function handleCrash() {
  console.log('PNACL APPLICATION CRASHED!'); 
}
function handleMessage(msg) {
  var element = document.getElementById('output');
  element.value += msg.data;
  element.scrollTop = element.scrollHeight;
}
var naclModule = document.getElementById('pnacl-module');
naclModule.addEventListener('progress', handleProgress, true);
naclModule.addEventListener('load', handleLoad, true);
naclModule.addEventListener('error', handleError, true);
naclModule.addEventListener('crash', handleCrash, true);
naclModule.addEventListener('message', handleMessage, true);
