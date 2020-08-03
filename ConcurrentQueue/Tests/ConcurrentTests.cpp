#include <array>
#include <thread>
#include <vector>
#include <iostream>
#include <future>
#include <random>

#include "catch.hpp"
#include "ConcurrentQueue.h"


int pushData(ConcurrentQueue<int>& queue)
{
    // C++11 <random> setup
    std::random_device rd;          // Will be used to obtain a seed for the random number engine
    std::mt19937 generator( rd() ); // Standard mersenne_twister_engine
    std::uniform_int_distribution dist(10000, 10000000); // Used to generate random vector size with min size and max size.

    // Generate randomly sized vector with random numbers
    int vector_size = dist(generator);
    std::vector<int> nums {vector_size};
    std::generate(nums.begin(), nums.end(), generator);

    // Push each number into the concurrent queue 
    // and calculate the total number
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

TEST_CASE("Sum numbers")
{
    ConcurrentQueue<int> queue;

    std::future<int> producer1Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> consumer1Total = std::async(std::launch::async, consumeData, std::ref(queue));
    std::future<int> producer2Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> producer3Total = std::async(std::launch::async, pushData, std::ref(queue));
    std::future<int> consumer2Total = std::async(std::launch::async, consumeData, std::ref(queue));
    std::future<int> consumer3Total = std::async(std::launch::async, consumeData, std::ref(queue));

    int prodTotal = producer1Total.get() + producer2Total.get() + producer3Total.get();
    int consTotal = consumer1Total.get() + consumer2Total.get() + consumer3Total.get();

    CHECK(prodTotal == consTotal);
}
