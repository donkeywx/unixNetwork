#include <iostream>
#include <libgo/coroutine.h>


// clang++ libgoTest.cpp -o libgoTest -llibgo -ldl -lpthread -std=c++11
int main(int argc, char** argv)
{
    go[]
    {
        sleep(10);
        // std::cout << "1" << std::endl;
        // co_yield;
        std::cout << "2" << std::endl;
        
    };

    go[]
    {
        sleep(10);
        // std::cout << "3" << std::endl;
        // co_yield;
        std::cout << "4" << std::endl;
        co_sched.Stop();
    };

    // co_sched.UseAloneTimerThread();
    co_sched.Start();
    return 0;
}