(function() {
  "use strict";

  /*
   * Machine namespace
   */
  window.Mach = new Object();

  /**
   * Create the machine element.
   *   @mach: The machine.
   *   @idx: The request index.
   *   &returns: The DOM element.
   */
  window.Mach.elem = function(mach, idx) {
    mach.idx = idx;
    mach.sel = 0;
    mach.undo = new Array();
    mach.redo = new Array();
    mach.vel = 1;
    mach.pads = new Array();

    var elem = Gui.div("mach");

    elem.appendChild(Mach.elemHead(mach));

    for(var i = 0; i < mach.inst.length; i++)
      elem.appendChild(Mach.elemInst(mach, i));

    return elem;
  };
  /**
   * Create the header element.
   *   @mach: The machine.
   *   &returns: The header element.
   */
  window.Mach.elemHead = function(mach) {
    var head = Gui.div("head");

    head.appendChild(Gui.div("title", Gui.text("Machine")));

    head.appendChild(Gui.Toggle(["Enabled", "Disabled"], {def:true}, function() {
    }));

    head.appendChild(Gui.Slider({cls: "vel"}, function(slider, pos) {
      mach.vel = Math.round(65534 * pos + 1);
      slider.guiTrack.style.backgroundColor = Mach.getColor(mach.vel);
    }));

    head.appendChild(Key.select({}, function() {
    }));

    var presets = Array.range(1, 10).map(function(v) { return "preset " + v });
    head.appendChild(Gui.select(presets, "presets", function(e) {
    }));

    return head;
  };
  /**
   * Create an instance element.
   *   @mach: The machine.
   *   @idx: The instance index.
   */
  window.Mach.elemInst = function(mach, idx) {
    var elem = Gui.div("inst"), inst = mach.inst[idx];

    var action = Gui.div("action");
    action.appendChild(Gui.Button("⚙", {}, function(e) {
      document.body.appendChild(Mach.conf(mach, idx, function() {
        var old = elem;
        old.parentNode.replaceChild(elem = Mach.elemInst(mach, idx), old);
      }));
    }));
    action.appendChild(Gui.Toggle(["On", "Off"], {def:inst.on}, function(v) {
      Mach.toggle(mach, idx, v);
    }));

    var label = Gui.div("label");
    label.appendChild(Gui.div("name", Gui.text(inst.id)));
    label.appendChild(action);
    elem.appendChild(label);

    switch(inst.type) {
    case "drum": label.appendChild(Gui.div("dev", Gui.text("Drum " + inst.dev + ":" + inst.key))); break;
    case "note": label.appendChild(Gui.div("dev", Gui.text("Note " + inst.dev))); break;
    case "ctrl": label.appendChild(Gui.div("dev", Gui.text("Ctrl " + inst.dev))); break;
    }

    var pads = new Array();
    mach.pads.push(pads);

    var grid = Gui.div("grid");
    for(var i = 0; i < mach.conf.nbars; i++) {
      var bar = Gui.div("bar");
      for(var j = 0; j < mach.conf.nbeats; j++) {
        var beat = Gui.div("beat");
        for(var k = 0; k < mach.conf.ndivs; k++) {
          var off = Mach.getOff(mach, i, j, k);
          var pad = Mach.elemPad(mach, idx, off);
          pads.push(pad);
          beat.appendChild(pad);
        }
        bar.appendChild(beat);
      }
      grid.appendChild(bar);
    }
    elem.appendChild(grid);

    return elem;
  };
  /**
   * Create a pad element.
   *   @mach: The machine.
   *   @idx: The instance index.
   *   @off: The offset.
   *   &returns: The pad element.
   */
  window.Mach.elemPad = function(mach, idx, off) {
    var pad = Gui.div("pad gui-noselect gui-pointer");
    var find = Mach.getEvent(mach, mach.sel, idx, off);
    if(find) { pad.style.backgroundColor = Mach.getColor(find.vel, find.key); }

    pad.addEventListener("click", function(e) {
      Mach.set(mach, mach.sel, idx, off, 0, mach.vel);
    });

    return pad;
  };

  /**
   * Update the machine.
   *   @mach: The machine.
   *   @data: The update data.
   */
  window.Mach.update = function(mach, data) {
  };


  /**
   * Set an event.
   *   @mach: The machine.
   *   @sel: The selected preset.
   *   @idx: The instance index.
   *   @off: The offset.
   *   @vel: The velocity.
   */
  window.Mach.set = function(mach, sel, idx, off, key, vel) {
    var add = { sel:sel, idx:idx, off:off, key:key, vel:vel };
    var rem = Mach.getEvent(mach, sel, idx, off);

    if(rem == null) {
      mach.events[sel][off].push(add);
      rem = { sel:sel, idx:idx, off:off, key:0, vel:0 };
    } else if((rem.vel == vel) && (rem.key == key)) {
      mach.events[sel][off].remove(rem);
      add = { sel:sel, idx:idx, off:off, key:0, vel:0 };
    } else {
      mach.events[sel][off].remove(rem);
      mach.events[sel][off].push(add);
    }

    Web.put(mach.idx, { type: "edit", data: add });

    mach.undo.push({ type: "edit", data: rem });
    mach.redo = new Array();

    if(sel == mach.sel) { mach.pads[idx][off].style.backgroundColor = Mach.getColor(add.vel); }
  };

  /**
   * Toggle an instance.
   *   @mach: The machine.
   *   @idx: The index.
   *   @on: The one flag.
   */
  window.Mach.toggle = function(mach, idx, on) {
    var rem = { idx: idx, conf: mach.inst[idx].copy() };
    var add = { idx: idx, conf: mach.inst[idx] };

    mach.inst[idx].on = on;

    Web.put(mach.idx, { type: "conf", data: add });

    mach.undo.push({ type: "conf", data: rem });
    mach.redo = new Array();
  };

  /**
   * Toggle an instance.
   *   @mach: The machine.
   *   @idx: The index.
   *   @type: The type.
   *   @rel: The release flag.
   *   @dev: The device number.
   *   @key: The key number.
   */
  window.Mach.reconf = function(mach, idx, type, rel, dev, key) {
    var rem = { idx: idx, conf: mach.inst[idx].copy() };
    var add = { idx: idx, conf: mach.inst[idx] };

    mach.inst[idx].type = type;
    mach.inst[idx].rel = rel;
    mach.inst[idx].dev = dev;
    mach.inst[idx].key = key;

    Web.put(mach.idx, { type: "conf", data: add });

    mach.undo.push({ type: "conf", data: rem });
    mach.redo = new Array();
  };


  /**
   * Create the configuration element for an instance.
   *   @mach: The machine.
   *   @idx: The instance index.
   */
  window.Mach.conf = function(mach, idx, func) {
    var popup, id, dev, key;
    var elem = Gui.div("mach-conf");
    var inst = mach.inst[idx];

    var err = Gui.div("line error");
    elem.appendChild(err);

    var drum, note, ctrl, line = Gui.div("line");
    line.appendChild(drum = Gui.Button("Drum", {cls:(inst.type == "drum") ? "gui-sel" : ""}, function(e) {
      drum.classList.add("gui-sel");
      note.classList.remove("gui-sel");
      ctrl.classList.remove("gui-sel");
      key.disabled = false;
    }));
    line.appendChild(note = Gui.Button("Note", {cls:(inst.type == "note") ? "gui-sel" : ""}, function(e) {
      drum.classList.remove("gui-sel");
      note.classList.add("gui-sel");
      ctrl.classList.remove("gui-sel");
      key.disabled = true;
    }));
    line.appendChild(ctrl = Gui.Button("Control", {cls:(inst.type == "ctrl") ? "gui-sel" : ""}, function(e) {
      drum.classList.remove("gui-sel");
      note.classList.remove("gui-sel");
      ctrl.classList.add("gui-sel");
      key.disabled = true;
    }));
    elem.appendChild(line);

    var line = Gui.div("line");
    line.appendChild(Gui.div("label", Gui.text("Name")));
    line.appendChild(id = Gui.Entry({def: inst.id}, function() {
    }));
    elem.appendChild(line);

    var line = Gui.div("line");
    line.appendChild(Gui.div("label", Gui.text("Device")));
    line.appendChild(dev = Gui.Entry({def: inst.dev}, function() {
    }));
    elem.appendChild(line);

    var line = Gui.div("line");
    line.appendChild(Gui.div("label", Gui.text("Key")));
    line.appendChild(key = Gui.Entry({def: inst.key, enable: inst.type == "drum"}, function() {
    }));
    elem.appendChild(line);
    
    var line = Gui.div(["line","actions"]);
    line.appendChild(Gui.Button("Accept", {}, function() {
      var conf = new Object();
      conf.dev = Parse.getUInt16(dev.value);
      conf.key = Parse.getUInt16(key.value);

      if(isNaN(conf.dev) || isNaN(conf.key)) {
        Gui.replace(err, Gui.text("Error"));
      } else {
        var type;
        if(drum.classList.contains("gui-sel")) {
          type = "drum";
        } else if(note.classList.contains("gui-sel")) {
          type = "note";
        } else if(drum.classList.contains("gui-sel")) {
          type = "ctrl";
        } else {
          throw "Invalid selection.";
        }

        Mach.reconf(mach, idx, type, false, conf.dev, conf.key);
        popup.guiDismiss();
        func();
      }
    }));
    line.appendChild(Gui.Button("Cancel", {}, function() {
      popup.guiDismiss();
    }));
    elem.appendChild(line);

    return popup = Gui.Popup(elem, function(e) { });
  };


  /**
   * Compute the offset of a given pad.
   *   @mach: The machine.
   *   @bar: The bar.
   *   @beat: The beat.
   *   @div: The division.
   *   &returns: The offset.
   */
  window.Mach.getOff = function(mach, bar, beat, div) {
    return div + mach.conf.ndivs * (beat + mach.conf.nbeats * bar);
  };

  /**
   * Get the event at a given position.
   *   @mach: The machine.
   *   @sel: The selected preset.
   *   @idx: The instance index.
   *   @off: The offset.
   *   &returns: The event or null.
   */
  window.Mach.getEvent = function(mach, sel, idx, off) {
    var arr = mach.events[sel][off];
    var n = arr.map(function(v){ return v.idx; }).indexOf(idx);
    return (n >= 0) ? arr[n] : null;
  };

  /**
   * Get the event velocity at a given position.
   *   @mach: The machine.
   *   @sel: The selected preset.
   *   @idx: The instance index.
   *   @off: The offset.
   *   &returns: The velocity.
   */
  window.Mach.getVel = function(mach, sel, idx, off) {
    var ret = Mach.getEvent(mach, sel, idx, off);
    return ret ? ret.vel : 0;
  };

  /**
   * Retrieve the color string for a velocity and key.
   *   @vel: The velocity.
   *   &returns: The CSS color string.
   */
  window.Mach.getColor = function(vel) {
    if(vel == 0) {
      return "#fff";
    } else {
      var c = 192 - Math.round(192 * vel / 65535);
      return "rgb(255,"+c+","+c+")";
    }
  };
})();
