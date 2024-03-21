#include "master.h"

Master::Master()
{
#ifdef DEBUG_MODE
    log.open("log.txt");
#endif
}

void Master::init()
{
    char str[3];
    int boat_capacity;

    for (int i = 0; i < N; ++i)
        fgets(map[i], N + 2, stdin);

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
                  { b.max_capacity = b.rest_capacity = boat_capacity; });

    // preprocess();
    /* OK */
    scanf("%s", str);
    printf("OK\n");
    fflush(stdout);
}

void Master::run()
{
    update();
    // printRobots();
    printBoats();
    printBerths();
    assignRobots();
    assignBoats();
    control();
}

void Master::preprocess() const
{
    int count = 0;

    srand(time(nullptr));

    while (count < CACHED_PATH_MAX_SIZE)
    {
        int x = rand() % N;
        int y = rand() % N;
        int point_hash = x * N + y;

        auto it = nearest_berth.find(point_hash);

        if (map[x][y] == PATHWAY_SYMBOL && nearest_berth.cend() == it) // not in cache
        {
            int berth_idx = -1, min_dist = INTEGER_MAX;
            std::vector<int> path, shortest_path;

            for (int j = 0; j < BERTH_NUM; ++j)
            {
                if (FindPath(x, y, berths[j].x, berths[j].y, path))
                {
                    if (path.size() < min_dist)
                    {
                        berth_idx = j;
                        min_dist = path.size();
                        shortest_path = std::move(path);
                    }
                }
            }

            if (berth_idx != -1)
            {
                nearest_berth[point_hash] = berth_idx;
                point_pathidx[point_hash] = count;
                cached_paths[count] = std::move(shortest_path);
                ++count;
            }
        }
    }
}

void Master::update()
{
    static int last_sync = 0;

    char okk[3];
    int frame_id, frame_sub, current_money, new_items;

    scanf("%d%d", &frame_id, &current_money);

#ifdef DEBUG_MODE
    log << "--------------------------------------------------------------------------------" << frame_id
        << "--------------------------------------------------------------------------------\n";
    log.flush();
#endif

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

        if (boats[i].pos != -1) // cannot update berths where a ship has just left
        {
            if (!boats[i].status)
                ;
            else if (1 == boats[i].status)
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
        if (!robots[i].task && rest_invoke >= 2) // no tasks, select item
        {
            if (robots[i].status) // running well
            {
                int glob_item = -1, local_item, glob_max = 0, local_max = 0;

                for (int j = items.size() - 1; j >= 0 && items[j].life_span > 0; --j) // greater index, greater life_span
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

                if (glob_item != -1)
                {
                    if (local_max) // find a max value item in 9*9
                    {
                        --rest_invoke;

                        if (FindPath(robots[i].x, robots[i].y, items[local_item].x, items[local_item].y, robots[i].directions) &&
                            robots[i].directions.size() <= items[local_item].life_span) // FindPath success
                        {
                            robots[i].task = 1;
                            robots[i].target_item = local_item;
                            item_selected[local_item] = true;
                            break;
                        }
                    }

                    --rest_invoke;
                    // there must be a global max value
                    if (FindPath(robots[i].x, robots[i].y, items[glob_item].x, items[glob_item].y, robots[i].directions) &&
                        robots[i].directions.size() <= items[glob_item].life_span) // FindPath success
                    {
                        robots[i].task = 1;
                        robots[i].target_item = glob_item;
                        item_selected[glob_item] = true;
                    }
                    // else ; nothing to do
                }
            }
        }
        else if (2 == robots[i].task && robots[i].directions.empty()) // select berth, include recover
        {
            int point_hash = robots[i].x * N + robots[i].y;
            auto it = nearest_berth.find(point_hash);

            if (nearest_berth.cend() == it && rest_invoke >= BERTH_NUM) // cache miss
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

                rest_invoke -= BERTH_NUM;
            }
            else // cache hit
            {
                robots[i].target_berth = it->second;
                robots[i].directions = cached_paths[point_pathidx[point_hash]]; // don't move, will be used later
            }
        }
    }
}

void Master::assignBoats()
{
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        if (!boats[i].status) // moving
            ;
        else if (1 == boats[i].status) // running well
        {
            if (-1 == boats[i].pos) // transport completed, at virtual point
            {
                boats[i].rest_capacity = boats[i].max_capacity;

                int berth_idx = -1;
                int weights[BERTH_NUM], max = 0;

                for (int j = 0; j < BERTH_NUM; ++j)
                {
                    if (berths[j].current_boat != -1 || berths[j].be_targeted ||
                        berths[j].waiting_boat != -1 || berths[j].piled_values.empty())
                    {
                        weights[j] = 0;
                        continue;
                    }

                    weights[j] = berths[j].total_value;

                    if (weights[j] > max)
                    {
                        max = weights[j];
                        berth_idx = j;
                    }
                }

                if (berth_idx != -1)
                {
                    boats[i].pos = berth_idx;
                    berths[berth_idx].be_targeted = 1;
                    log << "boat#" << i << " select berth succeed\n";
                }
                else
                    log << "boat#" << i << " select berth failed\n";

                log.flush();
            }
            else // loading
            {
                if (!boats[i].rest_capacity) // full
                {
                    berths[boats[i].pos].current_boat = berths[boats[i].pos].waiting_boat;
                    berths[boats[i].pos].waiting_boat = -1;
                    berths[boats[i].pos].be_targeted = 0;
                    boats[i].pos = -1; // this statement must be placed behind
                    log << "boat#" << i << " is ready to return virtual point\n";
                }
                else if (!berths[boats[i].pos].piled_values.empty()) // not full, berth has items, continue loading
                {
                    Berth &berth = berths[boats[i].pos];

                    int min = boats[i].rest_capacity < berth.piled_values.size()
                                  ? boats[i].rest_capacity
                                  : berth.piled_values.size();
                    int sub_value = std::accumulate(berth.piled_values.cbegin(), berth.piled_values.cbegin() + min, 0);

                    boats[i].rest_capacity -= min;
                    berth.total_value -= sub_value;
                    berth.piled_values.erase(berth.piled_values.begin(), berth.piled_values.begin() + min);

                    if (!boats[i].rest_capacity || berth.piled_values.empty()) // full or berth has no items, go to virtual point
                    {
                        berths[boats[i].pos].current_boat = berths[boats[i].pos].waiting_boat;
                        berths[boats[i].pos].waiting_boat = -1;
                        berths[boats[i].pos].be_targeted = 0;
                        boats[i].pos = -1; // this statement must be placed behind
                        log << "boat#" << i << " is ready to return virtual point\n";
                    }
                }
            }
        }
        else // waiting
            ;
    }
}

void Master::control()
{
    /* ROBOT */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status) // running well
        {
            if (!robots[i].task) // no tasks
                ;
            else if (!robots[i].directions.empty()) // has tasks && has found path
            {
                // detect collision
                // avoid collision
                printf("move %d %d\n", i, robots[i].directions.back());
#ifdef DEBUG_MODE
                log << "move " << i << ' ' << robots[i].directions.back() << '\n';
#endif
                robots[i].directions.pop_back();

                if (robots[i].directions.empty()) // has arrived
                {
                    if (1 == robots[i].task) // get
                    {
                        if (items[robots[i].target_item].life_span > 0)
                        {
                            printf("get %d\n", i);
#ifdef DEBUG_MODE
                            log << "get " << i << '\n';
#endif
                            robots[i].task = 2;
                        }
                        else // item disappear
                            robots[i].task = 0;
                    }
                    else // pull
                    {
                        printf("pull %d\n", i);
#ifdef DEBUG_MODE
                        log << "pull " << i << '\n';
#endif
                        robots[i].task = 0; // ...

                        int value = items[robots[i].target_item].value;
                        berths[robots[i].target_berth].total_value += value;
                        berths[robots[i].target_berth].piled_values.emplace_back(value);
                    }
                }
            }
        }
    }

    /* BOAT */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        if (1 == boats[i].status) // transport completed or loading
        {
            if (-1 == boats[i].pos) // go to virtual point
            {
                printf("go %d\n", i);
#ifdef DEBUG_MODE
                log << "go " << i << '\n';
#endif
            }
            else // go to berth
            {
                if (berths[boats[i].pos].current_boat != i)
                {
                    printf("ship %d %d\n", i, boats[i].pos);
#ifdef DEBUG_MODE
                    log << "ship " << i << ' ' << boats[i].pos << '\n';
#endif
                }
            }
        }
    }

    printf("OK\n");
    fflush(stdout);
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
            << " |pos: " << std::setw(2) << boats[i].pos
            << " |rest_capacity: " << std::setw(3) << boats[i].rest_capacity
            << " |max_capacity: " << std::setw(3) << boats[i].max_capacity << '\n';
    log.flush();
}

void Master::printBerths()
{
    for (int i = 0; i < BERTH_NUM; ++i)
        log << "Berth#" << std::setw(1) << i
            << " |x: " << std::setw(3) << berths[i].x
            << " |y: " << std::setw(3) << berths[i].y
            << " |transport_time: " << std::setw(4) << berths[i].transport_time
            << " |loading_speed: " << std::setw(1) << berths[i].loading_speed
            << " |total_value: " << std::setw(4) << berths[i].total_value
            << " |current_boat: " << std::setw(2) << berths[i].current_boat
            << " |waiting_boat: " << std::setw(2) << berths[i].waiting_boat
            << " |be_targeted: " << std::setw(1) << berths[i].be_targeted
            << " |piled_values.size(): " << std::setw(2) << berths[i].piled_values.size() << '\n';
    log.flush();
}

void Master::printItems()
{
    for (int i = 0; i < items.size(); ++i)
        log << "Items#" << std::setw(3) << i
            << " |x: " << std::setw(3) << items[i].x
            << " |y: " << std::setw(3) << items[i].y
            << " |value: " << std::setw(3) << items[i].value
            << " |life_span: " << std::setw(3) << items[i].life_span
            << " |is_selected: " << std::setw(3) << item_selected[i] << '\n';

    log.flush();
}

#endif