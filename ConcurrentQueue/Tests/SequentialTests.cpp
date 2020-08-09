#include <thread>
#include <string>
#include <chrono>  

#include "catch.hpp"
#include "ConcurrentQueue.h"

SCENARIO ("Basic Usage tryFrontPop()")
{
    GIVEN  ("A queue of 20 strings") 
	{
        ConcurrentQueue<std::string> queue;
        for (int i = 0; i < 20; i++)
        {
            queue.push(std::to_string(i));
        }

        THEN ("The queue will not be empty")
        {
            CHECK (queue.empty() == false);
        }

        WHEN ("The queue is popped 20 times")
        {		
            for (int i = 0; i < 20; i++)
            {
                auto val = queue.tryFrontPop();
                CHECK (val.has_value() == true);
                CHECK (val == std::to_string(i));
            }

            THEN ("The queue will be empty")
            {
                CHECK (queue.empty() == true);
            }

            THEN ("Accessing the queue will return nullopt")
            {
                auto val = queue.tryFrontPop();
                CHECK (val.has_value() == false);
            }
        }
    }
} 

SCENARIO ("Basic Usage waitingFrontPop()")
{
    GIVEN  ("A queue of 20 strings") 
	{
        ConcurrentQueue<std::string> queue;
        for (int i = 0; i < 20; i++)
        {
            queue.push(std::to_string(i));
        }

        WHEN ("The queue is popped 20 times")
        {		
            for (int i = 0; i < 20; i++)
            {
                auto val = queue.waitingFrontPop();
                CHECK (val == std::to_string(i));
            }

            THEN ("The queue will be empty")
            {
                CHECK (queue.empty() == true);

                WHEN("A thread pops an empty queue")
                {
                    std::string data;
                    std::thread thread1([&queue, &data]() {
                        data = queue.waitingFrontPop();
                });
                    WHEN("Data is inserted into the queue after a short while")
                    {
                        std::this_thread::sleep_for (std::chrono::milliseconds(200));
                        queue.push("A new item");
                        THEN("The waiting thread will get the new item")
                        {
                            thread1.join();
                            CHECK (data == "A new item");
                        }
                    }
                }
            }    
        }
    }
}

SCENARIO ("Copy construct & copy assign a queue")
{
    GIVEN  ("A queue of 20 strings") 
	{
        ConcurrentQueue<std::string> queue;
        for (int i = 0; i < 20; i++)
        {
            queue.push(std::to_string(i));
        }
            WHEN ("The queue is copy constructed")
            {		
                ConcurrentQueue<std::string> copied_queue(queue);

                THEN ("The queue will have the same data as the original queue")
                {		
                    for (int i = 0; i < 20; i++)
                    {
                        auto val = queue.waitingFrontPop();
                        CHECK (val == std::to_string(i));
                    }
                }
            }
            WHEN ("The queue is copy assigned")
            {		
                ConcurrentQueue<std::string> copied_queue; 
                // Assigment must be done on a seperate line from the declaration otherwise the copy constructor will be used.
                copied_queue = queue;

                THEN ("The queue will have the same data as the original queue")
                {		
                    for (int i = 0; i < 20; i++)
                    {
                        auto val = queue.waitingFrontPop();
                        CHECK (val == std::to_string(i));
                    }
                }
            }
            WHEN ("The queue is self assigned")
            {		
                queue = queue;
                THEN ("The queue will be unchanged")
                {		
                    for (int i = 0; i < 20; i++)
                    {
                        auto val = queue.waitingFrontPop();
                        CHECK (val == std::to_string(i));
                    }
                }
            }
    }
}

struct Counter {
public:
    Counter()                     { ++ctorCount;     }
    Counter(const Counter& other) { ++copyCtorCount; }
    Counter(Counter&& other)      { ++moveCtorCount; }
    static size_t ctorCount;
    static size_t copyCtorCount;
    static size_t moveCtorCount;
};
size_t Counter::ctorCount     = 0;
size_t Counter::copyCtorCount = 0;
size_t Counter::moveCtorCount = 0;

SCENARIO("Move an object into the queue")
{
    GIVEN("An empty queue and a Counter object")
    {
        ConcurrentQueue<Counter> queue;
        Counter aCounter;
        CHECK(Counter::ctorCount == 1);

        WHEN("An lvalue object is copied into the queue")
        {
            queue.push(aCounter);

            THEN("The copy constructor will be called once")
            {
                CHECK(Counter::copyCtorCount == 1);
                CHECK(Counter::ctorCount == 1); // No change 

                WHEN("An rvalue object is moved into the queue")
                {
                    queue.push(std::move(aCounter));
                    THEN("The move constructor will be called once")
                    {
                        CHECK(Counter::moveCtorCount == 1);
                        CHECK(Counter::ctorCount == 1);     // No change 
                        CHECK(Counter::copyCtorCount == 1); // No change 
                    }
                }
            }
        }
    }
}