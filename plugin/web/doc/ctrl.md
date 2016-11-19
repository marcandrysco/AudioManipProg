Controller
==========


## Operating Modes

The controller uses the enable and record flags to change its mode of
operation. When disabled, the controller performs no actions and discards any
events or input. When enabled, the controller may either be in play or record
mode. In both modes, scheduled events and user actions are played to the output.
Only in record mode, user actions are recorded to the schedule. The record
mode cannot be toggled while AmpCore is running.

### Locking

There are two mutexes used by the controller: `sync` and `lock`. The `sync` or
synchronization mutex regulates control to the run flag, only held when
starting, stopping, or writing data to the event queue. The `lock` mutex
regulates access to the run-time data, being held when processing sequencer
events or held for the entire duration that the controller is disabled.


### Disabled

### Playing

### Recording


## Javascript Methods

This section covers the Javascript methods for the controller. All methods
fall under the `Ctrl` namspace.

### Initialization and Update -- `elem` `conf` `update`

    Ctrl.elem(ctrl:Ctrl, idx:int) : DOMElement

The `elem` function initializes the controller data `ctrl` and creates the
interactive HTML element. The `idx` parameter is the index used to make
requests to update the server-side ctrl.

    Ctrl.conf(ctrl:Ctrl, idx:int) : DOMElement

The `conf` function creates the popup DOM element for configuring the a
specific control.

    Ctrl.update(ctrl:Ctrl, data:Object) : void

The `update` function is called when the server provides update information.

### Handlers -- `mouse`

    Ctrl.mouse(ctrl:Ctrl) : EventListener

The `mouse` function creates the event listener for mouse events on the
canvas.

### Actions -- `remove`

### Retrieval Functions -- `getHeight`

    Ctrl.getHeight(ctrl:Ctrl) : int

The `getHeight` function computes the total height of the canvas.

### Draw UI -- `draw`

    Ctrl.draw(ctrl:Ctrl) : void

The `draw` function redraws the content on the canvas element.
