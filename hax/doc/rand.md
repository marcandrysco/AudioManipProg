Random Numbers
==============

## Global Generator -- `m_rand`

    struct m_rand_t m_rand

The `m_rand` variable contains the state of the global random number
generator. For any generator functions `m_rand_#type#`, the generator argument
`rand` may be omitted in order to use the global generator.

The global generator is not thread-safe. Use by multiple threads should first
acquire a lock to prevent indefined behavior.


## Seeding -- `m_rand_seed` `m_rand_init`

    void m_rand_seed(uint32_t seed);

The `m_rand_seed` function seeds the global random number generator.

    struct m_rand_t m_rand_init(uint32_t seed);

The `m_rand_init` initializes a random number generated with the given `seed`.


## Number Generation -- `m_rand_u32` `m_rand_u64` `m_rand_d`

    uint32_t m_rand_u32(struct m_rand_t *rand);
    uint64_t m_rand_u64(struct m_rand_t *rand);

The `m_rand_u32` function generates a random, unsigned integer covering the
entire range of 32-bit integers from `0` to `2^32-1`.

The `m_rand_u64` function generates a random, unsigned integer covering the
entire range of 64-bit integers from `0` to `2^64-1`.

    double m_rand_d(struct m_rand_t *rand);

The `m_rand_d` function generates a uniformly distributed random double
between `0.0` and `1.0`, not including `1.0`.
