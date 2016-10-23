HTTP
====

## Data Structures

### Header -- `struct http_head_t` `struct http_pair_t`

    struct http_head_t {
      char *verb, *path, *proto;
      struct http_pair_t *pair;
    };

The `http_head_t` structure contains all header information from a web
request. The `verb` field specifies the request verb, usually `GET` or `POST`.
The `path` specifies the requested file path always starting with `/`. The
`proto` field specifies the protocol, almost always `HTTP/1.1`. The pair field
is a linked list of all header key-value pairs.

    struct http_pair_t {
      char *key, *value;
      struct http_pair_t *next;
    };

The `http_pair_t` structure contains a single key-value pair and a pointer to
the next header pair. The `key` and `value` are simple, null-terminated
strings. The `next` field points to the next key-value pair or null if at the
end of the header list.

### Arguments -- `struct http_args_t`

    struct http_args_t {
      struct io_file_t file;
      const char *body;
      struct http_head_t req, resp;
    };

The `http_args_t` structure is the primary structure use to handle HTTP
requests. The `file` is used to output a response body to the client -- i.e.,
use `file` to write your HTML, Javascript, or other web file. The `body`
contains the body of the request. The `req` and `resp` fields contain the
request and response headers. Once handling the request is complete, the
`resp` headers are automatically sent to the client.


## Functions

### Asynchrounous (Simple) Server -- `http_server_async`

    struct sys_task_t *http_server_async(uint16_t port, http_handler_f func, void *arg)

The `http_serv_async` function starts an HTTP server running on port number
`port`. The server runs on an independent thread managed by the returned task.
All requests are processed by the handler function `func`.

    bool (*http_handler_f)(const char *path, struct http_args_t *args, void *arg)


## Asset Helpers

The asset helper function `http_asset_proc` and associated data structure
`struct http_asset_t` provide a convenient, prebuilt method for delivering
files from disk.

### Asset Structure -- `struct http_asset_t`

    struct http_asset_t {
      const char *req, *path, *type;
    };

The asset structure provides a mapping between request strings, asset paths,
and the asset content type. The `req` field specifies the request path for a
given asset. The `path` field specifies the path to the asset. The `type`
field specifies the content type (or MIME type) of the asset.

### Asset Processing -- `http_asset_proc`

    bool http_asset_proc(struct http_asset_t *assets, const char *req, struct http_args_t *args, const char *prefix)

The `http_asset_proc` function attempts to match the incoming request `req`
with an asset from the `assets` array. The `assets` parameter should point to
an array of `struct http_asset_t` structures delimited by a structure
containing a `null` request field. If the request matches an asset, the
function reads the path specified by `prefix + assets[i]->path` to read the
asset. The `prefix` may be set to `null` if unneeded.
