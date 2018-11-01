let menu=[
    {link:"index.html",title:"Sample 1. Basic loading image"},
    {link:"sample_2.html",title:"Sample 2. Basic Webcam reading"},
    {link:"sample_3.html",title:"Sample 3. Basic image processing"},
    {link:"sample_4.html",title:"Sample 4. Optical flow"},
    {link:"sample_5.html",title:"Sample 5. Face detection using HaarCascades"},
    //{link:"sample_6.html",title:"Sample 6. Object detection and recognition using DNN"},
]
$(function(){
    var menu_construct="";
    var page_active= window.location.href.split("/")[window.location.href.split("/").length-1];
    for(var i=0; i<menu.length; i++){
        if(menu[i].link==page_active)
        menu_construct+='<li class="nav-item"><a class="nav-link active" href="'+menu[i].link+'">'+menu[i].title+'</a></li>';
        else
            menu_construct+='<li class="nav-item"><a class="nav-link" href="'+menu[i].link+'">'+menu[i].title+'</a></li>';
    }
    $("#menu").html(menu_construct);
    
   
})

function loadImageToCanvas(url, cavansId) {
    let canvas = document.getElementById(cavansId);
    let ctx = canvas.getContext('2d');
    let img = new Image();
    img.crossOrigin = 'anonymous';
    img.onload = function() {
        canvas.width = img.width;
        canvas.height = img.height;
        ctx.drawImage(img, 0, 0, img.width, img.height);
    };
    img.src = url;
};

function createFileFromUrl(path, url, callback) {
    let request = new XMLHttpRequest();
    request.open('GET', url, true);
    request.responseType = 'arraybuffer';
    request.onload = function(ev) {
        if (request.readyState === 4) {
            if (request.status === 200) {
                let data = new Uint8Array(request.response);
                cv.FS_createDataFile('/', path, data, true, false, false);
                callback();
            } else {
                self.printError('Failed to load ' + url + ' status: ' + request.status);
            }
        }
    };
    request.send();
};