#ifndef MASTER_H
#define MASTER_H 1

#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif

#include <algorithm>
#include <numeric> // std::accumulate()
#include <unordered_map>
#include "harbor.h"
#include "utils.h"

#ifdef DEBUG_MODE
#include <chrono>
#include <fstream>
#include <iomanip>
#endif

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

#ifdef DEBUG_MODE
private:
    void printRobots();
    void printBoats();
    void printBerths();
#endif

private:
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];

    std::vector<Item> items;
    std::vector<bool> item_selected;


    std::unordered_map<int, int> nearest_berth; // key - hash(point), value - berth index
    std::unordered_map<int, int> point_pathidx; // key - hash(point), value - cached_paths' index
    std::vector<int> cached_paths[1000];

#ifdef DEBUG_MODE
    std::ofstream log;
#endif
};

#endif