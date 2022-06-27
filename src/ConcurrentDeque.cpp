#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>

#include <opencv2/opencv.hpp>

template <class T>
class ConcurrentDeque
{
private:
    std::deque<T> deque = std::deque<T>();
    std::mutex mutex;
    std::condition_variable cv;

public:
    void push(T el)
    {
        std::lock_guard<std::mutex> lock(mutex);
        deque.push_front(el);
        cv.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        
        while(deque.size() == 0) 
            cv.wait(lock);

        T el = deque.back();
        deque.pop_back();
        
        return el;
    }

    void push_empty(int nw, Mat frame)
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (int i = 0; i < nw; i++)
            deque.push_front(frame);

        cv.notify_all();
    }
};