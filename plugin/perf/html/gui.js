(function() {
  "use strict";

  /* GUI namespace */
  window.Gui = new Object();

  /**
   * Retrieve an element by its identifier.
   *   @id: The identifier.
   *   &Returns: The element or null.
   */
  window.Gui.byid = function(id) {
    return document.getElementById(id);
  };

  /**
   * Clear all children from an element.
   *   @el: The element.
   */
  window.Gui.clear = function(el) {
    while(el.firstChild) {
      el.removeChild(el.firstChild)
    }
  };

  /**
   * Replace all of the elements children with a new child.
   *   @el: The element.
   *   @child: The new child.
   */
  window.Gui.replace = function(el, child) {
    Gui.clear(el);
    el.appendChild(child);
  };

  /**
   * Create a text node.
   *   @text: The text.
   *   &returns: The node.
   */
  window.Gui.text = function(text) {
    return document.createTextNode(text);
  };

  /**
   * Crete a tag node.
   *   @tag: The tag.
   *   @cls: Optional. Class list.
   *   @child: Optional. The child.
   *   &returns: The node.
   */
  window.Gui.tag = function(tag, cls, child) {
    var el = document.createElement(tag);
    if(cls) { el.className = cls; }
    if(Array.isArray(child)) {
      for(var i = 0; i < child.length; i++) {
        el.appendChild(child[i]);
      }
    } else if(child) {
      el.appendChild(child);
    }
    return el;
  };

  /**
   * Crete a 'div' tag.
   *   @cls: Optional. Class list.
   *   @child: Optional. The child.
   *   &returns: The node.
   */
  window.Gui.div = function(cls, child) {
    return Gui.tag("div", cls, child);
  };

  /**
   * Convert JSON into a prettified 'pre' tag.
   *   @json: The JSON.
   *   &returns: The prettified 'pre' tag.
   */
  window.Gui.JSON = function(json) {
    return Gui.tag("pre", null, Gui.text(JSON.stringify(json, null, 2)));
  };


  /* Request namespace */
  window.Req = new Object();

  /**
  * Perform a get request from a URL.
  *   @url: The URL.
  *   @suc: The success function.
  */
  window.Req.get = function(url, suc, param) {
    var req = new XMLHttpRequest();
    req.addEventListener("load", function() {
      suc(req.responseText);
    });
    req.open("GET", url);
    req.send(param);
  };

  /**
   * Perform a post request to a URL.
   *   @url: The URL.
   *   @param: The post parameter.
   *   @suc: The success function.
   */
  window.Req.post = function(url, param, suc) {
    var req = new XMLHttpRequest();
    req.addEventListener("load", function() {
      suc(req.responseText);
    });
    req.open("POST", url);
    req.send(param);
  };
})();
