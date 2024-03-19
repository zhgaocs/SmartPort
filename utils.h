#ifndef UTILS_H
#define UTILS_H 1

#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
#include "constants.h"

namespace std
{
    template <>
    struct hash<std::pair<int, int>>
    {
        size_t operator()(const std::pair<int, int> &p) const
        {
            return p.first * N + p.second;
        }
    };
}

inline unsigned int Manhattan(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

void InitMap();

int FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<std::pair<int, int>> &path);

int Path2Directions(const std::vector<std::pair<int, int>> &reverse_path, std::vector<int> &directions);

#endif