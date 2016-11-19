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
    ctrl.area = null;
    ctrl.conf = { nbars: 200, nbeats: 4, ndivs: 4 };
    ctrl.sel = new Array();
    ctrl.undo = new Array();
    ctrl.redo = new Array();

    var div = Gui.div("ctrl");
    div.appendChild(Ctrl.elemHead(ctrl));
    div.appendChild(ctrl.canvas = Gui.tag("canvas", "ctrl"));
    div.appendChild(Ctrl.elemPanel(ctrl));

    Ctrl.draw(ctrl);
    window.addEventListener("resize", function() { Ctrl.draw(ctrl); });
    setTimeout(function() { Ctrl.draw(ctrl); }, 10);

    var dbg = div.appendChild(Gui.div("dbg"));

    Gui.replace(dbg, Gui.text(HSL.make(0,1,1)));
    ctrl.canvas.addEventListener("mousedown", Ctrl.mouse(ctrl));
    ctrl.canvas.addEventListener("wheel", Ctrl.wheel(ctrl));
    ctrl.canvas.addEventListener("mousemove", function(e) {
      Gui.replace(dbg, Gui.text(Ctrl.getRow(ctrl, e.offsetY) + " : " +
        JSON.stringify(Ctrl.getLoc(ctrl, e.offsetX, true))));
      //Gui.replace(dbg, Gui.text(Ctrl.getRow(ctrl, e.offsetY)));
    });

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

    head.appendChild(Gui.Toggle(["Enabled", "Disabled"], {def:true,fixed:true}, function(val) {
    }));

    head.appendChild(ctrl.rec = Gui.Toggle(["Play", "Record"], {def:true,fixed:true}, function(val) {
      Web.put(ctrl.idx, { type: "rec", data: !val });
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
          Web.put(ctrl.idx, { type: "action", data: { idx: idx, val: Math.round(65535 * val) } });
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
   * Update the controller.
   *   @ctrl: The controller.
   *   @data: The data.
   */
  window.Ctrl.update = function(ctrl, data) {
    for(var i = 0; i < data.length; i++) {
      switch(data[i].type) {
      case "rec":
        ctrl.rec.guiSet(!data[i].data);
        break;

      case "event":
        ctrl.events.push(data[i].data);
        ctrl.events = ctrl.events.sort(function(a, b) { return Loc.cmp(a.loc, b.loc); });
        break;
      }
    }

    Ctrl.draw(ctrl);
  };

 
  /**
   * Handle a mouse press on the controller.
   *   @ctr: The controller.
   *   &returns: The handler.
   */
  window.Ctrl.mouse = function(ctrl) {
    return function(e) {
      e.preventDefault();

      if(Status.mode == Mode.view) {
        Ctrl.mouseView(ctrl, e);
      }
    };
  };
  /**
   * Process a mouse press in view mode.
   *   @ctrl: The controller.
   *   @e: The event.
   */
  window.Ctrl.mouseView = function(ctrl, e) {
    var x = e.offsetX, y = e.offsetY;

    ctrl.sel = [];
    ctrl.area = { begin: { x: x, y: y }, end: { x: x, y: y } };

    Gui.dragNow(function(e, type) {
      switch(type) {
      case "move":
      case "up":
        if(e.target != ctrl.canvas) { return; }

        ctrl.area.end = { x: e.offsetX, y: e.offsetY };
        ctrl.sel = Ctrl.getSel(ctrl, ctrl.area.begin, ctrl.area.end);
        Ctrl.draw(ctrl);
        break;

      case "done":
        ctrl.area = null;
        Ctrl.draw(ctrl);
        break;
      }
    });
  };

  /**
   * Handle a wheel on the controller.
   *   @ctrl: The controller.
   *   &returns: The handler function.
   */
  window.Ctrl.wheel = function(ctrl) {
    return function(e) {
      ctrl.x += e.deltaY;
      if(ctrl.x < 0) { ctrl.x = 0; }
      Ctrl.draw(ctrl);
    };
  };

  /**
   * Handle a keypress.
   *   @ctrl: The controller.
   *   @e: The event.
   */
  window.Ctrl.keypress = function(ctrl, e) {
    switch(e.key) {
    case "d":
      Ctrl.remove(ctrl);
      break;

    case "r":
      Web.put(ctrl.idx, { type: "rec", data: ctrl.rec.guiState });
      break;
    }
  };


  /**
   * Remove all selected events.
   *   @ctrl: The controller.
   */
  window.Ctrl.remove = function(ctrl) {
    if(ctrl.sel.length == 0) { return; }

    Web.put(ctrl.idx, { type: "rem", data: ctrl.sel });

    Array.prototype.remove.apply(ctrl.events, ctrl.sel);
    Ctrl.draw(ctrl);

    ctrl.undo.push({type: "add", data: ctrl.sel});
    ctrl.redo = new Array();

    ctrl.sel = new Array();
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
    Ctrl.drawArea(ctrl, ctx);
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

    Grid.head(row, ctx, ctrl.layout.width, ctrl.x, ctrl.conf.nbeats, ctrl.conf.ndivs);
  };
  /**
   * Draw the controller body.
   *   @ctrl: The controller.
   *   @ctx: The context.
   *   @box: The box.
   */
  window.Ctrl.drawBody = function(ctrl, ctx, box) {
    var orig = box.copy();

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

        var x = Grid.getx(ctrl.events[j].loc, ctrl.layout.width, ctrl.conf.nbeats, ctrl.conf.ndivs) - ctrl.x;
        var c = 1.0 - 0.5 * ctrl.events[j].val / 65535;

        if(ctrl.sel.includes(ctrl.events[j])) {
          Draw.vert(row, x, ctx, 2, HSL.make(1/16, 1, c - 0.1));
          Draw.vert(row, x + 2, ctx, box.width - x, HSL.make(1/16, 0.5, c));
        } else {
          Draw.vert(row, x, ctx, 2, HSL.make(0, 1, c - 0.1));
          Draw.vert(row, x + 2, ctx, box.width - x, HSL.make(0, 0.5, c));
        }
      }

      ctx.restore();
    }


    var x = Grid.getx(Time.loc, ctrl.layout.width, ctrl.conf.nbeats, ctrl.conf.ndivs) - ctrl.x;
    if(x > 0) {
      ctx.fillStyle = "rgba(0,0,0,0.5)";
      ctx.beginPath();
      ctx.rect(orig.x + ctrl.layout.label + 2 + x, orig.y, 2, (ctrl.layout.height + 1) * ctrl.inst.length - 1);
      ctx.fill();
    }
  };
  /**
   * Draw the selection area, if it is set.
   *   @player: The player.
   *   @ctx: The context.
   */
  window.Ctrl.drawArea = function(ctrl, ctx) {
    if(ctrl.area == null) { return; }

    var begin = ctrl.area.begin, end = ctrl.area.end;
    if((begin.x == end.x) && (begin.y == end.y)) { return; };

    ctx.fillStyle = "rgba(0,0,255,0.2)";
    ctx.strokeStyle = "rgba(0,0,255,0.6)";
    ctx.lineWidth = 0.5;
    ctx.beginPath();
    ctx.rect(begin.x, begin.y, end.x - begin.x, end.y - begin.y);
    ctx.fill();
    ctx.stroke();
  };


  /**
   * Retrieve the current row.
   *   @ctrl: The controller.
   *   @y: The y-coordinate.
   *   &returns: The row number, or '-1' if not over a row.
   */
  window.Ctrl.getRow = function(ctrl, y) {
    return Grid.getRow(ctrl.layout.height, y, ctrl.inst.length);
  };

  /**
   * Retrieve a location.
   *   @ctrl: The controller.
   *   @x; The x-coordinate.
   *   @round: The rounding to div flag.
   *   &returns: The location or null.
   */
  window.Ctrl.getLoc = function(ctrl, x, round) {
    if(x < (ctrl.layout.label + 2)) { return null; }

    x += ctrl.x - ctrl.layout.label - 2;
    return Grid.getLoc(ctrl.layout.width, x, ctrl.conf.nbeats, ctrl.conf.ndivs, round);
  };

  /**
   * Retrieve the data under an area.
   *   @player; The player.
   *   @begin: The begin coordinates.
   *   @end: The end coordinates.
   *   &returns: The data array.
   */
  window.Ctrl.getSel = function(ctrl, begin, end) {
    var right = Ctrl.getLoc(ctrl, Math.max(begin.x, end.x));
    var left = Ctrl.getLoc(ctrl, Math.min(begin.x, end.x));

    var first = Ctrl.getRow(ctrl, Math.min(begin.y, end.y))
    var last = Ctrl.getRow(ctrl, Math.max(begin.y, end.y))

    if((first < 0) || (last < 0) || (left == null) || (right == null)) { return []; };

    var ret = new Array();

    for(var i = 0; i < ctrl.events.length; i++) {
      var cur = ctrl.events[i];

      if((cur.idx < first) || (cur.idx > last)) { continue; }

      if(cur.loc.bar > right.bar) { continue; }
      if((cur.loc.bar == right.bar) && (cur.loc.beat >= right.beat)) { continue; }

      if(cur.loc.bar < left.bar) { continue; }
      if((cur.loc.bar == left.bar) && (cur.loc.beat <= left.beat)) { continue; }

      ret.push(cur);
    }

    return ret;
  };
})();
