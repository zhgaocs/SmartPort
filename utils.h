#ifndef UTILS_H
#define UTILS_H 1

struct AStarNode
{
    int x, y;
    int g;
    int h;
    int f;
    AStarNode *prev;

    static const int INTEGER_MAX = INT_MAX;

    AStarNode(int x, int y)
        : x(x), y(y), g(INTEGER_MAX), h(0), f(INTEGER_MAX), prev(nullptr) {}
};

inline unsigned int Absolute(int x)
{
    return x < 0 ? -x : x;
}

inline unsigned int Manhattan(int x1, int y1, int x2, int y2)
{
    return Absolute(x1 - x2) + Absolute(y1 - y2);
}

#endif