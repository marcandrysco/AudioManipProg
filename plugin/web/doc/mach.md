Machine
=======

## Javascript Fields


## Javascript Methods

This section covers the primary machine Javascript methods.

### Initialization and Update -- `elem` `update`

    Mach.elem(mach:Mach, idx:int) : DOMElement

The `elem` function initializes the machine data `mach` and creates the
interactive HTML element. The `idx` parameter is the index used to make
requests to update the server-side machine.

    Mach.update(machine:Mach, data:Object) : void

The `update` method updates the machine UI elements. The `data` object is
unused by a machine. The machine updates the UI according to the new time.

### Actions -- `set` `toggle` `conf`

    Mach.set(mach:Mach, sel:int, idx:int, off:int, key:int, vel: int) : void

The `set` method sets the key-velocity pair at the desired location. A
velocity of `0` implicitly removes the event.

    Mach.toggle = function(mach:Mach, idx:int, on:bool)

The `toggle` method sets the `on` or enable flag of an instance.

    Mach.reconf = function(mach:Mach, idx:int, type:string, rel:bool, dev:int, key:int)

The `reconf` method reconfigures an instance with new parameters.

### Information Retrieval -- `getOff`

    Mach.getOff(mach:Mach, bar:int, beat:int, div:int) : int

The `getOff` computes the offset for a pad given the bar, beat, and division.

    Mach.getEvent(mach:Mach, sel:int, idx:int, off:int) : Event
    Mach.getVel(mach:Mach, sel:int, idx:int, off:int) : int

The `getEvent` method retrieves the event on preset `sel`, instance `idx`, and
offset `off`. If no such event exists, `getEvent` returns `null`.

The `getVel` method retrives the velicty of the event on preset `sel`,
instance `idx`, and offset `off`. If no such event exists, `getVel` returns
`0`.

    Mach.getColor(vel:int) : string

The `getColor` method computes the CSS color string for a given velocity and
key.
