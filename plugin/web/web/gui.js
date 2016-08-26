/**
 * GUI Javascript Toolkit.
 *
 * Version 1.0
 *
 * MIT Licensed.
 * Copyright (c) 2016 Marc Andrysco.
 */

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

    if(Array.isArray(cls)) {
      cls.forEach(function(v) { el.classList.add(v); });
    } else if(cls) {
      el.className = cls;
    }

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
   * Create a button.
   *   @text: The button text.
   *   @cls: Optional. Class list.
   *   @func: The callback when clicked.
   */
  window.Gui.button = function(text, cls, func) {
    var button = window.Gui.tag("button", cls, Gui.text(text));
    button.addEventListener("click", func);
    return button;
  };


  /**
   * Create a toggle.
   *   @text: The toggle text.
   *   @def: The default position.
   *   @func: The callback when clicked.
   */
  window.Gui.Toggle = function(text, def, func) {
    var toggle = window.Gui.tag("button", ["gui-toggle", def ? "gui-on" : "gui-off"]);
    if(Array.isArray(text)) {
      toggle.appendChild(Gui.text(def ? text[0] : text[1]));
    } else {
      toggle.appendChild(Gui.text(text));
    }
    toggle.guiState = def;
    toggle.guiEnable = function() { };
    toggle.guiDisable = function() { };
    toggle.guiFlip = function() { };
    toggle.addEventListener("click", function(e) {
      toggle.guiState = !toggle.guiState;
      toggle.classList.remove("gui-on", "gui-off");
      toggle.classList.add(toggle.guiState ? "gui-on" : "gui-off");
      if(Array.isArray(text)) {
        Gui.replace(toggle, Gui.text(toggle.guiState ? text[0] : text[1]));
      }
      if(func) { func(toggle.guiState); }
    });
    return toggle;
  };


  /**
   * Create a slider.
   *   &returns: The slider.
   */
  window.Gui.Slider = function(opt, func) {
    var slider = Gui.div("gui-slider");

    var left = Gui.div("gui-left gui-noselect");
    slider.appendChild(left);

    slider.guiTrack = Gui.div("gui-track gui-noselect");
    left.appendChild(Gui.div("gui-space"));
    left.appendChild(slider.guiTrack);
    left.appendChild(Gui.div("gui-space"));

    var rail = Gui.div("gui-rail gui-noselect");
    left.appendChild(rail);

    slider.guiCur = Gui.div("gui-cur gui-noselect");
    rail.appendChild(slider.guiCur);

    slider.guiUpdate = function(x) {
      if(x < 0) { x = 0; } else if(x > 1) { x = 1; }
      slider.guiCur.style.left = (100 * x) + "%";
      slider.guiInput.value = Math.round(slider.guiMax * x);
      if(func) { func(slider, x); }
    };

    left.addEventListener("mousedown", function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var move = function(e) {
        var x = (e.clientX - left.offsetLeft) / left.clientWidth;
        slider.guiUpdate(x);
      };
      var out = function(e) {
        if(e.relatedTarget == window.body) { up(e); }
      };
      var up = function(e) {
        window.removeEventListener("mousemove", move);
        window.removeEventListener("mouseup", up);
        window.removeEventListener("mouseout", out);
      };
      window.addEventListener("mousemove", move);
      window.addEventListener("mouseout", out);
      window.addEventListener("mouseup", up);
      move(e);
    });

    if(typeof opt == "object") {
      if(opt.input) {
        slider.guiMax = opt.input;
        slider.guiInput = Gui.tag("input");
        slider.guiInput.type = "text";
        slider.appendChild(slider.guiInput);
      }
    }

    slider.guiUpdate(0.5);

    return slider;
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
