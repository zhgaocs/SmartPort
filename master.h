#ifndef MASTER_H
#define MASTER_H 1

#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <utility>

#include "harbor.h"
#include "utils.h"

class Master
{
public:
    Master() = default;
    ~Master() = default;

public:
    void init();
    void update();
    void assignTasks();
    void control();

private:
    /// @return reverse path: dst->src
    std::vector<std::pair<int, int>> findPath(int src_x, int src_y, int dst_x, int dst_y) const; // return reverse path: dst->src
    std::vector<std::pair<int, int>> findBerthPoint(int berth_x, int berth_y) const;

    /// @param path dst->src, and path must not be empty
    /// @return directions src->dst, but the vector elements are reversed
    static std::vector<int> path2Directions(const std::vector<std::pair<int, int>> &path);

public:
    static constexpr int N = 200;
    static constexpr int ROBOT_NUM = 10;
    static constexpr int BERTH_NUM = 10;
    static constexpr int BOAT_NUM = 5;
    static constexpr char PATHWAY_SYMBOL = '.';

private:
    char map[N][N];
    Robot robots[ROBOT_NUM];
    Berth berths[BERTH_NUM];
    Boat boats[BOAT_NUM];
    std::vector<Item> items;
};

/* -------------------------------------used in findPath-------------------------------------- */
namespace std
{
    template <>
    struct hash<std::pair<int, int>>
    {
        size_t operator()(const std::pair<int, int> &p) const
        {
            return p.first * Master::N + p.second;
        }
    };
}
/* -------------------------------------used in findPath-------------------------------------- */

#endif