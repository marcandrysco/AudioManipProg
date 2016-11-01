(function() {
  "use strict";

  /*
   * Audit namespace
   */
  window.Audit = new Object();

  /**
   * Create the audit element.
   *   @audit: The audit.
   *   @idx: The request index.
   *   &returns: The DOM element.
   */
  window.Audit.elem = function(audit, idx) {
    audit.idx = idx;

    var div = Gui.div();

    var str = atob(audit.data);
    var buf = new ArrayBuffer(str.length);
    var raw = new Uint8Array(buf);
    var arr = new Float32Array(buf);
    for(var i = 0; i < str.length; i++) {
      raw[i] = str.charCodeAt(i);
    }

    div.appendChild(Gui.text(arr.length + " : " + arr[0] + " : " + arr[1] + " : " + arr[2] + " : " + arr[3]));
    debugger;

    return div;
  };
})();
