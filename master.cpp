#include "master.h"

void Master::init()
{
    char ok_str[3];
    int boat_capacity;

    /* init map */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            std::cin >> map[i][j];

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        int berth_id;
        std::cin >> berth_id;
        std::cin >> berths[berth_id].x >> berths[berth_id].y >> berths[berth_id].transport_time >> berths[berth_id].loading_speed;
    }

    /* boat capacity */
    std::cin >> boat_capacity;
    std::for_each(boats, boats + BOAT_NUM, [boat_capacity](Boat &b)
                  { b.capacity = boat_capacity; });

    /* OK */
    std::cin >> ok_str;
    std::cout << "OK" << std::flush;
}

void Master::update()
{
    char ok_str[3];
    int frame_num, current_money, new_items_cnt; /* frame_num increase from 1 */
    Item item;

    std::cin >> frame_num >> current_money;

    /* old item */
    std::for_each(items.begin(), items.end(), [](Item &item)
                  { item.life_span -= 1; });
    items.erase(std::remove_if(items.begin(), items.end(),
                               [](const Item &item)
                               { return item.life_span == 0; }),
                items.end());

    /* new item */
    std::cin >> new_items_cnt;
    for (int i = 0; i < new_items_cnt; ++i)
    {
        std::cin >> item.x >> item.y >> item.value;
        items.push_back(item);
    }

    /* robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
        std::cin >> robots[i].has_item >> robots[i].x >> robots[i].y >> robots[i].status;

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        std::cin >> boats[i].status >> boats[i].pos;

        if (boats[i].pos != -1)
            berths[boats[i].pos].boat_index = i;
    }

    /* OK */
    std::cin >> ok_str;
}

void Master::assignTasks()
{
    /*------------------------------------------------------------------------------------------------------*/
    typedef std::pair<int, int> IMPair; // I - Item's index in member items, M - manhattan distance to robot

    struct CompareIMPair // Max-heap, find k-min
    {
        bool operator()(const IMPair &lhs, const IMPair &rhs) const
        {
            return lhs.second < rhs.second;
        }
    };

    int nearest_num = 8; // the number of nearest items based on Manhattan distance
    std::priority_queue<IMPair, std::vector<IMPair>, CompareIMPair> queue;
    std::vector<std::pair<int, int>> path, shortest_path;
    /*------------------------------------------------------------------------------------------------------*/

    /* Robot */
    for (int i = 0; i < ROBOT_NUM && robots[i].status; ++i)
    {
        if (!robots[i].has_task) // no tasks
        {
            if (!robots[i].has_item) // no items, select item
            {
                for (int j = 0; j < items.size(); ++j)
                {
                    IMPair pair = std::make_pair(j, Manhattan(robots[i].x, robots[i].y, items[j].x, items[j].y));

                    if (queue.size() < nearest_num)
                        queue.push(pair);
                    else if (pair.second < queue.top().second)
                    {
                        queue.pop();
                        queue.push(pair);
                    }
                }

                int item_idx, current_dist, min_dist = INTEGER_MAX;
                IMPair pair;

                while (!queue.empty())
                {
                    pair = queue.top();

                    path = FindPath(map, robots[i].x, robots[i].y, items[pair.first].x, items[pair.first].y);

                    if ((current_dist = path.size()) && current_dist < min_dist && current_dist - 1 < items[pair.first].life_span) // accessible
                    {
                        item_idx = pair.first;
                        min_dist = current_dist;
                        shortest_path = path;
                    }
                    queue.pop();
                }

                if (INTEGER_MAX != min_dist) // accessible
                {
                    Item item = items[item_idx];
                    robots[i].has_task = 1;
                    robots[i].target_value = item.value;
                    robots[i].target_lifespan = item.life_span;
                    robots[i].path = Path2Directions(shortest_path);
                    items.erase(items.begin() + item_idx);
                }
            }
        }
        else if (robots[i].has_item && robots[i].path.empty()) // find berth
        {
            int berth_idx, current_dist, min_dist = INTEGER_MAX;
            std::vector<std::pair<int, int>> vec_p, path, shortest_path;

            for (int j = 0; j < BERTH_NUM; ++j)
            {
                vec_p = findBerthPoint(j);

                for (int k = 0; k < vec_p.size(); ++k)
                {
                    path = FindPath(map, robots[i].x, robots[i].y, vec_p[k].first, vec_p[k].second);

                    if ((current_dist = path.size()) && current_dist < min_dist)
                    {
                        berth_idx = j;
                        min_dist = current_dist;
                        shortest_path = path;
                    }
                }
            }

            if (INTEGER_MAX != min_dist)
            {
                robots[i].target_berth = berth_idx;
                robots[i].path = Path2Directions(shortest_path);
            }
            // robots[i] cannot find berth ???
        }
    }

    /* Boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        if (0 == boats[i].status) // on the way to virtual point or berth
        {
            if (boats[i].target_pos != -2 && -1 == boats[i].target_pos)
                boats[i].target_pos = -2;
        }
        else if (1 == boats[i].status)
        {
            if (-1 == boats[i].pos && -2 == boats[i].target_pos) // virtual point, go to one berth
            {
                int berth_idx, w, min_w = INTEGER_MAX;

                for (int j = 0; j < BERTH_NUM; ++j)
                {
                    if (-1 == berths[j].boat_index) // no boats in berth
                        w = berths[j].transport_time - berths[j].loading_speed - berths[j].piled_items.size() - berths[j].total_value;
                    else
                    {
                        bool will_two_boats = false;

                        for (int k = 0; k < BOAT_NUM; ++k)
                        {
                            if (j == boats[k].target_pos)
                            {
                                will_two_boats = true;
                                break;
                            }
                        }

                        if (will_two_boats)
                            continue;

                        /* may have two boats */
                        int boat_capacity = boats[berths[j].boat_index].capacity,
                            rest_items_cnt = berths[j].piled_items.size() - boat_capacity;

                        if (rest_items_cnt <= 0) // one boat can load it all
                            w = berths[j].transport_time - berths[j].loading_speed;
                        else // may need two boats
                        {
                            int rest_value = std::accumulate(berths[j].piled_items.cbegin() + boat_capacity, berths[j].piled_items.cend() + rest_items_cnt, 0);
                            w = berths[j].transport_time - berths[j].loading_speed - rest_items_cnt - rest_value;
                        }
                    }

                    if (w < min_w)
                    {
                        min_w = w;
                        berth_idx = j;
                    }
                }

                if (min_w != INTEGER_MAX)
                    boats[i].target_pos = berth_idx;
            }
            else // in berth, loading
            {
                if (!boats[i].capacity) // full,  go to virtual point
                {
                    boats[i].target_pos = -1;
                    berths[boats[i].pos].boat_index = -1;
                }
                else // not full
                {
                    Berth &berth = berths[boats[i].pos];

                    int min = Min(boats[i].capacity, berth.loading_speed, berth.piled_items.size());
                    int sub_value = std::accumulate(berth.piled_items.cbegin(), berth.piled_items.cbegin() + min, 0);

                    boats[i].capacity -= min;
                    berth.total_value -= sub_value;
                    berth.piled_items.erase(berth.piled_items.begin(), berth.piled_items.begin() + min);
                }
            }
        }
        else
        {
        }
    }
}

void Master::control()
{
    /* Robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status && robots[i].has_task)
        {
            if (!robots[i].has_item) // on the way
            {
                if (robots[i].target_lifespan < robots[i].path.size())
                {
                    robots[i].has_task = false;
                    continue;
                }
                else
                    --robots[i].target_lifespan;
            }

            std::cout << "move " << i << ' ' << robots[i].path.back() << '\n';
            robots[i].path.pop_back();

            if (robots[i].path.empty())
            {
                if (robots[i].has_item) // pull
                {
                    std::cout << "pull " << i << '\n';

                    berths[robots[i].target_berth].piled_items.push_back(robots[i].target_value);
                    berths[robots[i].target_berth].total_value += robots[i].target_value;
                    robots[i].has_task = false;
                }
                else // get
                {
                    std::cout << "get " << i << '\n';
                    robots[i].has_item = true;
                }
            }
        }
    }

    /* Boat */
    for (int i = 0; i < BOAT_NUM && 1 == boats[i].status; ++i)
    {
        if (boats[i].pos != boats[i].target_pos)
        {
            if (-1 == boats[i].pos) // go to berth
                std::cout << "ship " << i << ' ' << boats[i].target_pos << '\n';
            else // go to virtual point
                std::cout << "go" << i << '\n';
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

std::vector<std::pair<int, int>> Master::findBerthPoint(int berth_idx)
{
    int x = berths[berth_idx].x, y = berths[berth_idx].y;
    std::vector<std::pair<int, int>> vec_p;

    // berth above
    if (x)
        for (int i = 0; i < BERTH_SIZE; ++i)
            if (PATHWAY_SYMBOL == map[x - 1][y + i])
                vec_p.push_back(std::make_pair(x, y + i));

    // berth below
    if (x + 1 != N)
        for (int i = 0; i < BERTH_SIZE; ++i)
            if (PATHWAY_SYMBOL == map[x + BERTH_SIZE][y + i])
                vec_p.push_back(std::make_pair(x + BERTH_SIZE - 1, y + i));

    // berth left
    if (y)
        for (int i = 0; i < BERTH_SIZE; ++i)
            if (PATHWAY_SYMBOL == map[x + i][y - 1])
                vec_p.push_back(std::make_pair(x + i, y));

    // berth right
    if (y + 1 != N)
        for (int i = 0; i < BERTH_SIZE; ++i)
            if (PATHWAY_SYMBOL == map[x + i][y + BERTH_SIZE])
                vec_p.push_back(std::make_pair(x + i, y + BERTH_SIZE - 1));

    return vec_p;
}