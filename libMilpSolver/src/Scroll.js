var c = 0;
var t;
var timer_is_on = 0;
var DX = 0;
var DY = 0;
var nSteps = 20;
var scrollMilliseconds = nSteps * 50;

function timedCount() {
  if (c < nSteps) {
    window.scrollBy(DX/2, DY/2)
    DX = DX/2;
    DY = DY/2;
    c = c + 1
    t = setTimeout("timedCount()", scrollMilliseconds/nSteps);
  } else {
    DX = 0
    DY = 0
    c = 0
    stopCount();
  }
}

function doTimer(dx, dy) {
  if (!timer_is_on) {
    timer_is_on = 1;
    DX = dx;
    DY = dy;
    c = 0;
    timedCount();
  }
}

function stopCount() {
  clearTimeout(t);
  timer_is_on = 0;
  c = 0;
}

function centerWindow() {
    setZoomLevelToOne();

    // We suppose that document is bigger than window here
    // Should work for all DOM browsers including IE
    var docW = document.documentElement.width.baseVal.value; // clientWidth;
    var docH = document.documentElement.height.baseVal.value; // clientHeight;

    var winW = window.innerWidth;
    var winH = window.innerHeight;

    var mx = (docW - winW) / 24 * 7; // Where trains begin
    var my = (docH - winH) / 2;
    window.scroll(mx, my); // sets this doc point as top left corner in win
}

function setZoomLevelToOne() {
    document.documentElement.currentScale = 1;  
}

function jumpFromSrcIdToDstId(srcId, dstId) {
    var srcX = document.getElementById(srcId).x.baseVal.value;
    var srcY = document.getElementById(srcId).y.baseVal.value;
    var dstX = document.getElementById(dstId).x.baseVal.value;
    var dstY = document.getElementById(dstId).y.baseVal.value;
    var dx = dstX - srcX;
    var dy = dstY - srcY;
    window.zoomIndependentScrollByLogicalPixels(dx,dy);
}

function getZoomFactor() {
    var zoom = document.documentElement.currentScale;
    //alert(zoom);
    return zoom;

    var factor = 1;
    //if (document.getBoundingClientRect) {
        // rect is only in physical pixel size in IE before version 8 
        var rect = document; //.getBoundingClientRect();
        var physicalW = rect.right - rect.left;
        var logicalW = document.offsetWidth;
        // the zoom level is always an integer percent value
        factor = Math.round ((physicalW / logicalW) * 100) / 100;
    //}
    return factor;
}

function zoomIndependentScrollByLogicalPixels (dx, dy) {
    var zoomFactor = getZoomFactor();
    //window.scrollBy (dx * zoomFactor, dy * zoomFactor);
    doTimer(dx * zoomFactor, dy * zoomFactor);
}

