#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <chrono>


void gettime(){
    auto currentTime = std::chrono::system_clock::now();

    // Convert the time to a time_point with seconds precision
    auto timePoint = std::chrono::time_point_cast<std::chrono::seconds>(currentTime);

    // Get the time as a std::time_t value
    std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);

    // Convert the time_t value to a string representation
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", std::localtime(&timeT));

    // Print the current time
    std::cout << "Current time: " << timeString << std::endl;
}
int dosomething(std::string file) {
    long long sum = 0;
    for(long long i = 0; i < 10000000000; i++) {
        sum += i-200000;
    }
    std::cout << "do complete: " << file << std::endl;
    gettime();
    return 404;
}

void interact() {
    std::string name;
    std::cout<<"please enter you name:";
    std::cin >> name;
    std::cout << "Hi, " << name<< std::endl;
    gettime();
}

int main() {
	// std::future<int> fret = std::async(dosomething,"oldwang"); 
    std::future<int> fret = std::async([&] {
        return dosomething("hello.zip"); 
    });
    interact();
    int ret = fret.get(); // 阻塞等待获取
    std::cout << "Download result: " << ret << std::endl;
    return 0;
}
