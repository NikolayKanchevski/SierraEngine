//
// Created by Nikolay Kanchevski on 18.02.24.
//

#pragma once

namespace SierraEngine
{

    struct ThreadPoolCreateInfo
    {
        uint32 threadCount = 0;
    };

    class SIERRA_ENGINE_API ThreadPool final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using Task = std::function<void()>;

        /* --- CONSTRUCTORS --- */
        explicit ThreadPool(const ThreadPoolCreateInfo &createInfo)
            : running(true)
        {
            threads.resize(glm::clamp(createInfo.threadCount, 1U, std::thread::hardware_concurrency()));
            std::ranges::generate(threads, [this]() -> std::thread { return std::thread(&ThreadPool::ThreadLoop, this); });
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetThreadCount() const { return static_cast<uint32>(threads.size()); }
        [[nodiscard]] bool IsPaused() const { return paused; }

        [[nodiscard]] uint32 GetQueuedTaskCount() const { const std::lock_guard taskLock(taskMutex); return static_cast<uint32>(taskQueue.size()); }
        [[nodiscard]] uint32 GetRunningTaskCount() const { const std::lock_guard taskLock(taskMutex); return totalTaskCount - static_cast<uint32>(taskQueue.size()); }
        [[nodiscard]] uint32 GetTotalTaskCount() const { return totalTaskCount; }

        /* --- POLLING METHODS --- */
        void Pause()
        {
            paused = true;
        }

        void Resume()
        {
            paused = false;
        }

        template<typename T, typename... Args>
        void PushTask(T&& task, Args&&... args)
        {
            // Insert task into queue
            {
                const Task taskFunction = std::bind(std::forward<T>(task), std::forward<Args>(args)...);
                const std::lock_guard taskLock(taskMutex);
                taskQueue.push(taskFunction);
            }

            // Increment task count (safe, since on main thread)
            ++totalTaskCount;
            taskAvailable.notify_one();
        }

        template<typename T, typename... Args, typename R = std::invoke_result_t<std::decay_t<T>, std::decay_t<Args>...>>
        [[nodiscard]] std::future<R> Submit(T&& task, Args&&... args)
        {
            // Create a promise task
            std::function<R()> taskFunction = std::bind(std::forward<T>(task), std::forward<Args>(args)...);
            std::shared_ptr<std::promise<R>> promise = std::make_shared<std::promise<R>>();

            // Push task and handle return values and exceptions
            PushTask([taskFunction, promise]
            {
                try
                {
                    if constexpr (std::is_void_v<R>)
                    {
                        std::invoke(taskFunction);
                        promise->set_value();
                    }
                    else
                    {
                        promise->set_value(std::invoke(taskFunction));
                    }
                }
                catch (...)
                {
                    try { promise->set_exception(std::current_exception()); }
                    catch (...) { /* NOLINT(*-empty-catch) */ }
                }
            });

            return promise->get_future();
        }

        void WaitForTasks()
        {
            waiting = true;
            std::unique_lock taskLock(taskMutex);
            taskCompleted.wait(taskLock, [this]() -> bool { return totalTaskCount == (paused ? taskQueue.size() : 0); });
            waiting = false;
        }

        /* --- OPERATORS --- */
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        /* --- DESTRUCTOR --- */
        ~ThreadPool()
        {
            WaitForTasks();

            running = false;
            taskAvailable.notify_all();
            for (std::thread &thread : threads) thread.join();
        }

    private:
        std::vector<std::thread> threads = { };
        std::atomic<bool> paused = false;
        std::atomic<bool> running = false;
        std::atomic<bool> waiting = false;

        std::queue<Task> taskQueue = { };
        std::atomic<uint32> totalTaskCount = 0;

        mutable std::mutex taskMutex = { };
        std::condition_variable taskAvailable = { };
        std::condition_variable taskCompleted = { };

        void ThreadLoop()
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


    };

}
