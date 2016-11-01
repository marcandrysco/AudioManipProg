GUI Toolkit
===========

The entirety of the GUI toolkit is contained by the files `gui.js` and
`gui.css`. All functionality is contained in under the namespace of `Gui`.


## Base functions


### Gui.byid

    Gui.byid(id)  // = document.getElementById(id)

Retrieve the element with the matching string identifier `id`. This is an
alias of `document.getElementById` only provided as a shorter, more convenient
form.

### Gui.clear

    Gui.clear(el:Node)

Clear all children elements of a Node in efficient manner.

### Gui.replace

    Gui.replace(parent:Node, child:Node)

Replace all children elements of the node `parent` with the single child
`child`. The function is equivalent to calling the sequence of functions
`Gui.clear` followed by `parent.appendChild(child)`.



## Creation functions


### Gui.text

    Gui.text(str)  // = document.createTextNode(str)

Create a text node consisting of the string `str`. This is an alias of
`document.createElement` only provided as a shorter, more convenient form.

### Gui.tag

    Gui.tag(tag, cls, child)

The `tag` function creates a DOM element with the given tag, class list, and
children. The `cls` or `child` arguments can be omitted using a null value.
Both the `cls` and `child` arguments may be an array indicating that the
elements should use the list of classes of list of children.

### Convenince constructors -- `Gui.div`, `Gui.span`

    Gui.div(cls, child)   // = Gui.tag("div", cls, child)
    Gui.span(cls, child)  // = Gui.tag("span", cls, child)

The `div` and `span` methods are a wrapper for creating `div` and `span` DOM
elements.

### Gui.button


## Widgets

### Gui.Toggle


### Gui.Button


## Standard Styles

### Interaction -- `gui-noselect`

The `gui-noselect` style prevents the user from selecting the target element.

### Visibility -- `gui-hide` `gui-noshow`

The `gui-hide` style completely hides an element using `display:none` so that
the element will not influence the flow of surrounding elements.

Te `gui-noshow` style sets an element to invisibile using `visibility:hidden`
so that the element occupies space without being displayed.
