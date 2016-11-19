(function() {
  "use strict";

  /*
   * Grid namespace
   */
  window.Grid = new Object();

  /**
   * Create the grid header.
   *   @box: The target box.
   *   @ctx: The context.
   *   @width: The width.
   *   @x: The starting x-coordinate.
   *   @nbeats: The number of beats.
   *   @ndivs: The number of divisions.
   */
  window.Grid.head = function(box, ctx, width, x, nbeats, ndivs) {
    ctx.save();
    Draw.clip(box, ctx);
    var bar = 1, width = (width + 1) * ndivs - 1;
    x = -x;
    while(x < box.width) {
      Draw.text(box, ctx, bar++, {x: (4 + x), y: 2, color: "#000", font: "16px sans-serif"});
      for(var i = 1; i < nbeats; i++) {
        x += width;
        Draw.vert(box, x++, ctx, 1, "#888");
        Draw.text(box, ctx, (i+1), {x: (4 + x), y: 2, color: "#000", font: "12px sans-serif"});
      }
      x += width;
      Draw.vert(box, x, ctx, 2, "#000");
      x += 2;
    }
    ctx.restore();
  };

  /**
   * Retrieve the x-coordinate.
   *   @loc: The location.
   *   @width: The width.
   *   @nbeats: The number of beats.
   *   @ndivs: The number of divisions.
   *   &returns: The x-coordinate.
   */
  window.Grid.getx = function(loc, width, nbeats, ndivs) {
    var barlen = (width + 1) * ndivs * nbeats + 1;
    var beatlen = (width + 1) * ndivs;

    return Math.round(barlen * loc.bar + beatlen * loc.beat);
  };

  /**
   * Retrieve the row.
   *   @height: The height.
   *   @y: The y-coordinate.
   *   @limit: The limit.
   *   &returns: The row or '-1'.
   */
  window.Grid.getRow = function(height, y, limit) {
    var row = Math.floor((y - height - 2) / (height + 1));
    
    return ((row >= 0) && (row < limit)) ? row : -1;
  };

  /**
   * Retrieve the location.
   *   @height: The width.
   *   @y: The x-coordinate.
   *   @nbeats: The number of beats.
   *   @ndivs: The number of divisions.
   *   @round: The rounding to div flag.
   *   &returns: The location or null.
   */
  window.Grid.getLoc = function(width, x, nbeats, ndivs, round) {
    var barlen = (width + 1) * ndivs * nbeats + 1;
    var beatlen = (width + 1) * ndivs;

    var bar = Math.floor(x / barlen);
    var div = (x - bar * barlen) / (width + 1);
    var beat = (round ? Math.floor(div) : div) / ndivs;
    if(beat >= nbeats) { bar++; beat = 0; }

    return Loc.make(bar, beat);
  };
})();
