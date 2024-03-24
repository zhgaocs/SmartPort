#ifndef UTILS_H
#define UTILS_H 1

#include <cstring>
#include <utility>
#include <vector>
#include "constants.h"

extern char map[N][N_PLUS_TWO];

inline int Minimum_3(int x, int y, int z)
{
    return std::min(x, std::min(y, z));
}

bool FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions);

#endif