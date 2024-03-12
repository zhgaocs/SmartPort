#ifndef HARBOR_H
#define HARBOR_H 1

struct Robot
{
    int id;
    int x, y;
    int has_item; /* 0 - no items, 1 - has items */
    int status;   /* 0 - recover, 1 - running */
    int dst_x, dst_y;

    Robot() = default;  //Feature of c++11
    Robot(int robot_id, int startX, int startY) :id(robot_id), x(startX), y(startY) {}

    //This robot move to next valid grid
    void move(int direction){
        if(direction >= 0 && direction <= 3){
            std::cout << "move " + this -> id << " " + direction << std::endl;
        }
        else{
            std::cerr << "move : wrong instruction!";
        }
    }

    //This robot picks up the item on this grid
    bool get(){
        if(this -> status == 1 && this -> has_item == 0){
            std::cout << "get " + this -> id << std::endl;
        }else if(this -> status == 0){
            std::cerr << "The robot is recovering!";
        }else if(this -> has_item== 1){
            std::cerr << "The robot has already carried an item!";
        }//暂留待删：主程序应当补充判断该位置是否存在货物
    }

    void pull(){
        if(this -> status == 1 && this -> has_item == 0){
            std::cout << "pull " + this -> id << std::endl;
        }else if(this -> status == 0){
            std::cerr << "The robot is recovering!";
        }else if(this -> has_item== 1){
            std::cerr << "The robot has already carried an item!";
        }//暂留待删：主程序应当补充判断该位置位于泊点
    }

};

struct Berth
{
    int id;             /* 0 <= id < BERTH_NUM */
    int x, y;           /* upper left corner */
    int transport_time; /* 1 <=  transport_time <= 1000, transport time to virtual point*/
    int loading_speed;  /* 1 <= loading_speed <= 5, the number of items that can be loaded per frame */

    Berth() = default;
    Berth(int x, int y, int transport_time, int loading_speed)
        : x(x), y(y), transport_time(transport_time), loading_speed(loading_speed) {}

};

struct Boat
{
    //按照任务书的意思，船应该是在泊点自动开始装货，装货时间=船capacity / 泊位装货速度loading_speed
    int id;              /* 0 <= id < BOAT_NUM */
    int target_berth_id; /* -1 - virtual point */
    int status;          /* 0 - moving, 1 - running, 2 - waiting */
    int capacity;        /* 1 <= capacity <= 1000*/

    //This ship moves to the target berth
    void ship(int berth_id){
        if(berth_id < 0 || berth_id > 9){
            std::cerr << "ship : wrong instruction!";
            return;
        }
        
        if(this -> status == 1){
            std::cout << "ship " + this -> id << " " + berth_id << std::endl;
        }else if(this -> status == 0){
            std::cerr << "The ship has been moving!";
        }else if(this -> status== 2){
            std::cerr << "The ship is still watiting at a berth!";
        }//暂留待删：主程序应当在船到达时补充判断该位置已经有船在装货，从而把ship.status改为2
    } 

    //This ship moves to the virtual point
    void go(){
        if(this -> status == 1){
            std::cout << "go " + this -> id << std::endl;
        }else if(this -> status == 0){
            std::cerr << "The ship has been moving!";
        }else if(this -> status== 2){
            std::cerr << "The ship is still watiting at a berth!";
        }
    }

};

struct Item /* goods */
{
    int id;
    int x, y;
    int value; /* 0 < value <= 1000 */

    // bool operator==(const Item &other) const
    // {
    //     return x == other.x && y == other.y && value == other.value;
    // }
    
};

#endif