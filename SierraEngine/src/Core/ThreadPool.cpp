//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#include "ThreadPool.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    ThreadPool::ThreadPool(const ThreadPoolCreateInfo &createInfo)
        : running(true)
    {
        threads.resize(glm::clamp(createInfo.threadCount, 1U, std::thread::hardware_concurrency()));
        std::ranges::generate(threads, [this]() -> std::thread { return std::thread(&ThreadPool::ThreadLoop, this); });
    }

    /* --- POLLING METHODS --- */

    void ThreadPool::Pause()
    {
        paused = true;
    }

    void ThreadPool::Resume()
    {
        paused = false;
    }

    void ThreadPool::WaitForTasks()
    {
        waiting = true;
        std::unique_lock taskLock(taskMutex);
        taskCompleted.wait(taskLock, [this]() -> bool { return totalTaskCount == (paused ? taskQueue.size() : 0); });
        waiting = false;
    }

    /* --- DESTRUCTOR --- */

    ThreadPool::~ThreadPool()
    {
        WaitForTasks();

        running = false;
        taskAvailable.notify_all();
        for (std::thread &thread : threads) thread.join();
    }

    /* --- PRIVATE METHODS --- */

    void ThreadPool::ThreadLoop()
    {
        // While pool is functional
        while (running)
        {
            // Block thread until a task has been added to queue
            std::unique_lock taskLock(taskMutex);
            taskAvailable.wait(taskLock, [this]() -> bool { return !taskQueue.empty() || !running; });

            // If not paused and a task is present
            if (running && !paused)
            {
                // Retrieve task
                const Task task = std::move(taskQueue.front());
                taskQueue.pop();

                // Execute task and unlock mutex
                taskLock.unlock();
                task();

                // Safely decrement task count (by locking mutex)
                taskLock.lock();
                --totalTaskCount;

                // If waiting, signal upon completion
                if (waiting) taskCompleted.notify_one();
            }
        }
    }

}