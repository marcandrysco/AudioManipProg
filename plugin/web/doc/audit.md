Controller
==========


### Create and Update -- `elem` `update`

    Audit.elem(audit:Audit, idx:int) : DOMElement

The `elem` function initializes the audit data `audit` and creates the
interactive HTML element. The `idx` parameter is the index used to make
requests to update the server-side audit.

    Audit.update(audit:Audit, data:Object) : void
