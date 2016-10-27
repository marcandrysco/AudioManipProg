Player
======

## Javascript Fields

### Interaction -- `active` `sel` `area`

    Player.sel : Array<Player.Data>
    Player.active : Player.Data
    Player.area : { begin:{ x:int, y:int }, end:{ x:int, y:int } }

The `active` field contains the data point that is currently being added by
mouse interactions. When no insert is happening, the field is set to `null`.

The `sel` field contains an array of all selected data points.

The `area` field contains the pair of coordinates denoting the selection box
when the user clicks and drags.

### Position -- `x`

### Velocity -- `vel`

### Button Elements -- `keys` `insert` `delete` `copy`

    Player.keys : HTMLElement
    Player.insert : HTMLElement
    Player.delete : HTMLElement
    Player.copy : HTMLElement

The `keys` field refers to the `Button` that displays the key selection
dialog.

The `insert` field refers to the `Button` that modifies the current action of
either `select`, `insert`, or `delete`.


## Javascript Methods

This section covers the primary player Javascript methods.

### Initialization and Update -- `init` `elem` `update`

    Player.elem(player:Player, idx:int) : DOMElement
    Player.update(player:Player) : void

The `elem` function initializes the player data `player` and creates the
interactive HTML element. The `idx` parameter is the index used to make
requests to update the server-side player.

### Draw -- `draw`

    Player.draw(player:Player) : void

Redraws the player using the canvas.

### Events -- `mouse` `keypress`

    Player.mouse(player:Player, e:event) : void
    Player.keypress(player:Player, e:event) : void

The `mouse` function directly handles mouse events on the player.

The `keypress` function handles global keypress events that are centrally
dispatched.

### Actions -- `add` `remove` `move` `past` `undo` `redo`

    Player.add(player:Player, dat:Dat) : void
    Player.remove(player:Player) : void
    Player.move(player:Player, dir:string, sm:float) : void
    Player.add(player:Player, add:List<Dat>) : void

The `add` function adds a single data point `dat`. An undo will remove the
data point.

The `remove` function removes all selected data points. An undo will readd the
removed data point.

The `move` function moves all selected data points in the specified direction.
The direction `dir` must be set to `up`, `down`, `left`, or `right`. The `sm`
parameter is optional and specifies the amount to move left or right as a
multiple of divisions; if omitted, the default is `1`.

The `paste` function pastes (or adds) as the set of data from the `add`
parameter. All data will be removed or added simultaneous from an undo or a
redo.

### Information Retrieval -- `getRow` `getLoc` `getDat` `getActive`

    Player.getRow(player:Player, y:int) : int
    Player.getLoc(player:Player, x:int, round:bool) : Loc
    Player.getDat(player:Player, begin:Coord, end:Coord) : Array<Dat>
    Player.getActive(player:Player, row:bool) : Dat

The `getRow` function retrieves the row index given a y-coordinate. If no row
exists at the coordinate, `getRow` returns `-1`.

The `getLoc` function retrieves the `Loc` object for a given x-coordinate. If
no location exists at the coordinate, `getLoc` returns `null`. The optional
`round` flag specifies whether or not the location should be rounded to the
nearest division.

The `getDat` function retrieves the an array of all data laying under the area
covered by the `begin` and `end` coordinates. Each pair of coordinates is an
object consisting of the fields `x` and `y` which correspond to piano roll
coordinates.

The `getActive` function retrieves the current active data point. If there is
no active data, the function returns `null`. The `row` flag determines whether
or not the row index should be included -- the row must be omitted when
performing a web request.

### Miscellaneous -- `sort` `copy`

    Player.sort(keys:Array<int>) : Array<Int>
    Player.copy(dat:Dat, zero:bool) : Dat

The `sort` method takes an array of keys and returns the sorted array.

The `copy` method performs a deep copy of data point. The optional `zero` flag
indicates that the velocity should be set to zero.

### Data Mapping -- `mapKey` `mapBegin`

The `mapKey` function takes a list of data points and maps each point to the
row corresponding to the key. If the key does not map to a row, it is filtered
and omitted from the returned list.

The `mapBegin` function takes a list of data points and maps each point to the
beginning location expressed in bars as a single floating point value.
