#ifndef UTILS_H
#define UTILS_H 1

#include <cstring>
#include <unordered_map>
#include <utility>
#include <vector>
#include "constants.h"
#include "harbor.h"

extern char map[N][N_PLUS_TWO];
extern std::unordered_map<int, int> nearest_berth; // key - hash(point), value - berth index
extern std::unordered_map<int, int> point_pathidx; // key - hash(point), value - cached_paths' index
extern std::vector<int> cached_paths[CACHED_PATH_MAX_SIZE];
extern std::unordered_map<int, int> nearest_berth_dis; // key - hash(point), value - berth index
extern int rest_invoke;

inline int Minimum_3(int x, int y, int z)
{
    return std::min(x, std::min(y, z));
}

int FindPathDirections(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions);
void Path2Directions(std::vector<int> &directions, std::vector<std::pair<int, int>> &path);
int FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<std::pair<int, int>> &path);
int FindShortDirections(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions, int limit);
#endif