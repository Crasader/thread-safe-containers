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

    void push(const T& object);   
    void push(T&& object); 
    T waitingFrontPop();
    std::optional<T> tryFrontPop();
    bool empty() const; 

private:
    mutable std::shared_mutex mMutex;
    std::queue<T, Container> mQueue;
    std::condition_variable_any mConVar;
};



template<typename T, class Container>
ConcurrentQueue<T, Container>::ConcurrentQueue(const ConcurrentQueue& other)
{
    std::shared_lock sharedLock(other.mMutex);
    mQueue = other.mQueue;
}

template<typename T, class Container>
ConcurrentQueue<T>& ConcurrentQueue<T, Container>::operator=(const ConcurrentQueue& other)
{
    // Check for self assigment
    if (this != &other)
    {
        // Lock both mutexes at the same time
        std::unique_lock unique_lock(mMutex, std::defer_lock);
        std::unique_lock other_unique_lock(other.mMutex, std::defer_lock);
        std::lock(unique_lock, other_unique_lock);

        mQueue = other.mQueue;
    }
    return *this;
}

template<typename T, class Container>
void ConcurrentQueue<T, Container>::push(const T& object)
{
    std::unique_lock uniqueLock(mMutex);
    bool const was_empty = mQueue.empty();

    mQueue.push(object);

    // If the queue was empty there may be some consumers waiting for data.
    if (was_empty)
    {
        // Unlock mutex before notifying so waiting threads do not have to wait for the mutex after being woken.
        uniqueLock.unlock();
        mConVar.notify_one();
    }
}

template<typename T, class Container>
void ConcurrentQueue<T, Container>::push(T&& object)
{
    std::unique_lock uniqueLock(mMutex);
    bool const was_empty = mQueue.empty();

    mQueue.push(std::move(object));

    // If the queue was empty there may be some consumers waiting for data.
    if (was_empty)
    {
        // Unlock mutex before notifying so waiting threads do not have to wait for the mutex after being woken.
        uniqueLock.unlock();
        mConVar.notify_one();
    }
}

template<typename T, class Container>
T ConcurrentQueue<T, Container>::waitingFrontPop()
{
    std::unique_lock uniqueLock(mMutex);
    mConVar.wait(uniqueLock, [this] {return !mQueue.empty(); });
    T object = std::move(mQueue.front());
    mQueue.pop();
    return object;
}

template<typename T, class Container>
std::optional<T> ConcurrentQueue<T, Container>::tryFrontPop()
{
    std::scoped_lock scopedLock(mMutex);
    if (!mQueue.empty())
    {
        T object = std::move(mQueue.front());
        mQueue.pop();
        return object;
    }
    else
    {
        return std::nullopt;
    }
}

template<typename T, class Container>
bool ConcurrentQueue<T, Container>::empty() const
{
    std::shared_lock sharedLock(mMutex);
    return mQueue.empty();
}

#endif // Header Guard