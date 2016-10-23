Sockets
=======


## Base Functionality

### Types -- `sys_fd_t`

    sys_fd_t

The `sys_fd_t` type is the same type used for low-level file desriptor, and it
may be implemented as either a native type or a structure.

    bool sys_isfd(sys_fd_t fd)

The `sys_isfd` function checks if `fd` is a valid file descriptor or socket.

### Connections -- `sys_socket` `sys_connect` `sys_closesocket`

    char *sys_socket(sys_fd_t *fd, int af, int type, int prot)
    char *sys_connect(sys_fd_t *fd, int type, const char *host, uint16_t port)
    void sys_closesocket(sys_fd_t fd)

### Read/Write -- `sys_recv` `sys_send`

    ssize_t sys_recv(sys_fd_t fd, void *buf, size_t nbytes, int flags);
    ssize_t sys_send(sys_fd_t fd, const void *buf, size_t nbytes, int flags);

### Server Functions -- `sys_bind` `sys_listen` `sys_accept`

    char *sys_bind(sys_fd_t fd, void *addr, fdlen_t len);
    char *sys_listen(sys_fd_t fd, int backlog);
    char *sys_accept(sys_fd_t fd, sys_fd_t *client, struct sockaddr *addr, socklen_t *len);

### Miscellaneous -- `sys_setsockopt`

    char *sys_setsockopt(sys_fd_t fd, int level, int opt, const void *val, socklen_t len);
