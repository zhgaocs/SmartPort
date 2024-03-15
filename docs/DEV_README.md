# 1. 仓库分支管理规则
1. **main** 和 **dev** 分支由我负责维护，其他人禁止向这两个分支提交任何更改
2. **dev** 分支是最新的开发分支，所有新的代码开发都应参考此分支
3. **test** 分支用于调试，不建议使用
4. 所有新建的分支都应以 **dev** 分支为基础
5. 请使用英文命名分支，并在提交信息中使用英文

# 2. 代码结构说明

## 2.1 harbor.h
"harbor"（港湾）：此头文件定义了机器人、泊位、船只、货物的结构体

## 2.2 utils.h
"utilities"（实用程序）：此头文件包含以下两个函数：

```C++
unsigned int Absolute(int x); // 计算x的绝对值
```

```C++
unsigned int Manhattan(int x1, int y1, int x2, int y2); // 计算(x1, y1)与(x2, y2)的曼哈顿距离
```

## 2.3 master.h
"master"（主人，管理员）：此类具有全局视角，可以在任何时刻获取港口的所有信息

### 2.3.1 静态成员变量

```C++
static constexpr int N = 200; // 地图大小：N*N
static constexpr int ROBOT_NUM = 10; // 机器人数量
static constexpr int BERTH_NUM = 10; // 泊位数量
static constexpr int BOAT_NUM = 5; // 船只数量
static constexpr int ITEM_MAX_LIFESPAN = 1000; // 货物的最大生存期
```

*注意，`BOAT_NUM` 和 `ITEM_MAX_LIFESPAN` 是我自定义的两个常量，以增加程序的通用性*

### 2.3.2 非静态成员变量

```C++
char map[N][N]; // 地图
Robot robots[ROBOT_NUM]; // 机器人数组
Berth berths[BERTH_NUM]; // 泊位数组
Boat boats[BOAT_NUM]; // 船只数组
std::vector<Item> items; // 港口所有货物，不使用id，而是使用下标代替id
```

```C++
bool has_tasks[ROBOT_NUM]; // 对应的机器人是否有任务
```

*需要这个数组的原因是：`has_items == 0 && status == 1`时机器人有可能在赶去货物的路上，也有可能什么也不干，需要此数组加以区分*

```C++
std::vector<int> paths[ROBOT_NUM]; // 每个机器人应走的路径
```

### 2.3.3 非静态成员函数

```C++
void init(); // 初始化地图、泊位、船只的容量
```

```C++
void update(); // 刷新函数，更新货物信息、货物的生存期限、机器人信息、船只信息等
```

```C++
/**
 * 函数 assignTasks 的执行流程如下：
 * 
 * 1. 处理机器人任务分配：
 *      1.1 对于无任务且空载的机器人，分配寻找货物的任务。
 *      1.2 对于有任务且携带货物的机器人，指派其前往指定泊位。
 *      1.3 对于有任务但空载的机器人，继续执行其前往目标地点的任务。
 * 
 * 2. 处理船只任务分配：
 *      2.1 根据船只当前状态和需求，分配相应的任务。（具体任务待进一步定义）
 */
void assignTasks(); // 给机器人分配货物，找到放置货物的泊位等
```

```C++
void controlOutput() const; // 输出控制指令，因为下一帧需要接收机器人和船的更新信息，所以这里就不更新了
```

```C++
/**
 * 使用A*算法进行寻路
 * 如果两点不可达，则返回空的vector
 * vector存放终点到起点的路径（也就是逆置）
 */ 
std::vector<std::pair<int, int>> findPath(int src_x, int src_y, int dst_x, int dst_y);
```

### 2.3.4 静态成员函数（类内辅助函数）

```C++
// 将逆置的路径转换为方向（方向也逆置了，第一个方向为最后一个元素）
static std::vector<int> path2Directions(const std::vector<std::pair<int, int>> &path);
```

# 3. 策略

## 3.1 机器人找货物
+ 原则：就近
+ 实现：
  + 设定一个阈值*k*
  + 先在所有货物中搜寻与机器人**曼哈顿距离**最近的*k*个货物
  + 再从这*k*个（至多）中选出实际距离最近的货物
 
## 3.2 机器人找泊位
+ 原则：就近

## 3.3 船只找泊位
+ 原则：就近
