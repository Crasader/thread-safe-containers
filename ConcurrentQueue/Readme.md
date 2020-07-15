@page ConcurrentQueue
# Thread Safe Concurrent Queue
## Background
Thread Safe Concurrent Queue - a wrapper around std::queue to make it thread safe.
First in, first out (FIFO)

## Functions
__size__ Returns size
__empty__ Test whether the container is empty
__tryFront__ Try to access the next element. Returns Status::FOUND if successful and Status::EMPTY if not.
__pop__ Remove the next element
__tryFrontAndPop__ Access the next elementt and remove it from the container.
__push__ Insert element


## Usage
Use it like a standard std::queue.

```C++
ConcurrentQueue<int> myQueue;
```
	
## Time complexity
See std::queue
	
|    Function	call     |Complexity  |
|------------------------|------------|
|	__size__	         | O(1) 	   |
|	__empty__            | O(1)       |
|	__tryFront__         | O(1)       |
|	__pop__              | O(1)       |
|	__tryFrontAndPop__   | O(1)       |
|	__push__             | O(1)       |

## Notes

### Multithreading
The push, pop, and tryFrontAndPop functions uses an exclusive lock on the mutex so nothing else can run in parallel.  
The size, empty and tryFront functions uses a shared_lock on the mutex so multiple calls to get_at can run in parallel.

This set up is essentially multiple reader, single writer.
 