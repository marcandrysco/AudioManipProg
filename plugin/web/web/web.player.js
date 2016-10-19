(function() {
  "use strict";

  /*
   * Player namespace
   */
  window.Player = new Object();

  /**
   * Initialize a player.
   *   @work: The workspace.
   *   @data: The player data.
   */
  window.Player.init = function(work, player) {
    work.classList.add("player");
    player.active = null;
    player.sel = new Array();
    player.x = 0;
    player.conf.rows = Player.sort(player.conf.keys);
    player.vel = 32768;

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
    work.appendChild(head);

    player.roll = Gui.tag("canvas", "roll");
    player.roll.addEventListener("mousemove", Player.mouseMove(player));
    player.roll.addEventListener("mousedown", Player.mouseDown(player));
    player.roll.addEventListener("contextmenu", Player.context(player));
    player.roll.addEventListener("wheel", Player.wheel(player));
    player.roll.style.height = (layout.height() + layout.scroll + layout.head + 4) + 4 + "px";
    player.roll.style.border = "2px solid black";
    work.appendChild(player.roll);

    work.appendChild(player.debug = Gui.div());
    player.debug.id = "debug";

    Player.draw(player);
    window.addEventListener("resize", function() { Player.draw(player); });

    player.init = true;
    return player;
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
  window.Player.mouseDown = function(player) {
    return function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var x = e.offsetX, y = e.offsetY;
      var box = Pack.canvas(player.roll);
      Pack.vert(box, player.layout.scroll + 2, true);
      Pack.vert(box, player.layout.head + 2);
      Pack.horiz(box, player.layout.label + 2);

      if(box.inside(x, y)) {
        Player.mouseRoll(player, e, x, y, box);
      }
    };
  };

  /**
   * Handle a mouse down on the roll.
   *   @payer: The player.
   *   @e: The event.
   *   @x: The x-coordinate.
   *   @y: The y-coordinate.
   *   @box: The bounding box.
   */
  window.Player.mouseRoll = function(player, e, x, y, box) {
    var dat = Player.getDat(player, x, y);
    if(dat != null) {
      if(player.sel.indexOf(dat) >= 0) {
        if(e.shiftKey) {
          player.sel.splice(player.sel.indexOf(dat), 1);
        } else if(player.sel.length == 1) {
          player.sel = new Array();
        } else {
          player.sel = new Array(dat);
        }
      } else {
        if(!e.shiftKey) { player.sel = new Array(); }
        player.sel.push(dat);
        player.slider.guiUpdate(dat.vel / 65535);
      }
      Player.draw(player);
      return;
    }

    var xy = Player.getxy(player, x-box.x, y - box.y, true);
    if(xy == null) { return; }

    player.sel = new Array();
    player.active = {
      row: player.conf.rows[xy.row],
      begin: Loc.make(xy.bar, xy.beat),
      end: Loc.make(xy.bar, xy.beat)
    };

    Player.draw(player);

    Gui.dragNow(function(e, type) {
      switch(type) {
      case "move":
        var xy = Player.getxy(player, e.offsetX - box.x, e.offsetY - box.y, true);
        if(xy == null) { return; }
        player.active.end = Loc.make(xy.bar, xy.beat)
        Player.draw(player);
        break;

      case "down":
        break;

      case "up":
        var active = player.active;
        var begin = active.begin.copy(), end = active.end.copy();
        Loc.reorder(begin, end);
        end.beat += 1 / player.conf.ndivs;

        var dat = { key: active.row, begin: begin, end: end, vel: player.vel };
        player.data.push(dat);
        Player.draw(player);

        Web.put(player.idx, { type: "add", data: dat });
        break;

      case "done":
        player.active = null;
        Player.draw(player);
        break;
      }
    });
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
   *   @player
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
   * Retrieve the current datum.
   *   @player; The player.
   *   @x: The x-coordinate.
   *   @y: The y-coordinate.
   *   &returns: The datum or null.
   */
  window.Player.getDat = function(player, x, y) {
    var loc = Player.getLoc(player, x, false);
    var row = Player.getRow(player, y);

    if((loc == null) || (row < 0)) { return null; }

    for(var i = 0; i < player.data.length; i++) {
      var dat = player.data[i];
      if(dat.key != player.conf.rows[row]) { continue; }
      if(dat.begin.bar > loc.bar) { continue; }
      if((dat.begin.bar == loc.bar) && (dat.begin.beat >= loc.beat)) { continue; }
      if(dat.end.bar < loc.bar) { continue; }
      if((dat.end.bar == loc.bar) && (dat.end.beat <= loc.beat)) { continue; }
      return dat;
    }
    return null;
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
   * Handle a context window event.
   *   @player: The player.
   *   &returns: The handler.
   */
  window.Player.context = function(player) {
    return function(e) {
      e.preventDefault();

      var layout = player.layout;
      var box = Pack.canvas(player.roll);
      Pack.vert(box, layout.scroll + 2, true);
      Pack.vert(box, layout.head + 2);
      Pack.horiz(box, layout.label + 2);

      if(!box.inside(e.offsetX, e.offsetY)) { return; }

      var idx = Player.getidx(player, e.offsetX - box.x, e.offsetY - box.y);
      if(idx < 0) { return; }

      var dat = player.data[idx];
      Web.put(player.idx, { type: "remove", data: dat });

      var sel = player.sel.indexOf(dat);
      if(sel >= 0) { player.sel.splice(sel, 1); }

      player.data.splice(idx, 1);
      Player.draw(player);
    };
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

    Player.drawBack(ctx, box, conf, layout);

    // vertical lines and heading
    var blank = Pack.horiz(head, layout.label);
    Pack.horiz(head, 2);
    var b = 0, x = -player.x, bar = layout.bar(), ndivs = layout.ndivs(), width = layout.width();
    while(x < -bar) { x += bar; b++; }
    while(x < width) {
      Draw.text(head, ctx, (b++) + 1, {x: 4+x, y: 2, color: "#000", font: "16px sans-serif"});
      for(var i = 1; i < ndivs; i++) {
        x += layout.cell.width;
        if((i % conf.ndivs) == 0) {
          Draw.vert(box, x, ctx, 1, "#000");
          Draw.vert(head, x, ctx, 1, "#000");
          Draw.text(head, ctx, (i / conf.ndivs) + 1, {x: 4+x, y: 2, color: "#000", font: "12px sans-serif"});
        } else {
          Draw.vert(box, x, ctx, 1, "#bbb");
        }
        x++;
      }
      x += layout.cell.width;
      Draw.vert(box, x, ctx, 2, "#000");
      Draw.vert(head, x, ctx, 2, "#000");
      x += 2;
    }
    Draw.fill(blank, ctx, "#fff");
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

    // active box
    if(player.active) {
      var begin = player.active.begin.copy(), end = player.active.end.copy();
      Loc.reorder(begin, end);
      end.beat += 1 / conf.ndivs;

      var left = pwa(player, begin);
      var right = pwa(player, end);
      var width = Math.min(2, right - left - 1);
      var row = player.conf.rows.indexOf(player.active.row);
      if(row >= 0) {
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
    }

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

    // labels
    for(var i = 0; i < conf.rows.length; i++) {
      var key = (player.conf.rows[i] == 128) ? "Pedal" : Key.str(player.conf.rows[i]);
      var pack = Pack.vert(label, layout.cell.height);
      Draw.fill(pack, ctx, (key.length > 3) ? "#494949" : ((key.length > 2) ? "#000" : "#fff"));
      Draw.text(pack, ctx, key, {x: 2, y: 1, font: "16px sans-serif", align: "right", color: (key.length > 2) ? "#fff" : "#000"});
      Draw.fill(Pack.vert(label, 1), ctx, "#000");
    }
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
   *   @ctx: The context.
   *   @box: The rener area box.
   *   @conf: The configuration.
   *   @layout: The layout.
   */
  window.Player.drawBack = function(ctx, box, conf, layout) {
    var cur = box.copy();
    for(var i = 0; i < conf.rows.length; i++) {
      var row = Pack.vert(cur, layout.cell.height);
      Pack.vert(cur, 1);

      if((i % 2) == 0) { Draw.fill(row, ctx, "#eee"); }
    }
  };


  /**
   * Sort keys.
   *   @keys: The unsorted keys.
   *   &returns: The sorted keys.
   */
  window.Player.sort = function(keys) {
    return keys.sort(function(a,b) { return ((a < 128) && (b < 128)) ? (b - a) : (a - b); });
  };
})();
