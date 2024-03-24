#include "master.h"

int Master::frame_id;

void Master::init()
{
    char str[3];
    int boat_capacity;
    memset(point_has_item, -1, sizeof(point_has_item));

    std::vector<std::pair<int, int>> berth_transtime(BERTH_NUM);

    for (int i = 0; i < N; ++i)
        fgets(map[i], N + 2, stdin);

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berths[id].x, &berths[id].y, &berths[id].transport_time, &berths[id].loading_speed);

        berth_transtime[i] = {berths[id].transport_time, id};

        berths[id].latest_return = MAX_FRAME_ID - berths[id].transport_time;
    }

    std::sort(berth_transtime.begin(), berth_transtime.end());
    for (int i = 0; i < BOAT_NUM; ++i)
        trans_fast_berth[i] = berth_transtime[i].second;

    /* boat capacity */
    scanf("%d", &boat_capacity);
    std::for_each(boats, boats + BOAT_NUM, [boat_capacity](Boat &b)
                  { b.max_capacity = b.rest_capacity = boat_capacity; });

    preprocess();
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

void Master::preprocess()
{
    struct node
    {
        int x, y, idx, dis;
    };

    node q[N * N + 100];
    int hh = 0, tt = -1;
    for (int i = 0; i < BERTH_NUM; i++)
    {
        int x = berths[i].x;
        int y = berths[i].y;
        q[++tt] = {x, y, i, 0};
    }
    while (hh <= tt)
    {
        node t = q[hh++];
        for (int i = 0; i < 4; i++)
        {
            int nx = t.x + DX[i];
            int ny = t.y + DY[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N || map[nx][ny] != PATHWAY_SYMBOL)
                continue;
            int ne_hs = nx * N + ny;
            if (nearest_berth.count(ne_hs))
                continue;
            nearest_berth[ne_hs] = t.idx;
            nearest_berth_dis[ne_hs] = t.dis + 1;
            q[++tt] = {nx, ny, t.idx, t.dis + 1};
        }
    }
}

int pre_items = 0;

void Master::update()
{
    rest_invoke = 30;
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
        int idx = items.size() - 1;
        point_has_item[x][y] = idx;
        heap.push({value, idx});
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
    pre_items += new_items;
    if (frame_id > ITEM_LIFESPAN_MAX)
    {
        std::for_each(items.begin() + pre_items, items.end(), [](Item &item)
                      { --item.life_span; });
    }
    else
    {
        std::for_each(items.begin(), items.end(), [](Item &item)
                      { --item.life_span; });
    }

    /* OK */
    scanf("%s", okk);
}

int compare_value_p(int dis1, int value1, int dis2, int value2)
{
    return static_cast<double>(value1 * 1.0 / dis1) >
           static_cast<double>(value2 * 1.0 / dis2);
}

int Master::robot_scan(int i, std::vector<int> &directions)
{
    int max_value = 0, max_dis = 1, max_item = -1;
    std::vector<int> max_directions;
    for (int xx = robots[i].x - DETECT_RADIUS; xx <= robots[i].x + DETECT_RADIUS; xx++) // greater index, greater life_span
    {
        if (xx < 0 || xx >= N)
            continue;
        for (int yy = robots[i].y - DETECT_RADIUS; yy <= robots[i].y + DETECT_RADIUS; yy++)
        {
            if (yy < 0 || yy >= N)
                continue;
            int j = point_has_item[xx][yy];
            if (j == -1 || item_selected[j])
                continue;
            if (FindShortDirections(robots[i].x, robots[i].y, xx, yy, max_directions, LIMIT) == -1)
            {
                continue;
            }
            int value = items[j].value, dis = max_directions.size() + nearest_berth_dis[xx * N + yy];
            if (directions.size() < items[j].life_span && compare_value_p(dis, value, max_dis, max_value))
            {
                max_item = j;
                max_value = value;
                max_dis = dis;
                directions = max_directions;
            }
        }
    }
    return max_item;
}

void Master::assignRobots()
{

    for (int i = 0; i < ROBOT_NUM && rest_invoke; ++i)
    {
        if (robots[i].is_dead)
            continue;        // the robot is dead
        if (!robots[i].task) // no tasks, select item
        {
            if (robots[i].status) // running well
            {
                std::vector<int> directions_around;
                std::vector<int> directions_max;
                int item_idx = -1;
                item_idx = robot_scan(i, directions_around);
                int item_dis = 1;
                int item_value = 0;
                if (item_idx != -1)
                {
                    item_value = items[item_idx].value;
                    int hs = items[item_idx].x * N + items[item_idx].y;
                    item_dis = directions_around.size() + nearest_berth_dis[hs];
                }
                // find global max
                while (heap.size() && item_selected[heap.top().second])
                {
                    heap.pop();
                }
                if (heap.size())
                {
                    auto max_item = heap.top();
                    int idx = max_item.second;
                    int value = max_item.first;
                    int dis = 0x3f3f3f3f;
                    if (rest_invoke && FindPathDirections(robots[i].x, robots[i].y, items[idx].x, items[idx].y, directions_max))
                    {
                        if (directions_max.size() < items[idx].life_span)
                        {
                            dis = directions_max.size();
                        }
                    }
                    else if (rest_invoke <= 0)
                    {
                        continue;
                    }
                    if (compare_value_p(dis, value, item_dis, item_value))
                    {
                        item_idx = idx;
                        robots[i].directions = directions_max;
                    }
                    else
                    {
                        robots[i].directions = directions_around;
                    }
                }

                if (item_idx == -1) // cannot find an item
                {
                    continue;
                }
                int hs = items[item_idx].x * N + items[item_idx].y;
                robots[i].target_berth = nearest_berth[hs];
                robots[i].target_item = item_idx;
                item_selected[item_idx] = true;
                robots[i].task = 1;
            }
        }

        else if (1 == robots[i].task)
        {
            std::vector<int> directions;
            int item_idx = robot_scan(i, directions);
            if (item_idx != -1)
            {
                int hs = items[item_idx].x * N + items[item_idx].y;
                int item_dis = directions.size() + nearest_berth_dis[hs];
                int item_value = items[item_idx].value;
                int cur_dis = robots[i].directions.size() + nearest_berth_dis[items[robots[i].target_item].x * N + items[robots[i].target_item].y];
                int cur_value = items[robots[i].target_item].value;
                if (compare_value_p(item_dis, item_value, cur_dis, cur_value))
                {
                    item_selected[robots[i].target_item] = false;
                    robots[i].target_item = item_idx;
                    robots[i].target_berth = nearest_berth[hs];
                    robots[i].directions = directions;
                    item_selected[item_idx] = true;
                }
            }
        }
        else if (2 == robots[i].task && robots[i].directions.empty()) // select berth, include recover
        {
            int point_hash = robots[i].x * N + robots[i].y;
            robots[i].target_berth = nearest_berth[point_hash];
            if (rest_invoke)
                FindPathDirections(robots[i].x, robots[i].y, berths[robots[i].target_berth].x, berths[robots[i].target_berth].y, robots[i].directions);
            else
                continue;
            nearest_berth_dis[point_hash] = robots[i].directions.size();
        }
        else if (robots[i].directions.size() + frame_id + berths[robots[i].target_berth].piled_values.size() / berths[robots[i].target_berth].loading_speed > berths[robots[i].target_berth].latest_return)
        {
            int ids[ROBOT_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::shuffle(ids, ids + 10, std::default_random_engine(seed));
            for (int i = 0; i < BERTH_NUM; i++)
            {
                int j = ids[i];
                if (i == j)
                    continue;
                std::vector<int> directions;
                if (rest_invoke && FindPathDirections(robots[i].x, robots[i].y, berths[j].x, berths[j].y, directions))
                {
                    if (directions.size() + frame_id + berths[j].piled_values.size() / berths[j].loading_speed < berths[j].latest_return)
                    {
                        robots[i].target_berth = j;
                        robots[i].directions = directions;
                        break;
                    }
                }
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
                boats[i].rest_capacity = boats[i].max_capacity;
                boats[i].target_pos = -2;

                // select berth
                bool is_targeted = false;
                int berth_idx = -1, w, max_w = 0;

                if (frame_id == 1)
                    berth_idx = trans_fast_berth[i];
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
                if (frame_id < berths[boats[i].pos].latest_return)
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

                                if (frame_id + TIME_BETWEEN_BERTH >= berths[j].latest_return)
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

// int next_pos[ROBOT_NUM];

void Master::control()
{
    auto t_start = std::chrono::high_resolution_clock::now();

    /* ROBOT */
    std::vector<std::pair<int, int>> next_pos(10);
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        std::pair<int, int> cur_pos = {robots[i].x, robots[i].y};
        if (robots[i].directions.empty())
        {
            next_pos[i] = cur_pos;
        }
        else
        {
            get_next_pos(next_pos[i], cur_pos, robots[i].directions.back());
        }
    }

    std::unordered_set<int> collision_robots;
    int collision_robots_num = has_collision(collision_robots, next_pos);
    if (collision_robots_num > 5)
    {
        int cnt = collision_robots_num / 2;
        int robot_id = -1;
        while (cnt--)
        {
            robot_id = *collision_robots.begin();
            robots[robot_id].is_stoped = 1;
            next_pos[robot_id] = {robots[robot_id].x, robots[robot_id].y};
            collision_robots.erase(collision_robots.begin());
        }
    }

    if (collision_robots_num)
    {
        collision_avoidance(collision_robots, next_pos);
    }

    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status) // running well
        {
            // if (!robots[i].task) // no tasks
            //     ;
            if (!robots[i].directions.empty()) // has tasks && has found path
            {
                if (robots[i].is_stoped == 1)
                {
                    robots[i].is_stoped = 0;
                    continue;
                }

                // do move
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
                        robots[i].task = 0; // ...

                        int value = items[robots[i].target_item].value;
                        berths[robots[i].target_berth].total_value += value;
                        berths[robots[i].target_berth].piled_values.emplace_back(value);
                    }
                }
            }
        }
    }

    for (int i = 0; i < BOAT_NUM; ++i)
    {
        if (1 == boats[i].status)
        {
            if (-2 == boats[i].target_pos) // no select
                ;
            else if (-1 == boats[i].target_pos) // go to virtual point
            {
                printf("go %d\n", i);
            }
            else // move to berth
            {
                printf("ship %d %d\n", i, boats[i].target_pos);
            }
        }
    }

    printf("OK\n");
    fflush(stdout);
}

// Detect for collisions and return the number of robots that will collide, need robots' next position
int Master::has_collision(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos)
{
    collision_robots.clear();
    int ids[ROBOT_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(ids, ids + 10, std::default_random_engine(seed));
    for (int k = 0; k < ROBOT_NUM; k++)
    {
        int i = ids[k];
        for (int l = 0; l < ROBOT_NUM; l++)
        {
            int j = ids[l];
            if (i == j)
                continue;
            std::pair<int, int> pi_1 = {robots[i].x, robots[i].y};
            std::pair<int, int> pi_2 = next_pos[i];
            std::pair<int, int> pj_1 = {robots[j].x, robots[j].y};
            std::pair<int, int> pj_2 = next_pos[j];

            int pi_hash = point_hash(pi_2);
            int ei_hash = edge_hash(pi_1, pi_2);
            int pj_hash = point_hash(pj_2);
            int ej_hash = edge_hash(pj_1, pj_2);
            if (pi_hash == pj_hash || ei_hash == ej_hash)
            {
                collision_robots.emplace(i);
                collision_robots.emplace(j);
            }

            // if (pi_2 == pj_2 || pi_1 == pj_2)
            // {
            //     collision_robots.emplace(i);
            //     collision_robots.emplace(j);
            // }
        }
    }

    return collision_robots.size();
}

// [a edge:((x1, y1)->(x2, y2))] -> int
int Master::edge_hash(std::pair<int, int> &p1, std::pair<int, int> &p2)
{
    if (p1 > p2)
    {
        std::pair<int, int> t = p1;
        p1 = p2;
        p2 = t;
    }
    int res = 0;
    res = p1.first * N + p1.second;
    res = res * N + p2.first;
    res = res * N + p2.second;
    return res;
}

void Master::get_next_pos(std::pair<int, int> &ne, std::pair<int, int> &cur, int dir)
{
    if (dir == -1)
        ne = cur;
    if (dir == RIGHT)
        ne = {cur.first, cur.second + 1};
    else if (dir == LEFT)
        ne = {cur.first, cur.second - 1};
    else if (dir == UP)
        ne = {cur.first - 1, cur.second};
    else if (dir == DOWN)
        ne = {cur.first + 1, cur.second};
}

bool collision_flag = false;
int cnt = 0;
bool vis[ROBOT_NUM];
void Master::collision_avoidance(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos)
{
    std::vector<int> robs;

    // collision robots first in DFS
    cnt = 0;
    for (int i = 0; i < ROBOT_NUM; i++)
        if (collision_robots.count(i))
            robs.emplace_back(i); // is collision robots
    for (int i = 0; i < ROBOT_NUM; i++)
        if (!collision_robots.count(i))
            robs.emplace_back(i); // not collision robots

    collision_flag = true;
    int dx[] = {-1, 1, 0, 0, 0};
    int dy[] = {0, 0, -1, 1, 0};
    memset(vis, 0, sizeof(vis));
    std::vector<std::pair<int, int>> res(ROBOT_NUM);

    collision_solver(collision_robots, next_pos, robs, 0, dx, dy, res);

    if (cnt >= 500)
    {
        collision_solver(collision_robots, next_pos, robs, 0, dx, dy, res);
    }
    // replanning paths for robots changed
    for (int i = 0; i < ROBOT_NUM; i++)
    {
        std::pair<int, int> ne_new = res[i];
        int ne_dir = -1;
        if (robots[i].directions.size())
        {
            ne_dir = robots[i].directions.back();
        }
        std::pair<int, int> ne;
        std::pair<int, int> cur = {robots[i].x, robots[i].y};
        get_next_pos(ne, cur, ne_dir);

        if (ne_new != ne)
        {
            if (ne_new == cur)
            {
                // stop 1 frame
                robots[i].is_stoped = 1;
            }
            else
            {
                // get dst
                int dst_x = robots[i].x, dst_y = robots[i].y;
                if (robots[i].task == 1)
                { // get item
                    Item item = items[robots[i].target_item];
                    dst_x = item.x;
                    dst_y = item.y;
                }
                else if (robots[i].task == 2)
                { // pull item
                    Berth berth = berths[robots[i].target_berth];
                    dst_x = berth.x;
                    dst_y = berth.y;
                }
                robots[i].directions.clear();
                if (!(dst_x == robots[i].x && dst_y == robots[i].y) && !robots[i].is_dead)
                {
                    FindPathDirections(ne_new.first, ne_new.second, dst_x, dst_y, robots[i].directions);
                }
                // add next direction
                int dir, tmp;
                if ((tmp = robots[i].x - ne_new.first)) // x
                    dir = tmp > 0 ? UP : DOWN;
                else if ((tmp = robots[i].y - ne_new.second)) // y
                    dir = tmp > 0 ? LEFT : RIGHT;
                robots[i].directions.emplace_back(dir);
            }
        }
    }
}

// DFS, solution is stored in res
void Master::collision_solver(std::unordered_set<int> &collision_robots, std::vector<std::pair<int, int>> &next_pos,
                              std::vector<int> &robs, int cur, int dx[], int dy[], std::vector<std::pair<int, int>> &res)
{
    cnt++;
    if (cnt >= 500)
    {
        return;
    }
    if (!collision_flag)
        return;
    if (!has_collision(collision_robots, next_pos))
    {
        for (int i = 0; i < ROBOT_NUM; i++)
        {
            res[i] = next_pos[i];
        }
        collision_flag = false;
        return;
    }

    if (cur == collision_robots.size())
    {
        if (!has_collision(collision_robots, next_pos))
        {
            // log << "dfs success" << "\n";
            for (int i = 0; i < ROBOT_NUM; i++)
            {
                res[i] = next_pos[i];
                // log << "dfs: " << i << " " << next_pos[i].first << " " << next_pos[i].second << '\n';
            }
            collision_flag = false;
        }
        // collision_flag = false;
        return;
    }
    int ids[5] = {0, 1, 2, 3, 4};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(ids, ids + 5, std::default_random_engine(seed));
    for (int i = 0; i < 5; i++)
    {

        // vis[i] = true;
        int id = robs[cur];

        int x = robots[id].x, y = robots[id].y;
        std::pair<int, int> p = next_pos[id];
        int nx = x + dx[ids[i]], ny = y + dy[ids[i]];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N || (map[nx][ny] != PATHWAY_SYMBOL && map[nx][ny] != BERTH_SYMBOL))
            continue;
        next_pos[id] = {nx, ny};
        collision_solver(collision_robots, next_pos, robs, cur + 1, dx, dy, res);
        next_pos[id] = p;
    }
}