# 开发指南

## 仓库分支管理规则

1. **main** 和 **dev** 分支由我负责维护，其他人禁止向这两个分支提交任何更改。
2. **dev** 分支是最新的开发分支，所有新的代码开发都应参考此分支。
3. **test** 分支用于调试，不建议使用。
4. 所有新建的分支都应以 **dev** 分支为基础。
5. 请使用英文命名分支，并在提交信息中使用英文。

## 代码结构说明

### barbor.h

"harbor"（港湾）：此头文件定义了机器人、泊位、船只、货物的结构体。

### utils.h

此头文件包含以下两个函数：

```C++
unsigned int Absolute(int x); // 计算x的绝对值
```

```C++
unsigned int Manhattan(int x1, int y1, int x2, int y2); // 计算(x1, y1)与(x2, y2)的曼哈顿距离
```

### master.h

"master"（主人，管理员）：此类具有全局视角，可以在任何时刻获取港口的所有信息。

1. 静态成员变量

```C++
static constexpr int N = 200; // 地图大小：N*N
static constexpr int ROBOT_NUM = 10; // 机器人数量
static constexpr int BERTH_NUM = 10; // 泊位数量
static constexpr int BOAT_NUM = 5; // 船只数量
static constexpr int ITEM_MAX_LIFESPAN = 1000; // 货物的最大生存期
```

+ 注意，`BOAT_NUM` 和 `ITEM_MAX_LIFESPAN` 是我自定义的两个常量，以增加程序的通用性。
+ 在 `Master` 类的作用域中，这些常量可以被当作宏和全局变量使用，类作用域之外使用 `Master::N` 这种形式。

2. 非静态成员变量

```C++
char map[N][N]; // 地图
Robot robots[ROBOT_NUM]; // 机器人数组
Berth berths[BERTH_NUM]; // 泊位数组
Boat boats[BOAT_NUM]; // 船只数组
std::vector<Item> items; // 港口所有货物，不使用id，而是使用下标代替id
```

---

**注意：以下成员变量的类型可能会在后续进行修改**

```C++
std::pair<int, int> tasks[ROBOT_NUM]; // 每个机器人分配的货物，每个pair的第一个元素是货物的下标，第二个元素是代价
```

---

---

**注意：以下成员变量的类型可能会在后续进行修改**

```C++
std::vector<std::pair<int, int>> paths[ROBOT_NUM]; // 每个机器人应走的路径
```

---

3. 成员函数

```C++
void init(); // 初始化地图、泊位、船只的容量
```

```C++
void update(); // 刷新函数，更新货物信息、货物的生存期限、机器人信息、船只信息等
```

```C++
void assignTasks(); // 给机器人分配货物，找到放置货物的泊位等
```

```C++
// 使用A*算法进行寻路
// 如果两点不可达，则返回空的vector
std::vector<std::pair<int, int>>
findPath(int src_x, int src_y, int dst_x, int dst_y);
```