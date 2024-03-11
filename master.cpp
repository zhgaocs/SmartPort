#include "master.h"

void Master::init()
{
    int id, boat_capacity;
    char ok_cstr[3];

    /* init map */
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            std::cin >> map[i][j];

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
        std::cin >> id >> berths[id].x >> berths[id].y >> berths[id].transport_time >> berths[id].loading_speed;

    /* boat capacity */

    std::cin >> boat_capacity;
    for (int i = 0; i < BOAT_NUM; ++i)
        boats[i].capacity = boat_capacity;

    /* OK */
    std::cin >> ok_cstr;
    std::cout << "OK" << std::flush;
}

void Master::refresh()
{
    int frame_num, current_money; /* frame_num increase from 1 */
    char ok_cstr[3];

    std::cin >> frame_num >> current_money;

    /* new item */
    int K;
    std::cin >> K;
    for (int i = 0; i < K; ++i)
    {
        Item item;
        std::cin >> item.x >> item.y >> item.value;
        items.insert(item);
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

void Master::control()
{
}

namespace std
{
    template <>
    struct hash<std::pair<int, int>>
    {
        size_t operator()(const std::pair<int, int> &p) const
        {
            auto h1 = std::hash<int>{}(p.first);
            auto h2 = std::hash<int>{}(p.second);

            return h1 ^ (h2 << 1);
        }
    };
}

std::vector<std::pair<int, int>>
Master::findPath(int src_x, int src_y, int dst_x, int dst_y)
{
    struct CompareAStarNode
    {
        bool operator()(const AStarNode *lhs, const AStarNode *rhs) const
        {
            return lhs->f < rhs->f;
        }
    };

    std::multiset<AStarNode *, CompareAStarNode> open_set;
    std::unordered_set<std::pair<int, int>> close_set;
    std::vector<std::pair<int, int>> path;

    AStarNode *start = new AStarNode(src_x, src_y);
    start->g = 0, start->h = Manhattan(src_x, src_y, dst_x, dst_y);
    start->f = start->g + start->h;

    open_set.insert(start);

    while (!open_set.empty())
    {
        auto iter = open_set.begin();
        AStarNode *current = *iter;

        // if current is destination, then the path has been found
        if (current->x == dst_x && current->y == dst_y)
        {
            while (current)
            {
                path.push_back(std::make_pair(current->x, current->y));
                current = current->prev;
            }

            std::for_each(open_set.begin(), open_set.end(), [](AStarNode *node)
                          { delete node; });

            std::reverse(path.begin(), path.end());
            return path;
        }

        // move current from open_set to close_set
        open_set.erase(iter);
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
                                     [nx, ny](AStarNode *nodeptr)
                                     { return nx == nodeptr->x && ny == nodeptr->y; });

            int g = current->g + COST[i];
            AStarNode *neighbor = nullptr;

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
                neighbor = new AStarNode(nx, ny);
                neighbor->g = g, neighbor->h = Manhattan(nx, ny, dst_x, dst_y);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->prev = current;
                open_set.insert(neighbor);
            }
        }
    }

    return path;
}