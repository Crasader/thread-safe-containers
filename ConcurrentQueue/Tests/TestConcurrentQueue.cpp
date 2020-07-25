#include <iostream>
#include <thread>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ConcurrentQueue.h"

/*
//*********** Single thread tests********** 
SCENARIO ("Populating then emptying a queue with a single thread", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    int max= 25;
    
    GIVEN  ("An empty queue") 
	{
        for (int i = 0; i < max; i++)
        {
            concurrentQueue.push(std::to_string(i));
        }

        WHEN ("The queue is populated with std::strings")
		{
            THEN ("The queue will contain values")
			{
                CHECK ( concurrentQueue.empty() == false);
                CHECK ( concurrentQueue.size() == max);
                
            }
        }        
    }

    GIVEN  ("A full queue") 
	{
        WHEN ("The queue is emptied")
		{		
            for (int i = 0; i < max; i++)
            {
                concurrentQueue.pop();
            }

            THEN ("The queue will be empty values")
			{
                CHECK ( concurrentQueue.empty() == true);
                CHECK ( concurrentQueue.size() == 0);
            }
        }        
    }
}

SCENARIO ("All queue elements contain a value", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    int max= 25;

    for (int i = 0; i < max; i++)
    {
        concurrentQueue.push(std::to_string(i));
    }
    
    GIVEN  ("A full queue") 
	{
        WHEN ("We loop through every front element")
		{
            THEN ("The queue will contain values")
			{
                int expectedValue = 0;
                while(!concurrentQueue.empty())
                {
                    std::string testString;
                    concurrentQueue.tryFront(testString);
                    CHECK (testString == std::to_string(expectedValue));
                    concurrentQueue.pop();
                    expectedValue++;
                }
            }
        }        
    }
}

SCENARIO ("TryFront returns a valid status depending on the state of the queue", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    
    GIVEN  ("A queue is empty") 
	{
        WHEN ("We call tryFront()")
		{
            THEN ("The status returned will be EMPTY and nothing will be taken from front")
			{
                std::string testString;
                QueueStatus status = concurrentQueue.tryFront(testString);
                CHECK (testString == "");
                CHECK (status == QueueStatus::EMPTY);    
            }
        }        
    }

    GIVEN  ("A queue is not empty") 
	{
        std::string expectedOutput = "Expected output";
        concurrentQueue.push(expectedOutput);

        WHEN ("We call tryFront()")
		{
            THEN ("The status returned will be FOUND and the value from front will be taken")
			{
                std::string testString;
                QueueStatus status = concurrentQueue.tryFront(testString);
                CHECK (testString == expectedOutput);
                CHECK (status == QueueStatus::FOUND);    
            }
        }        
    }
}

SCENARIO ("TryFrontAndPop returns a valid status depending on the state of the queue", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    
    GIVEN  ("A queue is empty") 
	{
        WHEN ("We call tryFrontAndPop()")
		{
            THEN ("The status returned will be EMPTY and nothing will be taken from front")
			{
                std::string testString;
                QueueStatus status = concurrentQueue.tryFrontAndPop(testString);
                CHECK (testString == "");
                CHECK (status == QueueStatus::EMPTY);    
            }
        }        
    }

    GIVEN  ("A queue is not empty") 
	{
        std::string expectedOutput = "Expected output";
        concurrentQueue.push(expectedOutput);

        WHEN ("We call tryFrontAndPop()")
		{
            THEN ("The status returned will be FOUND and the value from front will be taken")
			{
                std::string testString;
                QueueStatus status = concurrentQueue.tryFrontAndPop(testString);
                CHECK (testString == expectedOutput);
                CHECK (status == QueueStatus::FOUND);    
                CHECK (concurrentQueue.empty() == true);   
            }
        }        
    }
}

SCENARIO ("A queue element can be grabbed then removed from the queue within the same function", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    std::string expectedValue = "Hello";
    concurrentQueue.push(expectedValue);

    GIVEN  ("A populated queue") 
	{
        WHEN ("We call frontAndPop")
		{
            std::string tempString;
            concurrentQueue.tryFrontAndPop(tempString);

            THEN ("The item will be stored locally")
			{
                CHECK (tempString == expectedValue);
            }

            THEN ("The item will be removed from the queue")
			{
                CHECK (concurrentQueue.empty() == true);
            }
        }        
    }
}

SCENARIO ("An object can be moved into a queue", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;
    std::string originalValue = "Hello";

    GIVEN("An empty queue and object to move into it") 
	{
        WHEN ("We move an item into the queue")
		{
            concurrentQueue.push(std::move(originalValue));
            
            THEN ("The object will be in the queue")
			{
                std::string tempString;
                concurrentQueue.tryFront(tempString);
                CHECK (tempString == "Hello");
            }

            THEN ("The item will be removed from the queue")
			{
                CHECK (originalValue == "");
            }
        }        
    }
}

//*********** Multi thread tests********** 

SCENARIO ("The queue can be filled concurrently", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;

    GIVEN("An empty queue") 
	{
        WHEN ("We use two threads to fill it")
		{
            const int maxQueue1 = 250;
            const int maxQueue2 = 250;
            const int maxTotal =  maxQueue1 + maxQueue2;

            std::thread thread1([&concurrentQueue, &maxQueue1]() {
                for (int i = 0; i < maxQueue1; i++)
                {        
                    concurrentQueue.push(std::to_string(i));
                }
            });

            std::thread thread2([&concurrentQueue, &maxQueue2]() {
                for (int i = 0; i < maxQueue2; i++)
                {
                    concurrentQueue.push(std::to_string(i));
                }
            });

            thread1.join();
            thread2.join();            
            
            THEN ("The queue will contain data")
			{
                CHECK (concurrentQueue.size() == maxTotal);
            }
        }  
    }
}

SCENARIO ("The queue can be emptied concurrently", "[ConcurrentQueue]") 
{
    ConcurrentQueue<std::string> concurrentQueue;

    int max= 2500;
    for (int i = 0; i < max; i++)
    {        
        concurrentQueue.push(std::to_string(i));
    }

    GIVEN("A filled queue") 
	{
        WHEN ("We use three threads to empty it")
		{
            std::thread thread1([&concurrentQueue]() {
                while (!concurrentQueue.empty())
                {
                    std::string tempString;
                    concurrentQueue.tryFront(tempString);                    
                    concurrentQueue.pop();
                }
            });

            std::thread thread2([&concurrentQueue]() {
                while (!concurrentQueue.empty())
                {                    
                    std::string tempString;
                    concurrentQueue.tryFront(tempString);
                    concurrentQueue.pop();
                }
            });
            std::thread thread3([&concurrentQueue]() {
                while (!concurrentQueue.empty())
                {
                    std::string tempString;
                    concurrentQueue.tryFront(tempString);                    
                    concurrentQueue.pop();
                }
            });

            thread1.join();
            thread2.join();
            thread3.join();
                   
            THEN ("The queue will be empty")
			{
                CHECK (concurrentQueue.empty() == true);
            }
        }       
    }
}

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