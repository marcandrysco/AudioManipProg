JSON
====


## Validation and Conversion

The validation and converion functions take the form `json_get_(type)` where
`(type)` is a C type. Each function returns `true` if the JSON value
corresponds to the `type`. Additionally, the converted value may be stored
in the `out` parameter if not null.

### Number Conversion

    bool json_get_double(struct json_t *json, double *out);

The `json_get_double` function verifies that the value is a number and
optionally writes the number to `out`.

    bool json_get_int(struct json_t *json, int *out);
    bool json_get_uint(struct json_t *json, unsigned int *out);
    bool json_get_int16(struct json_t *json, int16_t *out);
    bool json_get_uint16(struct json_t *json, uint16_t *out);

The integer functions `json_get_int#` or `json_get_uint#` verify that the
number is exactly an integer and fits into the requested bit width. The
integer is optionally written to `out`.
