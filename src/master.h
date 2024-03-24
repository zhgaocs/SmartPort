#ifndef MASTER_H
#define MASTER_H 1

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <numeric> // std::accumulate()
#include "utils.h"
#include "harbor.h"

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
    void assignRobots();
    void assignBoats();
    void control();

private:
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];

    std::vector<Item> items;
    std::vector<bool> item_selected;

    static int frame_id;
};

#endif
