/* Cache geometry and policy inference using only the public probe API. */
#include "solver.h"

static int all_resident(int count, uint32_t stride)
{
    int ok = 1;
    cache_flush();
    for (int i = 0; i < count; ++i) cache_load((uint32_t)i * stride);
    for (int i = 0; i < count; ++i)
        if (!cache_load((uint32_t)i * stride)) ok = 0;
    return ok;
}

void solve(void)
{
    int b = 0, s = 0, E = 1;
    int lru = 1, fifo = 1, policy = POLICY_LRU;
    cache_flush();
    cache_load(0);
    while (b < 7 && cache_load(1u << b)) ++b;

    /* A stride of 2^(b+9) maps to set zero for every permitted geometry.
       An all-hit second pass is possible exactly when count <= E. */
    uint32_t stride = 1u << (b + 9);
    while (E < 17 && all_resident(E + 1, stride)) ++E;

    /* E+1 blocks fit when the stride still distributes them across sets. */
    while (s < 9 && all_resident(E + 1, 1u << (b + s))) ++s;

    cache_flush();
    cache_store(0);
    int write_allocate = cache_load(0);

    if (E > 1) {
        /* Two independent experiments per round. FIFO evicts 0 and LRU
           evicts 1 after touching 0. Repetition rejects random coincidences. */
        for (int r = 0; r < 64 && (lru || fifo); ++r) {
            for (int probe = 0; probe < 2; ++probe) {
                cache_flush();
                for (int i = 0; i < E; ++i) cache_load((uint32_t)i * stride);
                cache_load(0);
                cache_load((uint32_t)E * stride);
                int hit = cache_load((uint32_t)probe * stride);
                if (hit != (probe == 0)) lru = 0;
                if (hit != (probe == 1)) fifo = 0;
            }
        }
        policy = lru ? POLICY_LRU : fifo ? POLICY_FIFO : POLICY_RANDOM;
    }
    report(b, s, E, policy, write_allocate);
}
