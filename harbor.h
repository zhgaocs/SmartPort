#ifndef HARBOR_H
#define HARBOR_H 1

struct Robot
{
    int x, y;
    int goods;
    int status;
    int mbx, mby;

    Robot() = default;
    Robot(int startX, int startY) : x(startX), y(startY) {}
};

struct Berth
{
    int x, y;
    int transport_time;
    int loading_speed;

    Berth() = default;
    Berth(int x, int y, int transport_time, int loading_speed)
        : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}
};

struct Boat
{
    int num;
    int pos;
    int status;
};

#endif