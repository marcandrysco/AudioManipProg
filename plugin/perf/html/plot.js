(function() {
	'use strict';

  /*
   * plot namespace
   */
  window.Plot = new Object();

  /**
   * List of linear points.
   *   @low: The low point.
   *   @high: The high point.
   *   @step: The step size.
   *   &returns: The list of points.
   */
	window.Plot.list = function(low, high, step) {
		var list = new Array();

		for(var v = low; v <= high; v += step)
			list.push(v);

		return list;
	};

	var ticks = [];
	for(var j = 0; j < 6; j++) {
		for(var i = 1; i <= 10; i++) {
			ticks.push(i * Math.pow(10, j));
		}
	}

  /**
   * Map an x-coordinate.
   *   @ctx: The context.
   *   @x: The input x-coordinate.
   *   &returns: The output x-coordinate.
   */
	window.Plot.mapX = function(ctx, x) {
    switch(ctx.xscale) {
    case "lin": x = Plot.calcLin(x, ctx.xmin, ctx.xmax); break;
    case "exp": x = Plot.calcExp(x, ctx.xmin, ctx.xmax); break;
    default: throw "Invalid x-scaling";
    }
		return Math.round(x * ctx.canvas.width) + 0.5;
	};

  /**
   * Map an y-coordinate.
   *   @ctx: The context.
   *   @x: The input y-coordinate.
   *   &returns: The output y-coordinate.
   */
	window.Plot.mapY = function(ctx, y) {
    switch(ctx.yscale) {
    case "lin": y = Plot.calcLin(y, ctx.ymin, ctx.ymax); break;
    case "exp": y = Plot.calcExp(y, ctx.ymin, ctx.ymax); break;
    default: throw "Invalid y-scaling";
    }
		return ctx.canvas.height - Math.round(y * ctx.canvas.height) - 0.5;
	};

  /**
   * Calculate a linearly extrapolated value.
   *   @v: The value.
   *   @min: The minimum.
   *   @max: The maximum.
   */
	window.Plot.calcLin = function(v, min, max) {
		return (v - min) / (max - min);
	};

  /**
   * Calculate an exponentially extrapolated value.
   *   @v: The value.
   *   @min: The minimum.
   *   @max: The maximum.
   */
	window.Plot.calcExp = function(v, min, max) {
		return Math.log(v / min) / Math.log(max / min);
	};

	var calcDb = function(v) {
		return 20 * Math.log(v) / Math.log(10);
	};

	var dot = function(ctx, pt) {
		ctx.beginPath();
		ctx.moveTo(pt.x, pt.y-3);
		ctx.lineTo(pt.x, pt.y+3);
		ctx.moveTo(pt.x-3, pt.y);
		ctx.lineTo(pt.x+3, pt.y);
		ctx.stroke();
	};

	var drawXTicks = function(ctx, min, max) {
		for(var i = 0; i < ticks.length; i++) {
			var x = Plot.mapX(ctx, calcExp(ticks[i], min, max));

			ctx.beginPath();
			ctx.moveTo(x, ctx.canvas.height);
			ctx.lineTo(x, ctx.canvas.height-5);
			ctx.stroke();
		}
	};

	var drawYTicks = function(ctx, list, map) {
		if(!map) { map = function(v) { return v; }; }

		for(var i = 0; i < list.length; i++) {
			var y = Plot.mapY(ctx, map(list[i]));

			ctx.beginPath();
			ctx.moveTo(0, y);
			ctx.lineTo(5, y);
			ctx.moveTo(ctx.canvas.width, y);
			ctx.lineTo(ctx.canvas.width-5, y);
			ctx.stroke();
		}
	};

	var drawYLines = function(ctx, list, map) {
		if(!map) { map = function(v) { return v; }; }

		ctx.strokeStyle = "#ccc";

		for(var i = 0; i < list.length; i++) {
			var y = Plot.mapY(ctx, map(list[i]));

			ctx.beginPath();
			ctx.moveTo(0, y);
			ctx.lineTo(ctx.canvas.width, y);
			ctx.stroke();
		}

		ctx.strokeStyle = "#000";
	};

	var drawXLines = function(ctx, min, max) {
		ctx.strokeStyle = "#ccc";

		for(var i = 0; i < ticks.length; i++) {
			var x = Plot.mapX(ctx, calcExp(ticks[i], min, max));

			ctx.beginPath();
			ctx.moveTo(x, 0);
			ctx.lineTo(x, ctx.canvas.height);
			ctx.stroke();
		}

		ctx.strokeStyle = "#000";
	};

	/**
	 * Draw a dot a given coordinates.
	 *   @ctx: The context.
	 *   @x: The x-coordinate.
	 *   @y: The y-coordinate.
	 */
	window.Plot.drawPoint = function(ctx, x, y) {
    x = Plot.mapX(ctx, x);
    y = Plot.mapY(ctx, y);

		ctx.ctx.beginPath();
		ctx.ctx.moveTo(x, y-2.5);
		ctx.ctx.lineTo(x, y+2.5);
		ctx.ctx.moveTo(x-2.5, y);
		ctx.ctx.lineTo(x+2.5, y);
		ctx.ctx.stroke();
	};

  window.Plot.drawXTick = function(ctx, x) {
    x = Plot.mapX(ctx, x);

		ctx.ctx.beginPath();
		ctx.ctx.moveTo(x, 0);
		ctx.ctx.lineTo(x, 4);
		ctx.ctx.moveTo(x, ctx.canvas.height);
		ctx.ctx.lineTo(x, ctx.canvas.height - 4);
		ctx.ctx.stroke();

		ctx.ctx.beginPath();
		ctx.ctx.moveTo(x, 0);
		ctx.ctx.lineTo(x, ctx.canvas.height);
    ctx.ctx.strokeStyle = '#ccc';
		ctx.ctx.stroke();
    ctx.ctx.strokeStyle = '#000';
  };

  window.Plot.drawYTick = function(ctx, y) {
    y = Plot.mapY(ctx, y);

		ctx.ctx.beginPath();
		ctx.ctx.moveTo(0, y);
		ctx.ctx.lineTo(4, y);
		ctx.ctx.moveTo(ctx.canvas.width, y);
		ctx.ctx.lineTo(ctx.canvas.width - 4, y);
		ctx.ctx.stroke();

		ctx.ctx.beginPath();
		ctx.ctx.moveTo(0, y);
		ctx.ctx.lineTo(ctx.canvas.width, y);
    ctx.ctx.strokeStyle = '#ccc';
		ctx.ctx.stroke();
    ctx.ctx.strokeStyle = '#000';
  };

  /**
   * Refresh the size of a canvas.
   *   @canvas: The canvas.
   */
  window.Plot.refresh = function(canvas) {
    if(canvas.width != canvas.offsetWidth) { canvas.width = canvas.offsetWidth; }
    if(canvas.height != canvas.offsetHeight) { canvas.height = canvas.offsetHeight; }
	  canvas.getContext("2d").clearRect(0, 0, canvas.width, canvas.height);
  };

  /**
   * Create a plot context.
   *   @ctx: The canvas context.
   *   &returns: The plot context.
   */
  window.Plot.context = function(ctx) {
    return {
      xmin: 0,
      xmax: 1000,
      xscale: "lin",
      ymin: 0,
      ymax: 1000,
      yscale: "lin",
      ctx: ctx,
      canvas: ctx.canvas
    };
  };
})();
