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


  Array.range = function() {
    if(arguments.length == 0) {
      throw "Array.range requires at least one argument";
    } else if(arguments.length == 1) {
      var arr = new Array();
      for(var i = 0; i < arguments[0]; i++) {
        arr.push(i);
      }
      return arr;
    } else if(arguments.length == 2) {
      var arr = new Array();
      for(var i = arguments[0]; i <= arguments[1]; i++) {
        arr.push(i);
      }
      return arr;
    } else if(arguments.length == 3) {
      var arr = new Array();
      for(var i = arguments[0]; i <= arguments[1]; i += arguments[2]) {
        arr.push(i);
      }
      return arr;
    } else {
      throw "Array.range take at most 3 arguments";
    }
  };
})();
