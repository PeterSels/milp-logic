
function centerWindow() {
    setZoomLevelToOne();

    // We suppose that document is bigger than window here
    // Should work for all DOM browsers including IE
    var docW = document.documentElement.width.baseVal.value; // clientWidth;
    var docH = document.documentElement.height.baseVal.value; // clientHeight;

    var winW = window.innerWidth;
    var winH = window.innerHeight;

    var mx = (docW - winW) / 2;
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
    window.scrollBy (dx * zoomFactor, dy * zoomFactor);
}

