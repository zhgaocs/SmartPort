#ifndef HARBOR_H
#define HARBOR_H 1

#include <vector>

struct Robot
{
    int x, y;
    int has_item; /* 0 - no items, 1 - has items */
    int status;   /* 0 - recover, 1 - running */
    /*-------------------------------------------------------------------*/
    int has_task = 0;      // 0 represents no tasks
    int target_berth = -1; // -1 represents no berth selected
    std::vector<int> path; // reverse path
    /*-------------------------------------------------------------------*/
};

struct Berth
{
    int x, y;           /* upper left corner */
    int transport_time; /* 1 <=  transport_time <= 1000, transport time to virtual point*/
    int loading_speed;  /* 1 <= loading_speed <= 5, the number of items that can be loaded per frame */
    /*-------------------------------------------------------------------*/
    int items_count = 0; // the number of items
    int total_value = 0; // total value of the berth
    /*-------------------------------------------------------------------*/
};

struct Boat
{
    int target_berth = -1;    /* -1 - virtual point */
    int status = 1;           /* 0 - moving, 1 - running, 2 - waiting */
    static int boat_capacity; /* 1 <= capacity <= 1000*/
};

struct Item /* goods */
{
    int x, y;
    int value;     /* value <= 1000 */
    int life_span = MAX_LIFESPAN; /* life_span <= 1000 */
    static constexpr int MAX_LIFESPAN = 1000;
};

#endif