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


## Key-Value Pairs

### Key-Value Pair Structure -- `struct http_pair_t`

    struct http_pair_t {
      char *key, *value;

      struct http_pair_t *next;
    };

### Create and Delete -- `http_pair_new` `http_pair_clear`

    struct http_pair_t *http_pair_new(char *key, char *value);
    void http_pair_clear(struct http_pair_t *pair);

The `http_pair_new` function creates a new pair with consuming the `key` and
`value` strings. The `next` field is set to `null`, so that the returned pair
is a single element in a list.

### List Information -- `http_pair_len`

    unsigned int http_pair_len(struct http_pair_t *pair);

The `http_pair_len` function computes the length of the pair list by
traversal.

### Searching -- `http_pair_find` `http_pair_get`

    struct http_pair_t **http_pair_find(struct http_pair_t **pair, const char *key);
    const char *http_pair_get(struct http_pair_t **pair, const char *key);

The `http_pair_find` function finds a reference to the pair corresponding to
the given `key`. If no pair matches the key, the function returns `null`.

The `http_pair_get` directly retrieves the value corresponding to the `key`.
If no pair matches the key, the function returns `null`.

### Formatted Get -- `http_pair_getf`

    char *http_pair_getf(struct http_pair_t *pair, const char *restrict fmt, ...)

The `http_pair_getf` retrieves values from the pair list `pair` according to
the format string `fmt`. The function returns the string error on failure.

The format string `fmt` consists of a list of specifications separated by
whitespace or commas. The specifier takes the form `key:type` where `key` is
the string key and `type` is the one letter type.

    http_pair_get(pair, "user:s level:d $");


## Cookie Helpers

    char *http_cookies_string(struct http_pair_t *pair);
    struct http_pair_t *http_cookies_parse(const char *str);

    void http_cookie_sanitize(char *str);


## Form Helpers

The form helper functions simplify working with url encoded form data.

### Parsing -- `http_form_parse`

    char *http_form_parse(struct http_pair_t **pair, const char *str);

The `http_form_parse` function takes a form as input `str` encoded with type
`application/x-www-form-urlencoded` and converts it to a list of key value
pairs stored in `pair`.


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
