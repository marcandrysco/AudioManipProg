JSON
====


## Validation and Conversion

### Integer Conversion

    bool json_int_get(struct json_t *json, int *out);
    bool json_int_range(struct json_t *json, int low, int high, int *out)

The `json_int` function verifies that the JSON value is an integer, e.g. a
whole valued floating-point number. The `json_int_range` function additionally
checks that the value falls in the requested range. The integer value is
optionally stored in `out`.
