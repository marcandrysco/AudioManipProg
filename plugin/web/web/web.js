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
    Req.get("/init", function(v) {
      var page = Gui.byid("page");
      Gui.clear(page);
      Web.data = JSON.parse(v);

      var head = Gui.div("head");
      page.appendChild(head);

      var sel = Gui.div("sel");
      head.appendChild(sel);

      for(var i = 1; i < Web.data.length; i++) {
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

      head.appendChild(Time.elem(Web.data[0].data));

      Web.refresh();
    });
  };

  window.Gui.wait = function(delay, func) {
    setTimeout(func, delay);
  };

  /**
   * Refresh the data using an http request.
   */
  window.Web.refresh = function() {
    Req.get("/get", function(resp) {
      var json = JSON.parse(resp);

      Time.update(json.time);
      //var bar = Math.floor(json.time.bar).toString();
      //var beat = json.time.beat.toFixed(1);
      //var time = { b
      //Gui.replace(disp, Gui.text("0".repeat(3 - bar.length) + bar + ":" + beat));
     
      Gui.wait(500, function() {
        requestAnimationFrame(function() { Web.refresh(); });
      });
    });
  };

  /**
   * Send a request to put.
   *   @idx: The target index.
   *   @data: The data.
   */
  window.Web.put = function(idx, data) {
    var json = JSON.stringify({ id: 1, idx: idx, data: data });

    var suc = function(v) {
      console.log("SUC");
    };

    var fail = function(v) {
      console.log("FAIL");
    };

    Req.post("/put", json, suc, fail);
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
   * initialize on load
   */
  window.addEventListener("load", function() {
    Web.init();
  });
})();
