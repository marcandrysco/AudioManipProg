Draw Toolkit
============

The draw toolkit provides a large set of functions for drawing using a canvas
and 2d drawing context.


## Packing

The packing functions provide a vairety of ways to compute layouts for
drawing. The base structure for packing elements is the box-rect object -- it
is a piece of structured data that specifies a bounding box. The packing
functions operating on box-rect objects and can generate futher box-rect
objects.

### Box-Rect

    Pack.box(x, y, width, height)
    Pack.rect(left, right, top, bottom)

The box-rect object is created using one of two constructors: `Pack.box` and
`Pack.rect`. The first function uses a pair of `x`, `y` coordinates and a
`width` and `height`. The second specifies all edges `left`, `right`, `top`,
and `bottom`. The returned box-rect contains all of these fields for
conveience.

    BoxRect.copy()
    BoxRect.inside(x, y)

The box-rect object carries two functions with it. The `copy` function
produces an identical copy of the box-rect. The `inside` function takes a pair
of coordinates and determines if they fall inside the box-rect.


## Drawing

    Draw.fill(box:BoxRect, ctx:Context, color:string)
    Draw.vert(box:BoxRect, x:int, ctx:Context, width:int, color:string)
