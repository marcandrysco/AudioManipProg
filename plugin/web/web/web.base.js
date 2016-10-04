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
