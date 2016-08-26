(function() {
  "use strict";

  /*
   * Pack namespace
   */
  window.Pack = new Object();

  /**
   * Create a box-rect from a box.
   *   @x: The x-coordinate.
   *   @y: The y-coordinate.
   *   @width: The width.
   *   @height: The height.
   *   &returns: The box-rect.
   */
  window.Pack.box = function(x, y, width, height) {
    return {
      x: x, y: y,
      width: width, height: height,
      left: x, right: x + width - 1,
      top: y, bottom: y + height - 1,
      inside: function(x, y) { return (x >= this.left) && (x <= this.right) && (y >= this.top) && (y <= this.bottom); },
      copy: function() { return Pack.rect(this.left, this.right, this.top, this.bottom); }
    }
  };

  /**
   * Create a box-rect from a rect.
   *   @left: The left edge.
   *   @right: The right edge.
   *   @top: The top edge.
   *   @bottom: The bottom edge.
   *   &returns: The box-rect.
   */
  window.Pack.rect = function(left, right, top, bottom) {
    return {
      x: left, y: top,
      width: right - left + 1, height: bottom - top + 1,
      left: left, right: right,
      top: top, bottom: bottom,
      inside: function(x, y) { return (x >= this.left) && (x <= this.right) && (y >= this.top) && (y <= this.bottom); },
      copy: function() { return Pack.rect(this.left, this.right, this.top, this.bottom); }
    }
  };

  /**
   * Create a box-rect with the requested padding.
   *   @box: The box.
   *   @(top, left, right, bottom): Padding all edges.
   *   @(vert, horiz): Vertical and horizontal components.
   *   @all: Identical padding on all sides.
   */
  window.Pack.pad = function(box, padx, pady) {
    if(pady === undefined) { pady = padx; }
    return Pack.rect(box.left + padx, box.right - padx, box.top + pady, box.bottom - pady);
  };

  /**
   * Create a box-rect from a canvas.
   *   @canvas: The canvas.
   *   &returns: The box-rect.
   */
  window.Pack.canvas = function(canvas) {
    return Pack.box(0, 0, canvas.width, canvas.height);
  };

  /**
   * Vertically pack a box.
   *   @box: Ref. The box to remove from.
   *   @height: The height to pack.
   *   @rev: Reverse flag.
   *   &returns: The removed box.
   */
  window.Pack.vert = function(box, height, rev) {
    if(height > box.height) { height = box.height; }

    var y;
    if(!rev) {
      y = box.y;
      box.height -= height;
      box.y += height;
    } else {
      box.height -= height;
      y = box.height;
    }

    return Pack.box(box.x, y, box.width, height);
  };

  /**
   * Horizontally pack a box.
   *   @box: Ref. The box to remove from.
   *   @width: The width to pack.
   *   @rev: Reverse flag.
   *   &returns: The removed box.
   */
  window.Pack.horiz = function(box, width, rev) {
    if(width > box.width) { width = box.width; }

    var x;
    if(!rev) {
      x = 0;
      box.width -= width;
      box.x += width;
    } else {
      box.width -= width;
      x = box.width;
    }

    return Pack.box(x, box.y, width, box.height);
  };

  /**
   * Draw the box-rect using the context.
   *   @box: The box.
   *   @ctx: The context.
   */
  window.Pack.draw = function(box, ctx) {
    ctx.rect(box.x, box.y, box.width, box.height);
  };

  /**
   * Draw text using the box-rect.
   *   @box: The box-rect.
   *   @ctx: The context.
   *   @text: The text.
   *   @align: Optional. The alignment. Default is "start".
   */
  window.Pack.text = function(box, ctx, text, align) {
    if(align === undefined) {
      ctx.fillText(text, box.left, box.bottom);
    } else {
      ctx.textAlign = align;
      ctx.fillText(text, box.right, box.bottom);
      ctx.textAlign = "start";
    }
  };
})();
