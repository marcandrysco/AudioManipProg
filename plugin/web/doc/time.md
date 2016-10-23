Time
====

## Javascript

The `Time` namespace is contains all functionality related with managing and
displaying the time.

### Public Fields -- `run` `loc` `nbeats`

    Time.run : bool
    Time.loc : Loc
    Time.nbeats : float;

The `run` flag indicates if the clock is currently running.

The `loc` field is set the current location and is updated as the display
refresh rate.

The `nbeats` field specifies the number of beats per measure as a
floating point number.

### Initialization and Update -- `init` `elem` `update`

    Time.init() : void
    Time.elem(data) : HTMLElement
    Time.update(data) : void

The `init` method initializes the `Time` namespace and sets all public
fields.

The `elem` function creates the HTML element for displaying the clock on the
page. The element is automatically updates itself with the current time when
`update` is called.

The `update` function is used to update the `Time` namespace and all HTML
elements with the current time.

### Events -- `keypress`

    Time.keypress(e: Event) : void

The `keypress` handles a global keypress event.

### Popup -- `popup`

   Time.popup() : HTMLElement

The `popup` function creates the time dialog popup element. The returned
element should be appended to the document `body`. The dialog will
automatically handle cleanup when it is closed.

### Miscellaneous -- `string`

    Time.string(loc:Loc) : String

Convert a location `loc` into a string format amenable to being displayed as a
time. The returned string always has bar with four digits using leading zeros
and a decimal beat with a single integer and fractional digit.
