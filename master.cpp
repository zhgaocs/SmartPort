#include "master.h"

Master::Master()
{
#ifdef DEBUG_MODE
    log.open("log.txt");
#endif
}

void Master::init()
{
    char okk[3];
    int boat_capacity;

    InitMap();

    /* init berth */
    for (int i = 0; i < BERTH_NUM; ++i)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berths[id].x, &berths[id].y, &berths[id].transport_time, &berths[id].loading_speed);
    }

    /* boat capacity */
    scanf("%d", &boat_capacity);
    std::for_each(boats, boats + BOAT_NUM, [boat_capacity](Boat &b)
                  { b.capacity = boat_capacity; });

    /* OK */
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

void Master::update()
{
    static int last_sync = 0;

    char okk[3];
    int frame_id, frame_sub, current_money, new_items;

    scanf("%d%d", &frame_id, &current_money);

    /* new items */
    scanf("%d", &new_items);
    for (int i = 0; i < new_items; ++i)
    {
        int x, y, value;
        scanf("%d%d%d", &x, &y, &value);
        items.emplace_back(x, y, value, ITEM_LIFESPAN_MAX);
        item_selected.emplace_back(false);
    }

    /* robot */
    for (Robot &robot : robots)
        scanf("%d%d%d%d", &robot.has_item, &robot.x, &robot.y, &robot.status);

    /* boat */
    for (int i = 0; i < BOAT_NUM; ++i)
    {
        scanf("%d%d\n", &boats[i].status, &boats[i].target_pos);

        if (boats[i].target_pos != -1)
        {
            if (!boats[i].status)
                ;
            if (1 == boats[i].status)
                berths[boats[i].target_pos].current_boat = i;
            else
                berths[boats[i].target_pos].waiting_boat = i;
        }
    }

    /* all items */
    frame_sub = frame_id - last_sync;
    std::for_each(items.begin(), items.end(), [frame_sub](Item &item)
                  { item.life_span -= frame_sub; });

    if (1 != frame_sub)
        last_sync = frame_sub;
    else
        ++last_sync;

    /* OK */
    scanf("%s", okk);
}

void Master::assignRobots()
{
}

void Master::assignBoats()
{
}

void Master::control()
{
    /* ROBOT */
    // TODO

    /* BOAT */
    // TODO
}

void Master::run()
{
    update();
    assignRobots();
    assignBoats();
    control();
}

#ifdef DEBUG_MODE
void Master::printRobots()
{
    for (int i = 0; i < ROBOT_NUM; ++i)
        log << "Robot#" << std::setw(1) << i
            << " |has_item: " << std::setw(1) << robots[i].has_item
            << " |x: " << std::setw(3) << robots[i].x
            << " |y: " << std::setw(3) << robots[i].y
            << " |status: " << std::setw(1) << robots[i].status
            << " |task: " << std::setw(1) << robots[i].task
            << " |target_item: " << std::setw(2) << robots[i].target_item
            << " |target_berth " << std::setw(2) << robots[i].target_berth
            << " |directions.empty(): " << std::setw(1) << robots[i].directions.empty()
            << " |directions.size(): " << std::setw(3) << robots[i].directions.size() << '\n';

    log.flush();
}

void Master::printBoats()
{
    for (int i = 0; i < BOAT_NUM; ++i)
        log << "Boat#" << std::setw(1) << i
            << " |status: " << std::setw(1) << boats[i].status
            << " |target_pos: " << std::setw(2) << boats[i].target_pos
            << " |capacity: " << std::setw(3) << boats[i].capacity << '\n';
    log.flush();
}

void Master::printBerths()
{
    for (int i = 0; i < BERTH_NUM; ++i)
        log << "Berth#" << std::setw(1) << i
            << " |x: " << std::setw(3) << berths[i].x
            << " |y: " << std::setw(3) << berths[i].y
            << " |transport_time: " << std::setw(3) << berths[i].transport_time
            << " |loading_speed: " << std::setw(3) << berths[i].loading_speed
            << " |total_value: " << std::setw(3) << berths[i].total_value
            << " |current_boat: " << std::setw(3) << berths[i].current_boat
            << " |waiting_boat: " << std::setw(3) << berths[i].waiting_boat
            << " |piled_values.size(): " << std::setw(3) << berths[i].piled_values.size() << '\n';
    log.flush();
}
#endif