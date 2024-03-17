#ifndef UTILS_H
#define UTILS_H 1

#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
#include "constants.h"

inline unsigned int Manhattan(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

int Min(int x, int y, int z);

std::vector<std::pair<int, int>> FindPath(const char (&map)[N][N], int src_x, int src_y, int dst_x, int dst_y);

std::vector<int> Path2Directions(const std::vector<std::pair<int, int>> &path);

#endif