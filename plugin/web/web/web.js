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
    Req.get("/all", function(v) {
      var page = Gui.byid("page");
      Gui.clear(page);
      Web.data = JSON.parse(v);

      var sel = Gui.div("sel");
      page.appendChild(sel);

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

      //Gui.byid("page").innerHTML = v;
    });
  };


  /*
   * Key namespace
   */
  window.Key = new Object();

  window.Key.letter = function(val) {
    switch((val % 12 + 12) % 12) {
    case 0: return "B";
    case 1: return "C";
    case 2: return "C♯";
    case 3: return "D";
    case 4: return "E♭";
    case 5: return "E";
    case 6: return "F";
    case 7: return "F♯";
    case 8: return "G";
    case 9: return "A♭";
    case 10: return "A";
    case 11: return "B♭";
    }
    throw("Invalid value.");
  };

  window.Key.str = function(val) {
    return Key.letter(val) + Math.floor(val / 12).toString();
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
    player.conf.rows = [32,33,34,35,36,37,38,39,40,41,42,43,44];
    player.scroll = {
      box: function() {
        return layout.height(false) + 3.5
      }
    };
    player.layout = {
      head: { h: 18, ln: 2, off: 20 },
      cell: { w: 16, h: 16 },
      width: { disp: 0, total: 0, max: 0 },
      scroll: 20, label: 40,
      div: function(ln) {
        return player.layout.cell.w + (ln ? 1 : 0);
      },
      beat: function(ln) {
        return (player.layout.cell.w + 1) * player.conf.ndivs - 1 + (ln ? 1 : 0);
      },
      bar: function(ln) {
        return (player.layout.cell.w + 1) * player.conf.nbeats * player.conf.ndivs - 1 + (ln ? 2 : 0);
      },
      height: function(scroll) {
        return player.layout.head.off + 17 * player.conf.rows.length - 1 + (scroll ? (player.layout.scroll + 2) : 0);
      },
      refresh: function() {
        var width = player.layout.width;
        width.disp = player.roll.offsetWidth;
        width.total = player.layout.bar(true) * player.conf.nbars + 2;
        width.max = width.total - width.disp;
      }
    };

    player.roll = Gui.tag("canvas", "roll");
    player.roll.addEventListener("mousedown", Player.mouse(player));
    player.roll.addEventListener("contextmenu", Player.context(player));
    player.roll.addEventListener("wheel", Player.wheel(player));
    player.roll.style.height = player.layout.height(true) + 4 + "px";
    player.roll.style.border = "2px solid black";
    work.appendChild(player.roll);

    Player.draw(player);
    window.addEventListener("resize", function() { Player.draw(player); });

    return player;
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
   * Handle a mouse down on the player.
   *   @player: The player.
   *   &returns: The handler function.
   */
  window.Player.mouse = function(player) {
    return function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var box = Pack.canvas(player.roll);
      var scroll = Pack.vert(box, player.layout.scroll, true);
      Pack.vert(box, 2);

      if(scroll.inside(e.offsetX, e.offsetY)) {
        scroll = Pack.pad(scroll, 1, 0);
        var x = e.offsetX - scroll.x;
        var y = e.offsetY - scroll.y;

        var width = Math.floor(scroll.width * player.layout.width.disp / player.layout.width.total);

        x = (x - width / 2);
        x = x / (scroll.width - width);

        player.x = Math.floor(x * (player.layout.width.total - player.layout.width.disp));
        Player.draw(player);
      } else {
        var coord = Player.coord(player, e.offsetX, e.offsetY);
        if((coord.row < 0) || (coord.row >= player.conf.rows.length)) { return; }

        var sel = Player.get(player, coord.row, coord.idx);
        if(sel >= 0) {
          player.sel = (player.sel == sel) ? -1 : sel;
          Player.draw(player);
          return;
        }

        player.sel = -1;
        player.active = { row: coord.row, begin: coord.idx, end: coord.idx };

        var move = function(e) {
          player.active.end = Player.coord(player, e.offsetX, e.offsetY).idx;
          Player.draw(player);
        };
        var out = function(e) {
          if(e.relatedTarget == window.body) { cleanup(e); }
        };
        var up = function(e) {
          var dat = new Object();
          dat.row = player.active.row;
          dat.begin = Math.min(player.active.begin, player.active.end);
          dat.end = Math.max(player.active.begin, player.active.end);
          player.data.push(dat);

          cleanup();
        };
        var cleanup = function() {
          window.removeEventListener("mousemove", move);
          window.removeEventListener("mouseup", up);
          window.removeEventListener("mouseout", out);
          player.active = null;
          Player.draw(player);
        };
        window.addEventListener("mousemove", move);
        window.addEventListener("mouseout", out);
        window.addEventListener("mouseup", up);
        move(e);
      }
    };
  };

  window.Player.context = function(player) {
    return function(e) {
      var coord = Player.coord(player, e.offsetX, e.offsetY);
      if(coord.row < 0) { return; }

      e.preventDefault();

      var sel = Player.get(player, coord.row, coord.idx);
      if(sel < 0) { return; }

      var ret = player.data.splice(sel, 1);
      player.sel = -1;
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
      Player.draw(player);
    };
  };

  /**
   * Draw the player to the canvas.
   *   @player: The player.
   */
  window.Player.draw = function(player) {
    var hline = function(y, full) {
      ctx.moveTo(full ? 0 : layout.label, y + ctx.lineWidth / 2);
      ctx.lineTo(w, y + ctx.lineWidth / 2);
    };
    var vline = function(x, x0) {
      ctx.moveTo(x + ctx.lineWidth / 2 + layout.label, x0);
      ctx.lineTo(x + ctx.lineWidth / 2 + layout.label, player.layout.height(false));
    };
    var block = function(h, l, r) {
      var t = layout.head.off + h * (hrow + 1);
      ctx.rect(left(l), t, right(r) - left(l), hrow);
      ctx.fill();
      ctx.beginPath();
      ctx.moveTo(left(l) - 0.5, t);
      ctx.lineTo(left(l) - 0.5, t + hrow);
      ctx.moveTo(right(r) + 0.5, t);
      ctx.lineTo(right(r) + 0.5, t + hrow);
      ctx.stroke();
    };

    var layout = player.layout;
    var conf = player.conf;

    // validation
    if(player.x >= player.layout.width.max) { player.x = player.layout.width.max; }
    if(player.x < 0) { player.x = 0 }

    var w = player.roll.width = player.roll.clientWidth;
    var h = player.roll.height = player.roll.clientHeight;
    player.layout.refresh();

    var ctx = player.roll.getContext("2d");

    var box = Pack.canvas(player.roll);
    var scroll = Pack.vert(box, layout.scroll, true);
    Pack.vert(box, 2, true);
    var head = Pack.vert(box, 18);
    Pack.vert(box, 2);
    var label = Pack.horiz(box, layout.label - 2);
    Pack.horiz(box, 2);

    ctx.clearRect(0, 0, player.roll.width, player.roll.height);

    var htop = 18;
    var hrow = 16;

    var wdiv = 16;
    var wbeat = wdiv * conf.ndivs + (conf.ndivs - 1);
    var wbar = wbeat * conf.nbeats + (conf.nbeats - 1);
    var nbar = wbar + 2;

    ctx.fillStyle = "#eee";
    for(var i = 0; i < player.conf.rows.length; i += 2) {
      ctx.rect(0, layout.head.off + i * (hrow + 1), w, hrow);
      ctx.fill();
    }

    ctx.fillStyle = "#000";
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#bbb";
    ctx.beginPath();
    var x = -player.x;
    while(x < w) {
      x += wdiv;
      for(var i = 1; i < conf.ndivs * conf.nbeats; i++) {
        if(x >= 0) { vline(x, layout.head.off); }
        x += wdiv + 1;
      }
      x += 2;
    }
    ctx.stroke();

    ctx.font = "12px sans-serif";
    ctx.strokeStyle = "#000";
    ctx.beginPath();
    var x = -player.x;
    while(x < w) {
      x += wbeat;
      for(var i = 1; i < conf.nbeats; i++) {
        if(x >= 0) { vline(x, 0); }
        ctx.fillText(i+1, x + 4 + layout.label, 15);
        x += wbeat + 1;
      }
      x += 2;
    }
    ctx.stroke();

    ctx.font = "16px sans-serif";
    ctx.lineWidth = 2;
    ctx.beginPath();
    var x = -player.x - 2;
    var i = 1;
    while(x < w) {
      if(x >= 0) { vline(x, 0); }
      ctx.fillText(i++, x + 4 + layout.label, 15);
      x += wbar + 2;
    }
    ctx.stroke();

    ctx.lineWidth = 2;
    ctx.strokeStyle = "#000";
    ctx.beginPath();
    hline(htop, true);
    ctx.stroke();

    ctx.lineWidth = 1;
    ctx.beginPath();
    for(var i = 0; i < player.conf.rows.length; i++) {
      hline(layout.head.off + i * (hrow + 1) - 1, true);
    }
    ctx.stroke();

    var left = function(idx) {
      var bar = Math.floor(idx / (conf.ndivs * conf.nbeats));
      var div = idx % (conf.ndivs * conf.nbeats);
      return (wbar + 2) * bar + (wdiv + 1) * div - player.x + player.layout.label;
    };
    var right = function(idx) {
      return left(idx) + wdiv;
    };

    ctx.strokeStyle = "#000";
    ctx.lineWidth = 1;
    
    for(var i = 0; i < player.data.length; i++) {
      if(i == player.sel) {
        ctx.fillStyle = "#f80";
      } else {
        ctx.fillStyle = "#f00";
      }
      var dat = player.data[i];
      block(dat.row, dat.begin, dat.end);
    }

    if(player.active != null) {
      var l = Math.min(player.active.begin, player.active.end);
      var r = Math.max(player.active.begin, player.active.end);
      ctx.fillStyle = "#f80";
      block(player.active.row, l, r);
    }

    ctx.lineWidth = 2;
    ctx.beginPath();
    hline(layout.height(false), true);
    ctx.stroke();

    var l = Math.floor((w-3) * Math.max(0, player.x / layout.width.total));
    var r = Math.floor((w-3) * Math.min(1, (player.x + layout.width.disp) / layout.width.total));
    ctx.fillStyle = "#eee";
    ctx.strokeStyle = "#bbb";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.rect(l + 1.5, layout.height(false) + 3.5, r - l, layout.scroll - 3);
    ctx.fill();
    ctx.stroke();

    ctx.font = "14px sans-serif";
    ctx.fillStyle = "#000";
    ctx.strokeStyle = "#000";
    ctx.beginPath();
    for(var i = 0; i < player.conf.rows.length; i++) {
      var row = player.conf.rows[i];
      var cur = Pack.vert(label, layout.cell.h);
      Pack.vert(label, 1);
      Pack.text(Pack.pad(cur, 2), ctx, Key.str(row), "right");
    }
    ctx.fill();

    //ctx.clearRect(0, 0, layout.label, layout.height.head);
    //ctx.clearRect(0, layout.height.head, layout.label, layout.height(false));
    ctx.clearRect(0, 0, layout.label, layout.head.h);
    //ctx.clearRect(0, layout.head.off, layout.label, layout.height(false) - layout.head.off);
    ctx.strokeStyle = "#000";
    ctx.lineWidth = 2;
    ctx.beginPath();
    vline(-2, 0);
    ctx.stroke();

    //ctx.fillStyle = "rgba(0,0,255,0.5)";
    //ctx.beginPath();
    //Pack.draw(label, ctx);
    //ctx.fill();
  };

  /*
   * initialize on load
   */
  window.addEventListener("load", function() {
    Web.init();
  });
})();
