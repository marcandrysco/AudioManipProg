(function() {
  "use strict";

  Object.prototype.keys = function() {
    return Object.keys(this);
  };

  window.isEqual = function(a, b) {
    if(a === b) { return true; }
    
    if((a instanceof Array) && (b instanceof Array)) {
      if(a.length != b.length) { return false; }

      for(var i = 0; i < a.length; i++) {
        if(!isEqual(a[i], b[i])) { return false; }
      }

      return true;
    }

    if((a instanceof Object) && (b instanceof Object)) {
      var keys = a.keys();
      if(!isEqual(keys.length, b.keys().length)) { return false; }

      for(var i = 0; i < keys.length; i++) {
        if(!isEqual(a[keys[i]], b[keys[i]])) { return false; }
      }

      return true;
    }

    return false
  };

  Array.prototype.getIndex = function(val) {
    for(var i = 0; i < this.length; i++) {
      if(isEqual(this[i], val)) { return i; }
    }
    return -1;
  };

  /**
   * Remove elements from an array by value.
   *   @val, ...: The list of values to remove.
   */
  Array.prototype.remove = function() {
    for(var i = 0; i < arguments.length; i++) {
      var idx = this.getIndex(arguments[i]);
      if(idx >= 0) { this.splice(idx, 1); }
    }
  };
})();
