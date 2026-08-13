#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <netinet/in.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_SIZE 64
class tw_timer;
struct client_data // Bind socket and timer
{
    sockaddr_in address;
    int         sockfd;
    char        buf[BUFFER_SIZE];
    tw_timer*   timer;
};

class tw_timer // Timer class
{
public:
    tw_timer(int rot, int ts)
        : next(NULL)
        , prev(NULL)
        , rotation(rot)
        , time_slot(ts) {
    }

public:
    int rotation;  // Records how many rotations of the time wheel before this
                   // timer takes effect
    int time_slot; // Records which slot the timer belongs to
    void (*cb_func)(client_data*); // Timer callback function
    client_data* user_data;        // User data
    tw_timer*    next;             // Pointer to the previous timer
    tw_timer*    prev;             // Pointer to the next timer
};

class time_wheel // Time wheel manages timers
{
public:
    time_wheel()
        : cur_slot(0) {
        for (int i = 0; i < N; ++i) {
            slots[i] = NULL; // Initialize the head node of each slot to NULL
        }
    }
    ~time_wheel() {
        for (int i = 0; i < N; ++i) {
            tw_timer* tmp = slots[i];
            while (tmp) {
                slots[i] = tmp->next;
                delete tmp; // Iterate each slot to destroy timers allocated on
                            // the heap with new
                tmp = slots[i];
            }
        }
    }
    tw_timer* add_timer(
        int timeout) // Add a new timer and insert it into the appropriate slot
    {
        if (timeout < 0) // Invalid time
        {
            return NULL;
        }
        int ticks = 0;
        if (timeout < TI) // If less than the interval of each slot, set to 1
        {
            ticks = 1;
        } else {
            ticks = timeout
                / TI; // Number of slots relative to the current position
        }
        int rotation
            = ticks / N; // Records how many rotations before taking effect
        int ts = (cur_slot + (ticks % N))
            % N; // Determine the slot position to insert
        tw_timer* timer
            = new tw_timer(rotation, ts); // Insert into the corresponding slot
                                          // based on position and rotation
        if (!slots[ts]) // Head node of the slot is empty, insert directly
        {
            printf("add timer, rotation is %d, ts is %d, cur_slot is %d\n",
                rotation, ts, cur_slot);
            slots[ts] = timer;
        } else {
            timer->next     = slots[ts];
            slots[ts]->prev = timer;
            slots[ts]       = timer;
        }
        return timer; // Return the timer with time info and slot position
    }
    void del_timer(tw_timer * timer) // Remove timer from the time wheel
    {
        if (!timer) {
            return;
        }
        int ts = timer->time_slot; // Find the slot it belongs to
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
        tw_timer* tmp
            = slots[cur_slot]; // Get the head node of the current slot
        printf("current slot is %d\n", cur_slot);
        while (tmp) // Iterate
        {
            printf("tick the timer once\n");
            if (tmp->rotation > 0) {
                tmp->rotation--;
                tmp = tmp->next;
            } else {
                tmp->cb_func(
                    tmp->user_data); // Condition met, invoke callback function
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
