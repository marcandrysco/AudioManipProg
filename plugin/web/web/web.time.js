(function() {
  "use strict";

  /*
   * Time namespace
   */
  window.Time = new Object();

  /**
   * Initialize the time information.
   *   @run: Currently running flag.
   *   @loc: The current location.
   *   @save: The save locations.
   *   @dialog: Popup dialog UI elements.
   */
  window.Time.init = function() {
    Time.run = false;
    Time.loc = Loc.create(0, 0.0);
    Time.save = new Object();
    Time.dialog = null;
    for(var i = 1; i <= 9; i++) {
      Time.save[i] = Loc.create(0, 0.0);
    }
  };

  /**
   * Create the time UI element.
   *   @data: The initial data.
   *   &returns: The DOM element.
   */
  window.Time.elem = function(data) {
    Time.run = data.run;

    var time = Gui.div("time");
    var record = Gui.div("record" + (Time.run ? " on" : ""), Gui.text("⏺"));
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

    Time.display = Gui.div("display", Gui.text("000:0.0"));
    Time.display.addEventListener("click", function(e) { document.body.appendChild(Time.popup()); });
    time.appendChild(Time.display);

    document.body.appendChild(Time.popup());

    return time;
  };

  /**
   * Update the time elements.
   *   @data: The time data.
   */
  window.Time.update = function(data) {
    Time.run = data.run;
    Time.loc = Loc.create(data.loc.bar, data.loc.beat);

    var bar = Math.floor(data.loc.bar).toString();
    var beat = data.loc.beat.toFixed(1);
    var time = "0".repeat(3 - bar.length) + bar + ":" + beat;
    Gui.replace(Time.display, Gui.text(time));

    if(Time.dialog) {
      Time.dialog.display.value = time;
      Time.dialog.start.classList[!Time.run ? "remove" : "add"]("hidden");
      Time.dialog.stop.classList[!Time.run ? "add" : "remove"]("hidden");
      Time.dialog.display.disabled = Time.run;
    }
  };

  /**
   * Create a string time using a location.
   *   @loc: The location.
   *   &returns: The string.
   */
  window.Time.string = function(loc) {
    var bar = Math.floor(loc.bar).toString();
    var beat = loc.beat.toFixed(1);
    return "0".repeat(3 - bar.length) + bar + ":" + beat;
  };

  /**
   * Create the time popup.
   *   &returns: The popup.
   */
  window.Time.popup = function() {
    Time.dialog = new Object();

    var el = Gui.div("time-popup");

    el.appendChild(Time.popupAction());
    el.appendChild(Time.popupCur());

    var seek = Gui.div("seek");
    seek.appendChild(Gui.button("⏮", "begin gui-button", function(e) {
    }));
    seek.appendChild(Gui.button("⏪", "back gui-button", function(e) {
    }));
    seek.appendChild(Gui.button("⏩", "forward gui-button", function(e) {
    }));
    seek.appendChild(Gui.button("⏭", "end gui-button", function(e) {
    }));
    el.appendChild(seek);

    var saved = Gui.div("saved");

    for(var i = 1; i < 9; i++) {
      var line = Gui.div("line");
      line.appendChild(Gui.span("num", Gui.text(i)));

      var input = Gui.tag("input", "display");
      input.type = "text";
      input.value = Time.string(Time.save[i]);
      line.appendChild(input);

      line.appendChild(Gui.button("✓", "set gui-button", function(e) {
      }));
      line.appendChild(Gui.button("✗", "reset gui-button", function(e) {
      }));

      saved.appendChild(line);
    }

    el.appendChild(saved);

    var popup = Gui.Popup(el, function(e) { Time.dialog = null; });

    return popup;
  };
  /**
   * Create the action area of the popup dialog.
   *   &returns: The action element.
   */
  window.Time.popupAction = function() {
    var action = Gui.div("action");

    Time.dialog.start = Gui.button("Start", "start gui-button gui-green", function(e) {
      Web.put(0, { type: "start" });
    });

    Time.dialog.stop = Gui.button("Stop", "stop gui-button gui-red", function(e) {
      Web.put(0, { type: "stop" });
    });

    Time.dialog[Time.run ? "start" : "stop"].classList.add("hidden");

    action.appendChild(Time.dialog.start);
    action.appendChild(Time.dialog.stop);

    return action;
  };
  /**
   * Create the current time display.
   *   &returns: The current time element.
   */
  window.Time.popupCur = function() {
    var cur = Gui.div("cur");

    var display = Gui.tag("input", "display");
    display.disabled = Time.run;
    display.type = "text";
    display.value = Time.string(Time.loc);
    display.addEventListener("input", function() {
    });
    cur.appendChild(display);

    var button = Gui.button("✓", "set gui-button gui-green", function(e) {
    });
    button.disabled = true;
    cur.appendChild(button);

    var button = Gui.button("✗", "reset gui-button gui-red", function(e) {
    });
    button.disabled = true;
    cur.appendChild(button);

    Time.dialog.display = display;

    return cur;
  };
})();
