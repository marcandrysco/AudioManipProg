(function() {
  "use strict";

  /*
   * Controller namespace
   */
  window.Ctrl = new Object();

  /**
   * Create the controller element.
   *   @ctrl: The controller.
   *   @idx: The request index.
   *   &returns: The DOM element.
   */
  window.Ctrl.elem = function(ctrl, idx) {
    ctrl.idx = idx;
    ctrl.layout = Ctrl.layout();
    
    var div = Gui.div("ctrl");
    div.appendChild(ctrl.canvas = Gui.tag("canvas", "ctrl"));

    Ctrl.draw(ctrl);
    window.addEventListener("resize", function() { Ctrl.draw(ctrl); });
    setTimeout(function() { Ctrl.draw(ctrl); }, 10);

    return div;
  };

  /**
   * Crreate the layout object.
   *   &returns: The layout.
   */
  window.Ctrl.layout = function() {
    return {
      height: 20,
      width: 30,
      label: 80
    };
  };


  /**
   * Retrieve the height of the canvas.
   *   @ctrl: The control.
   *   &returns: The height.
   */
  window.Ctrl.getHeight = function(ctrl) {
    return (ctrl.layout.height + 1) * ctrl.inst.length - 1;
  };


  /**
   * Draw the canvas.
   *   @ctrl: The control.
   */
  window.Ctrl.draw = function(ctrl) {
    ctrl.canvas.style.height = Ctrl.getHeight(ctrl) + 4 + "px";
    ctrl.canvas.style.border = "2px solid black";
    ctrl.canvas.width = ctrl.canvas.clientWidth;
    ctrl.canvas.height = ctrl.canvas.clientHeight;

    var ctx = ctrl.canvas.getContext("2d", {alpha: false});
    var box = Pack.canvas(ctrl.canvas);

    ctx.fillStyle = "#fff";
    ctx.rect(0, 0, ctrl.canvas.width, ctrl.canvas.height);
    ctx.fill();

    for(var i = 0; i < ctrl.inst.length; i++) {
      var row = Pack.vert(box, ctrl.layout.height);
      var label = Pack.horiz(row, ctrl.layout.label);
      var line = Pack.horiz(row, 1);
      Draw.fill(line, ctx, "#000");

      var line = Pack.vert(box, 1);
      Draw.fill(line, ctx, "#000");
    }
  };
})();
