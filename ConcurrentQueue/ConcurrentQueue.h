#ifndef CONCURRENTQUEUE_H
#define CONCURRENTQUEUE_H

#include <queue>
#include <shared_mutex>
#include <condition_variable>
#include <optional>

template<typename T, class Container = std::deque<T>>
class ConcurrentQueue
{
public:
    ConcurrentQueue() = default;
    ConcurrentQueue(const ConcurrentQueue& other);
    ConcurrentQueue<T>& operator=(const ConcurrentQueue& other);
    ~ConcurrentQueue() = default;

    void push(const T& obj);   
    T waitingPop();
    std::optional<T> tryPop();
    bool empty() const; 

private:
    mutable std::shared_mutex mMutex;
    std::queue<T, Container> mQueue;
    std::condition_variable mConVar;
};



template<typename T, class Container>
ConcurrentQueue<T, Container>::ConcurrentQueue(const ConcurrentQueue& other)
{
    std::shared_lock<std::shared_mutex> sharedLock(other.mMutex);
    mQueue = other.mQueue;
}

template<typename T, class Container>
ConcurrentQueue<T>& ConcurrentQueue<T, Container>::operator=(const ConcurrentQueue& other)
{
    // Check not self assigment
    if (this != &other)
    {
        // Lock both mutexes at the same time
        std::scoped_lock<std::shared_mutex> scoped_lock(mMutex, std::defer_lock);
        std::shared_lock<std::shared_mutex> other_shared_lock(other.mMutex, std::defer_lock);
        std::lock(scoped_lock, other_shared_lock);

        mQueue = other.mQueue;
    }
    return *this;
}

template<typename T, class Container>
void ConcurrentQueue<T, Container>::push(const T& obj)
{
    std::unique_lock<std::shared_mutex> uniqueLock(mMutex);
    bool const was_empty = mQueue.empty();

    mQueue.push(obj);

    // If the queue was empty there may be some consumers waiting for data.
    if (was_empty)
    {
        // Unlock mutex before notifying so waiting threads do not have to wait for the mutex after being woken.
        uniqueLock.unlock();
        mConVar.notify_one();
    }
}

template<typename T, class Container>
T ConcurrentQueue<T, Container>::waitingPop()
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    mConVar.wait(scopedLock, [&this] {return !mQueue.emtpy(); });
    T val = mQueue.front() //TODO:  value = std::move(data_queue.front()) ?
    mQueue.pop();
    return value;
}

template<typename T, class Container>
std::optional<T> ConcurrentQueue<T, Container>::tryPop()
{
    std::scoped_lock<std::shared_mutex> scopedLock(mMutex);
    if (!mQueue.empty())
    {
        T obj = mQueue.front();
        mQueue.pop();
        return obj;
    }
    else
    {
        return std::nullopt;
    }
}

template<typename T, class Container>
bool ConcurrentQueue<T, Container>::empty() const
{
    std::shared_lock<std::shared_mutex> sharedLock(mMutex);
    return mQueue.empty();
}

#endif