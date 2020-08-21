#include <string>
#include <thread>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define _ALLOW_KEYWORD_MACROS
#define private public
#include "CircularBuffer.h"

//*********** Start of black box tests********** 
SCENARIO ("Example use of circular buffer using std::string data", "[CircularBuffer]") 
{
    GIVEN  ("A buffer of size 10 is created") 
	{
        CircularBuffer<std::string, 10> circBuff;
        WHEN ("std::string data is inserted along side its assoicated double time")
		 {		
            circBuff.push(0.1,"Hello");
            circBuff.push(0.2,"world");
            circBuff.push(0.3,"Good");
            circBuff.push(0.4,"Morning");
            circBuff.push(1.0,"Earth");
            circBuff.push(100,"Bye");	
            THEN ("The data can be retrieved using the time as a key")
			{
                std::string output;
                circBuff.get_at(0.1, output);
                CHECK ( output == "Hello");
                circBuff.get_at(0.2, output);
				CHECK( output == "world");
                circBuff.get_at(0.3, output);
				CHECK( output == "Good");
                circBuff.get_at(0.4, output);
				CHECK( output == "Morning");
                circBuff.get_at(1, output); 
				CHECK( output == "Earth");
				circBuff.get_at(100, output);
				CHECK(output == "Bye");		
           }
        }        
    }
}

SCENARIO("Requesting data from an empty circular buffer shall not throw an exception and will return EMPTY", "[CircularBuffer]") 
{
	GIVEN("An empty buffer")
	{
		CircularBuffer<int, 20> circBuff;
		WHEN("An element is requested")
		{
			int output;
			FoundStatus result = circBuff.get_at(0.1, output);
			THEN("The return value is EMPTY")
			{
				CHECK(result == FoundStatus::EMPTY);
			}
		}
	}
}

SCENARIO("Requesting data from a buffer with 1 only element shall not throw an exception", "[CircularBuffer]") 
{
	GIVEN("A buffer with one element of value 1")
	{
		CircularBuffer<int, 20> circBuff;
        circBuff.push(0.1,1);
		WHEN("The element that exists is requested") 
		{
			int output;
			FoundStatus result = circBuff.get_at(0.1, output);
			THEN("The return is FOUND and the output is 1")
			 {
				CHECK(result == FoundStatus::FOUND);
                CHECK(output == 1);		
			}
		}
		WHEN("An element that doesn't exist is requested")
		 {
			int output;
			FoundStatus result = circBuff.get_at(10, output);
			THEN("The return value is TOO_NEW and the output is 1") 
			{
				CHECK(result == FoundStatus::TOO_NEW);
                CHECK(output == 1);		
			}
		}
	}
}

SCENARIO("Retrieving data which is outwith the limits", "[CircularBuffer]") 
{
	GIVEN("A buffer with some data") 
	{
		CircularBuffer<int, 20> circBuff;
		circBuff.push(10, 1);
		circBuff.push(20, 2);
		circBuff.push(30, 3);
		WHEN("Requesting data which is too old")
		{
			int output1;
			FoundStatus result1 = circBuff.get_at(5, output1);
			THEN("The return is TOO_OLD and the output is the oldest element") 
			{
				CHECK(result1 == FoundStatus::TOO_OLD);
				CHECK(output1 == 1);
			}
		}
		WHEN("Requesting data which is too new")
		{
			int output1;
			FoundStatus result1 = circBuff.get_at(50, output1);
			THEN("The return is TOO_NEW and the output is the newest element") 
			{
				CHECK(result1 == FoundStatus::TOO_NEW);
				CHECK(output1 == 3);
			}
		}
	}
}

SCENARIO("A buffer with a lot of elements can be used", "[CircularBuffer]")
{
	GIVEN("A buffer of size 47832")
	{
		CircularBuffer<int, 47832> circBuff;
		WHEN("100,000 elements are inserted")
		{
            for (int i=0; i<100000; ++i)
            {
                circBuff.push(i, i);
            }
			
			THEN("The 600000th element can be retrieved")
			{
				int output;
				circBuff.get_at(60000, output);
				CHECK(output == 60000);
			}
		}
	}
}

SCENARIO ("When two elements have the same time the earliest one will be retrieved", "[CircularBuffer]")
{
	GIVEN("A buffer with two elements with the same time") 
	{
		CircularBuffer<int, 5 > circBuff;
		circBuff.push(1, 1);
		circBuff.push(2, 2);
		circBuff.push(10, 3);
		circBuff.push(10, 4);
		circBuff.push(10, 5);
	    circBuff.push(20, 6);
		WHEN("10 is requested") 
		{
			int output;
			circBuff.get_at(10, output);
			THEN("the earlier element with time = 10 will be retrieved") 
			{
				CHECK(output == 3);
			}
		}
	}
}


SCENARIO ("When an even sized buffer has wrapped", "[CircularBuffer]")
{
	GIVEN("A buffer of size of 6") 
	{
		CircularBuffer<char, 6> circBuff;

		WHEN("More than 6 elements are pushed in") 
		{
			circBuff.push(1, 'a');
			circBuff.push(2, 'b');
			circBuff.push(3, 'c');
			circBuff.push(4, 'd');
			circBuff.push(5, 'e');
			circBuff.push(6, 'f');
			circBuff.push(7, 'g');
			circBuff.push(8, 'h');
			circBuff.push(9, 'i');
			THEN("the last 6 elements can be retrieved") 
			{
				char output;
				circBuff.get_at(4, output);
				CHECK(output == 'd');
				circBuff.get_at(5, output);
				CHECK(output == 'e');
				circBuff.get_at(6, output);
				CHECK(output == 'f');
				circBuff.get_at(7, output);
				CHECK(output == 'g');
				circBuff.get_at(8, output);
				CHECK(output == 'h');
				circBuff.get_at(9, output);
				CHECK(output == 'i');
			}
		}
	}
}

SCENARIO ("When an odd sized buffer has wrapped", "[CircularBuffer]")
{
	GIVEN("A buffer of size of 7") 
	{
		CircularBuffer<char, 7> circBuff;

		WHEN("More than 7 elements are pushed in") 
		{
			circBuff.push(1, 'a');
			circBuff.push(2, 'b');
			circBuff.push(3, 'c');
			circBuff.push(4, 'd');
			circBuff.push(5, 'e');
			circBuff.push(6, 'f');
			circBuff.push(7, 'g');
			circBuff.push(8, 'h');
			circBuff.push(9, 'i');
			THEN("The last 7 elements can be retrieved") 
			{
				char output;
				circBuff.get_at(3, output);
				CHECK(output == 'c');
				circBuff.get_at(4, output);
				CHECK(output == 'd');
				circBuff.get_at(5, output);
				CHECK(output == 'e');
				circBuff.get_at(6, output);
				CHECK(output == 'f');
				circBuff.get_at(7, output);
				CHECK(output == 'g');
				circBuff.get_at(8, output);
				CHECK(output == 'h');
				circBuff.get_at(9, output);
				CHECK(output == 'i');
			}
		}
	}
}

SCENARIO ("Passing in time not in order. Time less than mNewest", "[CircularBuffer]")
{	
	GIVEN("A buffer where the last element inserted has a time of 100") 
	{
		CircularBuffer<char, 3> circBuff;
		circBuff.push(4, 'a');
		circBuff.push(20, 'b');
		circBuff.push(50, 'c');
		circBuff.push(100, 'd');

		WHEN("An insertion of lower than 100 is attempted") 
		{
			bool result = circBuff.push(99.9, 'e');
			THEN("The buffer size is not increased and it returns a false")
			{
				CHECK(result == false);
				char output;
				circBuff.get_at(20, output);
				CHECK(output == 'b');
			}
			THEN("the element is not in the buffer")
			{
				char output;
				circBuff.get_at(99.9, output);
				CHECK(output == 'd');	// Find tha value for 100 instead
			}
		}
		WHEN("An insertion of greater than 100 is attempted") 
		{
			bool result = circBuff.push(100.1, 'e');
			THEN("The insertion is successful and it returns a true")
			{
				CHECK(result == true);
				char output;
				circBuff.get_at(100.1, output);
				CHECK(output == 'e');
			}
		}
	}
}

SCENARIO ("The circular buffer is full but has not wrapped yet", "[CircularBuffer]")
{
	GIVEN("A buffer of size 5 with 5 elements in it") 
	{
		CircularBuffer<char, 5> circBuff;
		circBuff.push(4, 'a');
		circBuff.push(20, 'b');
		circBuff.push(50, 'c');
		circBuff.push(100, 'd');
		circBuff.push(150, 'e');

		THEN("The data is retrievable")
		{
			char output;
			circBuff.get_at(20, output);
			CHECK(output == 'b');
			circBuff.get_at(50, output);
			CHECK(output == 'c');
		    circBuff.get_at(150, output);
			CHECK(output == 'e');
		}
	}
}

SCENARIO ("The circular buffer is not full", "[CircularBuffer]")
{
	GIVEN("A buffer of size 50 with 5 elements in it") 
	{
		CircularBuffer<char, 50> circBuff;
		circBuff.push(4, 'a');
		circBuff.push(20, 'b');
		circBuff.push(50, 'c');
		circBuff.push(100, 'd');
		circBuff.push(150, 'e');

		THEN("The data is retrievable")
		{
			char output;
			circBuff.get_at(20, output);
			CHECK(output == 'b');
			circBuff.get_at(50, output);
			CHECK(output == 'c');
		    circBuff.get_at(150, output);
			CHECK(output == 'e');
		}
	}
}

SCENARIO ("The requested time is inbetween", "[CircularBuffer]")
{
	GIVEN("A buffer of size 7 with 5 elements in it") 
	{
		CircularBuffer<char, 7> circBuff;
		circBuff.push(1, 'a');
		circBuff.push(2, 'b');
		circBuff.push(3, 'c');
		circBuff.push(4, 'd');
		circBuff.push(5, 'e');
		circBuff.push(6, 'f');
		circBuff.push(7, 'g');
		circBuff.push(8, 'h');
		circBuff.push(9, 'i');
		circBuff.push(10, 'j');

		THEN("The data is retrievable")
		{
			char output;
			circBuff.get_at(7.3, output);
			CHECK(output == 'g');
		}
		THEN("The data is retrievable")
		{
			char output;
			circBuff.get_at(7.8, output);
			CHECK(output == 'h');
		}
	}
}

SCENARIO ("When an element is requested that does not exist the closest match shall be returned", "[CircularBuffer]")
{
	GIVEN("A buffer without the value 4 or 6") 
	{
		CircularBuffer<int, 6> circBuff;
		circBuff.push(1, 1);
		circBuff.push(1, 2);
		circBuff.push(10, 10);
		circBuff.push(20, 20);
		circBuff.push(30, 30);
		circBuff.push(40, 40);
		WHEN("4 is requested") 
		{
			int output;
			circBuff.get_at(4, output);
			THEN("the closest match (10) will be retrieved") 
			{
				CHECK(output == 2);
			}
		}
		WHEN("6 is requested") 
		{
			int output;
			circBuff.get_at(6, output);
			THEN("the closest match (10) will be retrieved") 
			{
				CHECK(output == 10);
			}
		}
		WHEN("25.1 is requested")
		{
			int output;
			circBuff.get_at(25.1, output);
			THEN("the closest match (10) will be retrieved")
			{
				CHECK(output == 30);
			}
		}
		WHEN("The smallest element is requested")
		{
			int output;
			circBuff.get_at(1, output);
			THEN("the closest match (1) will be retrieved")
			{
				CHECK(output == 1);
			}
		}
		WHEN("The largest element is requested")
		{
			int output;
			circBuff.get_at(40, output);
			THEN("the closest match (1) will be retrieved")
			{
				CHECK(output == 40);
			}
		}
		WHEN("1.3 is requested")
		{
			int output;
			circBuff.get_at(1.3, output);
			THEN("the closest match (2) will be retrieved")
			{
				CHECK(output == 2);
			}
		}
	}
}

//*********** End of black box tests********** 

//*********** Start of multi-threaded tests********** 
SCENARIO("Populate the buffer with one thread and read from it from another", "[CircularBuffer]") 
{
	GIVEN("A circular buffer")
	{
		CircularBuffer<int, 10000> circBuff;

		WHEN("A thread is populating the buffer") 
		{
			
			std::thread populator([&circBuff]() {
				for (int i = 0; i < 10000; ++i)
				{
					circBuff.push(i, i);
				}
			});

			// Sleep to give the publisher a head start.
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			THEN("Another thread can concurrently read from the buffer")
			{
				std::thread consumer([&circBuff]() {
					for (int i=0; i < 10000; ++i)
					{
						int output;
						circBuff.get_at(i, output);
						CHECK(output == i);
					}
				});
				consumer.join();
			}
			populator.join();
			
		}
	}
}
//*********** End of multi-threaded tests********** 

//*********** Start of white box tests********** 
SCENARIO("Verify the buffer wraps when the number of elements inserted is greater than the size", "[CircularBuffer]") 
{
	GIVEN("A buffer of size 3") {
		CircularBuffer<int, 3> circBuff;
		WHEN("7 elements are inserted") 
		{
			circBuff.push(10, 1);
			circBuff.push(20, 2);
			circBuff.push(30, 3);
			circBuff.push(40, 4);
			circBuff.push(50, 5);
			circBuff.push(60, 6);
			circBuff.push(70, 7);
			
			THEN("The oldest value is 5") 
			{
				int output;
				FoundStatus result;
				result = circBuff.get_at(10, output);
				CHECK(result == FoundStatus::TOO_OLD);
				CHECK(output == 5);
			}
			THEN("The values at each index are as expected") 
			{
                CHECK(circBuff.mBuffer.at(0).second == 7);
                CHECK(circBuff.mBuffer.at(1).second == 5);
                CHECK(circBuff.mBuffer.at(2).second == 6);
			}
		}
	}
}

SCENARIO("The 'mNewest' and 'mOldest' iterators are in the correct place", "[CircularBuffer]") 
{
	GIVEN("A buffer of size 3") 
	{
		CircularBuffer<int, 5> circBuff;
		WHEN("The buffer is wraps") 
		{
			circBuff.push(10, 1);
			circBuff.push(20, 2);
			circBuff.push(30, 3);
			circBuff.push(40, 4);
			circBuff.push(50, 5);
			circBuff.push(60, 6);
			circBuff.push(70, 7);
			
			THEN("The iterator positions are as expected") 
			{
                CHECK(circBuff.mNewest->second == 7);
                CHECK(circBuff.mOldest->second == 3);
			}
		}
        WHEN("the buffer is not full") 
		{
			circBuff.push(10, 1);
			circBuff.push(20, 2);
			circBuff.push(30, 3);
			
			THEN("Iterator positions are as expected") 
			{
                CHECK(circBuff.mNewest->second == 3);
                CHECK(circBuff.mOldest->second == 1);
			}
		}
	}
}
//*********** End of white box tests********** 
