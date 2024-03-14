# 开发说明

## 仓库分支管理

1. **main**和**dev**分支由我维护，禁止其他提交
2. **dev**分支是开发和最新的参考分支
3. **test**分支用于debug，不供使用
4. 新分支应基于**dev**创建
5. 分支、提交信息等使用**英文**

## 代码说明

### barbor.h

harbor（港湾），此头文件用以存放机器人、泊位、船只、货物的结构体定义

### utils.h

此头文件目前包含如下两个函数：

```C++
unsigned int Absolute(int x); // 取x绝对值
```

```C++
unsigned int Manhattan(int x1, int y1, int x2, int y2); // (x1, y1)与(x2, y2)的曼哈顿距离
```

### master.h

master（主人，管理员），此类具有全局视角，旨在任意时刻掌握港口任何信息

1. 静态成员变量

```C++
static constexpr int N = 200; // 地图：N*N
static constexpr int ROBOT_NUM = 10; // 机器人数量
static constexpr int BERTH_NUM = 10; // 泊位数量
static constexpr int BOAT_NUM = 5; // 船只数量
static constexpr int ITEM_MAX_LIFESPAN = 1000; // 货物最大生存期
```

+ 需要注意的是，`BOAT_NUM`和`ITEM_MAX_LIFESPAN`是我自己定义的两个常量，使程序更加通用

+ 在`Master`类的作用域中，可以把这些常量当作**宏**和**全局变量**使用，类作用域之外使用`Master::N`这种形式

2. 非静态成员变量

```C++
char map[N][N]; // 地图
Robot robots[ROBOT_NUM]; // 机器人数组
Berth berths[BERTH_NUM]; // 泊位数组
Boat boats[BOAT_NUM]; // 船只数组
std::vector<Item> items; // 港口所有货物，没有使用id，使用下标代替id
```

---

**此项成员变量后续类型可能会修改**

```C++
std::pair<int, int> tasks[ROBOT_NUM]; // 每个机器人分配货物，每个pair的第一个是货物的下标，第二个为代价
```

---

---

**此项成员变量后续类型可能会修改**

```C++
std::vector<std::pair<int, int>> paths[ROBOT_NUM]; // 每个机器人该走的路径
```

---

3. 成员函数

```C++
void init(); // 初始化地图、泊位、船容量
```

```C++
void update(); // 刷新函数，刷新货物信息，更新货物时效期限，机器人信息，船只信息等等
```

```C++
void assignTasks(); // 给机器人分配货物，找泊位放货物等
```

```C++
// 使用A*算法进行寻路
// 两点不可达时返回空vector
std::vector<std::pair<int, int>>
findPath(int src_x, int src_y, int dst_x, int dst_y);
```
