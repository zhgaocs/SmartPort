#ifndef MASTER_H
#define MASTER_H 1

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_set>
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
    void preprocess();
    void update();
    void assignRobots();
    void assignBoats();
    void control();
    int choose_berth(int i);
    int robot_scan(int i, std::vector<int> &directions);

    inline int point_hash(std::pair<int, int> &p)
    {
        return p.first * N + p.second;
    }
    int edge_hash(std::pair<int, int> &p1, std::pair<int, int> &p2);
    void get_next_pos(std::pair<int, int> &ne, std::pair<int, int> &cur, int dir);
    int has_collision(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos);
    void collision_avoidance(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos);
    void collision_solver(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos,
                          std::vector<int> &robs, int cur, int dx[], int dy[], std::vector<std::pair<int, int>> &res);

private:
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];

    std::vector<Item> items;
    std::vector<bool> item_selected;
    std::priority_queue<std::pair<int, int>> heap;
    int point_has_item[N][N];

    int trans_fast_berth[BOAT_NUM];

    static int frame_id;
};

#endif