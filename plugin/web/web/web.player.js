(function() {
  "use strict";

  /*
   * Player namespace
   */
  window.Player = new Object();

  /**
   * Create the element associated with the player.
   *   @player: The player.
   *   @idx: The index for requests.
   *   &returns: The DOM element.
   */
  window.Player.elem = function(player, idx) {
    var elem = Gui.div("player");

    player.active = null;
    player.idx = idx;
    player.sel = new Array();
    player.undo = new Array();
    player.redo = new Array();
    player.area = null;
    player.x = 0;
    player.conf.rows = Player.sort(player.conf.keys);
    player.vel = 32768;
    player.orig = {};

    player.layout = {
      scroll: 20,
      label: 48,
      head: 18,
      cell: { width: 16, height: 16 },
      width: function() { // render area width in pixels
        return player.roll.width - (player.layout.label + 2);
      },
      height: function() { // render area height in pixels
        return player.conf.rows.length * (player.layout.cell.height + 1) - 1;
      },
      bar: function() { // size of a bar in pixels
        return player.conf.ndivs * player.conf.nbeats * (player.layout.cell.width + 1) + 1;
      },
      ndivs: function() { // number of divs per bar
        return Math.round(player.conf.nbeats * player.conf.ndivs);
      },
      resize: function() {
        player.roll.style.height = (layout.height() + layout.scroll + layout.head + 4) + 4 + "px";
      }
    };

    var conf = player.conf;
    var layout = player.layout;

    var head = Gui.div("head");
    head.appendChild(Gui.div("title", Gui.text("Player")));
    head.appendChild(Gui.Toggle(["Enabled", "Disabled"], true, function() {
    }));
    head.appendChild(player.slider = Gui.Slider({input: 100}, function(slider, v) {
      player.vel = Math.round(v * 65535);
      slider.guiTrack.style.backgroundColor = Player.color(player.vel, false);

      for(var i = 0; i < player.sel.length; i++) {
        var dat = player.sel[i];
        dat.vel = player.vel;
        //Req.get("/" + player.idx + "/player/set/" + dat.key + "/" + dat.begin.bar + ":" + dat.begin.beat + "/" + dat.end.bar + ":" + dat.end.beat + "/" + dat.vel, function() {
        //});
      }

      if(player.init) { Player.draw(player); }
    }));
    head.appendChild(Gui.Button("Keys", {}, function(e) {
      Player.keys(player);
    }));
    Player.elemActions(player).forEach(function(el) { head.appendChild(el) });
    elem.appendChild(head);

    player.roll = Gui.tag("canvas", "roll");
    player.roll.addEventListener("mousemove", Player.mouseMove(player));
    player.roll.addEventListener("mousedown", Player.mouse(player));
    player.roll.addEventListener("wheel", Player.wheel(player));
    player.roll.style.height = (layout.height() + layout.scroll + layout.head + 4) + 4 + "px";
    player.roll.style.border = "2px solid black";
    elem.appendChild(player.roll);

    elem.appendChild(player.debug = Gui.div());
    player.debug.id = "debug";

    Player.draw(player);
    window.addEventListener("resize", function() { Player.draw(player); });

    player.init = true;
    return elem;
  };
  /**
   * Create the action buttons for the player.
   *   @player: The player.
   *   &returns: The button element.
   */
  window.Player.elemActions = function(player) {
    player.insert = Gui.Button("Insert", {}, function(e) {
      Status.update((Status.mode == Mode.insert) ? Mode.view : Mode.insert);
      Player.update(player);
    });
    player.remove = Gui.Button("Delete", {cls:"gui-red"}, function(e) {
      Player.remove(player);
    });
    player.copy = Gui.Button("Copy", {}, function(e) {
      player.copy.classList.add("gui-blue");
      Status.update(Mode.copy);

      player.orig.row = Player.mapKey(player, player.sel).reduce(function(p, v) { return Math.min(p, v); });
      player.orig.bar = Player.mapBegin(player, player.sel).reduce(function(p, v) { return Math.min(p, v); });
    });

    return [ player.insert, player.remove, player.copy ];
  };

  /**
   * Update the player.
   *   @player: The player.
   */
  window.Player.update = function(player) {
    Player.updateActions(player);
    Player.draw(player);
  };
  /**
   * Update the action buttons of the player.
   *   @player: The player.
   */
  window.Player.updateActions = function(player) {
    var flag = (Status.mode == Mode.insert);
    player.insert.classList[flag ? "add" : "remove"]("gui-green");
      
    var flag = (player.sel.length > 0);
    player.remove.classList[flag ? "remove" : "add"]("gui-hide");
    player.copy.classList[flag ? "remove" : "add"]("gui-hide");

    var flag = (Status.mode == Mode.copy);
    player.copy.classList[flag ? "add" : "remove"]("gui-blue");
  };


  /**
   * Create a mouse move handler.
   *   @player: The player.
   *   &returns: The event handler.
   */
  window.Player.mouseMove = function(player) {
    return function(e) {
    /*
      var dbg = Gui.byid("debug");
    
      var xy = Player.getLoc(player, e.offsetX, true);
      Gui.replace(dbg, Gui.text(JSON.stringify(xy)));
      */
    };
  };

  /**
   * Handle a mouse down on the player.
   *   @player: The player.
   *   &returns: The handler function.
   */
  window.Player.mouse = function(player) {
    return function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var x = e.offsetX, y = e.offsetY;
      var box = Pack.canvas(player.roll);
      Pack.vert(box, player.layout.scroll + 2, true);
      Pack.vert(box, player.layout.head + 2);
      Pack.horiz(box, player.layout.label + 2);

      if(!box.inside(x, y)) { return; }

      if(Status.mode == Mode.view) {
        Player.mouseView(player, e);
      } else if(Status.mode == Mode.copy ) {
        Player.mouseCopy(player, e);
      } else if(Status.mode == Mode.insert) {
        Player.mouseInsert(player, e);
      }
    };
  };
  /**
   * Handle a mouse down in view mode.
   *   @player: The player.
   *   @e: The Event.
   */
  window.Player.mouseView = function(player, e) {
    var x = e.offsetX, y = e.offsetY;
    player.area = { begin: { x: x, y: y }, end: { x: x, y: y } };

    Gui.dragNow(function(e, type) {
      switch(type) {
      case "move":
        if(e.target != player.roll) { return; }

        player.area.end = { x: e.offsetX, y: e.offsetY };
        Player.draw(player);
        break;

      case "up":
        var sel = Player.getDat(player, player.area.begin, player.area.end);
        if((sel.length == 1) && (player.sel.length == 1) && (sel[0] == player.sel[0])) {
          player.sel = [];
        } else {
          player.sel = sel;
        }
        break;

      case "done":
        player.area = null;
        Player.draw(player);
        break;
      }
    });
  };
  /**
   * Handle a mouse down in copy mode.
   *   @player: The player.
   *   @e: The Event.
   */
  window.Player.mouseCopy = function(player, e) {
    var nbeats = player.conf.nbeats;
    var row = Player.getRow(player, e.offsetY);
    var loc = Player.getLoc(player, e.offsetX, true);

    var add = new Array();
    var x = Loc.bar(loc, player.conf.nbeats) - player.orig.bar;
    var y = row - player.orig.row;

    for(var i = 0; i < player.sel.length; i++) {
      var dat = player.sel[i];
      var idx = player.conf.rows.indexOf(dat.key);
      if(idx < 0) { continue; }

      idx += y;
      if((idx < 0) || (idx >= player.conf.rows.length)) { continue; }

      add.push({
        key: player.conf.rows[idx],
        vel: dat.vel,
        begin: Loc.makef(Loc.bar(dat.begin, nbeats) + x, nbeats),
        end: Loc.makef(Loc.bar(dat.end, nbeats) + x, nbeats)
      });
    }

    Player.add(player, add);
  };
  /**
   * Handle a mouse down in insert mode.
   *   @player: The player.
   *   @e: The Event.
   */
  window.Player.mouseInsert = function(player, e) {
    player.sel = new Array();
    player.active = {
      row: Player.getRow(player, e.offsetY),
      begin: Player.getLoc(player, e.offsetX, true),
      end: Player.getLoc(player, e.offsetX, true)
    };

    Player.draw(player);

    Gui.dragNow(function(e, type) {
      switch(type) {
      case "move":
        player.active.end = Player.getLoc(player, e.offsetX, true);
        Player.draw(player);
        break;

      case "up":
        var dat = Player.getActive(player, false);

        Player.add(player, [dat]);
        break;

      case "done":
        player.active = null;
        Player.draw(player);
        break;
      }
    });
  };

  /**
   * Handle a mouse down on the player.
   *   @player: The player.
   *   @e: The event.
   */
  window.Player.keypress = function(player, e) {
    switch(e.key) {
    case "d":
      Player.remove(player);
      break;
    
    case "ArrowUp":
    case "k":
      Player.move(player, "up");
      break;
    
    case "ArrowDown":
    case "j":
      Player.move(player, "down");
      break;
    
    case "ArrowLeft":
    case "h":
      Player.move(player, "left");
      break;
    
    case "ArrowRight":
    case "l":
      Player.move(player, "right");
      break;

    case "u":
      Player.undo(player, false);
      break;

    case "U":
      Player.undo(player, true);
      break;
    }
  };


  /**
   * Insert a data point.
   *   @player: The player.
   *   @add: The list data points.
   */
  window.Player.add = function(player, add) {
    Web.put(player.idx, { type: "edit", data: add });

    Array.prototype.push.apply(player.data, add);
    Player.draw(player);

    player.undo.push({type: "remove", data: add});
    player.redo = new Array();
  };

  /**
   * Delete all selected data points.
   *   @player: The player.
   */
  window.Player.remove = function(player) {
    if(player.sel.length == 0) { return; }

    var rem = player.sel.map(function(dat) { return Player.copy(dat, true) });
    Web.put(player.idx, { type: "edit", data: rem });

    Array.prototype.remove.apply(player.data, player.sel);
    Player.draw(player);

    player.undo.push({type: "add", data: player.sel});
    player.redo = new Array();

    player.sel = new Array();
  };

  /**
   * Move selected elements.
   *   @player: The player.
   *   @dir: The direction. Either "up", "down", "left", or "right".
   *   @step: Optional. The step size, only applies to left or right shifts.
   */
  window.Player.move = function(player, dir, step) {
    if(player.sel.length == 0) { return; }
    if(!step) { step = 1 / player.conf.ndivs }

    var dup = player.sel.map(function(dat) { return Player.copy(dat); });

    switch(dir) {
    case "up":
      player.sel.forEach(function(dat) {
        var idx = player.conf.rows.indexOf(dat.key);
        if(idx > 0) { dat.key = player.conf.rows[idx-1]; }
      });
      break;

    case "down":
      player.sel.forEach(function(dat) {
        var idx = player.conf.rows.indexOf(dat.key);
        if((idx >= 0) && (idx < (player.conf.rows.length - 1))) { dat.key = player.conf.rows[idx+1]; }
      });
      break;

    case "left":
      player.sel.forEach(function(dat) { dat.begin.beat -= step; dat.end.beat -= step; });
      break;

    case "right":
      player.sel.forEach(function(dat) { dat.begin.beat += step; dat.end.beat += step; });
      break;

    default:
      throw "Invalid move direction.";
    }

    dup.forEach(function(dat) { dat.vel = 0; });
    dup = dup.concat(player.sel);
    Web.put(player.idx, { type: "edit", data: dup });

    Player.draw(player);
  };

  /**
   * Undo an action if available.
   *   @player: The player.
   *   @rev: The reverse flag.
   */
  window.Player.undo = function(player, rev) {
    var list = rev ? player.redo : player.undo;
    var back = rev ? player.undo : player.redo

    if(list.length == 0) { return; }

    var act = list.pop();
    if(act.type == "remove") {
      act.data.forEach(function(dat) { player.data.remove(dat); });

      var dup = act.data.map(function(dat) { return Player.copy(dat, true); });
      Web.put(player.idx, { type: "edit", data: dup });

      back.push({ type: "add", data: act.data });
    }
    else if(act.type == "add") {
      act.data.forEach(function(dat) { player.data.push(dat); });
      Web.put(player.idx, { type: "edit", data: act.data });

      back.push({ type: "remove", data: act.data });
    }
  };

  /**
   * Retrieve the current row.
   *   @player: The player.
   *   @y: The y-coordinate.
   *   &returns: The row number, or '-1' if not over a row.
   */
  window.Player.getRow = function(player, y) {
    var rowHeight = player.layout.cell.height + 1;
    var rowLimit = player.conf.rows.length;
    var row = Math.floor((y - player.layout.head - 2) / rowHeight);
    
    return ((row >= 0) && (row < rowLimit)) ? row : -1;
  };

  /**
   * Retrieve the current location.
   *   @player: The player.
   *   @x: The x-coordinate.
   */
  window.Player.getLoc = function(player, x, round) {
    var layout = player.layout, conf = player.conf;

    x -= layout.label + 2;
    if(x < 0) { return null; }

    var bar = Math.floor((x + player.x) / layout.bar());
    var div = (x - bar * layout.bar()) / (layout.cell.width + 1);
    var beat = (round ? Math.floor(div) : div) / conf.ndivs;
    if(beat >= conf.nbeats) { bar++; beat = 0; }

    return Loc.make(bar, beat);
  };

  /**
   * Retrieve the data under an area.
   *   @player; The player.
   *   @begin: The begin coordinates.
   *   @end: The end coordinates.
   *   &returns: The data array.
   */
  window.Player.getDat = function(player, begin, end) {
    var right = Player.getLoc(player, Math.max(begin.x, end.x), false);
    var left = Player.getLoc(player, Math.min(begin.x, end.x), false);

    var first = Player.getRow(player, Math.min(begin.y, end.y))
    var last = Player.getRow(player, Math.max(begin.y, end.y))

    if((first < 0) || (last < 0) || (left == null) || (right == null)) { return []; };

    var ret = new Array(), rows = new Array();
    for(var i = first; i <= last; i++) {
      rows.push(player.conf.rows[i]);
    }

    for(var i = 0; i < player.data.length; i++) {
      var dat = player.data[i];

      if(rows.indexOf(dat.key) < 0) { continue; }

      if(dat.begin.bar > right.bar) { continue; }
      if((dat.begin.bar == right.bar) && (dat.begin.beat >= right.beat)) { continue; }

      if(dat.end.bar < left.bar) { continue; }
      if((dat.end.bar == left.bar) && (dat.end.beat <= left.beat)) { continue; }

      ret.push(dat);
    }

    return ret;
  };

  /**
   * Retrieve the active range with ordered begin and ending.
   *   @player: The player.
   *   &returns: The data or null.
   */
  window.Player.getActive = function(player, row) {
    if(player.active == null) { return null; }

    var dat = new Object();
    dat.key = player.conf.rows[player.active.row];
    dat.vel = player.vel;
    dat.begin = player.active.begin.copy();
    dat.end = player.active.end.copy();
    Loc.reorder(dat.begin, dat.end);

    dat.end.beat += 1 / player.conf.ndivs;
    if(row) { dat.row = player.active.row; }

    return dat;
  };


  /**
   * Create the keys configuration UI.
   *   @player: The player.
   *   &returns: The UI.
   */
  window.Player.keys = function(player) {
    var keys = Gui.div("player-keys");

    var grid = Gui.div("grid");
    keys.appendChild(grid);

    var head = Gui.div("row head");
    head.appendChild(Gui.div("label"));
    for(var v = 0; v < 12; v++) {
      head.appendChild(Gui.div("col", Gui.text(Key.letter(v))));
    }
    grid.appendChild(head);

    var idx = 0, rows = new Array();
    for(var n = 0; n <= 9; n++) {
      var row = Gui.div("row");
      row.appendChild(Gui.div("label", Gui.text(n)));

      for(var v = 0; v < 12; v++) {
        row.appendChild(Player.keys.box(player.conf, idx++, rows));
      }

      grid.appendChild(row);
    }

    var row = Gui.div("row");
    row.appendChild(Gui.div("label"));
    var pedal = Player.keys.box(player.conf, 128, rows);
    pedal.classList.add("pedal");
    pedal.appendChild(Gui.text("Pedal"));
    row.appendChild(pedal);
    grid.appendChild(row);

    grid.addEventListener("mousedown", function(e) {
      if(!e.target.classList.contains("box")) { return; }
      e.preventDefault();
      var func = e.target.classList.contains("sel") ? "remove" : "add";
      Gui.dragNow(function(e, type) {
        if(!e.target.classList.contains("box")) { return; }
        e.target.classList[func]("sel");
        var idx = rows.indexOf(e.target.xIdx);
        if(func == "add") {
          if(idx < 0) { rows.push(e.target.xIdx); }
        } else {
          if(idx >= 0) { rows.splice(idx, 1); }
        }
      });
    });

    var action = Gui.div("action");
    action.appendChild(Gui.Button("Accept", {}, function(e) {
      player.conf.rows = Player.sort(rows);
      player.layout.resize();
      popup.guiDismiss();
      Player.draw(player);
      Req.post("/" + player.idx + "/player/keys", JSON.stringify(rows), function(v) {
      });
    }));
    action.appendChild(Gui.Button("Cancel", {}, function(e) {
      popup.guiDismiss();
    }));
    keys.appendChild(action);

    var popup = Gui.Popup(keys, function(e) {
    });
    popup.addEventListener("keyup", function(e) {
      if(e.keyCode == 27) { popup.guiDismiss(); }
    });
    document.body.appendChild(popup);
  };
  /**
   * Create a box for the keys display.
   *   @conf: The player configuration.
   *   @idx: The index.
   *   @rows: The rows arrays.
   */
  window.Player.keys.box = function(conf, idx, rows) {
    var box = Gui.div("box");
    box.xIdx = idx;
    if(conf.rows.indexOf(box.xIdx) >= 0) {
      box.classList.add("sel");
      rows.push(box.xIdx);
    }
    return box;
  };

  /**
   * Retrieve the row, beat and bar information from a set of coordinates.
   *   @player: The player.
   *   @x: The x-coordinate.
   *   @y: THe y-coordinate.
   *   @round: The rounding.
   *   &returns: The information or null.
   */
  window.Player.getxy = function(player, x, y, round) {
    var layout = player.layout, conf = player.conf;
    var row = Math.min(Math.floor(y / (layout.cell.height + 1)), conf.rows.length - 1);
    var bar = Math.floor(x / layout.bar());
    var div = (x - bar * layout.bar()) / (layout.cell.width + 1);
    var beat = (round ? Math.floor(div) : div) / conf.ndivs;
    if(beat >= conf.nbeats) { bar++; beat = 0; }

    return { row: row, bar: bar, beat: beat, div: div };
  };

  /**
   * Retrieve the data index at a given coordinate.
   *   @player; The player.
   *   @x: The x-coordinate.
   *   @y: The y-coordinate.
   *   &returns: The index or '-1' if no matches.
   */
  window.Player.getidx = function(player, x, y) {
    var xy = Player.getxy(player, x, y, false);
    if(xy == null) { return; }

    for(var i = 0; i < player.data.length; i++) {
      var dat = player.data[i];
      if(dat.key != player.conf.rows[xy.row]) { continue; }
      if(dat.begin.bar > xy.bar) { continue; }
      if((dat.begin.bar == xy.bar) && (dat.begin.beat >= xy.beat)) { continue; }
      if(dat.end.bar < xy.bar) { continue; }
      if((dat.end.bar == xy.bar) && (dat.end.beat <= xy.beat)) { continue; }
      return i;
    }
    return -1;
  };

  /**
   * Compute the row and index from coordinates.
   *   @player: The player.
   *   @x: The x-coordinate.
   *   @y: The y-coordinate.
   *   &returns: The row and index pair.
   */
  window.Player.coord = function(player, x, y) {
    if(x < player.layout.label) { return { row: -1, idx: -1 }; }
    x -= player.layout.label;
    var layout = player.layout;
    var row = Math.floor((y - layout.head.off) / (layout.cell.h + 1))
    var col = x + player.x;
    var bar = Math.floor(col / layout.bar(true));
    col -= bar * layout.bar(true);
    var beat = Math.floor(col / layout.beat(true));
    col -= beat * layout.beat(true);
    var div = Math.floor(col / layout.div(true));
    var idx = div + player.conf.ndivs * (beat + bar * player.conf.nbeats);
    return { row: row, idx: idx };
  };

  /**
   * Retrieve the index of an instance at a row and index.
   *   @player: The player.
   *   @row: The row.
   *   @idx: The index.
   *   &returns: The index or negative.
   */
  window.Player.get = function(player, row, idx) {
    for(var i = 0; i < player.data.length; i++) {
      var dat = player.data[i];
      if((row == dat.row) && (idx >= dat.begin) && (idx <= dat.end)) {
        return i;
      }
    }
    return -1;
  };
 
  /**
   * Compute the CSS color for a value.
   *   @val: The value.
   *   @orange: Orange flag.
   *   &returns: The CSS color string.
   */
  window.Player.color = function(val, orange) {
    var t = 200 - Math.round(val / 65535 * 200);
    return "rgb(255,"+(orange?Math.floor(128+t/2):t)+","+t+")";
  };

  /**
   * Handle a wheel on the player.
   *   @player: The player.
   *   &returns: The handler function.
   */
  window.Player.wheel = function(player) {
    return function(e) {
      player.x += e.deltaY;
      if(player.x < 0) { player.x = 0; }
      Player.draw(player);
    };
  };

  /**
   * Draw the player to the canvas.
   *   @player: The player.
   */
  window.Player.draw = function(player) {
    player.roll.width = player.roll.clientWidth;
    player.roll.height = player.roll.clientHeight;

    var conf = player.conf;
    var layout = player.layout;
    var ctx = player.roll.getContext("2d", {alpha: false});

    Player.drawBlank(player, ctx);

    var box = Pack.canvas(player.roll);
    var scroll = Pack.vert(box, layout.scroll, true);
    Draw.fill(Pack.vert(box, 2, true), ctx, "#000");
    var line = Pack.horiz(Pack.horiz(box.copy(), layout.label + 2), 2, true);
    var head = Pack.vert(box, layout.head);
    Draw.fill(Pack.vert(box, 2), ctx, "#000");
    var label = Pack.horiz(box, layout.label);
    Pack.horiz(box, 2);

    Player.drawBack(player, ctx, box);

    // vertical lines and heading
    var blank = Pack.horiz(head, layout.label);
    Pack.horiz(head, 2);
    Player.drawHead(player, ctx, head);
    Player.drawBody(player, ctx, box);
    Draw.fill(label, ctx, "#fff");
    Draw.fill(line, ctx, "#000");

    // horizontal lines
    var y = layout.cell.height;
    for(var i = 1; i < conf.rows.length; i++) {
      Draw.horiz(box, i * (layout.cell.height + 1) - 1, ctx, 1, "#000");
    }

      var pwa = function(player, loc) {
        var x = -player.x;
        x += loc.bar * player.layout.bar();
        x += loc.beat * (player.layout.cell.width + 1) * player.conf.ndivs;
        return Math.round(x);
      };

    // remaining boxes
    for(var i = 0; i < player.data.length; i++) {
      var dat = player.data[i];
      var left = pwa(player, dat.begin);
      var right = pwa(player, dat.end);
      var width = Math.min(2, right - left - 1);
      var row = player.conf.rows.indexOf(dat.key);
      if(row < 0) { continue; }
      var y = row * (layout.cell.height + 1)
      ctx.fillStyle = Player.color(dat.vel, player.sel.indexOf(dat) >= 0);
      ctx.beginPath();
      ctx.rect(box.x + left, box.y + y, right - left - 1, layout.cell.height);
      ctx.fill();
      ctx.fillStyle = "#000";
      ctx.beginPath();
      ctx.rect(box.x + left - 1, box.y + y, 1, layout.cell.height);
      ctx.rect(box.x + right - 1, box.y + y, 1, layout.cell.height);
      ctx.fill();
    }

    // active box
    if(player.active) {
      var dat = Player.getActive(player, true);

      var left = pwa(player, dat.begin);
      var right = pwa(player, dat.end);
      var width = Math.min(2, right - left - 1);
      var row = dat.row;
      var y = row * (layout.cell.height + 1)
      ctx.fillStyle = Player.color(player.vel, true);
      ctx.beginPath();
      ctx.rect(box.x + left, box.y + y, right - left - 1, layout.cell.height);
      ctx.fill();
      ctx.fillStyle = "#000";
      ctx.beginPath();
      ctx.rect(box.x + left - 1, box.y + y, 1, layout.cell.height);
      ctx.rect(box.x + right - 1, box.y + y, 1, layout.cell.height);
      ctx.fill();
    }

    // labels
    for(var i = 0; i < conf.rows.length; i++) {
      var key = (player.conf.rows[i] == 128) ? "Pedal" : Key.str(player.conf.rows[i]);
      var pack = Pack.vert(label, layout.cell.height);
      Draw.fill(pack, ctx, (key.length > 3) ? "#494949" : ((key.length > 2) ? "#000" : "#fff"));
      Draw.text(pack, ctx, key, {x: 2, y: 1, font: "16px sans-serif", align: "right", color: (key.length > 2) ? "#fff" : "#000"});
      Draw.fill(Pack.vert(label, 1), ctx, "#000");
    }

    Player.drawArea(player, ctx);
  };
  /**
   * Draw the blank background.
   *   @player: The player.
   *   @ctx: The context.
   */
  window.Player.drawBlank = function(player, ctx) {
    ctx.fillStyle = "#fff";
    ctx.rect(0, 0, player.roll.width, player.roll.height);
    ctx.fill();
  };
  /**
   * Draw the background stripes.
   *   @player: The player.
   *   @ctx: The context.
   *   @box: The rener area box.
   */
  window.Player.drawBack = function(player, ctx, box) {
    var cur = box.copy();
    for(var i = 0; i < player.conf.rows.length; i++) {
      var row = Pack.vert(cur, player.layout.cell.height);
      Pack.vert(cur, 1);

      if((i % 2) == 0) { Draw.fill(row, ctx, "#eee"); }
    }
  };
  /**
   * Draw the text and lines in the header.
   *   @player: The player.
   *   @ctx: The noctext.
   *   @head: The header box.
   */
  window.Player.drawHead = function(player, ctx, head) {
    ctx.save();
    Draw.clip(head, ctx);
    var conf = player.conf, layout = player.layout;
    var b = 0, x = -player.x, bar = layout.bar(), width = layout.width();
    while(x < -bar) { x += bar; b++; }
    while(x < width) {
      Draw.text(head, ctx, (b++) + 1, {x: (4 + x), y: 2, color: "#000", font: "16px sans-serif"});
      for(var i = 1; i < conf.nbeats; i++) {
        x += (layout.cell.width + 1) * conf.ndivs;
        Draw.vert(head, x-1, ctx, 1, "#000");
        Draw.text(head, ctx, i + 1, {x: (4 + x), y: 2, color: "#000", font: "12px sans-serif"});
      }
      x += (layout.cell.width + 1) * conf.ndivs + 1;
      Draw.vert(head, x - 2, ctx, 2, "#000");
    }
    ctx.restore();
  };
  /**
   * Draw the text and lines in the header.
   *   @player: The player.
   *   @ctx: The noctext.
   *   @body: The body box.
   */
  window.Player.drawBody = function(player, ctx, body) {
    ctx.save();
    Draw.clip(body, ctx);

    var conf = player.conf, layout = player.layout;
    var b = 0, x = -player.x, bar = layout.bar(), width = layout.width(), ndivs = layout.ndivs();
    while(x < -bar) { x += bar; b++; }
    while(x < width) {
      for(var i = 1; i < ndivs; i++) {
        x += layout.cell.width + 1;
        Draw.vert(body, x - 1, ctx, 1, ((i % conf.ndivs) == 0) ? "#000" : "#bbb");
      }
      x += layout.cell.width + 2;
      Draw.vert(body, x - 2, ctx, 2, "#000");
    }

    var line = player.layout.bar() * Loc.bar(Time.loc, player.conf.nbeats);
    Draw.vert(body, Math.floor(line - player.x), ctx, 2, "#5a5");

    ctx.restore();
  };
  /**
   * Draw the selection area, if it is set.
   *   @player: The player.
   *   @ctx: The context.
   */
  window.Player.drawArea = function(player, ctx) {
    if(player.area == null) { return; }

    var begin = player.area.begin, end = player.area.end;
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
   * Sort keys.
   *   @keys: The unsorted keys.
   *   &returns: The sorted keys.
   */
  window.Player.sort = function(keys) {
    return keys.sort(function(a,b) { return ((a < 128) && (b < 128)) ? (b - a) : (a - b); });
  };

  /**
   * Copy a data point.
   *   @dat: The data point.
   *   @zero: The zero flag for removing velocity.
   *   &returns: The copied data point.
   */
  window.Player.copy = function(dat, zero) {
    return {
      key: dat.key,
      vel: zero ? 0 : dat.vel,
      begin: { bar: dat.begin.bar, beat: dat.begin.beat },
      end: { bar: dat.end.bar, beat: dat.end.beat }
    };
  };


  /**
   * Map a list of data to their keys, filtering invalid rows.
   *   @player: The player.
   *   @data: The data list.
   *   &returns: The mapped list.
   */
  window.Player.mapKey = function(player, data) {
    data = data.map(function(v) { return player.conf.rows.indexOf(v.key); });
    data = data.filter(function(v) { return v >= 0; });
    return data;
  };

  /**
   * Map a list of data to their beginning time as a floating point bar.
   *   @player: The player.
   *   @data: The data list.
   *   &returns: The mapped list.
   */
  window.Player.mapBegin = function(player, data) {
    return data.map(function(dat) { return Loc.bar(dat.begin, player.conf.nbeats); });
  };
})();
