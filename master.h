#ifndef MASTER_H
#define MASTER_H

#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_set>
#include <utility>

#include "harbor.h" // <vector>
#include "utils.h"

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
    static constexpr int N = 200;
    static constexpr int ROBOT_NUM = 10;
    static constexpr int BERTH_NUM = 10;
    static constexpr int BOAT_NUM = 5;
    static constexpr int ITEM_MAX_LIFESPAN = 1000;

private:
    char map[N][N];
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];
    std::vector<Item> items;
};

#endif