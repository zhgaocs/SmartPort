#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <limits>

/* map */
constexpr int N = 200;
constexpr char PATHWAY_SYMBOL = '.';
constexpr int BERTH_SIZE = 4; // berth: 4*4

/* settings */
constexpr int ROBOT_NUM = 10;
constexpr int BERTH_NUM = 10;
constexpr int BOAT_NUM = 5;
constexpr int ITEM_MAX_LIFESPAN = 1000;

/* used in FindPath */
constexpr int NUM_OF_DIRECTIONS = 4;
constexpr int DX[NUM_OF_DIRECTIONS] = {-1, 1, 0, 0};
constexpr int DY[NUM_OF_DIRECTIONS] = {0, 0, -1, 1};
constexpr int COST[NUM_OF_DIRECTIONS] = {1, 1, 1, 1};
constexpr int INTEGER_MAX = std::numeric_limits<int>::max();

/* used in Path2Directions */
constexpr int RIGHT = 0;
constexpr int LEFT = 1;
constexpr int UP = 2;
constexpr int DOWN = 3;

#endif