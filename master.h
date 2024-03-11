#ifndef MASTER_H
#define MASTER_H

#include <algorithm>
#include <climits>
#include <iostream>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

#include "harbor.h"
#include "macros.h"
#include "utils.h"

namespace std
{
    template <>
    struct hash<Item>
    {
        size_t operator()(const Item &item) const
        {
            return hash<int>()(item.x) ^ hash<int>()(item.y) ^ hash<int>()(item.value);
        }
    };
}

class Master
{
public:
    Master() = default;
    ~Master() = default;

public:
    void init();
    void refresh();
    void control();

private:
    std::vector<std::pair<int, int>>
    findPath(int src_x, int src_y, int dst_x, int dst_y);

private:
    char map[N][N];
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];
    std::unordered_set<Item> items;

private: 
         /**
          * these vars used in findPath 
          * for map:
          * the downward direction is the positive x direction,
          * the right direction is the positive y direction.
          */
    static constexpr int DX[NUM_OF_DIRECTIONS] = {-1, 1, 0, 0};
    static constexpr int DY[NUM_OF_DIRECTIONS] = {0, 0, -1, 1};
    static constexpr int COST[NUM_OF_DIRECTIONS] = {1, 1, 1, 1};
};

#endif