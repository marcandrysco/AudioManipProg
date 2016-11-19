Server
======


## Locking

The server uses a synchornization mutex `sync` and a run mutex `lock`. The
`sync` mutex is held whenever manipulating the client list. The run `lock` is
held when either processing data on an instance or modifying the instance
list.
