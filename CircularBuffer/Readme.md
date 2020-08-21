@page CircularBuffer
# Thread Safe Circular Buffer
## Usage

### Construction
```C++
CircularBuffer<std::string, 10> circBuff;
```
In this example `std::string` is the type and `10` is the size.

### Insertion
Data is inserted into the buffer by providing a time stamp for the data as the first parameter and the data itself as the second parameter.
```C++
std::string txt1 ("Hello World");
std::string txt2 ("Good Morning");
std::string txt3 ("Good Night");
circBuff.push(4321.32,txt1)
circBuff.push(4452.23,txt2)
circBuff.push(4567.21,txt3)
```
In this example `4321.32` is the time stamp and `txt1` is the data.

The buffer expects subsequent insertions to have an equal or later time stamp. An attempt to insert data with an older timestamp than the latest element will cause the function to return false without inserting the element.

### Retrieval 
To Retrieve the data provide a time stamp for which data you want and an out parameter for the data to be written to.
```C++
std::string output;
circBuff.get_at(4322, output);
```
In this example `4322` is the time stamp and `output` is the data.  
When retrieving data the timestamp does not need to equal the timestamp of the data. The function will return the data with the closest timestamp.
In this example it will return `4321.32`.

If the data requested is not within the buffer the function will return the nearest value and an ENUM to indicate if it is too old or too new.
```C++
std::string output1;
std::string output2;
FoundStatus result1 = circBuff.get_at(3982.54, output1);
FoundStatus result2 = circBuff.get_at(4975.23, output2);
```
In this example output1 will be `4321.32` and result1 will be `FoundStatus::TOO_OLD`.   
Output2 will be `4567.21` and result2 will be `FoundStatus::TOO_NEW`.   


For more example usage see the Tests folder.
	
## Time complexity
	
|Task	  |Function	call  |Complexity  |Note																   |
|---------|---------------|------------|-----------------------------------------------------------------------|
|Retrieve |	get_at	      | O(1) 	   |Actually O(logSIZE) where SIZE is a constant defined at construction   |
|Insert	  |	push     | O(1)       |						   											   |
			
## Limitations
1. It is assumed the circular buffer will always be sorted by time. This is assumed because newer data will have a later time stamp (bigger number) and will be inserted after older data.  
2. The size of the buffer must be known at compile time 
 
## Notes

### Duplicate time stamps
When there are two elements with the same time in the buffer and that time is requested the earliest data will be retrieved.  

### Multithreading
The push function uses an exclusive lock on the mutex so nothing else can run in parallel.  
The get_at function uses a shared_lock on the mutex so multiple calls to get_at can run in parallel. 

### Very large buffer
A very large buffer may result in a stack overflow. The buffer can be created on the heap to avoid this problem
 