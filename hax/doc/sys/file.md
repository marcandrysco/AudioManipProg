Files
=====


## File Interface

    sys_fd_t

All files are managed using file descriptors with the type `sys_fd_t`. The
type is platform dependent and may be defined as either a native type or a
structure type.

### Open and Close -- `sys_open` `sys_close`

    void sys_close(sys_fd_t fd);

### Invalid Descriptors -- `sys_badfd` `sys_isfd`

    const sys_fd_t sys_badfd

The `sys_badfd` variable represents an invalid file descriptor. The value can
be assigned to a file descriptor to make it invalid, i.e. `myfd = sys_badfd`.
The `sys_badfd` value can be thought of as a `null` for descriptors.

    bool sys_isfd(sys_fd_t fd)

The `sys_isfd` function check if the file desciptor is valid. The check does
not verify that the desciptor works (e.g. if it was closed); instead, it only
checks against the `sys_badfd` value.
