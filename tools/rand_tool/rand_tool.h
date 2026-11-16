#ifndef TOOLS_RAND_TOOL_RAND_TOOL_H
#define TOOLS_RAND_TOOL_RAND_TOOL_H
#define RANXOSHI256_EXTERN
#include "third_part/ranxoshi256/ranxoshi256.h"
typedef unsigned long long rand_u64;
[[nodiscard]] extern rand_u64 Tool_GetRandU64();
#endif // !TOOLS_RAND_TOOL_RAND_TOOL_H