#include "master.h"

int Master::frame_id;

void Master::init()
{
    char str[3];

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
    scanf("%d", &Boat::capacity);

    /* OK */
    scanf("%s", str);
    printf("OK\n");
    fflush(stdout);
}

void Master::run()
{
    update();
    assignRobots();
    assignBoats();
    control();
}

void Master::update()
{
    char okk[3];
    int current_money, new_items;

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

        if (boats[i].status && boats[i].pos != -1)
        {
            boats[i].target_pos = -2; // once arrived, update to -2
            berths[boats[i].pos].current_boat = i;
        }
    }

    /* all items */
    std::for_each(items.begin(), items.end(), [](Item &item)
                  { --item.life_span; });

    /* OK */
    scanf("%s", okk);
}

void Master::assignRobots()
{
    int rest_invoke = 100;

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
            if (rest_invoke >= BERTH_NUM)
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

                rest_invoke -= BERTH_NUM;
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
            if (-1 == boats[i].pos) // transport completed
            {
                boats[i].rest_capacity = Boat::capacity;
                boats[i].target_pos = -2;

                // select berth
                bool is_targeted = false;
                int berth_idx = -1, w, max_w = 0;

                if (frame_id == 1)
                    berth_idx = i;
                else
                {
                    for (int j = 0; j < BERTH_NUM; ++j)
                    {
                        if (berths[j].current_boat != -1 || berths[j].piled_values.empty())
                            continue;

                        for (const Boat &boat : boats)
                        {
                            if (boat.target_pos == j)
                            {
                                is_targeted = true;
                                break;
                            }
                        }

                        if (is_targeted)
                            continue;

                        w = -berths[j].transport_time - (berths[j].piled_values.size() << 4) + (berths[j].total_value << 11);

                        if (w > max_w)
                        {
                            max_w = w;
                            berth_idx = j;
                        }
                    }
                }

                if (berth_idx != -1)
                    boats[i].target_pos = berth_idx;
            }
            else // loading
            {
                if (frame_id + berths[boats[i].pos].transport_time < MAX_FRAME_ID)
                {
                    if (!boats[i].rest_capacity) // full
                        boats[i].target_pos = -1;
                    else if (!berths[boats[i].pos].piled_values.empty()) // not full && berth has items
                    {
                        Berth &berth = berths[boats[i].pos];

                        int min = Minimum_3(berth.loading_speed, boats[i].rest_capacity, berth.piled_values.size());
                        int sub_value = std::accumulate(berth.piled_values.cbegin(), berth.piled_values.cbegin() + min, 0);

                        boats[i].rest_capacity -= min;
                        berth.total_value -= sub_value;
                        berth.piled_values.erase(berth.piled_values.begin(), berth.piled_values.begin() + min);

                        if (!boats[i].rest_capacity) // full
                            boats[i].target_pos = -1;
                        else if (berths[boats[i].pos].piled_values.empty()) // not full && berth has no items
                        {
                            bool is_targeted = false;
                            int berth_idx = -1, w, max_w = 0;

                            for (int j = 0; j < BERTH_NUM; ++j)
                            {
                                if (berths[j].current_boat != -1 || berths[j].piled_values.empty())
                                    continue;

                                if (frame_id + TIME_BETWEEN_BERTH + berths[j].transport_time >= MAX_FRAME_ID)
                                    continue;

                                for (const Boat &boat : boats)
                                {
                                    if (boat.target_pos == j)
                                    {
                                        is_targeted = true;
                                        break;
                                    }
                                }

                                if (is_targeted)
                                    continue;

                                w = berths[j].total_value;

                                if (w > max_w)
                                {
                                    max_w = w;
                                    berth_idx = j;
                                }
                            }

                            if (berth_idx != -1)
                                boats[i].target_pos = berth_idx;
                        }
                    }
                    else // not full && berth has no items, but there is no other berth to go to
                        boats[i].target_pos = -1;
                }
                else
                    boats[i].target_pos = -1;

                berths[boats[i].pos].current_boat = -1;
            }
        }
        else
            ; // waiting outside
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
                robots[i].directions.pop_back();

                if (robots[i].directions.empty()) // has arrived
                {
                    if (1 == robots[i].task) // get
                    {
                        if (items[robots[i].target_item].life_span > 0)
                        {
                            printf("get %d\n", i);
                            robots[i].task = 2;
                        }
                        else // item disappear
                            robots[i].task = 0;
                    }
                    else // pull
                    {
                        printf("pull %d\n", i);
                        robots[i].task = 0;

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
        if (1 == boats[i].status)
        {
            if (-2 == boats[i].target_pos) // no select
                ;
            else if (-1 == boats[i].target_pos) // go to virtual point
                printf("go %d\n", i);
            else // move to berth
                printf("ship %d %d\n", i, boats[i].target_pos);
        }
    }

    printf("OK\n");
    fflush(stdout);
}