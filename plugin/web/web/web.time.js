(function() {
  "use strict";

  /*
   * Time namespace
   */
  window.Time = new Object();
  window.Time.run = false;

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

    return time;
  };

  /**
   * Update the time elements.
   *   @time: The time information.
   */
  window.Time.update = function(time) {
    var bar = Math.floor(time.bar).toString();
    var beat = time.beat.toFixed(1);

    Gui.replace(Time.display, Gui.text("0".repeat(3 - bar.length) + bar + ":" + beat));
  };

  /**
   * Create the time popup.
   *   &returns: The popup.
   */
  window.Time.popup = function() {
    var el = Gui.div("time-popup");

    var num = Gui.tag("input", "num");
    num.disabled = Web.rec;
    num.type = "text";

    el.appendChild(num);

    var seek = Gui.div("div");
    el.appendChild(seek);

    var action = Gui.div("action");
    el.appendChild(action);

    var start = Gui.button("Start", "start gui-button gui-green", function(e) {
      Web.put(0, { type: "start" });
    });

    var stop = Gui.button("Stop", "stop gui-button gui-red", function(e) {
      Web.put(0, { type: "stop" });
    });

    action.appendChild(start);
    action.appendChild(stop);

    (Web.rec ? start : stop).classList.add("hidden");

    var popup = Gui.Popup(el, function(e) { });

    return popup;
  };
})();
