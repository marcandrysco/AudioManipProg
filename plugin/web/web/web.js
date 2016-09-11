(function() {
  "use strict";

  /*
   * Web namespace
   */
  window.Web = new Object();

  /**
   * Initialize the web framework.
   */
  window.Web.init = function() {
    Web.data = null;
    Web.cur = 0;
    Web.rec = true;
    Req.get("/all", function(v) {
      var page = Gui.byid("page");
      Gui.clear(page);
      Web.data = JSON.parse(v);

      var head = Gui.div("head");
      page.appendChild(head);

      var sel = Gui.div("sel");
      head.appendChild(sel);

      for(var i = 0; i < Web.data.length; i++) {
        var div = Gui.div("item", Gui.text(Web.data[i].id));
        sel.appendChild(div);

        var work = Gui.div("work");
        work.id = "work" + i;
        page.appendChild(work);

        switch(Web.data[i].type) {
        case "mach": Mach.init(work, Web.data[i].data).idx = i; break;
        case "player": Player.init(work, Web.data[i].data).idx = i; break;
        }
      }

      var time = Gui.div("time");

      var record = Gui.div("record" + (Web.rec ? " on" : ""), Gui.text("⏺"));
      record.addEventListener("click", function(e) {
        if(Web.rec = !Web.rec) {
          record.classList.add("on");
          Req.post("/time", "start", function(v) { });
        } else {
          record.classList.remove("on");
          Req.post("/time", "stop", function(v) { });
        }
      });
      time.appendChild(record);

      var disp = Gui.div("disp", Gui.text("000:0.0"));
      time.appendChild(disp);

      var refresh = function() {
        Req.get("/time", function(v) {
          v = JSON.parse(v);
          var bar = Math.floor(v.bar).toString();
          var beat = v.beat.toFixed(1);
          Gui.replace(disp, Gui.text("0".repeat(3 - bar.length) + bar + ":" + beat));

          requestAnimationFrame(function() { refresh(); });
        });
      };

      refresh();

      head.appendChild(time);
    });
  };


  /*
   * Key namespace
   */
  window.Key = new Object();

  /**
   * Retrieve the letter associate with a given value.
   *   @val: The key value.
   *   &returns: The letter and accidental.
   */
  window.Key.letter = function(val) {
    switch((val % 12 + 12) % 12) {
    case 0: return "C";
    case 1: return "C♯";
    case 2: return "D";
    case 3: return "E♭";
    case 4: return "E";
    case 5: return "F";
    case 6: return "F♯";
    case 7: return "G";
    case 8: return "A♭";
    case 9: return "A";
    case 10: return "B♭";
    case 11: return "B";
    }
    throw("Invalid value.");
  };

  /**
   * Convert a key value into its string.
   *   @val: The key value.
   *   &returns: The string.
   */
  window.Key.str = function(val) {
    return Key.letter(val) + Math.floor(val / 12).toString();
  };

  /**
   * Parse a string into a key value.
   *   @str: The string.
   *   &returns: The value if successful, null otherwise.
   */
  window.Key.parse = function(str) {
    if(typeof str != "string") { return null; }
    var val;
    switch(str.charAt(0).toUpperCase()) {
    case "C": val = 0; break;
    case "D": val = 2; break;
    case "E": val = 4; break;
    case "F": val = 5; break;
    case "G": val = 7; break;
    case "A": val = 9; break;
    case "B": val = 11; break;
    default: return null;
    }
    str = str.substr(1);
    if(str.charAt(0) == "#") {
      val++;
      str = str.substr(1);
    } else if(str.charAt(0) == "b") {
      val--;
      str = str.substr(1);
    }
    var n = Number.parseInt(str);
    if(str.length != n.toString().length) { return null; }
    return val + 12 * n;
  };


  /*
   * Location namespace
   */
  window.Loc = new Object();

  /**
   * Copy a location.
   *   @loc: The original location.
   *   &returns: The copied location.
   */
  window.Loc.copy = function(loc) {
    return { bar: loc.bar, beat: loc.beat };
  };

  /**
   * Compare two locations for their order.
   *   @left: The left location.
   *   @right: The right location.
   *   &returns: True if properly ordered.
   */
  window.Loc.cmp = function(left, right) {
    if(left.bar < right.bar)
      return true;
    else if(left.bar > right.bar)
      return false;
    else
      return left.beat <= right.beat;
  };

  /**
   * Compare two locations for their order.
   *   @left: The left location.
   *   @right: The right location.
   *   &returns: True if properly ordered.
   */
  window.Loc.order = function(left, right) {
    if(left.bar < right.bar)
      return true;
    else if(left.bar > right.bar)
      return false;
    else
      return left.beat <= right.beat;
  };

  /**
   * Reorder two locations to be in order.
   *   @left: The left location.
   *   @right: The right location.
   */
  window.Loc.reorder = function(left, right) {
    if(Loc.order(left, right)) { return; }

    var tmp;
    tmp = left.bar; left.bar = right.bar; right.bar = tmp;
    tmp = left.beat; left.beat = right.beat; right.beat = tmp;
  };


  /*
   * Note namespace
   */
  window.Note = new Object();

  /**
   * Convert a velocity to an integer value.
   *   @vel: The velocity.
   *   &returns: The integer value.
   */
  window.Note.vel2val = function(vel) {
    var val = Math.round(65535 * vel);
    if(val < 0) { val = 0; } else if(val > 65535) { val = 65535; }
    return val;
  };

  /**
   * Convert a integer value to a velocity.
   *   @val: The integer value.
   *   &returns: The velocity.
   */
  window.Note.val2vel = function(val) {
    var vel = val / 65535;
    if(vel < 0) { vel = 0; } else if(vel > 1) { vel = 1; }
    return vel;
  };


  window.Note.vel2color = function(vel) {
    var color = Math.round(200 * vel + 55);
    if(color < 0) { color = 0; } else if(color > 255) { color = 255; }
    return color;
  };

  window.Note.val2color = function(val) {
    var color = Math.round(200 * val / 65535 + 55);
    if(color < 0) { color = 0; } else if(color > 255) { color = 255; }
    return color;
  };


  /*
   * Machine namespace
   */
  window.Mach = new Object();

  /**
   * Initialize a machine.
   *   @work: The workspace.
   *   @data: The machine data.
   */
  window.Mach.init = function(work, mach) {
    mach.vel = 1;
    work.classList.add("mach");
    var head = Gui.div("head");
    head.appendChild(Gui.div("title", Gui.text("Drum Machine")));
    head.appendChild(Gui.div("element", Gui.Toggle(["Enabled", "Disabled"], true)));
    head.appendChild(Gui.div("element", mach.slider = Gui.Slider({input: 100}, function(slider, v) {
      mach.vel = v;
      var t = Math.floor(255 * (1 - v));
      slider.guiTrack.style.backgroundColor = "rgb(255,"+t+","+t+")";
    })));

    work.appendChild(head);
    for(var n = 0; n < 3; n++) {
      var idx = 0;
      var instr = Gui.div("instr");
      work.appendChild(instr);
      var head = Gui.div("head");
      head.appendChild(Gui.text("Instrument"));
      instr.appendChild(head);
      for(var bar = 0; bar < mach.nbars; bar++) {
        var row = Gui.div("row");
        instr.appendChild(row);
        for(var div = 0; div < mach.ndivs * mach.nbeats; div++) {
          var beat = Math.floor(div / mach.nbeats);
          var cell = Gui.div("cell");
          cell.vel = 0;
          for(var i = 0; i < mach.data.length; i++) {
            var pt = mach.data[i];
            if((n == pt.key) && (idx == pt.idx)) {
              var t = Math.floor(255 * (1 - pt.vel / 65535));
              cell.vel = pt.vel;
              cell.style.backgroundColor = "rgb(255,"+t+","+t+")";
            }
          }
          cell.addEventListener("mouseup", Mach.click(mach, n, bar * (mach.ndivs * mach.nbeats) + div, cell));
          cell.appendChild(Gui.div("fill"));
          if((div % mach.ndivs) == 0) {
            cell.classList.add("beat");
          }
          row.appendChild(cell);
          idx++;
        }
      }
    }
    return mach;
  };

  /**
   * Machine click handler.
   *   @intr: The instrument.
   *   @bar: The bar.
   *   @div: The division.
   *   @cell: The cell.
   */
  window.Mach.click = function(mach, key, idx, cell) {
    return function(e) {
      if(e.which == 1) {
        var vel = Math.floor(mach.vel * 65535);
        if(cell.vel == vel) { vel = 0; }
        cell.vel = vel;
        var t = Math.floor(255 * (1 - vel / 65535));
        cell.style.backgroundColor = "rgb(255,"+t+","+t+")";
        Req.get("/" + mach.idx + "/mach/set/" + idx + "/" + key + "/" + vel, function(v) {
        });
      } else if(e.which == 2) {
        if(cell.vel > 0) {
          mach.slider.guiUpdate(cell.vel / 65535);
        }
      } else if(e.which == 3) {
        if(cell.vel > 0) {
          cell.vel = 0;
          cell.style.backgroundColor = "rgb(255,255,255)";
          Req.get("/" + mach.idx + "/mach/set/" + idx + "/" + key + "/" + 0, function(v) {
          });
        }
      }
      e.preventDefault();
    };
  };


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
    player.sel = -1;
    player.x = 0;
    player.conf.rows = player.conf.keys;
    player.vel = 32768;

    player.layout = {
      scroll: 20,
      label: 40,
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

      if(player.sel >= 0) {
        var dat = player.data[player.sel];
        dat.vel = player.vel;
        Req.get("/" + player.idx + "/player/set/" + dat.key + "/" + dat.begin.bar + ":" + dat.begin.beat + "/" + dat.end.bar + ":" + dat.end.beat + "/" + dat.vel, function() {
      });
      }

      if(player.init) { Player.draw(player); }
    }));
    head.appendChild(Gui.Button("Keys", {}, function(e) {
      Player.keys(player);
    }));
    work.appendChild(head);

    player.roll = Gui.tag("canvas", "roll");
    player.roll.addEventListener("mousedown", Player.mouse(player));
    player.roll.addEventListener("contextmenu", Player.context(player));
    player.roll.addEventListener("wheel", Player.wheel(player));
    player.roll.style.height = (layout.height() + layout.scroll + layout.head + 4) + 4 + "px";
    player.roll.style.border = "2px solid black";
    work.appendChild(player.roll);

    Player.draw(player);
    window.addEventListener("resize", function() { Player.draw(player); });

    console.log(Key.parse("C4"));
    player.init = true;
    return player;
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
        var box = Gui.div("box");
        box.xIdx = idx++;
        if(player.conf.rows.indexOf(box.xIdx) >= 0) {
          box.classList.add("sel");
          rows.push(box.xIdx);
        }
        row.appendChild(box);
      }

      grid.appendChild(row);
    }

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
      player.conf.rows = rows.sort();
      player.layout.resize();
      popup.guiDismiss();
      Player.draw(player);
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
   * Handle a mouse down on the player.
   *   @player: The player.
   *   &returns: The handler function.
   */
  window.Player.mouse = function(player) {
    return function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var x = e.offsetX, y = e.offsetY;
      var layout = player.layout;
      var box = Pack.canvas(player.roll);
      Pack.vert(box, layout.scroll + 2, true);
      Pack.vert(box, layout.head + 2);
      Pack.horiz(box, layout.label + 2);

      if(box.inside(x, y)) {
        x -= box.x;
        y -= box.y;

        var idx = Player.getidx(player, x, y);
        if(idx >= 0) {
          if(idx != player.sel) {
            player.sel = idx;
            player.slider.guiUpdate(player.data[idx].vel / 65535);
          } else {
            player.sel = -1;
          }
          Player.draw(player);
          return;
        }

        var xy = Player.getxy(player, x, y, true);
        if(xy == null) { return; }

        player.sel = -1;
        player.active = {
          row: player.conf.rows[xy.row],
          begin: { bar: xy.bar, beat: xy.beat },
          end: { bar: xy.bar, beat: xy.beat }
        };

        Player.draw(player);

        Gui.dragNow(function(e, type) {
          switch(type) {
          case "move":
            var xy = Player.getxy(player, e.offsetX - box.x, e.offsetY - box.y, true);
            if(xy == null) { return; }
            player.active.end = { bar: xy.bar, beat: xy.beat };
            Player.draw(player);
            break;

          case "down":
            break;

          case "up":
            var active = player.active;
            var begin = Loc.copy(active.begin), end = Loc.copy(active.end);
            Loc.reorder(begin, end);
            end.beat += 1 / player.conf.ndivs;

            var dat = { key: active.row, begin: begin, end: end, vel: player.vel };
            player.data.push(dat);
            Player.draw(player);

            Req.get("/" + player.idx + "/player/set/" + dat.key + "/" + dat.begin.bar + ":" + dat.begin.beat + "/" + dat.end.bar + ":" + dat.end.beat + "/" + dat.vel, function() {
            });
            break;

          case "done":
            player.active = null;
            Player.draw(player);
            break;
          }
        });
      }
      
    };
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
      Req.get("/" + player.idx + "/player/set/" + dat.key + "/" + dat.begin.bar + ":" + dat.begin.beat + "/" + dat.end.bar + ":" + dat.end.beat + "/" + 0, function() {
      });

      player.sel = -1;
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

    var ctx = player.roll.getContext("2d", {alpha: false});
    var conf = player.conf;
    var layout = player.layout;
    ctx.fillStyle = "#fff";
    ctx.rect(0, 0, player.roll.width, player.roll.height);
    ctx.fill();

    var box = Pack.canvas(player.roll);
    var scroll = Pack.vert(box, layout.scroll, true);
    Draw.fill(Pack.vert(box, 2, true), ctx, "#000");
    var line = Pack.horiz(Pack.horiz(box.copy(), layout.label + 2), 2, true);
    var head = Pack.vert(box, layout.head);
    Draw.fill(Pack.vert(box, 2), ctx, "#000");
    var label = Pack.horiz(box, layout.label);
    Pack.horiz(box, 2);

    // background colors
    var cur = box.copy();
    for(var i = 0; i < conf.rows.length; i++) {
      var row = Pack.vert(cur, layout.cell.height);
      Pack.vert(cur, 1);

      if((i % 2) == 0) { Draw.fill(row, ctx, "#eee"); }
    }

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
      var begin = Loc.copy(player.active.begin), end = Loc.copy(player.active.end);
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
      ctx.fillStyle = Player.color(dat.vel, player.sel == i);
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
      var key = Key.str(player.conf.rows[i]);
      var pack = Pack.vert(label, layout.cell.height);
      Draw.fill(pack, ctx, (key.length > 2) ? "#000" : "#fff");
      Draw.text(pack, ctx, key, {x: 2, y: 1, font: "16px sans-serif", align: "right", color: (key.length > 2) ? "#fff" : "#000"});
      Draw.fill(Pack.vert(label, 1), ctx, "#000");
    }
  };

  /*
   * initialize on load
   */
  window.addEventListener("load", function() {
    Web.init();
  });
})();
