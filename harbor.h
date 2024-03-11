#ifndef HARBOR_H
#define HARBOR_H 1

struct Robot
{
    int x, y;
    int has_item; /* 0 - no items, 1 - has items */
    int status;   /* 0 - recover, 1 - running */
    int dst_x, dst_y;

    Robot() = default;
    Robot(int startX, int startY) : x(startX), y(startY) {}
};

struct Berth
{
    int id;             /* 0 <= id < BERTH_NUM */
    int x, y;           /* upper left corner */
    int transport_time; /* 1 <=  transport_time <= 1000, transport time to virtual point*/
    int loading_speed;  /* 1 <= loading_speed <= 5, the number of items that can be loaded per frame */

    Berth() = default;
    Berth(int x, int y, int transport_time, int loading_speed)
        : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}
};

struct Boat
{
    int id;              /* 0 <= id < BOAT_NUM */
    int target_berth_id; /* -1 - virtual point */
    int status;          /* 0 - moving, 1 - running, 2 - waiting */
    int capacity;        /* 1 <= capacity <= 1000*/
};

struct Item /* goods */
{
    int x, y;
    int value; /* value <= 1000 */

    bool operator==(const Item &other) const
    {
        return x == other.x && y == other.y && value == other.value;
    }
};

#endif