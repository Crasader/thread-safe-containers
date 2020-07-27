#include <iostream>
#include <thread>
#include <string>
#include <chrono>         

#define CATCH_CONFIG_MAIN
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

// TODO: copy constructed queue
// TODO: assignment
// TODO: Move semantic tests
/*




SCENARIO ("The queue can be filled and emptied concurrently", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;

    int totalSize = 9999;

    GIVEN("An empty queue")
	{
        WHEN ("We fill and empty the queue at the same time")
		{    
            THEN ("The queue can be read and written to at the same time")
			{           
                int thread1Max = totalSize;     
                std::thread thread1([&]() {
                    for (int i = 0; i < thread1Max; i++)
                    {
                        concurrentQueue.push(std::to_string(i));
                    }
                });

                //use a count. once it hits the total size we know everything has been added
                int count = 0;
                std::thread thread2([&]() {
                    while (count != totalSize)
                    {
                        while (!concurrentQueue.empty())
                        {
                            concurrentQueue.pop();
                            count++;
                        }
                    }
                });

                thread1.join();
                thread2.join();
         
                CHECK (concurrentQueue.empty() == true);
            }
        }  

    }
}

//Elaborate test used here to verify the values are copied. 
//Basically sum them all up in for each thread
SCENARIO ("The queue can be frontPop'd concurrently", "[ConcurrentQueue]") 
{
    ConcurrentQueue<int> concurrentQueue;

    int max= 1000;
    int maxValue = 0;
    for (int i = 0; i < max; i++)
    {        
        concurrentQueue.push(i);
        maxValue += i;
    }

    GIVEN("A filled queue") 
	{
        WHEN ("We use three threads to frontAndPop it")
		{
            int countThread1 = 0;
            std::thread thread1([&concurrentQueue, &countThread1]() {
                while (!concurrentQueue.empty())
                {                    
                    int temp1 = 0;
                    concurrentQueue.tryFrontAndPop(temp1); 
                    countThread1 += temp1;
                }
            });

            int countThread2 = 0;
            std::thread thread2([&concurrentQueue, &countThread2]() {
                while (!concurrentQueue.empty())
                {
                    int temp2 = 0;
                    concurrentQueue.tryFrontAndPop(temp2); 
                    countThread2 += temp2;   
                }
            });

            int countThread3 = 0;
            std::thread thread3([&concurrentQueue, &countThread3]() {
                while (!concurrentQueue.empty())
                {
                    int temp3 = 0;
                    concurrentQueue.tryFrontAndPop(temp3); 
                    countThread3 += temp3;
                }
            });

            thread1.join();
            thread2.join();
            thread3.join();            
          
            THEN ("The queue will be empty")
			{
                CHECK (concurrentQueue.empty() == true);
            }

            int totalCount = countThread1 + countThread2 + countThread3;
            THEN ("All values where copied out")
			{
                CHECK (totalCount == maxValue);
            }
        }       
    }
}
*/