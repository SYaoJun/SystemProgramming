#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <netinet/in.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 64
class tw_timer;
struct client_data // 绑定socket和定时器
{
    sockaddr_in address;
    int         sockfd;
    char        buf[BUFFER_SIZE];
    tw_timer*   timer;
};

class tw_timer // 定时器类
{
public:
    tw_timer(int rot, int ts)
        : next(NULL)
        , prev(NULL)
        , rotation(rot)
        , time_slot(ts) {
    }

public:
    int rotation;                  // 记录该定时器在时间轮转多少圈后生效
    int time_slot;                 // 记录定时器属于那个槽
    void (*cb_func)(client_data*); // 定时器回调函数
    client_data* user_data;        // 用户数据
    tw_timer*    next;             // 指向上一个定时器
    tw_timer*    prev;             // 指向下一个定时器
};

class time_wheel // 事件轮管理定时器
{
public:
    time_wheel()
        : cur_slot(0) {
        for (int i = 0; i < N; ++i) {
            slots[i] = NULL; // 每个槽的头节点初始化为空
        }
    }
    ~time_wheel() {
        for (int i = 0; i < N; ++i) {
            tw_timer* tmp = slots[i];
            while (tmp) {
                slots[i] = tmp->next;
                delete tmp; // 遍历每个槽销毁new分配在堆中的定时器
                tmp = slots[i];
            }
        }
    }
    tw_timer* add_timer(int timeout) // 添加新的定时器，插入到合适的槽中
    {
        if (timeout < 0) // 时间错误
        {
            return NULL;
        }
        int ticks = 0;
        if (timeout < TI) // 小于每个槽的interval，则为1
        {
            ticks = 1;
        } else {
            ticks = timeout / TI; // 相对当前位置的槽数
        }
        int       rotation = ticks / N;                    // 记录多少圈后生效
        int       ts       = (cur_slot + (ticks % N)) % N; // 确定插入槽的位置
        tw_timer* timer
            = new tw_timer(rotation, ts); // 根据位置和圈数，插入对应的槽中
        if (!slots[ts])                   // 所在槽头节点为空，直接插入
        {
            printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n",
                rotation, ts, cur_slot);
            slots[ts] = timer;
        } else {
            timer->next     = slots[ts];
            slots[ts]->prev = timer;
            slots[ts]       = timer;
        }
        return timer; // 返回含有时间信息和所在槽位置的定时器
    }
    void del_timer(tw_timer * timer) // 从时间轮上删除定时器
    {
        if (!timer) {
            return;
        }
        int ts = timer->time_slot; // 找到所在槽
        if (timer == slots[ts]) {
            slots[ts] = slots[ts]->next;
            if (slots[ts]) {
                slots[ts]->prev = NULL;
            }
            delete timer;
        } else {
            timer->prev->next = timer->next;
            if (timer->next) {
                timer->next->prev = timer->prev;
            }
            delete timer;
        }
    }
    void tick() {
        tw_timer* tmp = slots[cur_slot]; // 取出当前槽的头节点
        printf("current slot is %d\n", cur_slot);
        while (tmp) // 遍历
        {
            printf("tick the timer once\n");
            if (tmp->rotation > 0) {
                tmp->rotation--;
                tmp = tmp->next;
            } else {
                tmp->cb_func(tmp->user_data); // 符合条件，调用回调函数
                if (tmp == slots[cur_slot]) {
                    printf("delete header in cur_slot\n");
                    slots[cur_slot] = tmp->next;
                    delete tmp;
                    if (slots[cur_slot]) {
                        slots[cur_slot]->prev = NULL;
                    }
                    tmp = slots[cur_slot];
                } else {
                    tmp->prev->next = tmp->next;
                    if (tmp->next) {
                        tmp->next->prev = tmp->prev;
                    }
                    tw_timer* tmp2 = tmp->next;
                    delete    tmp;
                    tmp = tmp2;
                }
            }
        }
        cur_slot = ++cur_slot % N;
    }

private:
    static const int N  = 60;
    static const int TI = 1;
    tw_timer*        slots[N];
    int              cur_slot;
};
#endif
