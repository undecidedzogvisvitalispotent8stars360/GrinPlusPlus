#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class SharedQueue
{
public:
    SharedQueue() = default;
    ~SharedQueue() = default;

    T& SharedQueue<T>::front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        return queue_.front();
    }

    void SharedQueue<T>::pop_front()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        queue_.pop_front();
    }

    void SharedQueue<T>::push_back(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(item);
        mlock.unlock();     // unlock before notificiation to minimize mutex con
        cond_.notify_one(); // notify one waiting thread

    }

    void SharedQueue<T>::push_back(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(std::move(item));
        mlock.unlock();     // unlock before notificiation to minimize mutex con
        cond_.notify_one(); // notify one waiting thread

    }

    int SharedQueue<T>::size()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        int size = queue_.size();
        mlock.unlock();
        return size;
    }

    bool SharedQueue<T>::empty()
    {
        return queue_.empty();
    }

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};