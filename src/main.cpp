#include "master.h"

int main()
{
    Master master;
    master.init();

    for (int i = 0; i < 15000; ++i)
    {
        master.run();
    }
}
