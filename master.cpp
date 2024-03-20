#include "master.h"

Master::Master()
{
#ifdef DEBUG_MODE
    log.open("log.txt");
#endif
}

void Master::init()
{
    char okk[3];
    int boat_capacity;

    InitMap();

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berths[id].x, &berths[id].y, &berths[id].transport_time, &berths[id].loading_speed);
    }

    /* boat capacity */
    scanf("%d", &boat_capacity);
    std::for_each(boats, boats + BOAT_NUM, [boat_capacity](Boat &b)
                  { b.rest_capacity = boat_capacity; });

    /* OK */
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

void Master::update()
{
    static int last_sync = 0;

    char okk[3];
    int frame_id, frame_sub, current_money, new_items;

    scanf("%d%d", &frame_id, &current_money);

    /* new items */
    scanf("%d", &new_items);
    for (int i = 0; i < new_items; ++i)
    {
        int x, y, value;
        scanf("%d%d%d", &x, &y, &value);
        items.emplace_back(x, y, value, ITEM_LIFESPAN_MAX);
        item_selected.emplace_back(false);
    }

    /* robot */
    for (Robot &robot : robots)
        scanf("%d%d%d%d", &robot.has_item, &robot.x, &robot.y, &robot.status);

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        scanf("%d%d\n", &boats[i].status, &boats[i].pos);

        if (boats[i].pos != -1)
        {
            if (!boats[i].status)
                ;
            if (1 == boats[i].status)
                berths[boats[i].pos].current_boat = i;
            else
                berths[boats[i].pos].waiting_boat = i;
        }
    }

    /* all items */
    frame_sub = frame_id - last_sync;
    std::for_each(items.begin(), items.end(), [frame_sub](Item &item)
                  { item.life_span -= frame_sub; });

    if (1 != frame_sub)
        last_sync = frame_sub;
    else
        ++last_sync;

    /* OK */
    scanf("%s", okk);
}

void Master::assignRobots()
{
    int rest_invoke = 30;

    for (int i = 0; i < ROBOT_NUM && rest_invoke; ++i)
    {
        if (!robots[i].status) // running well
        {
            if (!robots[i].task) // no tasks, select item
            {
                int glob_item, local_item, glob_max = 0, local_max = 0;

                for (int j = items.size() - 1; j >= 0 && items[j].life_span > 0; --j)
                {
                    if (item_selected[j])
                        continue;

                    if (items[j].value > local_max)
                    {
                        if (std::abs(items[j].x - robots[i].x) <= 4 &&
                            std::abs(items[j].y - robots[i].y) <= 4)
                        {
                            local_item = j;
                            local_max = items[j].value;
                        }
                    }
                    else
                        continue;

                    if (items[j].value > glob_max)
                    {
                        glob_item = j;
                        glob_max = items[j].value;
                    }
                }

                if (local_max) // find a max value item in 9*9
                {
                    --rest_invoke;

                    if (FindPath(robots[i].x, robots[i].y, items[local_item].x, items[local_item].y,
                                 robots[i].directions)) // FindPath success
                    {
                        robots[i].task = 1;
                        robots[i].target_item = local_item;
                        item_selected[local_item] = true;
                        break;
                    }
                    // find a path to global max value
                }

                --rest_invoke;
                // there must be a global max value
                if (FindPath(robots[i].x, robots[i].y, items[glob_item].x, items[glob_item].y,
                             robots[i].directions)) // FindPath success
                {
                    robots[i].task = 1;
                    robots[i].target_item = glob_item;
                    item_selected[glob_item] = true;
                }
                // else ; nothing to do
            }
            else if (2 == robots[i].task && robots[i].directions.empty()) // select berth
            {
                int point_hash = robots[i].x * N + robots[i].y;
                auto it = nearest_berth.find(point_hash);

                if (nearest_berth.cend() == it) // cache miss
                {
                    int min_dist = INTEGER_MAX, berth_idx = -1;
                    std::vector<int> path, shortest_path;

                    for (int j = 0; j < BERTH_NUM; ++j)
                    {
                        if (FindPath(robots[i].x, robots[i].y, berths[j].x, berths[j].y, path))
                        {
                            if (path.size() < min_dist)
                            {
                                berth_idx = j;
                                min_dist = path.size();
                                shortest_path = std::move(path);
                            }
                        }
                    }

                    if (berth_idx != -1) // can find a berth
                    {
                        robots[i].target_berth = berth_idx;
                        robots[i].directions = std::move(shortest_path);
                    }
                    else // cannot find a berth ???????????????????????????????????????
                        ;
                }
                else                                                                // cache hit
                    robots[i].directions = cached_paths[point_pathidx[point_hash]]; // don't move
            }
        }
        else // recover
        {
        }
    }
}

void Master::assignBoats()
{
}

void Master::control()
{
    /* ROBOT */
    // TODO

    /* BOAT */
    // TODO
}

void Master::run()
{
    update();
    assignRobots();
    assignBoats();
    control();
}

#ifdef DEBUG_MODE
void Master::printRobots()
{
    for (int i = 0; i < ROBOT_NUM; ++i)
        log << "Robot#" << std::setw(1) << i
            << " |has_item: " << std::setw(1) << robots[i].has_item
            << " |x: " << std::setw(3) << robots[i].x
            << " |y: " << std::setw(3) << robots[i].y
            << " |status: " << std::setw(1) << robots[i].status
            << " |task: " << std::setw(1) << robots[i].task
            << " |target_item: " << std::setw(2) << robots[i].target_item
            << " |target_berth " << std::setw(2) << robots[i].target_berth
            << " |directions.empty(): " << std::setw(1) << robots[i].directions.empty()
            << " |directions.size(): " << std::setw(3) << robots[i].directions.size() << '\n';

    log.flush();
}

void Master::printBoats()
{
    for (int i = 0; i < BOAT_NUM; ++i)
        log << "Boat#" << std::setw(1) << i
            << " |status: " << std::setw(1) << boats[i].status
            << " |target_pos: " << std::setw(2) << boats[i].pos
            << " |rest_capacity: " << std::setw(3) << boats[i].rest_capacity << '\n';
    log.flush();
}

void Master::printBerths()
{
    for (int i = 0; i < BERTH_NUM; ++i)
        log << "Berth#" << std::setw(1) << i
            << " |x: " << std::setw(3) << berths[i].x
            << " |y: " << std::setw(3) << berths[i].y
            << " |transport_time: " << std::setw(3) << berths[i].transport_time
            << " |loading_speed: " << std::setw(3) << berths[i].loading_speed
            << " |total_value: " << std::setw(3) << berths[i].total_value
            << " |current_boat: " << std::setw(3) << berths[i].current_boat
            << " |waiting_boat: " << std::setw(3) << berths[i].waiting_boat
            << " |piled_values.size(): " << std::setw(3) << berths[i].piled_values.size() << '\n';
    log.flush();
}
#endif