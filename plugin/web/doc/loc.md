Locations
=========


## Structure

    bar  : Int
    beat : Float

The location structure consists of two fields, `bar` and `beat`. Both are
nonnegative.

## Methods

    Loc.copy(loc:Loc)

The `copy` method performs a deep copy of the locations.

    Loc.order(left:Loc, right:Loc)
    Loc.reorder(left:Loc, right:Loc)

The `order` method checks if the locations `left` and `right` are ordered
correctly. The `reorder` method automatically orders the two locations.
