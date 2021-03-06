(function() {
  "use strict";

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
    if(!Number.isInteger(val)) { throw("Value must be integer."); }
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


  /**
   * Create a selector for a key.
   *   @opt: The options.
   *   @func: The function.
   */
  window.Key.select = function(opt, func) {
    var make = function(select) {
      var elem = Gui.div("key-select-opt");

      for(var n = 0; n < 8; n++) {
        var row = Gui.div("row");
        for(var k = 0; k < 12; k++) {
          (function() {
            var str = Key.str(n*12+k);
            var key = Gui.div("key", Gui.text(str));
            key.addEventListener("click", function(e) {
              Gui.replace(select.guiButton, Gui.text(str + " ▾"));
            });
            key.addEventListener("mouseup", function(e) {
              Gui.replace(select.guiButton, Gui.text(str + " ▾"));
            });
            row.appendChild(key);
          })();
        }
        elem.appendChild(row);
      }

      return elem;
    };

    return Gui.Select(make, { text:"C4 ▾", cls:"key-select" });
  };


  /*
   * Location namespace
   */
  window.Loc = new Object();

  /**
   * Create a location.
   *   @bar: The bar.
   *   @beat: The beat.
   *   &returns: The location.
   */
  window.Loc.make = function(bar, beat) {
    if(bar != Math.round(bar)) { throw("Loc.make: Bar must be an integer."); }
    if(isNaN(beat) || (beat < 0)) { throw("Loc.make: Beat must be nonnegative."); }

    return { bar: bar, beat: beat, copy: function() { return Loc.make(this.bar, this.beat); } };
  };

  /**
   * Make a location using a fractional bar and number of beats.
   *   @bar: The bar as a float.
   *   @nbeats: The number of beats.
   *   &returns: The location.
   */
  window.Loc.makef = function(bar, nbeats) {
    return Loc.make(Math.floor(bar), (bar - Math.floor(bar)) * nbeats);
  };


  /**
   * Compute the floating-point bar for a location.
   *   @loc: The location.
   *   @nbeats: The number of bytes.
   *   &return: The bar.
   */
  window.Loc.bar = function(loc, nbeats) {
    return loc.bar + loc.beat / nbeats;
  };


  /**
   * Add a number of beats to the location.
   *   @loc: The location.
   *   @beats: The number of beats.
   *   @nbeats: The number of bytes per bar.
   *   &returns: The new loation.
   */
  window.Loc.add = function(loc, beats, nbeats) {
    return Loc.makef(Loc.bar(loc, nbeats) + beats / nbeats, nbeats)
  };


  /**
   * Parse a location.
   *   @str: The string.
   *   @nbeats: The number of beats.
   *   &returns: The location or null.
   */
  window.Loc.parse = function(str, nbeats) {
    var sep = str.indexOf(":");
    if(sep < 0) {
      var bar = Number(str);
      if(isNaN(bar)) { return null; }

      return Loc.makef(bar, nbeats);
    } else {
      var bar = Number(str.substr(0, sep));
      if(bar != Math.round(bar)) { return null; }

      var beat = Number(str.substr(sep+1));
      if((beat < 0) || (beat >= nbeats) || isNaN(beat)) { return null; }

      return Loc.make(bar, beat);
    }
  };


  /**
   * Compare two locations for their order.
   *   @left: The left location.
   *   @right: The right location.
   *   &returns: True if properly ordered.
   */
  window.Loc.cmp = function(left, right) {
    if(left.bar < right.bar)
      return -2;
    else if(left.bar > right.bar)
      return 2;
    else if(left.beat < right.beat)
      return -1;
    else if(left.beat > right.beat)
      return 1;
    else
      return 0;
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
})();
