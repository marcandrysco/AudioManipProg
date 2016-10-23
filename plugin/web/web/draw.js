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
      box.top += height;
    } else {
      box.height -= height;
      box.bottom -= height;
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
      x = box.x;
      box.width -= width;
      box.x += width;
      box.left += width;
    } else {
      box.width -= width;
      box.right -= width;
      x = box.width;
    }

    return Pack.box(x, box.y, width, box.height);
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

  /**
   * Draw the box-rect using the context.
   *   @box: The box.
   *   @ctx: The context.
   */
  window.Pack.draw = function(box, ctx) {
    ctx.rect(box.x, box.y, box.width, box.height);
  };


  /*
   * Draw namespace
   */
  window.Draw = new Object();

  /**
   * Draw the box-rect using the context.
   *   @box: The box.
   *   @ctx: The context.
   */
  window.Draw.fill = function(box, ctx, color) {
    if(color !== undefined) { ctx.fillStyle = color; }
    ctx.beginPath();
    ctx.rect(box.x, box.y, box.width, box.height);
    ctx.fill();
  };

  /**
   * Draw the box-rect using the context.
   *   @box: The box.
   *   @ctx: The context.
   */
  window.Draw.clip = function(box, ctx) {
    ctx.beginPath();
    ctx.rect(box.x, box.y, box.width, box.height);
    ctx.clip();
  };

  /**
   * Draw a vertical line in a box-rect.
   *   @box: The box.
   *   @x: The x-coordinate.
   *   @ctx: The context.
   *   @width: The line width.
   *   @color: The line color.
   */
  window.Draw.vert = function(box, x, ctx, width, color) {
    if(color !== undefined) { ctx.fillStyle = color; }
    ctx.beginPath();
    ctx.rect(box.x + x, box.y, width, box.height);
    ctx.fill();
  };

  /**
   * Draw a horizontal line in a box-rect.
   *   @box: The box.
   *   @y: The y-coordinate.
   *   @ctx: The context.
   *   @width: The line width.
   *   @color: The line color.
   */
  window.Draw.horiz = function(box, y, ctx, width, color) {
    if(color !== undefined) { ctx.fillStyle = color; }
    ctx.beginPath();
    ctx.rect(box.x, box.y + y, box.width, width);
    ctx.fill();
  };

  /**
   * Draw text in a box-rect.
   *   @box: The box-rect.
   *   @ctx: The context.
   *   @text: The text.
   *   @opt: Optional. The options.
   */
  window.Draw.text = function(box, ctx, text, opt) {
    if(typeof opt != "object") { opt = new Object(); }
    if(opt.color) { ctx.fillStyle = opt.color; }
    if(opt.font) { ctx.font = opt.font; }
    if(opt.align == "right") {
      ctx.textAlign = "right";
      ctx.fillText(text, box.right - opt.x, box.bottom - opt.y);
      ctx.textAlign = "start";
    } else {
    ctx.fillText(text, box.left + opt.x, box.bottom - opt.y);
    }
  };
})();
