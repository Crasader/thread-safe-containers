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
    auto seed = rd();
    // Print seed so failed tests can be repeated
    // Not using Catch2 macros INFO/SCOPED_INFO because they are not thread safe (only 1 thread can use them)
    std::cout << "ConcurrentQueueTests - Using seed: " << seed << std::endl;  // TODO: thread synced logger?
    std::mt19937 generator( seed ); // Standard mersenne_twister_engine
    std::uniform_int_distribution dist(10000, 10000000); // Used to generate random vector size with min size and max size

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

int consumeData(ConcurrentQueue<int>& queue, std::atomic<bool>& complete)
{
    int total{0};

    while ( true)
    {
        if (auto data = queue.tryFrontPop())
        {
            total += *data;
        }
        else // there is no more data
        {
            if (complete) // The producers have stopped pushing data
            {
                return total;
            }
        }
    }
}

// The producers 
// - generate random data
// - push it to the queue
// - total the numbers they have generated
// - return the total number.
// In parallel the consumers 
// - read data from the queue
// - maintain a total for all the numbers they have read.
// The consumers stop reading when there is no more data in the queue and the producers have stopped pushing data.
// A check is performed to ensure the total of the numbers the producers put in the queue is the same as the total of the numbers 
// the consumers read from the queue.
TEST_CASE("Sum numbers")
{
    ConcurrentQueue<int> queue;
    std::atomic<bool> producersComplete{false};

    std::future<int> consumer1Total = std::async(consumeData, std::ref(queue), std::ref(producersComplete));
    std::future<int> producer1Total = std::async(pushData, std::ref(queue));
    std::future<int> producer2Total = std::async(pushData, std::ref(queue));
    std::future<int> consumer2Total = std::async(consumeData, std::ref(queue), std::ref(producersComplete));
    std::future<int> producer3Total = std::async(pushData, std::ref(queue));

    int prodTotal = producer1Total.get() + producer2Total.get() + producer3Total.get();
    producersComplete =true;
    int consTotal = consumer1Total.get() + consumer2Total.get();

    CHECK(prodTotal == consTotal);
}
