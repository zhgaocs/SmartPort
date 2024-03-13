#ifndef UTILS_H
#define UTILS_H 1

inline unsigned int Absolute(int x)
{
    return x < 0 ? -x : x;
}

inline unsigned int Manhattan(int x1, int y1, int x2, int y2)
{
    return Absolute(x1 - x2) + Absolute(y1 - y2);
}

#endif