#ifndef HARBOR_H
#define HARBOR_H 1

#include <deque>
#include <vector>

struct Robot
{
    int x;
    int y;
    int has_item; /* 0 - no items, 1 - has items */
    int status;   /* 0 - recover, 1 - running */
    /*-------------------------------------------------------------------*/
    int task = 0;                // 0 - no tasks, 1 - get item, 2 - pull item
    int target_item = -1;        // -1 - no items selected
    int target_berth = -1;       // -1 - no berths selected
    std::vector<int> directions; // reverse path
    /*-------------------------------------------------------------------*/
};

struct Berth
{
    int x;
    int y;              /* upper left corner */
    int transport_time; /* 1 <=  transport_time <= 1000, transport time to virtual point*/
    int loading_speed;  /* 1 <= loading_speed <= 5, the number of items that can be loaded per frame */
    /*-------------------------------------------------------------------*/
    int total_value = 0;
    int current_boat = -1; // -1 - no boats
    std::deque<int> piled_values;
    /*-------------------------------------------------------------------*/
};

struct Boat
{
    int status = 1; /* 0 - moving, 1 - running well, 2 - waiting */
    int pos = -1;   /* -1 - virtual point */
    int rest_capacity;
    int target_pos = -2;
    static int capacity; /* 1 <= capacity <= 1000*/
};

struct Item
{
    int x;
    int y;
    int value;     /* value <= 200 */
    int life_span; /* life_span <= 1000 */

    Item(int x, int y, int value, int life_span)
        : x(x), y(y), value(value), life_span(life_span) {}
};

#endif
