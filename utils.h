#ifndef UTILS_H
#define UTILS_H 1

#include <cstring>
#include <stdio.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include "constants.h"
#include "harbor.h"

extern char map[N][N];
extern std::unordered_map<int, int> nearest_berth; // key - hash(point), value - berth index
extern std::unordered_map<int, int> point_pathidx; // key - hash(point), value - cached_paths' index
extern std::vector<int> cached_paths[CACHED_PATH_MAX_SIZE];

inline void InitMap()
{
    for (int i = 0; i < N; ++i)
        scanf("%s", map[i]);
}

bool FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions);

#endif