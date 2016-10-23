File
====


### File Interface

    typedef size_t (*io_read_f)(void *ref, void *buf, size_t nbytes)
    typedef size_t (*io_write_f)(void *ref, const void *buf, size_t nbytes)
    typedef void (*io_close_f)(void *ref)

### File Functions

    size_t io_file_read(struct io_file_t file, void *buf, size_t nbytes)
    size_t io_file_write(struct io_file_t file, const void *buf, size_t nbytes)
    void io_file_close(struct io_file_t file)


## File Implementers

### Length File -- `io_file_len`

    struct io_file_t io_file_len(size_t *nbytes)

The `io_file_len` function creates a file that only tracks the number of bytes
written to it. The reference `nbytes` is incremented every time a write occurs
on the file.

### String Accumulator -- `io_file_accum`

    struct io_file_t io_file_accum(char **str)

### File Wrapper -- `io_file_wrap`

    struct io_file_t io_file_wrap(FILE *file)

### File Descriptor Wrapper -- `io_file_fd`

    struct io_file_t io_file_fd(sys_fd_t fd)

### Strin Reader -- `io_file_str`

    struct io_file_t io_file_str(const char *str)

### Buffer Writer -- `io_file_buf`

    struct io_file_t io_file_buf(void **ptr, size_t *nbytes
