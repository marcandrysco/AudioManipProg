Locations
=========


## Structure

    bar  : Int
    beat : Float
    copy : function

The location structure consists of two fields, `bar` and `beat`. Both are
nonnegative.

The `copy` mothod creates a copy of the location.

## Methods

    Loc.make(bar:Int, beat:Float)
    Loc.makef(bar:Float, nbeats:Float)

The `make` method directly create a location using a bar and beat. The
parameter `bar` must be a signed integer, and `beat` must be a nonnegative
float. Although not enforced by this method, the `beat` parameter should be
strictly less than the number of beats in a measure.

The `makef` method directly creates a location using a floating-point bar and
number of beats per measure. The

    Loc.parse(str:String, nbeats:Float)

The `parse` function parses a string to create a location. The string may take
the form of a single floating-point number represting the number of beats, or
the string may take the form of `bar:beat` where `bar` is an integer and
`beat` is a nonnegative floating-point number strictly less than `nbeats`.

    Loc.order(left:Loc, right:Loc)
    Loc.reorder(left:Loc, right:Loc)

The `order` method checks if the locations `left` and `right` are ordered
correctly.

The `reorder` method modifies the `left` and `right` locations to make sure
they are ordered.
