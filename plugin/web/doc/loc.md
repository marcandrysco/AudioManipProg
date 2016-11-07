Locations
=========

Locations are used to describe instances in time during a song. Matching
standard music notation, times consist of a whole numbered bar and a
fractional beat. For example, the string `4:1.5` refers to the 4th bar (or
measure) and beat 1.5 (or 1 and 1/2). 


## Structure

    bar  : int
    beat : float
    copy : function(void) : Loc

The location structure consists of two fields, `bar` and `beat`, of which the
beat must be nonnegative.

The `copy` mothod creates a deep copy of the location.


## Methods

### Creation -- `make` `makef`

    Loc.make(bar:int, beat:float) : Loc
    Loc.makef(bar:float, nbeats:float) : Loc

The `make` method directly create a location using a bar and beat. The
parameter `bar` must be a signed integer, and `beat` must be a nonnegative
float. Although not enforced by this method, the `beat` parameter should be
strictly less than the number of beats in a measure.

The `makef` method directly creates a location using a floating-point bar and
number of beats per measure.

### Arithmetic -- `add`

    Loc.add(loc:Loc, beat:float, nbeats:float)

The `add` method adds `beat` number of beats to the location `loc`, returning
the new location. The `nbeats` parameter is used to correctly compute the bar
and adjust the returning location so that the beat is at least `0` but no
more than `nbeats`.

### Conversion -- `bar`

    Loc.bar(loc:Loc, nbeats:float) : float

The `bar` method converts a location to a floating point bar. The floating
point format is ideal for performing arithmetic.

### Parsing -- `parse`

    Loc.parse(str:String, nbeats:Float)

The `parse` function parses a string to create a location. The string may take
the form of a single floating-point number represting the number of beats, or
the string may take the form of `bar:beat` where `bar` is an integer and
`beat` is a nonnegative floating-point number strictly less than `nbeats`.

### Ordering -- `order` `reorder`

    Loc.order(left:Loc, right:Loc)
    Loc.reorder(left:Loc, right:Loc)

The `order` method checks if the locations `left` and `right` are ordered
correctly.

The `reorder` method modifies the `left` and `right` locations to make sure
they are ordered.
