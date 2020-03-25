#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <chrono>

#define COUNT 10000000

long long time_interval(const struct timeval* start, const struct timeval* end) {
    //1 Ãë = 10^6 Î¢Ãë
    return ((end->tv_sec * 1000000ll) + end->tv_usec) - ((start->tv_sec * 1000000ll) + start->tv_usec);
}

void fun2()
{
    struct timeval start, end;
    gettimeofday(&start, nullptr);
    for (int i = 0; i < COUNT; i++) {
        auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        auto timeTick = timeValue.time_since_epoch().count();
    }

    gettimeofday(&end, nullptr);
    auto interval = time_interval(&start, &end);
    std::cout << "std::chrono spend:" << interval << " us " << interval * 0.000001 << " s" << std::endl;
}

void fun1()
{
    struct timeval start, end;
    gettimeofday(&start, nullptr);

    for (int i = 0; i < COUNT; i++)
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        auto timeTick = tv.tv_sec * 1000 + tv.tv_usec * 0.001;
    }

    gettimeofday(&end, nullptr);
    auto interval = time_interval(&start, &end);
    std::cout << "gettimeofday spend:" << interval << " us " << interval * 0.000001 << " s" << std::endl;
}

int main(void)
{
    fun2();
    fun1();    
    return 0;
}