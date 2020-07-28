#include <array>
#include <thread>
#include <vector>
#include <iostream>
#include <future>
#include <cstdlib> //Random number

#include "catch.hpp"
#include "ConcurrentQueue.h"


int pushData(ConcurrentQueue<int>& queue)
{
    std::vector<int> nums {100};
    srand (time(NULL));
    std::generate(nums.begin(), nums.end(), std::rand);
    int total{0};
    for (auto num : nums){
        queue.push(num);
        total += num;
    }
    return total;
}

int consumeData(ConcurrentQueue<int>& queue)
{
    int total{0};
    while (auto data = queue.tryFrontPop())
    {
        total += *data;
    }
    return total;
}


SCENARIO("Sum numbers")
{
    ConcurrentQueue<int> queue;
    std::future<int> producer1Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> consumer1Total = std::async(std::launch::async, consumeData, std::ref(queue));
    std::future<int> producer2Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> producer3Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> consumer2Total = std::async(std::launch::async, consumeData, std::ref(queue));

    int prodTotal = producer1Total.get() + producer2Total.get() + producer3Total.get();
    int consTotal = consumer1Total.get() + consumer2Total.get();

    CHECK(prodTotal == consTotal);
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