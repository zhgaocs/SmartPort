#ifndef UTILS_H
#define UTILS_H 1

#include <cstring>
#include <stdio.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include "constants.h"
#include "harbor.h"

extern char map[N][N_PLUS_TWO];
extern std::unordered_map<int, int> nearest_berth; // key - hash(point), value - berth index
extern std::unordered_map<int, int> point_pathidx; // key - hash(point), value - cached_paths' index
extern std::vector<int> cached_paths[CACHED_PATH_MAX_SIZE];

bool FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions);

#endif