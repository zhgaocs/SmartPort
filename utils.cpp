#include "utils.h"

int FindPath(const char (&map)[N][N], int src_x, int src_y, int dst_x, int dst_y, std::vector<std::pair<int, int>> &path)
{
    /* ------------------------------------------------------------------------------------------- */
    struct Node
    {
        int x, y;
        int g, h, f;
        Node *prev;

        Node(int x, int y)
            : x(x), y(y), g(INTEGER_MAX), h(0), f(INTEGER_MAX), prev(nullptr)
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
    std::vector<Node *> close_vec;

    Node *start = new Node(src_x, src_y);
    start->g = 0, start->h = Manhattan(src_x, src_y, dst_x, dst_y);
    start->f = start->g + start->h;

    path.clear();
    open_set.emplace(start);

    while (!open_set.empty())
    {
        auto it = open_set.begin();
        Node *current = *it;

        if (close_set.size() == MAX_EXPLORED_NODES)
        {
            std::for_each(open_set.begin(), open_set.end(), [](Node *node)
                          { delete node; });
            break;
        }

        // if current is destination, then the path has been found
        if (current->x == dst_x && current->y == dst_y)
        {
            while (current)
            {
                path.emplace_back(current->x, current->y);
                current = current->prev;
            }

            std::for_each(open_set.begin(), open_set.end(), [](Node *node)
                          { delete node; });

            /* std::reverse(path.begin(), path.end()); */
            break;
        }

        // move current from open_set to close_set
        open_set.erase(it);
        close_vec.emplace_back(current);
        close_set.emplace(current->x, current->y);

        for (int i = 0; i < NUM_OF_DIRECTIONS; ++i)
        {
            int nx = current->x + DX[i];
            int ny = current->y + DY[i];

            if (nx < 0 || ny < 0 || nx + 1 > N || ny + 1 > N ||
                (map[nx][ny] != PATHWAY_SYMBOL && map[nx][ny] != BERTH_SYMBOL))
                continue;

            if (close_set.end() != close_set.find(std::make_pair(nx, ny))) // if neighbor is in close_set
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
                open_set.emplace(neighbor);
            }
        }
    }

    std::for_each(close_vec.begin(), close_vec.end(), [](Node *nodeptr)
                  { delete nodeptr; });

    if (path.empty())
        return -1;
    else
        return 0;
}

int Path2Directions(const std::vector<std::pair<int, int>> &reverse_path, std::vector<int> &directions)
{
    if (reverse_path.empty())
        return -1;

    int tmp;
    directions.resize(reverse_path.size() - 1);

    for (int i = 0; i < directions.size(); ++i)
    {
        if ((tmp = reverse_path[i + 1].first - reverse_path[i].first)) // x
            directions[i] = tmp > 0 ? UP : DOWN;
        else if ((tmp = reverse_path[i + 1].second - reverse_path[i].second)) // y
            directions[i] = tmp > 0 ? LEFT : RIGHT;
    }
    
    return 0;
}