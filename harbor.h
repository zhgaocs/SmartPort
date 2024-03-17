#ifndef HARBOR_H
#define HARBOR_H 1

#include <deque>
#include <vector>
#include "constants.h"

struct Robot
{
    int x, y;
    int has_item; /* 0 - no items, 1 - has items */
    int status;   /* 0 - recover, 1 - running */
    /*-------------------------------------------------------------------*/
    int has_task = 0;         // 0 represents no tasks
    int target_berth = -1;    // -1 represents no berth selected
    int target_value = 0;     // The value of the item carried
    int target_lifespan = -1; // The remaining time of the carried item
    std::vector<int> path;    // reverse path
    /*-------------------------------------------------------------------*/
};

struct Berth
{
    int x, y;           /* upper left corner */
    int transport_time; /* 1 <=  transport_time <= 1000, transport time to virtual point*/
    int loading_speed;  /* 1 <= loading_speed <= 5, the number of items that can be loaded per frame */
    /*-------------------------------------------------------------------*/
    int total_value = 0; // total value of the berth
    int boat_index = -1; // -1 represents no boats
    std::deque<int> piled_items;
    /*-------------------------------------------------------------------*/
};

struct Boat
{
    int pos = -1;   /* -1 - virtual point */
    int status = 1; /* 0 - moving, 1 - running well, 2 - waiting */
    int capacity;   /* 1 <= capacity <= 1000*/
    /*-------------------------------------------------------------------*/
    int target_pos = -2; // -2 represents no selected berth
    /*-------------------------------------------------------------------*/
};

struct Item
{
    int x, y;
    int value;                         /* value <= 1000 */
    int life_span = ITEM_MAX_LIFESPAN; /* life_span <= 1000 */
};

#endif