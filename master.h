#ifndef MASTER_H
#define MASTER_H 1

#include <iostream>
#include <numeric> // std::accumulate()
#include <queue>
#include "harbor.h"
#include "utils.h"

#include <fstream>

class Master
{
public:
    Master();
    ~Master() = default;

public:
    void init();
    void run();

private:
    void update();
    void assignRobots();
    void assignBoats();
    void control();

private:
    char map[N][N];
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];
    
    std::vector<Item> items;
    std::vector<bool> item_selected;

    std::ofstream log;
};

#endif