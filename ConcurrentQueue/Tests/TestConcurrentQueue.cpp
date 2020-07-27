   

#define CATCH_CONFIG_MAIN
#include "catch.hpp"



// TODO: copy constructed queue
// TODO: assignment
// TODO: Move semantic tests

/*
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