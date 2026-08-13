
#include <stdbool.h>
#include <stdio.h>
typedef bool (*send_data_t)(char* data, bool is_sync);
typedef bool (*recv_data_t)(char* data);
bool tcp_send_data(char* msg_data, bool is_sync) {
    puts("send data");
    return true;
}

typedef struct callback {
    send_data_t send_func;
    recv_data_t recv_func;
} callback_t;

callback_t g_call_inst;

int main() {
    g_call_inst.send_func = tcp_send_data;
    char* data            = "hello";
    g_call_inst.send_func(data, true);
    return 0;
}
