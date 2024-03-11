#include <iostream>
#include "harbor.h"
#include "macros.h"

char ok_str[3] = "OK";

char map[N][N];
Robot robot[ROBOT_NUM];
Berth berth[BERTH_NUM];
Boat boat[BOAT_NUM];

void Init()
{
    /* map */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            std::cin >> map[i][j];

    /* berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        int id;
        std::cin >> id >> berth[id].x >> berth[id].y >> berth[id].transport_time >> berth[id].loading_speed;
    }

    /* boat capacity */
    int boat_capacity;
    std::cin >> boat_capacity;

    for (int i = 0; i < BOAT_NUM; ++i)
        boat[i].capacity = boat_capacity;

    /* OK */
    std::cin >> ok_str;
    std::cout << "OK" << std::flush;
}

void Input()
{
    int frame_num, current_money; /* frame_id increase from 1 */
    std::cin >> frame_num >> current_money;

    /* new item */
    int K;
    std::cin >> K;
    for (int i = 0; i < K; ++i)
    {
        Item item;
        std::cin >> item.x >> item.y >> item.value;
    }

    /* robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
        std::cin >> robot[i].has_item >> robot[i].x >> robot[i].y >> robot[i].status;

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
        std::cin >> boat[i].status >> boat[i].target_berth_id;

    /* OK */
    std::cin >> ok_str;
}

int main()
{
    Init();
    for (int frame_cnt = 0; frame_cnt < 15000; ++frame_cnt)
    {
        Input();
        for (int i = 0; i < ROBOT_NUM; ++i)
            ; // std::cout << "commands";

        std::cout << "OK" << std::flush;
    }

    return 0;
}
