Player
======

## Methods

    draw(player:Player)

Redraws the player using the canvas.

### Coordinate Retrieval -- `getRow` `getLoc` `getDat`

    getRow(player:Player, y:Int) : Int
    getLoc(player:Player, x:Int, round:Bool) : Loc
    getDat(player:Player, x:Int, y:Int) : Dat

The `getRow` function retrieves the row index given a y-coordinate. If no row
exists at the coordinate, `getRow` returns `-1`.

The `getLoc` function retrieves the `Loc` object for a given x-coordinate. If
no location exists at the coordinate, `getLoc` returns `null`. The optional
`round` flag specifies whether or not the location should be rounded to the
nearest division.

The `getDat` function retrieves the data given a pair of coordinates. If not
data exists at the coordinates, `getDat` returns `null`.

### Miscellaneous -- `sort`

    sort(keys:Array)

The `sort` method takes an array of keys and returns the sorted array.
