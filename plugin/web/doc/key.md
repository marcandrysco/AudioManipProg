Keys
====


## Methods

    Key.letter(key:Int)
    Key.str(key:Int)

The `letter` takes a key value and returns the associated letter. The `str`
method takes a key and generates the full string as the letter folled by the
octave.
  
    Key.parse(str:String)

The `parse` method takes a string and parses the key from it. If the string is
not a valid key, the method returns null.
