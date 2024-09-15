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
        explicit ThreadPool(const ThreadPoolCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetThreadCount() const { return static_cast<uint32>(threads.size()); }
        [[nodiscard]] bool IsPaused() const { return paused; }

        [[nodiscard]] uint32 GetQueuedTaskCount() const { const std::lock_guard taskLock(taskMutex); return static_cast<uint32>(taskQueue.size()); }
        [[nodiscard]] uint32 GetRunningTaskCount() const { const std::lock_guard taskLock(taskMutex); return totalTaskCount - static_cast<uint32>(taskQueue.size()); }
        [[nodiscard]] uint32 GetTotalTaskCount() const { return totalTaskCount; }

        /* --- POLLING METHODS --- */
        void Pause();
        void Resume();
        void WaitForTasks();

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

        /* --- COPY SEMANTICS --- */
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        /* --- MOVE SEMANTICS --- */
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ThreadPool();

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

        void ThreadLoop();

    };

}
