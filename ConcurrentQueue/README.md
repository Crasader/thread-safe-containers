# Overview

# Usage

# Rationale
## tryFrontPop() return type
This function needs to return 2 bits of information 
* Whether it was successful
* The popped data

### Iterator `Containter::const_iterator`
The underlying could be changed to std::deque to allow iterator access.  
The concurrentQueue should not return an iterator to the underlying container because then a user could break the encapsulation and bypass the thread safety to access queue elements.

### Unique Pointer `std::unique_ptr<T>`
The function could return a nullptr if the queue was empty and return a pointer to the data if it was possible.
This would in memory allocation on the heap every time data is accessed.

### Pair `std::pair<T,bool>`
If data was available the function could return the data, and `true` to signify the data is good.
Otherwise the return type would be default constructed and returned with a `false` to signify the data is bad.
The bool should be the 2nd param to be consistent with the [STL](https://en.cppreference.com/w/cpp/container/map/insert) and for [performance reasons](https://stackoverflow.com/questions/56761591/how-do-i-organize-members-in-a-struct-to-waste-the-least-space-on-alignment).
This means only default constructable objects can be stored in the queue.

### Out Parameter 
Similiar to the pair method except the data is returned via an out parameter. This puts the responsibility on the calling code to construct the object. 

### Optional `std::optional<T>`
Calling code doesn't have to construct any out param objects.
Clean syntax to check if data is valid.
