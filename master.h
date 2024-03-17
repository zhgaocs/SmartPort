#ifndef MASTER_H
#define MASTER_H 1

#include <iostream>
#include <numeric> // std::accumulate()
#include <queue>
#include "harbor.h"
#include "utils.h"

class Master
{
public:
    Master() = default;
    ~Master() = default;

public:
    void init();
    void run();

private:
    void update();
    void assignTasks();
    void control();

private:    
    std::vector<std::pair<int, int>> findBerthPoint(int berth_idx);

private:
    char map[N][N];
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];
    std::vector<Item> items;
};

#endif