#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <limits>

/* settings */
constexpr int N = 200;
constexpr int N_PLUS_TWO = N + 2;
constexpr char PATHWAY_SYMBOL = '.';
constexpr char BERTH_SYMBOL = 'B';
constexpr int ROBOT_NUM = 10;
constexpr int BERTH_NUM = 10;
constexpr int BOAT_NUM = 5;
constexpr int MAX_FRAME_ID = 15000;
constexpr int ITEM_LIFESPAN_MAX = 1001;
constexpr int TIME_BETWEEN_BERTH = 500;

constexpr int DX[4] = {-1, 1, 0, 0};
constexpr int DY[4] = {0, 0, -1, 1};
constexpr int INTEGER_MAX = std::numeric_limits<int>::max();
constexpr int RIGHT = 0;
constexpr int LEFT = 1;
constexpr int UP = 2;
constexpr int DOWN = 3;

#endif