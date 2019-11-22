define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    var onload = function() {
        CodeMirror.defineSimpleMode("my_mode", {
            // See https://codemirror.net/demo/simplemode.html
        });
        CodeMirror.defineMIME("text/x-python", "my_mode");
        CodeMirror.modeInfo.push({
            ext: ["py"],
            mime: "text/x-python",
            mode: "my_mode",
            name: "Python"
        });
    }
    return { onload: onload }
});