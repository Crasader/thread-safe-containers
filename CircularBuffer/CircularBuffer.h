#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <array>
#include <shared_mutex>
#include <mutex>
#include <utility>

enum class FoundStatus {
    EMPTY,
    TOO_OLD,
    TOO_NEW,
    FOUND
};

/**
* @brief Template class to provide a thread safe circular buffer. 
*/
template<typename T, std::size_t SIZE>
class CircularBuffer
{
    public :
    CircularBuffer();
    CircularBuffer(const CircularBuffer&) = delete;            // Disable copying of the class
    CircularBuffer& operator=(const CircularBuffer&) = delete; // Disable assignment of the class

    /**
    * @brief Get element given a time
    * @param time The requested time to be searched for
    * @param item Outparam to the item will be written to
    * @return false if param is outwith limits or buffer is empty
    */
    FoundStatus get_at(const double time, T& item) const;        

    /**
    * @brief Insert an item
    * @param time The time associated with the item
    * @param item The item to be inserted
    */
    bool push(const double time, const T& item);          


    private :
    mutable std::shared_mutex                                 mBufferLock; // Mutex to allow multithreaded access
	std::array<std::pair<double, T>, SIZE>                    mBuffer;     // Underlying buffer
    typename std::array<std::pair<double, T>, SIZE>::iterator mNewest;     // Iterator to the last element inserted
    typename std::array<std::pair<double, T>, SIZE>::iterator mOldest;     // Iterator to the oldest element
    bool                                                      mFull;       // Flag set to true once size == capacity
	bool													  mEmpty;	   // Flag set to false once first element is inserted

    /**
    * @brief Advance iterator. If it points to the last element then wrap it back to the first element
    * @param iter The iterator to be advanced
    */
    void increment_iter(typename std::array<std::pair<double, T>, SIZE>::iterator& iter) ;

   /**
    * @brief Split the array into two arrays using mNewest as the splitting point
    * Determine which array the value is in and perform a binary search for it.
    * @param time The requested time to be searched for
    * @param item Outparam to the item will be written to
    */
    void find_item(const double time, T& item) const;
};

template<typename T, std::size_t SIZE>
CircularBuffer<T, SIZE>::CircularBuffer() : mFull(false),
											mEmpty(true),
											mNewest(std::prev(mBuffer.end())),
											mOldest(mBuffer.begin())
{}

template<typename T, std::size_t SIZE>
bool CircularBuffer<T, SIZE>::push(const double time, const T& item)
{
    std::scoped_lock<std::shared_mutex> aScopedLock(mBufferLock);

	if (!mEmpty && time < mNewest->first)								    // If the inserted time is less than latest time then cancel the insertion
	{
		return false;
	}
	else
	{
		increment_iter(mNewest);
		*mNewest = std::make_pair(time, item);                              // Write the data
		mEmpty = false;
	}

	if(!mFull)																// Check if the buffer is now full
	{
        mFull  = (mNewest == std::prev(mBuffer.end())) ? true : false;
    }
	else
    {
		increment_iter(mOldest);                                            // Move the mOldest iter to the next place
    }
    return true;
}

template<typename T, std::size_t SIZE>
FoundStatus CircularBuffer<T, SIZE>::get_at(const double time, T& item) const
{
    std::shared_lock<std::shared_mutex> aSharedLock(mBufferLock);

	if (mEmpty)						   // Check the buffer is not empty
    {
		return FoundStatus::EMPTY;
	}
	else if (time < mOldest->first)   // Check if the requested data is too old 
    {
		item = mOldest->second;
		return FoundStatus::TOO_OLD;
	}
	else if (time > mNewest->first)   // Check if the requested data is too new 
	{
		item = mNewest->second;
		return FoundStatus::TOO_NEW;
	}
	else                              // We know the requested data is within the limits of the buffer
	{
        find_item(time,item);
		return FoundStatus::FOUND;
	}  
}

template<typename T, std::size_t SIZE>
void CircularBuffer<T, SIZE>::increment_iter(typename std::array<std::pair<double, T>, SIZE>::iterator& iter) 
{
    if (iter == std::prev(mBuffer.end())) // If iterator is currently pointing to last element
    {
        iter = mBuffer.begin();           // Then wrap around and point to the first
    }
    else
    {
        std::advance(iter, 1);            // Othewise progress to the next element
    }
}

// mBuffer is sorted but the start point is not the lowest and end point is not the highest.
// This means we have two sorted arrays. For example: 
//  <----arr1---><-------arr2--->
//  | 8 | 9 | 10 | 4 | 5 | 6 | 7 | 
// Use mNewest to find where to split. (mNewest is included in the first array)
//   Everything equal to and below mNewest is sorted.
//   Everything greater than mNewest is sorted.
// Find which array the value is in
//   If value is greater than or equal to arr[0] then in first half
//   If value is less than or equal to arr[END] then in second half
// Perform normal binary search on whichever array the value is in
template<typename T, std::size_t SIZE>
void CircularBuffer<T, SIZE>::find_item(const double time, T& item) const
{
    typename std::array<std::pair<double, T>, SIZE>::const_iterator start_arr;
    typename std::array<std::pair<double, T>, SIZE>::const_iterator end_arr;

    // Find which array the value is in
    // Set the start and end point for the array which the value is in
    if( time >= mBuffer.begin()->first)                
    {
        // The start to the newest (inclusive)
        start_arr = mBuffer.begin();
        end_arr = mNewest;
    }
    else if (time <= std::prev(mBuffer.end())->first) 
    {	
        // One past the newest to the end
        start_arr = std::next(mNewest); 
        end_arr = std::prev(mBuffer.end());
    }
    else //  last element > time > first element - so no binary search needed
    {
		// find which one is closer
		double above_diff = mBuffer.begin()->first - time;
		double below_diff = time - std::prev(mBuffer.end())->first;
		item = (below_diff < above_diff) ? std::prev(mBuffer.end())->second : mBuffer.begin()->second;

        return;
    }

    // Binary search using std::lower_bound to find the first element which does not compare less than time
    // Use std::lower_bound and use a binary predicate comparison which will return true is the first element is less than the second
    // Comparison lamba will use the time (pair.first) to compare
    auto above_val = lower_bound(start_arr, end_arr, std::make_pair(time, item),
                            [](std::pair<double, T> lhs, std::pair<double, T> rhs) -> bool {
                                return lhs.first < rhs.first; }); 

	if (above_val->first == time)
	{
		item = above_val->second;
	}
	else // If not an exact match then find the closest
	{
		// Execution will not get here if the requested item
		//	* is less than the first element the function 
		//  * exactly the first element
		// So safe to do std::prev without it going out of bounds
		auto below_val = std::prev(above_val);
		double above_diff = above_val->first - time;
		double below_diff = time - below_val->first;
		item = (below_diff < above_diff) ? below_val->second : above_val->second;
	}
}

#endif //CIRCULARBUFFER_H