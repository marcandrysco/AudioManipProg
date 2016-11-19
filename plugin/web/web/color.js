(function() {
  "use strict"


  /*
   * RGB namespace
   */
  window.RGB = new Object();

  /**
   * Create an RGBA color.
   *   @r: The red component.
   *   @g: The green component.
   *   @b: The blue component.
   *   @a: Optional. The alpha component.
   *   &returns: The RGB color.
   */
  window.RGB.make = function(r, g, b, a) {
    if(a === undefined) { a = 1.0; }

    return { r: r, g: g, b: b, a: a, copy: function() { return RGB.make(this.r, this.g, this.b, this.a); } };
  };

  /**
   * Convert an RGB color to an HSV color.
   *   @rgb: The RGB color.
   *   &returns: The HSV color.
   */
  window.RGB.hsv = function(rgb) {
    var max = Math.max(rgb.r, rgb.g, rgb.b);
    var min = Math.min(rgb.r, rgb.g, rgb.b);
    var h, s, d = max - min;

    if(d != 0) {
      s = d / max;
      if(max == rgb.r) {
        h = (rgb.g - rgb.b) / d;
      } else if(max == rgb.g) {
        h = (rgb.b - rgb.r) / d + 2;
      } else if(max == rgb.b) {
        h = (rgb.r - rgb.g) / d + 4;
      }
      h /= 6;
      if(h < 0) { h += 1; }
    }
    else {
      s = 0;
      h = 0;
    }
    
    return HSV.make(h, s, max);
  };

  window.RGB.str = function(rgb) {
    var r = Math.floor(255 * rgb.r);
    var g = Math.floor(255 * rgb.g);
    var b = Math.floor(255 * rgb.b);

    return "rgba(" + r + "," + g + "," + b + "," + a + ")";
  };


  /*
   * HSV namespace
   */
  window.HSV = new Object();

  /**
   * Create an HSV color.
   *   @h: The hue.
   *   @s: The saturation.
   *   @v: The value.
   *   @a: Optional. The alpha component.
   *   &returns: The HSV color.
   */
  window.HSV.make = function(h, s, v, a) {
    if(a === undefined) { a = 1.0; }

    return { h: h, s: s, v: v, a: a, copy: function() { return HSV.make(this.h, this.s, this.v, this.a); } };
  };

  /**
   * Convert an HSV color to an RGB color.
   *   @rgb: The HSV color.
   *   &returns: The RGB color.
   */
  window.HSV.rgb = function(hsv) {
    var c = hsv.v * hsv.s;
    var x = c * (1 - Math.abs((hsv.h * 6) % 2 - 1));
    var m = hsv.v - c;
    var r, g, b;

    if(hsv.h < (1/6)) {
      r = c; g = x; b = 0;
    } else if(hsv.h < (2/6)) {
      r = x; g = c; b = 0;
    } else if(hsv.h < (3/6)) {
      r = 0; g = c; b = x;
    } else if(hsv.h < (4/6)) {
      r = 0; g = x; b = c;
    } else if(hsv.h < (5/6)) {
      r = x; g = 0; b = c;
    } else {
      r = c; g = 0; b = x;
    }
    return RGB.make(r + m, g + m, b + m);
  };

  window.HSV.str = function(hsv) {
    var h = Math.floor(360 * hsv.h);
    var s = Math.floor(hsv.s);
    var v = Math.floor(hsv.v);

    return "hsva(" + h + "," + s + "," + v + "," + a + ")";
  };


  /*
   * HSL namespace
   */
  window.HSL = new Object();

  /**
   * Create an HSV color.
   *   @h: The hue.
   *   @s: The saturation.
   *   @l: The lightness.
   *   @a: Optional. The alpha component.
   *   &returns: The HSV color.
   */
  window.HSL.make = function(h, s, l, a) {
    if(a === undefined) { a = 1.0; }

    return {
      h: h, s: s, l: l, a: a,
      copy: function() { return HSL.copy(this); },
      toString: function() { return HSL.str(this) }
    };
  };

  window.HSL.copy = function(hsl) {
    return HSL.make(hsl.h, hsl.s, hsl.l, hsl.a);
  };

  window.HSL.str = function(hsl) {
    var h = Math.floor(360 * hsl.h);
    var s = Math.floor(100 * hsl.s);
    var l = Math.floor(100 * hsl.l);

    return "hsla(" + h + "," + s + "%," + l + "%," + hsl.a + ")";
  };


  /*
   * color namespace
   */
  window.Color = new Object();

  window.Color.isrgb = function(s) {
    return (s == "r") || (s == "g") || (s == "b");
  };

  window.Color.ishsv = function(s) {
    return (s == "h") || (s == "s") || (s == "v");
  };

  /**
   * Convert an RGB color to a hexidecimal code.
   *   @rgb: The RGB color.
   *   &returns: The hex string.
   */
  window.Color.rgb2hex = function(rgb) {
    var ch = function(c) {
      c = Math.floor(255 * c);
      return ((c < 16) ? "0" : "") + c.toString(16);
    };
    return ch(rgb.r) + ch(rgb.g) + ch(rgb.b);
  };

  /**
   * Convert a JSON value into a component.
   *   @json: The JSON.
   *   &returns: The component.
   */
  window.Color.json2comp = function(json) {
    if(typeof json != "number") { throw "Invalid component"; }
    if((json < 0.0) || (json > 1.0)) { throw "Component out of range"; }
    return json;
  };

  window.Color.json2rgb = function(json) {
    return Color.rgb(Color.json2comp(json.r), Color.json2comp(json.g), Color.json2comp(json.b));
  };

  /**
   * Create an RGB color.
   *   @r: The red component.
   *   @g: The green component.
   *   @b: The blue component.
   *   &returns: The RGB color.
   */
  window.Color.rgb = function(r, g, b) {
    return { r: r, g: g, b: b, copy: function() { return Color.rgb(this.r, this.g, this.b); } };
  };

  /**
   * Create an RGBA color.
   *   @r: The red component.
   *   @g: The green component.
   *   @b: The blue component.
   *   @a: The alpha component.
   *   &returns: The RGB color.
   */
  window.Color.rgba = function(r, g, b, a) {
    return { r: r, g: g, b: b, a: a, copy: function() { return Color.rgb(this.r, this.g, this.b, this.a); } };
  };

  /**
   * Create an HSV color.
   *   @h: The hue.
   *   @s: The saturation.
   *   @v: The value.
   *   &returns: The HSV color.
   */
  window.Color.hsv = function(h, s, v) {
    return { h: h, s: s, v: v, copy: function() { return Color.hsv(this.h, this.s, this.v); } };
  };

  /**
   * Create an HSVA color.
   *   @h: The hue.
   *   @s: The saturation.
   *   @v: The value.
   *   @a: The alpha.
   *   &returns: The HSV color.
   */
  window.Color.hsva = function(h, s, v, a) {
    return { h: h, s: s, v: v, a: a, copy: function() { return Color.hsv(this.h, this.s, this.v, this.a); } };
  };


  /*
   * Verify Namespace
   */
  window.Color.verify = new Object();

  /**
   * Verify if a pallet is valid.
   *   @pallet: The pallet.
   *   &returns: True if valid.
   */
  window.Color.verify.rgb = function(rgb) {
    if(typeof rgb != "object") { return false; }
    if(Object.keys(rgb).length != 3) { return false; }
    for(var i = 0; i < 3; i++) {
      var color = "rgb".charAt(i);
      if(!Number.isFinite(rgb[color])) { return false; }
      if((rgb[color] < 0.0) || (rgb[color] > 1.0)) { return false; }
    }
    return true;
  };

  /**
   * Convert an RGB color to an HSV color.
   *   @rgb: The RGB color.
   *   &returns: The HSV color.
   */
  var rgb2hsv = function(rgb) {
    var h, s;
    var max = Math.max(rgb.r, rgb.g, rgb.b);
    var min = Math.min(rgb.r, rgb.g, rgb.b);
    var d = max - min;
    if(d != 0) {
      s = d / max;
      if(max == rgb.r) {
        h = (rgb.g - rgb.b) / d;
      } else if(max == rgb.g) {
        h = (rgb.b - rgb.r) / d + 2;
      } else if(max == rgb.b) {
        h = (rgb.r - rgb.g) / d + 4;
      }
      h /= 6;
      if(h < 0) { h += 1; }
    }
    else {
      s = 0;
      h = 0;
    }
    
    return Color.hsv(h, s, max);
  };

  /**
   * Convert an HSV color to an RGB color.
   *   @rgb: The HSV color.
   *   &returns: The RGB color.
   */
  var hsv2rgb = function(hsv) {
    var c = hsv.v * hsv.s;
    var x = c * (1 - Math.abs((hsv.h * 6) % 2 - 1));
    var m = hsv.v - c;
    var r, g, b;
    if(hsv.h < (1/6)) {
      r = c; g = x; b = 0;
    } else if(hsv.h < (2/6)) {
      r = x; g = c; b = 0;
    } else if(hsv.h < (3/6)) {
      r = 0; g = c; b = x;
    } else if(hsv.h < (4/6)) {
      r = 0; g = x; b = c;
    } else if(hsv.h < (5/6)) {
      r = x; g = 0; b = c;
    } else {
      r = c; g = 0; b = x;
    }
    return Color.rgb(r + m, g + m, b + m);
  };

  var hsv2str = function(hsv) {
    var h = Math.floor(360 * hsv.h);
    var s = Math.floor(hsv.s);
    var v = Math.floor(hsv.v);
    return "hsv(" + h + "," + s + "," + v + ")";
  };

  window.Color.rgb2str = function(rgb) {
    var r = Math.floor(255 * rgb.r);
    var g = Math.floor(255 * rgb.g);
    var b = Math.floor(255 * rgb.b);
    return "rgb(" + r + "," + g + "," + b + ")";
  };

  window.Color.picker = function(onchange) {
    var picker = Gui.div("gui-color-picker");
    picker.rgb = Color.rgb(0, 0, 0);
    picker.hsv = Color.hsv(0, 0, 0);
    picker.color = { r: 0, g: 0, b: 0 };
    picker.onchange = onchange;

    var mkgrad = function(picker, comp) {
      var pt = function(val) {
        if("rgb".indexOf(comp) >= 0) {
          var rgb = picker.rgb.copy();
          if(comp == "r") rgb.r = val;
          if(comp == "g") rgb.g = val;
          if(comp == "b") rgb.b = val;
          return Color.rgb2str(rgb);
        }
        else {
          var hsv = picker.hsv.copy();
          if(comp == "h") hsv.h = val;
          if(comp == "s") hsv.s = val;
          if(comp == "v") hsv.v = val;
          return Color.rgb2str(hsv2rgb(hsv));
        }
      };
      if(comp == "h") {
        return "linear-gradient(left,"+pt(0)+","+pt(1/6)+","+pt(2/6)+","+pt(3/6)+","+pt(4/6)+","+pt(5/6)+","+pt(6/6)+")";
      } else {
        return "linear-gradient(left,"+pt(0)+","+pt(1)+")";
      };
    };
    var mkcolor = function(comp) {
      var color = Gui.div("color");
      picker.appendChild(color);

      var slider = Gui.div("slider noselect");
      color.appendChild(slider);

      color.bg = Gui.div("bg noselect");
      color.bg.style.backgroundImage = "-moz-" + mkgrad(picker, comp);
      color.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, comp);
      slider.appendChild(Gui.div("space"));
      slider.appendChild(color.bg);
      slider.appendChild(Gui.div("space"));

      var rail = Gui.div("rail noselect");
      slider.appendChild(rail);

      color.cur = Gui.div("cur noselect");
      rail.appendChild(color.cur);

      color.appendChild(Gui.div("space"));
      color.text = Gui.tag("input", "text");
      color.text.type = "text";
      color.appendChild(color.text);

      slider.addEventListener("mousedown", function(e) {
        if(e.button != 0) { return; }
        e.preventDefault();

        var move = function(e) {
          var x = (e.clientX - slider.offsetLeft) / slider.clientWidth;
          if(x < 0) { x = 0; }
          if(x > 1) { x = 1; }
          color.cur.style.left = (100 * x) + "%";
          if(Color.isrgb(comp)) {
            picker.rgb[comp] = x;
            picker.hsv = rgb2hsv(picker.rgb);
          } else {
            picker.hsv[comp] = x;
            picker.rgb = hsv2rgb(picker.hsv);
          }
          picker.change();
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

      return color;
    };

    picker.appendChild(picker.red = mkcolor("r"));
    picker.appendChild(picker.green = mkcolor("g"));
    picker.appendChild(picker.blue = mkcolor("b"));
    picker.appendChild(picker.hue = mkcolor("h"));
    picker.appendChild(picker.sat = mkcolor("s"));
    picker.appendChild(picker.val = mkcolor("v"));

    var detail = Gui.div("detail");
    picker.appendChild(detail);

    picker.preview = Gui.div("preview");
    detail.appendChild(picker.preview);

    picker.hex = Gui.tag("input", "hex");
    picker.hex.type = "text";
    picker.hex.value = "hi";
    detail.appendChild(picker.hex);

    picker.update = function(rgb) {
      picker.rgb.r = rgb.r;
      picker.rgb.g = rgb.g;
      picker.rgb.b = rgb.b;
      picker.hsv = rgb2hsv(picker.rgb);
      picker.refresh();
    };
    picker.change = function() {
      picker.refresh();
      picker.onchange(picker.rgb);
    };
    picker.refresh = function() {
      picker.red.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "r");
      picker.red.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "r");
      picker.red.text.value = Math.floor(255 * picker.rgb.r);
      picker.blue.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "b");
      picker.blue.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "b");
      picker.blue.text.value = Math.floor(255 * picker.rgb.b);
      picker.green.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "g");
      picker.green.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "g");
      picker.green.text.value = Math.floor(255 * picker.rgb.g);
      picker.hue.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "h");
      picker.hue.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "h");
      picker.hue.text.value = Math.floor(360 * picker.hsv.h);
      picker.sat.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "s");
      picker.sat.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "s");
      picker.sat.text.value = Math.floor(picker.hsv.s * 100);
      picker.val.bg.style.backgroundImage = "-webkit-" + mkgrad(picker, "v");
      picker.val.bg.style.backgroundImage = "-moz-" + mkgrad(picker, "v");
      picker.val.text.value = Math.floor(picker.hsv.v * 100);
      picker.preview.style.backgroundColor = Color.rgb2str(picker.rgb);
      picker.hex.value = Color.rgb2hex(picker.rgb);
      picker.red.cur.style.left = (100 * picker.rgb.r) + "%";
      picker.green.cur.style.left = (100 * picker.rgb.g) + "%";
      picker.blue.cur.style.left = (100 * picker.rgb.b) + "%";
      picker.hue.cur.style.left = (100 * picker.hsv.h) + "%";
      picker.sat.cur.style.left = (100 * picker.hsv.s) + "%";
      picker.val.cur.style.left = (100 * picker.hsv.v) + "%";
      //txt.innerHTML = "rgba("+picker.color.r+","+picker.color.g+","+picker.color.b+",1)";
    };

    picker.refresh();

    return picker;
  };
})();
