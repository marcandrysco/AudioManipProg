(function() {
  "use strict";

  /**
   * Mode enumerator.
   *   @view: View.
   *   @insert: Insert/interactive.
   *   @num: Number.
   *   @frac: Fraction.
   *   @copy: Copy.
   */
  window.Mode = { view: 0, insert: 1, num: 2, frac: 3, copy: 4 };

  /*
   * Status namespace
   */
  window.Status = new Object();

  /**
   * Initialize the status namespace.
   */
  window.Status.init = function() {
    Status.mode = Mode.view;
    Status.num = 0;
    Status.bar = null; 
    Status.msg = null;
  };

  /**
   * Create the status element.
   *   &returns: The element.
   */
  window.Status.elem = function() {
    Status.bar = Gui.div("status");
    Status.update();

    return Status.bar;
  };

  /**
   * Update the mode line of the status bar.
   *   @mode: Optional. The new mode.
   *   @msg: Optional. The new message.
   */
  window.Status.update = function(mode, msg) {
    if(mode != null) { Status.mode = mode; }
    if(msg != null) { Status.msg = msg; }

    if(Status.bar == null) { return ; }
    
    var text = "";

    switch(Status.mode) {
    case Mode.view: text = "View"; break;
    case Mode.insert: text = "Insert"; break;
    case Mode.copy: text = "Copy"; break;
    case Mode.num: text = Status.num; break;
    }

    if(Status.msg) { text += " - " + Status.msg; }

    Gui.replace(Status.bar, Gui.text(text));
  };

  /**
   * Handle a keypress with the status.
   *   @e: The event.
   */
  window.Status.keypress = function(e) {
    Status.msg = null;

    switch(Status.mode) {
    case Mode.view:
      if(e.key == "i") {
        Status.mode = Mode.insert;
      }
      if(!isNaN(parseInt(e.key, 10))) {
        Status.mode = Mode.num;
        Status.num = parseInt(e.key, 10);
      }
      break;

    case Mode.insert:
      if(e.code == "Escape") { Status.mode = Mode.view; }
      break;

    case Mode.num:
      if(e.code == "Escape") { Status.mode = Mode.view; }

      if(!isNaN(parseInt(e.key, 10))) {
        Status.num = 10 * Status.num + parseInt(e.key, 10);
      }
      break;

    case Mode.frac:
      break;

    case Mode.copy:
      if(e.code == "Escape") { Status.mode = Mode.view; }
      break;
    }

    Status.update();
  };
})();
