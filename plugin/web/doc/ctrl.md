Controller
==========


## Javascript Methods

This section covers the Javascript methods for the controller. All methods
fall under the `Ctrl` namspace.

### Initialization and Update -- `elem` `update`

    Ctrl.elem(ctrl:Ctrl, idx:int) : DOMElement

The `elem` function initializes the controller data `ctrl` and creates the
interactive HTML element. The `idx` parameter is the index used to make
requests to update the server-side ctrl.

    Ctrl.update(ctrl:Ctrl, data:Object) : void

The `update` function is called when the server provides update information.
