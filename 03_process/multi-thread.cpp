//
// Created by 姚军 on 2022/8/13.
//
#include <iostream>
#include <thread>

void hello() {
    std::cout << "hello world" << std::endl;
}
void world(std::string name) {
    std::cout << name << std::endl;
}
int main() {
    // 创建线程
    std::thread t(hello); // 创建一个thread对象，并指定入口函数。
    t.join();
    std::cout << "结束了1" << std::endl;

    std::thread t2([] { std::cout << "hello world lambda" << std::endl; });
    t2.join();
    std::cout << "结束了2" << std::endl;

    std::thread t3(world, "transfer parameter"); // 传递参数 拷贝传参
    t3.join();
    std::cout << "结束了3" << std::endl;
    return 0;
}
