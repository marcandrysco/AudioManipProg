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
    ctrl.x = 0;
    ctrl.idx = idx;
    ctrl.layout = Ctrl.layout();
    ctrl.conf = { nbars: 200, nbeats: 4, ndivs: 4 };
    
    var div = Gui.div("ctrl");
    div.appendChild(Ctrl.elemHead(ctrl));
    div.appendChild(ctrl.canvas = Gui.tag("canvas", "ctrl"));
    div.appendChild(Ctrl.elemPanel(ctrl));

    Ctrl.draw(ctrl);
    window.addEventListener("resize", function() { Ctrl.draw(ctrl); });
    setTimeout(function() { Ctrl.draw(ctrl); }, 10);

    return div;
  };
  /**
   * Create the header element.
   *   @ctrl: The controller.
   *   &returns: The header element.
   */
  window.Ctrl.elemHead = function(ctrl) {
    var head = Gui.div("head");

    head.appendChild(Gui.div("title", Gui.text("Controller")));

    head.appendChild(Gui.Toggle(["Enabled", "Disabled"], true, function() {
    }));

    head.appendChild(Gui.Toggle(["Play", "Record"], true, function() {
    }));

    return head;
  };
  /**
   * Create the panel subelement.
   *   @ctrl: The controller.
   *   &retuns: The DOM element.
   */
  window.Ctrl.elemPanel = function(ctrl) {
    var panel = Gui.div("panel");

    for(var i = 0; i < ctrl.inst.length; i++) {
      (function() {
        var idx = i, mod = Gui.div("mod"), init = false;

        mod.appendChild(Gui.div("name", Gui.text(ctrl.inst[i].id)))
        mod.appendChild(Gui.Slider({cls:"slider",vert:true,input:100}, function(el, val) {
          if(!init) { return; }
          Web.put(ctrl.idx, { type: "action", data: { idx: idx, val: Math.round(255 * val) } });
        }));
        mod.appendChild(Gui.Button("Config", {cls:"conf"}, function(e) {
          document.body.appendChild(Ctrl.conf(ctrl, idx));
        }));

        init = true;
        panel.appendChild(mod);
      })();
    }

    return panel;
  };

  /**
   * Create the configuration popup.
   *   @ctrl: The controller.
   *   @idx: The index.
   */
  window.Ctrl.conf = function(ctrl, idx) {
    var popup, elem = Gui.div("ctrl-conf");

    return popup = Gui.Popup(elem, function(e) { });
  };

  /**
   * Crreate the layout object.
   *   &returns: The layout.
   */
  window.Ctrl.layout = function() {
    return {
      height: 18,
      width: 18,
      label: 80
    };
  };


  /**
   * Retrieve the height of the canvas.
   *   @ctrl: The control.
   *   &returns: The height.
   */
  window.Ctrl.getHeight = function(ctrl) {
    return (ctrl.layout.height + 1) * ctrl.inst.length - 2 + ctrl.layout.height + 2;
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

    Ctrl.drawHead(ctrl, ctx, box);
    Ctrl.drawBody(ctrl, ctx, box);
  };
  /**
   * Draw the controller header.
   *   @ctrl: The controller.
   *   @ctx: The context.
   *   @box: The box.
   */
  window.Ctrl.drawHead = function(ctrl, ctx, box) {
    var row = Pack.vert(box, ctrl.layout.height);
    Pack.horiz(row, ctrl.layout.label);

    var hline = Pack.horiz(row, 2);
    Draw.fill(hline, ctx, "#000");

    var vline = Pack.vert(box, 2);
    Draw.fill(vline, ctx, "#000");

    Grid.head(row, ctx, ctrl.layout.width, 0, ctrl.conf.nbeats, ctrl.conf.ndivs);
  };
  /**
   * Draw the controller body.
   *   @ctrl: The controller.
   *   @ctx: The context.
   *   @box: The box.
   */
  window.Ctrl.drawBody = function(ctrl, ctx, box) {
    for(var i = 0; i < ctrl.inst.length; i++) {
      var row = Pack.vert(box, ctrl.layout.height);

      var label = Pack.horiz(row, ctrl.layout.label);
      Draw.text(label, ctx, ctrl.inst[i].id, {x: 4, y: 2, color: "#000", font: "16px sans-serif"});

      var hline = Pack.horiz(row, 2);
      Draw.fill(hline, ctx, "#000");

      var line = Pack.vert(box, 1);
      Draw.fill(line, ctx, "#000");

      ctx.save();
      Draw.clip(row, ctx);

      for(var j = 0; j < ctrl.events.length; j++) {
        if(ctrl.events[j].idx != i) { continue; }

        var x = Grid.getx(ctrl.events[j].loc, ctrl.layout.width, 0, ctrl.conf.nbeats, ctrl.conf.ndivs);
        Draw.vert(row, x, ctx, 2, "#000");
        var c = 255-Math.round(255*ctrl.events[j].val/65535);
        Draw.vert(row, x + 2, ctx, box.width - x, "rgb(255,"+c+","+c+")");
      }

      ctx.restore();
    }
  };


  /*
   * Grid namespace
   */
  window.Grid = new Object();

  /**
   * Create the grid header.
   *   @box: The target box.
   *   @ctx: The context.
   *   @width: The width.
   *   @x: The starting x-coordinate.
   *   @nbeats: The number of beats.
   *   @ndivs: The number of divisions.
   */
  window.Grid.head = function(box, ctx, width, x, nbeats, ndivs) {
    var bar = 1, width = (width + 1) * ndivs - 1;
    while(x < box.width) {
      Draw.text(box, ctx, bar++, {x: (4 + x), y: 2, color: "#000", font: "16px sans-serif"});
      for(var i = 1; i < nbeats; i++) {
        x += width;
        Draw.vert(box, x++, ctx, 1, "#888");
        Draw.text(box, ctx, (i+1), {x: (4 + x), y: 2, color: "#000", font: "12px sans-serif"});
      }
      x += width;
      Draw.vert(box, x, ctx, 2, "#000");
      x += 2;
    }
  };

  /**
   * Retrieve the x-coordinate.
   *   @loc: The location.
   *   @width: The width.
   *   @x: The starting x-coordinate.
   *   @nbeats: The number of beats.
   *   @ndivs: The number of divisions.
   */
  window.Grid.getx = function(loc, width, x, nbeats, ndivs) {
    var barlen = (width + 1) * ndivs * nbeats + 1;
    var beatlen = (width + 1) * ndivs;

    return barlen * loc.bar + beatlen * loc.beat;
  };
})();
