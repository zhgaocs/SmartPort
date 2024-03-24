#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <limits>

constexpr int DETECT_RADIUS = 25;
constexpr int LIMIT = 800;

/* settings */
constexpr int N = 200;
constexpr int N_PLUS_TWO = N + 2;
constexpr char PATHWAY_SYMBOL = '.';
constexpr char BERTH_SYMBOL = 'B';
constexpr int ROBOT_NUM = 10;
constexpr int BERTH_NUM = 10;
constexpr int BOAT_NUM = 5;
constexpr int ITEM_LIFESPAN_MAX = 1000;
constexpr int MAX_FRAME_ID = 15000;
constexpr int TIME_BETWEEN_BERTH = 500;

/* used in FindPath */
constexpr int CACHED_PATH_MAX_SIZE = 500;
constexpr int DX[4] = {0, 0,-1, 1};
constexpr int DY[4] = {1, -1, 0, 0};
constexpr int INTEGER_MAX = std::numeric_limits<int>::max();

constexpr int RIGHT = 0;
constexpr int LEFT = 1;
constexpr int UP = 2;
constexpr int DOWN = 3;

#endif