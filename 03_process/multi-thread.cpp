//
// Created by Yao Jun on 2022/8/13.
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
    // Create thread
    std::thread t(
        hello); // Create a thread object and specify the entry function.
    t.join();
    std::cout << "结束了1" << std::endl;

    std::thread t2([] { std::cout << "hello world lambda" << std::endl; });
    t2.join();
    std::cout << "结束了2" << std::endl;

    std::thread t3(world, "transfer parameter"); // Pass arguments by copy
    t3.join();
    std::cout << "结束了3" << std::endl;
    return 0;
}
