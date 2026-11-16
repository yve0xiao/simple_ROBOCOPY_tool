#include "rand_tool.h"
#include "time.h"
#define RANXOSHI256_IMPLEMENTATION
#include "third_part/ranxoshi256/ranxoshi256.h"
static unsigned char seed[32] = {0};
static struct ranxoshi256 rng = {.s = {0}};
/**
 * @brief 将 64 位时间戳扩展为 32 字节的安全种子
 *
 * @param seed
 *
 * @note
 */
void generate_32_byte_seed(unsigned char* seed)
{
    uint64_t t = (uint64_t)time(NULL);
    for (int i = 0; i < 32; i++)
    {
        // 简单的 SplitMix64 算法思想，用来打散种子
        t += 0x9e3779b97f4a7c15ULL;
        uint64_t z = t;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        seed[i] = (unsigned char)(z ^ (z >> 31));
    }
}
rand_u64 Tool_GetRandU64()
{
    static int is_seed = 0;
    if (!is_seed)
    {
        generate_32_byte_seed(seed);
        ranxoshi256Seed(&rng, seed);
        is_seed = 1;
    }
    rand_u64 my_random = 0ULL;
    do
    {
        my_random = ranxoshi256Next(&rng);
    } while (my_random == UINT64_MAX);
    return my_random;
}