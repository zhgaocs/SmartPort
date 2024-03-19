#include "master.h"
#include <chrono>
#include <iomanip>

Master::Master()
{
    std::ios::sync_with_stdio(false);

    log.open("log.txt");
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
    static int last_sync_id = 0;

    char ok_str[3];
    int frame_id, frame_sub, current_money, new_items_cnt, x, y, value; /* frame_num increase from 1 */

    std::cin >> frame_id >> current_money;

    log << "---------------------------------------------------" << frame_id
        << "---------------------------------------------------\n";

    /* new items */
    std::cin >> new_items_cnt;
    for (int i = 0; i < new_items_cnt; ++i)
    {
        std::cin >> x >> y >> value;
        items.emplace_back(x, y, value, ITEM_LIFESPAN_MAX);
        item_selected.emplace_back(false);
    }

    /* robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        std::cin >> robots[i].has_item >> robots[i].x >> robots[i].y >> robots[i].status;
    }

    log.flush();

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        std::cin >> boats[i].status >> boats[i].pos;

        if (boats[i].pos != -1)
            berths[boats[i].pos].current_boat = i;
    }

    /* all items */
    frame_sub = frame_id - last_sync_id;

    std::for_each(items.begin(), items.end(), [frame_sub](Item &item)
                  { item.life_span -= frame_sub; });

    if (1 != frame_sub)
        last_sync_id = frame_sub;
    else
        ++last_sync_id;

    /* OK */
    std::cin >> ok_str;
}

void Master::assignRobots()
{
    static int i = 0;

    int item_idx, berth_idx;
    std::vector<std::pair<int, int>> reverse_path;

    for (; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status)
        {
            int w, min_w = INTEGER_MAX;

            if (!robots[i].task) // no tasks, select item
            {
                for (int j = items.size() - 1; j >= 0; --j)
                {
                    if (items[j].life_span > 0)
                    {
                        if (!item_selected[j]) // has not be selected by other robots
                        {
                            w = Manhattan(robots[i].x, robots[i].y, items[j].x, items[j].y);

                            if (w < min_w)
                            {
                                min_w = w;
                                item_idx = j;
                            }
                        }
                    }
                    else // with index increasing, life_span becomes bigger
                        break;
                }

                auto start = std::chrono::high_resolution_clock::now();
                FindPath(map, robots[i].x, robots[i].y, items[item_idx].x, items[item_idx].y, reverse_path);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;

                if (!reverse_path.empty() && reverse_path.size() < items[item_idx].life_span)
                {
                    robots[i].task = 1;
                    robots[i].target_item = item_idx;
                    item_selected[item_idx] = true;
                    Path2Directions(reverse_path, robots[i].directions);

                    log << "Robots#" << i << " find item succeed, spends " << elapsed.count() << "ms\n";
                }
                else
                    log << "Robots#" << i << " find item failed, spends " << elapsed.count() << "ms\n";

                log.flush();
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

                auto start = std::chrono::high_resolution_clock::now();
                FindPath(map, robots[i].x, robots[i].y, berths[berth_idx].x, berths[berth_idx].y, reverse_path);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = end - start;

                if (!reverse_path.empty())
                {
                    robots[i].target_berth = berth_idx;
                    Path2Directions(reverse_path, robots[i].directions);

                    log << "Robots#" << i << " find berth path succeed, spends " << elapsed.count() << "ms\n";
                }
                else
                    log << "Robots#" << i << " find berth path failed, spends " << elapsed.count() << "ms\n";

                log.flush();
                break;
            }
        }
    }

    if (ROBOT_NUM == i)
        i = 0;
}

void Master::assignBoats()
{
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        if (1 == boats[i].status) // loading or transportation completed
        {
            if (-1 == boats[i].pos) // transportation completed, target_pos must euqal to -1
            {
                int berth_idx, w, min_w = INTEGER_MAX;

                for (int j = 0; j < BERTH_NUM; ++j)
                {
                    if (-1 == berths[j].current_boat) // no boats in berth
                        w = berths[j].transport_time -
                            berths[j].loading_speed -
                            berths[j].piled_values.size() -
                            berths[j].total_value;
                    else // there is already a ship at the berth
                    {
                        bool will_two_boats = false;

                        for (const Boat &boat : boats)
                        {
                            if (boat.target_pos == j)
                            {
                                will_two_boats = true;
                                break;
                            }
                        }

                        if (will_two_boats)
                            continue;

                        int current_boat_capacity = boats[berths[j].current_boat].capacity,
                            rest_items_cnt = berths[j].piled_values.size() - current_boat_capacity;

                        if (rest_items_cnt <= 0) // one boat can load it all
                            w = berths[j].transport_time - berths[j].loading_speed;
                        else
                        {
                            int rest_value = std::accumulate(berths[j].piled_values.cbegin() + rest_items_cnt,
                                                             berths[j].piled_values.cend(), 0);

                            w = berths[j].transport_time -
                                berths[j].loading_speed -
                                rest_items_cnt -
                                rest_value;
                        }
                    }

                    if (w < min_w)
                    {
                        min_w = w;
                        berth_idx = j;
                    }
                }

                boats[i].target_pos = berth_idx;
            }
            else // loading in berth
            {
                if (!boats[i].capacity) // full, go to virtual point
                    boats[i].target_pos = -1;
                else // not full
                {
                    Berth &berth = berths[boats[i].pos];

                    int min = Minimum_3(boats[i].capacity, berth.loading_speed, berth.piled_values.size());
                    int sub_value = std::accumulate(berth.piled_values.cbegin(), berth.piled_values.cbegin() + min, 0);

                    boats[i].capacity -= min;
                    berth.total_value -= sub_value;
                    berth.piled_values.erase(berth.piled_values.begin(), berth.piled_values.begin() + min);
                }
            }
        }
    }
}

void Master::control()
{
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        log << "Robots#" << std::setw(1) << i
            << " |has_item: " << std::setw(2) << robots[i].has_item
            << " |x: " << std::setw(3) << robots[i].x
            << " |y: " << std::setw(3) << robots[i].y
            << " |status: " << std::setw(2) << robots[i].status
            << " |task: " << std::setw(2) << robots[i].task;

        log << std::setw(5) << " |target_item: " << std::setw(2) << robots[i].target_item;

        log << std::setw(5) << " |target_berth " << std::setw(2) << robots[i].target_berth;

        log << " |directions.empty(): " << std::setw(2) << robots[i].directions.empty()
            << " |directions.size(): " << std::setw(2) << robots[i].directions.size() << '\n';
    }

    /* ROBOT */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (!robots[i].status) // recover
        {
            if (1 == robots[i].task && items[robots[i].target_item].life_span < robots[i].directions.size())
            {
                robots[i].task = 0;
                item_selected[robots[i].target_item] = false;
            }
        }
        else // running well
        {
            if (!robots[i].task) // no tasks
                ;
            else if (!robots[i].directions.empty()) // has tasks and has found path
            {
                // detect collision
                // avoid collision

                std::cout << "move " << i << ' ' << robots[i].directions.back() << '\n';
                log << "move " << i << ' ' << robots[i].directions.back() << '\n';

                robots[i].directions.pop_back();

                if (robots[i].directions.empty()) // has arrived
                {
                    if (1 == robots[i].task)
                    {
                        if (items[robots[i].target_item].life_span > 0) // get
                        {
                            std::cout << "get " << i << '\n';
                            log << "get " << i << '\n';
                            robots[i].task = 2;
                        }
                        else // item disappear
                            robots[i].task = 0;
                    }
                    else // pull
                    {
                        std::cout << "pull " << i << '\n';
                        log << "pull " << i << '\n';

                        int value = items[robots[i].target_item].value;

                        berths[robots[i].target_berth].total_value += value;
                        berths[robots[i].target_berth].piled_values.emplace_back(value);
                    }
                }

                log.flush();
            }
        }
    }

    std::cout << "OK" << std::flush;
}

void Master::run()
{
    update();
    assignRobots();
    assignBoats();
    control();
}
