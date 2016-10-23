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
    Time.run = true;
    Time.loc = Loc.make(0, 0.0);
    Time.save = new Object();
    Time.dialog = null;
    Time.record = null;
    Time.nbeats = 4.0;
    for(var i = 1; i <= 9; i++) {
      Time.save[i] = Loc.make(0, 0.0);
    }
  };

  /**
   * Create the time UI element.
   *   @data: The initial data.
   *   &returns: The DOM element.
   */
  window.Time.elem = function(data) {
    var time = Gui.div("time");
    Time.record = Gui.div("record", Gui.text("⏺"));
    Time.record.addEventListener("click", function(e) {
      Web.put(0, { type: Time.run ? "stop" : "start" });
    });
    time.appendChild(Time.record);

    Time.display = Gui.div("display", Gui.text("000:0.0"));
    Time.display.addEventListener("click", function(e) { document.body.appendChild(Time.popup()); });
    time.appendChild(Time.display);

    //document.body.appendChild(Time.popup());

    return time;
  };

  /**
   * Update the time elements. The run flag is updated last to ensure the last
   * "tick" from the server updates the display.
   *   @data: The time data.
   */
  window.Time.update = function(data) {
    Time.loc = Loc.make(data.loc.bar, data.loc.beat);

    var bar = Math.floor(data.loc.bar).toString();
    var beat = data.loc.beat.toFixed(1);
    var time = "0".repeat(3 - bar.length) + bar + ":" + beat;

    Gui.replace(Time.display, Gui.text(time));

    if(Time.dialog) {
      if(Time.run) { Time.dialog.display.value = time; }
      Time.dialog.start.classList[!data.run ? "remove" : "add"]("hidden");
      Time.dialog.stop.classList[!data.run ? "add" : "remove"]("hidden");
      Time.dialog.display.disabled = data.run;
    }

    if(Time.record) { Time.record.classList[data.run ? "add" : "remove"]("on"); }

    Time.run = data.run;
  };


  /**
   * Handle a keypress for the time.
   *   @e: The key press event.
   *   &returns: True if handled.
   */
  window.Time.keypress = function(e) {
    switch(e.key) {
    case "S":
      if(Status.mode != Mode.view) { break; }

      Web.put(0, { type: Time.run ? "stop" : "start" });
      return true;

    case "G":
      if(Time.run || (Status.mode != Mode.num)) { break; }

      Web.put(0, { type: "seek", bar: Status.num  });
      Status.update(Mode.view, "Seeked to " + Status.num);
      return true;
    }

    return false;
  };


  /**
   * Create the time popup.
   *   &returns: The popup.
   */
  window.Time.popup = function() {
    Time.dialog = new Object();

    var el = Gui.div("time-popup");

    el.appendChild(Time.popupAction());
    el.appendChild(Time.popupInput());

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

    for(var i = 1; i <= 9; i++) {
      saved.appendChild(Time.popupInput(i));
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
   * Create the element for displaying and editing time.
   *   &returns: The time element.
   */
  window.Time.popupInput = function(idx) {
    var el = Gui.div(idx ? "line" : "cur");

    if(idx != null) { el.appendChild(Gui.span("num", Gui.text(idx))); }

    var display = Gui.tag("input", "display");
    display.disabled = idx ? false : Time.run;
    display.type = "text";
    display.value = Time.string(idx ? Time.save[idx] : Time.loc);
    display.addEventListener("input", function() {
      reset.disabled = save.disabled = false;
    });
    el.appendChild(display);

    var save = Gui.button("✓", "set gui-button gui-green", function(e) {
      var loc = Loc.parse(display.value, Time.nbeats);
      if(loc == null) { return; }

      if(idx) {
        Time.save[idx] = loc;
      } else {
        Time.loc = loc;
        Web.put(0, { type: "seek", bar: Loc.bar(loc, Time.nbeats) });
      }
      
      display.value = Time.string(loc);
      reset.disabled = save.disabled = true;
    });
    save.disabled = true;
    el.appendChild(save);

    var reset = Gui.button("✗", "reset gui-button gui-red", function(e) {
      reset.disabled = save.disabled = true;
    });
    reset.disabled = true;
    el.appendChild(reset);

    if(idx == null) { Time.dialog.display = display; }

    return el;
  };


  /**
   * Create a string time using a location.
   *   @loc: The location.
   *   &returns: The string.
   */
  window.Time.string = function(loc) {
    var bar = Math.floor(loc.bar).toString();
    var beat = loc.beat.toFixed(1);
    return "0".repeat(Math.max(3 - bar.length, 0)) + bar + ":" + beat;
  };
})();
