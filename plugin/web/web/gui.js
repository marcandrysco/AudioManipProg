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

  /*
   * GUI namespace
   */
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
   *   @child: The new child or array of children.
   */
  window.Gui.replace = function(el, child) {
    Gui.clear(el);

    if(Array.isArray(child)) {
      for(var i = 0; i < child.length; i++) {
        el.appendChild(child[i]);
      }
    } else if(child) {
      el.appendChild(child);
    }
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
    if(typeof tag != "string") { throw "Parameter 'tag' must be string."; }

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
   * Crete a 'span' tag.
   *   @cls: Optional. Class list.
   *   @child: Optional. The child.
   *   &returns: The node.
   */
  window.Gui.span = function(cls, child) {
    return Gui.tag("span", cls, child);
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
   * Create an input.
   *   @type: The type.
   *   @value: The value.
   *   @cls: Optional. The class.
   *   @func: The change function.
   *   &returns: The DOM input element.
   */
  window.Gui.input = function(type, value, cls, func) {
    var input = Gui.tag("input");
    input.type = type;
    if((type == "checkbox") || (type == "radio")) {
      input.checked = value;
      if(func) { input.addEventListener("change", func); }
    } else {
      input.value = value;
      if(func) { input.addEventListener("input", func); }
    }
    if(cls) { input.classList.add(cls); }
    return input;
  };

  /**
   * Create a select with options.
   *   @opt: The options list.
   *   @cls: The class.
   *   @change: The change notifier.
   *   &returns: The DOM select element.
   */
  window.Gui.select = function(opt, cls, change) {
    var select = Gui.tag("select", cls);
    for(var i = 0; i < opt.length; i++) {
      if(opt[i] instanceof Node) {
        select.appendChild(Gui.tag("option", null, opt[i]));
      } else {
        select.appendChild(Gui.tag("option", null, Gui.text(opt[i])));
      }
    }
    if(cls) { select.classList.add(cls); }
    return select;
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
      toggle.appendChild(Gui.div(def ? null : "gui-noshow", Gui.text(text[0])));
      toggle.appendChild(Gui.div(def ? "gui-noshow" : null, Gui.text(text[1])));
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
        toggle.firstChild.classList.toggle("gui-noshow");
        toggle.lastChild.classList.toggle("gui-noshow");
      }
      if(func) { func(toggle.guiState); }
    });
    return toggle;
  };


  /**
   * Create a button.
   *   @text: The button text.
   *   @opt: The options object.
   *   @func: The click function.
   *   &returns: The button.
   */
  window.Gui.Button = function(text, opt, func) {
    if(typeof text == "string") {
      text = Gui.text(text);
    } else if(!(text instanceof Node) && !Array.isArray(text)) {
      throw "First argument to Gui.Button must be a string, a node, or a list or nodes.";
    }
    
    var button = Gui.tag("button", "gui-button", text);

    if(opt && opt.cls) {
      if(Array.isArray(opt.cls)) {
        opt.cls.forEach(function (v) { button.classList.add(v); });
      } else {
        button.classList.add(opt.cls);
      }
    }

    if(opt && opt.fit) {
      if(!Array.isArray(opt.fit)) { throw "Option 'fit' must be an array of strings."; }
      opt.fit.forEach(function(v) {
        button.appendChild(Gui.div("gui-fit", Gui.text(v)));
      });
    }

    button.addEventListener("click", func);

    return button;
  };

  /**
   * Create a slider.
   *   &returns: The slider.
   */
  window.Gui.Slider = function(opt, func) {
    var slider = Gui.div("gui-slider");

    if(opt.vert) { slider.classList.add("gui-vert"); }
    if(opt.cls) { slider.classList.add(opt.cls); }

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

      if(opt.vert) {
        slider.guiCur.style["top"] = (100 * (1 - x)) + "%";
      } else {
        slider.guiCur.style["left"] = (100 * x) + "%";
      }

      if(opt.input) { slider.guiInput.value = Math.round(slider.guiMax * x); }
      if(func) { func(slider, x); }
    };

    left.addEventListener("mousedown", function(e) {
      if(e.button != 0) { return; }
      e.preventDefault();

      var move = function(e) {
        var x;
        if(opt.vert) {
          x = 1 - (e.clientY - left.getBoundingClientRect().top) / left.clientHeight;
        } else {
          x = (e.clientX - left.getBoundingClientRect().left) / left.clientWidth;
        }
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

    if(opt.input) {
      slider.guiMax = opt.input;
      slider.guiInput = Gui.tag("input");
      slider.guiInput.type = "text";
      slider.appendChild(slider.guiInput);
    }

    slider.guiUpdate(opt.def ? opt.def : 0.5);

    return slider;
  };

  /**
   * Create a mousedown handler for dragging.
   *   @func: The external handler function.
   *   &returns: The handler.
   */
  Gui.drag = function(func) {
    return function(e) {
      if(e.button != 0) { return; }
      func(e, "down");
      Gui.dragNow(func);
    };
  };
  Gui.dragNow = function(func) {
    var move = function(e) {
      func(e, "move");
    };
    var out = function(e) {
      if(e.relatedTarget == window.body) {
        func(e, "out");
        done(e);
      }
    };
    var up = function(e) {
      func(e, "up");
      done(e);
    };
    var done = function(e) {
      window.removeEventListener("mousemove", move);
      window.removeEventListener("mouseup", up);
      window.removeEventListener("mouseout", out);
      func(e, "done");
    };
    window.addEventListener("mousemove", move);
    window.addEventListener("mouseout", out);
    window.addEventListener("mouseup", up);
  };


  /**
   * Create a popup, usually added to `document.body`.
   *   @le: The internal element.
   *   @func: The functio, when destroyedn.
   *   &returns: The popup.
   */
  Gui.Popup = function(el, func) {
    var popup = Gui.div("gui-popup");
    var wrap = Gui.div("gui-wrap", el);
    popup.appendChild(wrap);

    popup.guiDismiss = function() {
      if(popup.parentNode) {
        popup.parentNode.removeChild(popup);
      }
    };
    wrap.addEventListener("click", function(e) {
      e.stopPropagation();
    });
    popup.addEventListener("click", function(e) {
      if(popup.parentNode) {
        popup.parentNode.removeChild(popup);
        func(e);
      }
    });

    return popup;
  };


  /**
   * Create a select button.
   *   @elem: The displayed element on popup.
   *   @opt: The options.
   *   &returns: The select element.
   */
  window.Gui.Select = function(elem, opt) {
    var select = Gui.div("gui-select-wrap");

    if(opt.cls) { select.classList.add(opt.cls); }

    select.guiPopup = null;
    select.guiElem = elem;
    select.guiButton = Gui.tag("button", "gui-select-button", Gui.text(opt.text ? opt.text : "Select"));
    select.guiButton.addEventListener("mousedown", Gui.SelectShow(select, select.guiButton));
    select.appendChild(select.guiButton);

    return select;
  };
  /**
   * Handler to show the select.
   *   @select: The select element.
   *   @button: The button element.
   *   &returns: The handler.
   */
  window.Gui.SelectShow = function(select, button) {
    return function(e) {
      if(e.button != 0) { return; }
      if(select.guiPopup != null) { return; }

      select.guiPopup = Gui.div("gui-select-popup");
      select.appendChild(select.guiPopup);

      if(typeof select.guiElem == "function") {
        select.guiPopup.appendChild(select.guiElem(select));
      }
 
      var dismiss = function(e) {
        select.removeChild(select.guiPopup);
        select.guiPopup = null;
        window.removeEventListener("click", dismiss);
      };

      var click = function(e) {
        window.removeEventListener("click", click);
        window.removeEventListener("mouseup", mouseup);
        window.addEventListener("click", dismiss);
        e.stopPropagation();
      };

      var mouseup = function(e) {
        if(e.target == button) { return; }

        select.removeChild(select.guiPopup);
        select.guiPopup = null;
        window.removeEventListener("click", click);
        window.removeEventListener("mouseup", mouseup);
        e.stopPropagation();
      };

      window.addEventListener("click", click);
      window.addEventListener("mouseup", mouseup);
    };
  };


  /**
   * Create an entry widget.
   *   @opt: The options.
   *   @func: The change callback.
   *   &returns: The entry DOM element.
   */
  window.Gui.Entry = function(opt, func) {
    var entry = Gui.tag("input", "gui-entry");

    if(opt.cls) { entry.classList.add(opt.cls); }
    if(opt.def) { entry.value = opt.def; }

    return entry;
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
  window.Req.get = function(url, suc, fail) {
    var req = new XMLHttpRequest();
    req.addEventListener("load", function(e) {
      if(req.status == 200) {
        suc(req.responseText);
      } else if(fail) {
        fail();
      }
    });
    req.addEventListener("error", function() {
      if(fail) { fail(); }
    });
    req.open("GET", url);
    req.send();
  };

  /**
   * Perform a post request to a URL.
   *   @url: The URL.
   *   @param: The post parameter.
   *   @suc: The success function.
   */
  window.Req.post = function(url, param, suc, fail) {
    var req = new XMLHttpRequest();
    req.addEventListener("load", function(e) {
      if(req.status == 200) {
        suc(req.responseText);
      } else if(fail) {
        fail();
      }
    });
    req.addEventListener("error", function() {
      if(fail) { fail(); }
    });
    req.open("POST", url);
    req.send(param);
  };
})();
