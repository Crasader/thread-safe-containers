#ifndef CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_H

#include <queue>
#include <shared_mutex>

/**
* @brief Template class wrapper around std::queue to make it thread safe
* Uses mutexes
* 
* Allows multiple consumers and multiple producers to use the queue at the same time
*
* Features implemented:
* - Size
* - Empty
* - Pop
* - TryFront
* - TryFrontAndPop
* - Push
*
* @note Instances of the class cannot be copied or assigned to another instance
*/
enum class QueueStatus 
{
    EMPTY,
    FOUND
};

template<typename T>
class ConcurrentQueue
{
private:
    mutable std::shared_mutex mMutex;
    std::queue<T> mQueue;
public:
    ConcurrentQueue(){};
    ~ConcurrentQueue(){};
    ConcurrentQueue(const ConcurrentQueue&) = delete; // Disable copying of the class
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; //Disable assignment of the class

    void push(const T& obj); ///< Add object to the back of the queue
    void push(T&& obj);      ///< Move object to the back of the queue
    const QueueStatus getNext(T& obj); ///< Takes in a object and assigns it the first value in the queue, then pops it off the queue

    [[deprecated]] const QueueStatus tryFrontAndPop(T& obj); ///< Takes in a object and assigns it the first value in the queue, then pops it off the queue
    [[deprecated]]  size_t size() const; ///< Returns the size of the queue
    bool empty() const; ///< Returns true/false if the queue is empty or not
    ///@todo Remove this front() function once the everything has been updated to tryFront();
    [[deprecated]]  const T& front() const; ///< Get a reference to the first item on the queue
    [[deprecated]]  const QueueStatus tryFront(T& obj) const; ///< Takes in a object and assigns it the first value in the queue
    [[deprecated]]  void pop(); ///< Pop from the front of the queue


    
};

template<typename T>
size_t ConcurrentQueue<T>::size() const
{
    std::shared_lock<std::shared_mutex> sharedLock(mMutex);
    return mQueue.size();
}

template<typename T>
bool ConcurrentQueue<T>::empty() const
{
    std::shared_lock<std::shared_mutex> sharedLock(mMutex);
    return mQueue.empty();
}

template<typename T>
const T& ConcurrentQueue<T>::front() const
{
    std::shared_lock<std::shared_mutex> sharedLock(mMutex);
    return mQueue.front();    
}

template<typename T>
const QueueStatus ConcurrentQueue<T>::tryFront(T& obj) const
{
    std::shared_lock<std::shared_mutex> sharedLock(mMutex);

    if (!mQueue.empty())
    { 
        obj = mQueue.front();
        return QueueStatus::FOUND;
    }  
    else
    {
        return QueueStatus::EMPTY;
    }  
}

template<typename T>
void ConcurrentQueue<T>::pop()
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    if (!mQueue.empty())
    { 
        mQueue.pop();
    }  
}

template<typename T>
const QueueStatus ConcurrentQueue<T>::tryFrontAndPop(T& obj)
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    if (!mQueue.empty())
    { 
        obj = mQueue.front();
        mQueue.pop();
        return QueueStatus::FOUND;
    }   
    else 
    {
        return QueueStatus::EMPTY;
    }
}

template<typename T>
void ConcurrentQueue<T>::push(const T& obj)
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    mQueue.push(obj);
}

template<typename T>
void ConcurrentQueue<T>::push(T&& obj)
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    mQueue.push(std::move(obj));
}

template<typename T>
const QueueStatus ConcurrentQueue<T>::getNext(T& obj)
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    if (!mQueue.empty())
    {
        obj = mQueue.front();
        mQueue.pop();
        return QueueStatus::FOUND;
    }
    else
    {
        return QueueStatus::EMPTY;
    }
}

#endif