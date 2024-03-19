#include "master.h"
#include <chrono>

Master::Master()
{
    std::ios::sync_with_stdio(false);

    out.open("log.txt");
}

void Master::init()
{
    char ok_str[3];
    int boat_capacity, berth_id;

    /* init map */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            std::cin >> map[i][j];

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        std::cin >> berth_id;
        std::cin >> berths[berth_id].x >> berths[berth_id].y >> berths[berth_id].transport_time >> berths[berth_id].loading_speed;
    }

    /* boat capacity */
    std::cin >> boat_capacity;
    std::for_each(boats, boats + BOAT_NUM, [boat_capacity](Boat &b)
                  { b.capacity = boat_capacity; });

    /* OK */
    std::cin >> ok_str;
    std::cout << ok_str << std::flush;
}

void Master::update()
{
    static char ok_str[3];
    int frame_num, current_money, new_items_cnt, x, y, value; /* frame_num increase from 1 */

    std::cin >> frame_num >> current_money;

    out << "---------------------------------------------------" << frame_num
        << "---------------------------------------------------\n";

    /* old item */
    auto rit = std::find_if(items.rbegin(), items.rend(), [](const Item &item)
                            { return item.life_span; });

    if (items.rend() != rit)
        std::for_each(items.begin(), rit.base(), [](Item &item)
                      { --item.life_span; });

    /* new item */
    std::cin >> new_items_cnt;
    for (int i = 0; i < new_items_cnt; ++i)
    {
        std::cin >> x >> y >> value;
        items.emplace_front(x, y, value, ITEM_LIFESPAN_MAX, 0);
    }

    /* robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        std::cin >> robots[i].has_item >> robots[i].x >> robots[i].y >> robots[i].status;
        out << "Robots#" << i
            << " has_item: " << robots[i].has_item << '\t'
            << " x: " << robots[i].x << '\t'
            << " y: " << robots[i].y << '\t'
            << " status: " << robots[i].status << '\n';
    }

    out.flush();

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        std::cin >> boats[i].status >> boats[i].pos;

        if (boats[i].pos != -1)
            berths[boats[i].pos].current_boat = i;
    }

    /* OK */
    std::cin >> ok_str;
}

void Master::assignTasks()
{
    /* ROBOT */
    static bool forbidden[ROBOT_NUM]; // {false, false, ...}

    int item_idx, berth_idx;
    std::vector<std::pair<int, int>> reverse_path;

    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status && !forbidden[i])
        {
            int w, min_w = INTEGER_MAX;

            if (!robots[i].task) // no tasks, select item
            {
                for (int j = 0; j < items.size(); ++j)
                {
                    if (items[j].life_span && !items[j].is_selected)
                    {
                        w = Manhattan(robots[i].x, robots[i].y, items[j].x, items[j].y);

                        if (w < min_w)
                        {
                            min_w = w;
                            item_idx = j;
                        }
                    }
                }

                auto start = std::chrono::high_resolution_clock::now();
                FindPath(map, robots[i].x, robots[i].y, items[item_idx].x, items[item_idx].y, reverse_path);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;

                if (!reverse_path.empty() && reverse_path.size() < items[item_idx].life_span)
                {
                    robots[i].task = 1;
                    robots[i].target_item = item_idx;
                    items[item_idx].is_selected = 1;
                    Path2Directions(reverse_path, robots[i].directions);

                    out << "FindPath succeed, spends " << elapsed.count() << "ms\n";
                }
                else
                {
                    forbidden[i] = true;
                    out << "FindPath failed, spends " << elapsed.count() << "ms\n";
                }

                out.flush();

                if (ROBOT_NUM == i + 1)
                    std::for_each(forbidden, forbidden + ROBOT_NUM, [](bool &b)
                                  { b = false; });

                break;
            }
            else if (2 == robots[i].task && robots[i].directions.empty()) // find berth
            {
                for (int j = 0; j < BERTH_NUM; ++j)
                {
                    w = Manhattan(robots[i].x, robots[i].y, berths[j].x, berths[j].y);

                    if (w < min_w)
                    {
                        min_w = w;
                        berth_idx = j;
                    }
                }

                FindPath(map, robots[i].x, robots[i].y, berths[berth_idx].x, berths[berth_idx].y, reverse_path);

                if (!reverse_path.empty())
                {
                    robots[i].target_berth = berth_idx;
                    Path2Directions(reverse_path, robots[i].directions);
                }
                else
                    forbidden[i] = true;

                if (ROBOT_NUM == i + 1)
                    std::for_each(forbidden, forbidden + ROBOT_NUM - 1, [](bool &b)
                                  { b = false; });

                break;
            }
        }
    }
}

void Master::control()
{
    /* ROBOT */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (!robots[i].status) // recover
        {
            if (1 == robots[i].task && items[robots[i].target_item].life_span < robots[i].directions.size())
            {
                robots[i].task = 0;
                items[robots[i].target_item].is_selected = 0;
            }
        }
        else // running well
        {
            if (!robots[i].task) // no tasks
                ;
            else // has tasks
            {
                // detect collision
                // avoid collision

                std::cout << "move " << i << robots[i].directions.back() << '\n';
                out << "move " << i << robots[i].directions.back() << '\n';

                robots[i].directions.pop_back();

                if (robots[i].directions.empty()) // has arrived
                {
                    if (1 == robots[i].task) // get
                    {
                        std::cout << "get " << i << '\n';
                        out << "get " << i << '\n';

                        robots[i].task = 2;
                    }
                    else // pull
                    {
                        std::cout << "pull " << i << '\n';
                        out << "pull " << i << '\n';

                        int value = items[robots[i].target_item].value;

                        berths[robots[i].target_berth].total_value += value;
                        berths[robots[i].target_berth].piled_values.emplace_back(value);
                    }
                }
            }
        }
    }

    std::cout << "OK" << std::flush;
}

void Master::run()
{
    update();
    assignTasks();
    control();
}
