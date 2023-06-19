/**
 * @file    Thread_Pool.h
 * @author  Marvin Smith
 * @date    6/18/2023
 *
 * @note  This was "borrowed" from
 *        https://github.com/mtrebi/thread-pool/blob/master/include/ThreadPool.h
*/
#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "Blocking_Queue.hpp"

class Thread_Pool
{

private:
    class Thread_Worker
    {
    private:
        int m_id;
        Thread_Pool *m_pool;

    public:
        Thread_Worker(Thread_Pool *pool, const int id)
            : m_pool(pool), m_id(id)
        {
        }

        void operator()()
        {
            std::function<void()> func;
            bool dequeued;
            while (!m_pool->m_shutdown)
            {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    if (m_pool->m_queue.empty())
                    {
                        m_pool->m_conditional_lock.wait(lock);
                    }
                    dequeued = m_pool->m_queue.pop(func);
                }
                if (dequeued)
                {
                    func();
                }
            }
        }
    };

    std::atomic<bool> m_shutdown;
    Blocking_Queue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;

public:
    Thread_Pool(const int n_threads)
        : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false)
    {
    }

    Thread_Pool(const Thread_Pool &) = delete;
    Thread_Pool(Thread_Pool &&) = delete;

    Thread_Pool &operator=(const Thread_Pool &) = delete;
    Thread_Pool &operator=(Thread_Pool &&) = delete;

    // Inits thread pool
    void init()
    {
        for (int i = 0; i < m_threads.size(); ++i)
        {
            m_threads[i] = std::thread(Thread_Worker(this, i));
        }
    }

    // Waits until threads finish their current task and shutdowns the pool
    void shutdown()
    {
        m_shutdown = true;
        m_queue.done();

        for (int i = 0; i < m_threads.size(); ++i)
        {
            m_conditional_lock.notify_all();
            if (m_threads[i].joinable())
            {
                m_threads[i].join();
            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]()
        {
            (*task_ptr)();
        };

        // Enqueue generic wrapper function
        m_queue.push(wrapper_func);

        // Wake up one thread if its waiting
        m_conditional_lock.notify_one();

        // Return future from promise
        return task_ptr->get_future();
    }
};