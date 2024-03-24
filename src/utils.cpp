#include "utils.h"

char map[N][N_PLUS_TWO];
std::unordered_map<int, int> nearest_berth;
std::unordered_map<int, int> point_pathidx;
std::vector<int> cached_paths[CACHED_PATH_MAX_SIZE];
std::unordered_map<int, int> nearest_berth_dis;
int rest_invoke;

// return 0 - no path, 1 - path found
int FindPathDirections(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions)
{
    rest_invoke--;
    directions.clear();

    std::pair<int, int> pre[N + 10][N + 10];
    std::pair<int, int> queue[N * N + 10];
    int front = 0, rear = -1;
    int dis[N + 10][N + 10];
    std::vector<std::pair<int, int>> path;
    memset(dis, -1, sizeof(dis));
    dis[src_x][src_y] = 0;
    queue[++rear] = {src_x, src_y};
    pre[src_x][src_y] = {-1, -1};

    // bfs
    while (front <= rear)
    {
        std::pair<int, int> cur = queue[front++];

        if (cur.first == dst_x && cur.second == dst_y)
        {

            int step = dis[dst_x][dst_y];
            int x = dst_x, y = dst_y;
            while (x != -1 || y != -1)
            {
                path.emplace_back(x, y);
                std::pair<int, int> p = pre[x][y];
                x = p.first;
                y = p.second;
            }

            // path -> directions
            Path2Directions(directions, path);
            return 1;
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = cur.first + DX[i];
            int ny = cur.second + DY[i];
            if (nx < 0 || ny < 0 || nx + 1 > N || ny + 1 > N || (map[nx][ny] != PATHWAY_SYMBOL && map[nx][ny] != BERTH_SYMBOL) || dis[nx][ny] != -1)
                continue;
            pre[nx][ny] = cur;
            dis[nx][ny] = dis[cur.first][cur.second] + 1;
            queue[++rear] = {nx, ny};
        }
    }
    return 0;
}

int FindShortDirections(int src_x, int src_y, int dst_x, int dst_y, std::vector<int> &directions, int limit)
{
    directions.clear();

    std::pair<int, int> pre[N + 10][N + 10];
    std::pair<int, int> queue[N * N + 10];
    int front = 0, rear = -1;
    int dis[N + 10][N + 10];
    std::vector<std::pair<int, int>> path;
    memset(dis, -1, sizeof(dis));
    dis[src_x][src_y] = 0;
    queue[++rear] = {src_x, src_y};
    pre[src_x][src_y] = {-1, -1};

    // bfs
    while (front <= rear)
    {
        std::pair<int, int> cur = queue[front++];

        if (cur.first == dst_x && cur.second == dst_y)
        {

            int step = dis[dst_x][dst_y];
            if (step > limit)
            {
                return -1;
            }
            int x = dst_x, y = dst_y;
            while (x != -1 || y != -1)
            {
                path.emplace_back(x, y);
                std::pair<int, int> p = pre[x][y];
                x = p.first;
                y = p.second;
            }

            // path -> directions
            Path2Directions(directions, path);
            return 1;
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = cur.first + DX[i];
            int ny = cur.second + DY[i];
            if (nx < 0 || ny < 0 || nx + 1 > N || ny + 1 > N || (map[nx][ny] != PATHWAY_SYMBOL && map[nx][ny] != BERTH_SYMBOL) || dis[nx][ny] != -1)
                continue;
            pre[nx][ny] = cur;
            dis[nx][ny] = dis[cur.first][cur.second] + 1;
            queue[++rear] = {nx, ny};
        }
    }
    return 0;
}

void Path2Directions(std::vector<int> &directions, std::vector<std::pair<int, int>> &path)
{
    // path -> directions
    int tmp;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        if ((tmp = path[i + 1].first - path[i].first)) // x
            directions.emplace_back(tmp > 0 ? UP : DOWN);
        else if ((tmp = path[i + 1].second - path[i].second)) // y
            directions.emplace_back(tmp > 0 ? LEFT : RIGHT);
    }
}

int FindPath(int src_x, int src_y, int dst_x, int dst_y, std::vector<std::pair<int, int>> &path)
{
    rest_invoke--;
    path.clear();

    std::pair<int, int> pre[N + 10][N + 10];
    std::pair<int, int> queue[N * N + 10];
    int front = 0, rear = -1;
    int dis[N + 10][N + 10];
    memset(dis, -1, sizeof(dis));
    dis[src_x][src_y] = 0;
    queue[++rear] = {src_x, src_y};
    pre[src_x][src_y] = {-1, -1};

    // bfs
    while (front <= rear)
    {
        std::pair<int, int> cur = queue[front++];

        if (cur.first == dst_x && cur.second == dst_y)
        {
            int step = dis[dst_x][dst_y];
            int x = dst_x, y = dst_y;
            while (x != -1 || y != -1)
            {
                path.emplace_back(x, y);
                std::pair<int, int> p = pre[x][y];
                x = p.first;
                y = p.second;
            }
            return 1;
        }

        for (int i = 0; i < 4; ++i)
        {
            int nx = cur.first + DX[i];
            int ny = cur.second + DY[i];
            if (nx < 0 || ny < 0 || nx + 1 > N || ny + 1 > N || (map[nx][ny] != PATHWAY_SYMBOL && map[nx][ny] != BERTH_SYMBOL) || dis[nx][ny] != -1)
                continue;
            pre[nx][ny] = cur;
            dis[nx][ny] = dis[cur.first][cur.second] + 1;
            queue[++rear] = {nx, ny};
        }
    }
    return 0;
}