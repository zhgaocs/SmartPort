#ifndef UTILS_H
#define UTILS_H 1

#include <stdio.h>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>
#include "constants.h"

extern char map[N][N];

inline unsigned int Manhattan(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

inline void InitMap()
{
    for (int i = 0; i < N; ++i)
        scanf("%s", map[i]);
}

bool FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions);

#endif