Controller
==========


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

### Retrieval Functions -- `getHeight`

    Ctrl.getHeight(ctrl:Ctrl) : int

The `getHeight` function computes the total height of the canvas.

### Draw UI -- `draw`

    Ctrl.draw(ctrl:Ctrl) : void

The `draw` function redraws the content on the canvas element.
