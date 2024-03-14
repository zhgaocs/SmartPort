#include "master.h"

Master::Master()
{
    std::for_each(has_tasks, has_tasks + ROBOT_NUM, [](bool b)
                  { b = false; });
}

void Master::init()
{
    int boat_capacity;
    char ok_cstr[3];

    /* init map */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            std::cin >> map[i][j];

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
        std::cin >> berths[i].id >> berths[i].x >> berths[i].y >> berths[i].transport_time >> berths[i].loading_speed;

    /* boat capacity */
    std::cin >> boat_capacity;
    for (int i = 0; i < BOAT_NUM; ++i)
        boats[i].capacity = boat_capacity;

    /* OK */
    std::cin >> ok_cstr;
    std::cout << "OK" << std::flush;
}

void Master::update()
{
    int frame_num, current_money; /* frame_num increase from 1 */
    char ok_cstr[3];

    std::cin >> frame_num >> current_money;

    /* old item */
    std::for_each(items.begin(), items.end(), [](Item &item)
                  { item.life_span -= 1; });
    items.erase(std::remove_if(items.begin(), items.end(),
                               [](const Item &item)
                               { return item.life_span == 0; }),
                items.end());

    /* new item */
    int new_item_cnt;
    std::cin >> new_item_cnt;
    for (int i = 0; i < new_item_cnt; ++i)
    {
        Item item;
        std::cin >> item.x >> item.y >> item.value;
        item.life_span = ITEM_MAX_LIFESPAN;
        items.push_back(item);
    }

    /* robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
        std::cin >> robots[i].has_item >> robots[i].x >> robots[i].y >> robots[i].status;

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
        std::cin >> boats[i].status >> boats[i].target_berth_id;

    /* OK */
    std::cin >> ok_cstr;
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
    /*------------------------------------------------------------------------------------------------------*/

    int nearest_num = 8; // the number of nearest items based on Manhattan distance
    std::priority_queue<IMPair, std::vector<IMPair>, CompareIMPair> queue;
    std::vector<std::pair<int, int>> path, shortest_path;

    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (robots[i].status)
        {
            if (!has_tasks[i]) // no tasks
            {
                if (!robots[i].has_item) // no items - select item
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

                    int closest_idx, current_dist, min_dist = INT_MAX;
                    IMPair pair;

                    while (!queue.empty())
                    {
                        pair = queue.top();

                        path = findPath(robots[i].x, robots[i].y, items[pair.first].x, items[pair.first].y);

                        if ((current_dist = path.size()) && current_dist < min_dist && current_dist - 1 < items[pair.first].life_span) // accessible
                        {
                            closest_idx = pair.first;
                            min_dist = current_dist;
                            shortest_path = path;
                        }
                        queue.pop();
                    }

                    if (INT_MAX != min_dist) // accessible
                    {
                        has_tasks[i] = true;
                        paths[i] = path2Directions(shortest_path);
                        items.erase(items.begin() + closest_idx);
                    }
                }
            }
            else // has tasks
            {
                if (!robots[i].has_item) // no items - on the way
                    ;                    // nothing to do
                else                     // has item, select berth
                {
                }
            }
        }
    }
}

void Master::controlOutput() const
{
    /* Robot */
    for (int i = 0; i < ROBOT_NUM; ++i)
    {
        if (has_tasks[i])
        {
            // TODO
        }
    }

    /* Ship */
    // TODO
    std::cout << "OK" << std::flush;
}

std::vector<std::pair<int, int>> Master::findPath(int src_x, int src_y, int dst_x, int dst_y)
{
    /* ------------------------------------------------------------------------------------------- */
    static constexpr char PATHWAY_SYMBOL = '.';
    static constexpr int NUM_OF_DIRECTIONS = 4;
    static constexpr int DX[NUM_OF_DIRECTIONS] = {-1, 1, 0, 0};
    static constexpr int DY[NUM_OF_DIRECTIONS] = {0, 0, -1, 1};
    static constexpr int COST[NUM_OF_DIRECTIONS] = {1, 1, 1, 1};

    struct Node
    {
        int x, y;
        int g, h, f;
        Node *prev;

        Node(int x, int y)
            : x(x), y(y), g(INT_MAX), h(0), f(INT_MAX), prev(nullptr)
        {
        }
    };

    struct CompareNode
    {
        bool operator()(const Node *lhs, const Node *rhs) const
        {
            return lhs->f < rhs->f;
        }
    };
    /* ------------------------------------------------------------------------------------------- */

    std::multiset<Node *, CompareNode> open_set;
    std::unordered_set<std::pair<int, int>> close_set;
    std::vector<Node *> close_vec; // used to store pointers that are new and are not in open_set
    std::vector<std::pair<int, int>> path;

    Node *start = new Node(src_x, src_y);
    start->g = 0, start->h = Manhattan(src_x, src_y, dst_x, dst_y);
    start->f = start->g + start->h;

    open_set.insert(start);

    while (!open_set.empty())
    {
        auto iter = open_set.begin();
        Node *current = *iter;

        // if current is destination, then the path has been found
        if (current->x == dst_x && current->y == dst_y)
        {
            while (current)
            {
                path.push_back(std::make_pair(current->x, current->y));
                current = current->prev;
            }

            std::for_each(open_set.begin(), open_set.end(), [](Node *node)
                          { delete node; });

            /* std::reverse(path.begin(), path.end()); */

            break;
        }

        // move current from open_set to close_set
        open_set.erase(iter);
        close_vec.push_back(current);
        close_set.insert(std::make_pair(current->x, current->y));

        for (int i = 0; i < 4; ++i)
        {
            int nx = current->x + DX[i];
            int ny = current->y + DY[i];

            if (nx < 0 || ny < 0 || nx + 1 > N || ny + 1 > N || map[nx][ny] != PATHWAY_SYMBOL)
                continue;

            // if neighbor is in close_set
            if (close_set.end() != close_set.find(std::make_pair(nx, ny)))
                continue;

            auto iter = std::find_if(open_set.begin(), open_set.end(),
                                     [nx, ny](Node *nodeptr)
                                     {
                                         return nx == nodeptr->x && ny == nodeptr->y;
                                     });

            int g = current->g + COST[i];
            Node *neighbor = nullptr;

            if (open_set.end() != iter) // in open_set
            {
                neighbor = *iter;
                if (g < neighbor->g)
                {
                    neighbor->g = g;
                    neighbor->f = g + neighbor->h;
                    neighbor->prev = current;
                }
            }
            else
            {
                neighbor = new Node(nx, ny);
                neighbor->g = g, neighbor->h = Manhattan(nx, ny, dst_x, dst_y);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->prev = current;
                open_set.insert(neighbor);
            }
        }
    }

    std::for_each(close_vec.begin(), close_vec.end(), [](Node *nodeptr)
                  { delete nodeptr; });

    return path;
}

std::vector<int> Master::path2Directions(const std::vector<std::pair<int, int>> &path)
{
    // path.empty() == false
    int tmp;
    std::vector<int> vec_d(path.size() - 1);

    for (int i = 0; i < path.size() - 1; ++i)
    {
        if ((tmp = path[i + 1].first - path[i].first)) // x
            vec_d[i] = tmp > 0 ? 2 : 3;
        else // y
            vec_d[i] = tmp > 0 ? 1 : 0;
    }
    return vec_d;
}