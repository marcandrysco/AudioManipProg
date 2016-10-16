Memory Management
=================

The memory management function cover the standard functions and the additional
support cHax provides for tracking memory usage.


## Memory Tracking

The cHax library adds a shim to memory management functions to track the
amount of memory in use in order to aid finding memory leaks. The shim uses a
counter that is incremented for every allocation and decremented for every
free. Developers can check the count at the very end of a program to verify
that all resources were freed.


### Allocation Counter

    int hax_memcnt = 0;

The `hax_memcnt` variable tracks the number of allocations at a given time,
and is initialized to zero at program start.

    void hax_inc(void);
    void hax_dec(void);

The `hax_inc` and `hax_dec` functions increments and decrement the memory
counter in a thread-safe manner. You should not write to `hax_memcnt`
directory, although it is safe to read the value.

The `hax_%act%` functions adjust the counter automatically. It is useful to
check that `hax_memcnt` is exactly zero before exitting the program to verify
there are no memory leaks.


### Basic Allocation

    void *hax_alloc(size_t len);
    void *hax_realloc(void *ptr, size_t len);
    void hax_free(void *ptr);

The `hax_alloc` function allocates at least `len` bytes and increments the
memory counter.

The `hax_realloc` function adjusts the size of the memory pointed to by `ptr`.
If the pointer is null, the function allocates `len` bytes and increments the
memory counter.

The `hax_free` function frees the memory pointed to by `ptr` and decrements
the memory counter.


### Extra Functions

    void *hax_strdup(const char *str);

The `hax_strdup` function duplicates the string `str` and increments the
memory counter.


### Macro Renaming

The cHax library automatically renames all the functions to remove the prefix
`hax_` whenever the `DEBUG` flag is set. Therefore, a call to `malloc` will
automatically be changed to a call to `hax_malloc`. Omitting the `DEBUG` flag
will prevent renaming these memory management functions.


## Stanadard Functions

The functions for managing memory closely mimic the standard C library with a
few small additions.

### Allocation and Free

    void *malloc(size_t nbytes);
    void *realloc(size_t nbytes);

    void free(size_t nbytes);
    void erase(size_t nbytes);
