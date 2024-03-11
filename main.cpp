#include "master.h"

int main()
{
    Master master;
    master.init();

    for (int frame_cnt = 0; frame_cnt < 15000; ++frame_cnt)
    {
        master.refresh();
        master.control();
        std::cout << "OK" << std::flush;
    }
}
