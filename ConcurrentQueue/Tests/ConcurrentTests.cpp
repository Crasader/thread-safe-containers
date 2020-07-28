#include <array>
#include <thread>
#include <vector>
#include <iostream>

#include "catch.hpp"
#include "ConcurrentQueue.h"


void pushData(ConcurrentQueue<int>& queue, int& total)
{
    std::vector<int> nums{ 1,2,3,4,5 };
    for (auto num : nums){
        queue.push(num);
        total += num;
    }
}

void consumeData(ConcurrentQueue<int>& queue, int& total)
{
    while (auto data = queue.tryFrontPop())
    {
        total += *data;
    }
}


// TODO: this sometimes fails
SCENARIO("Sum numbers")
{
    ConcurrentQueue<int> queue;
    int pTotal{ 0 };
    std::thread producer1(pushData, std::ref(queue), std::ref(pTotal));
    int pTotal2{ 0 };
    std::thread producer2(pushData, std::ref(queue), std::ref(pTotal2));

    int cTotal{ 0 };
    std::thread consumer1(consumeData, std::ref(queue), std::ref(cTotal));
    int cTotal2{ 0 };
    std::thread consumer2(consumeData, std::ref(queue), std::ref(cTotal2));

    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();

    CHECK(pTotal + pTotal2 == cTotal + cTotal2);
}


/*
template<size_t producer_count, size_t consumer_count>
void sumNumbers()
{
    ConcurrentQueue<int> queue;

    std::array<std::thread, producer_count> producers;
    std::array<std::thread, consumer_count> consumers;
    for (size_t i = 0; i < producer_count; ++i)
    {

    }
}
*/