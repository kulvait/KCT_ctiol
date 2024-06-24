#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <atomic>
#include <cassert>
#include <optional>
#include <type_traits>

//Internal includes
#include "stringFormatter.h"
#include "PROG/KCTException.hpp"


namespace KCT::io {

/**
 * @class ThreadPool
 * @brief A simple thread pool implementation that supports pausing, resuming, and waiting for all tasks to complete.
 * 
 * The ThreadPool class manages a pool of worker threads that can execute tasks concurrently. Tasks are submitted to the
 * pool and are executed as threads become available. The pool can be paused and resumed, and it can wait for all tasks
 * to complete.
 * 
 * @tparam Worker The type of worker objects to be used by the threads. If no worker type is specified, the default is void.
 */
template <typename Worker = void>
class ThreadPool {
public:
    /**
     * @struct ThreadInfo
     * @brief Information about a thread in the pool.
     */
    struct ThreadInfo {
        size_t id;
        size_t pool_size;
        std::shared_ptr<Worker> worker;
    };

        /**
     * @brief Constructs a thread pool with the specified number of threads and worker objects.
     * 
     * @param num_threads The number of threads in the pool.
     * @param workers The worker objects to be used by the threads.
     */
    ThreadPool(size_t num_threads, std::vector<std::shared_ptr<Worker>> workers);

    /**
     * @brief Constructs a thread pool with the specified number of threads.
     * 
     * @param num_threads The number of threads in the pool.
     */
    ThreadPool(size_t num_threads);

    ~ThreadPool();

    /**
     * @brief Submits a task to the thread pool.
     *
     * This method submits a task to the thread pool for execution. The task is
     * provided as a function object and its arguments. The first argument to
     * the task function will be a shared pointer to ThreadInfo, which contains
     * information about the thread executing the task.
     *
     * @tparam Func The type of the function object.
     * @tparam Args The types of the function arguments.
     * @param func The function object to be executed.
     * @param args The arguments to pass to the function object.
     * @return A future representing the result of the task.
     */
    template <typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args) -> std::future<typename std::invoke_result<Func, std::shared_ptr<ThreadInfo>, Args...>::type>;

    /**
     * @brief Waits until all tasks in the queue are completed.
     *
     * This method blocks until all tasks in the queue have been processed
     * and all threads in the pool are idle.
     *
     * @note This method is equivalent to calling pause(true) followed by resume().
     */
    void waitAll();

    /**
     * @brief Pauses the thread pool.
     *
     * This method pauses the execution of tasks in the thread pool. If
     * blocking is set to true, it will wait until all currently running tasks
     * are completed and the queue is empty before returning.
     *
     * @param blocking If true, blocks until all tasks are completed.
     */
    void pause(bool blocking = false);

    /**
     * @brief Resumes the thread pool.
     *
     * This method resumes the execution of tasks in the thread pool after it
     * has been paused.
     */
    void resume();

    /**
     * @brief Sets the worker objects for the thread pool.
     *
     * This method updates the worker objects for the threads in the pool.
     * It can only be called when the pool is paused and the task queue is empty.
     *
     * @param workers The new worker objects.
     */
    void setWorkers(std::vector<std::shared_ptr<Worker>> workers);

private:
    size_t num_threads_; ///< The number of threads in the pool.
    std::vector<std::thread> workers_; ///< The worker threads.
    std::queue<std::function<void()>> tasks_; ///< The task queue.
    std::vector<std::shared_ptr<ThreadInfo>> thread_infos_; ///< Information about the threads.

    std::mutex queue_mutex_; ///< Mutex for synchronizing access to the task queue.
    std::condition_variable condition_; ///< Condition variable for synchronizing thread pool state changes.
    std::atomic_bool stop_; ///< Flag indicating whether the thread pool is stopping.
    std::atomic_bool paused_; ///< Flag indicating whether the thread pool is paused.
    std::atomic<size_t> runningTasksCount_; ///< The number of currently running tasks.
    std::vector<bool> isWorkerAssigned_; ///< Array indicating whether a worker is assigned to a thread.

    /**
     * @brief The function executed by each worker thread.
     * 
     * @param id The ID of the thread.
     * @param thread_info Information about the thread.
     */
    void workerThread(size_t id, std::shared_ptr<ThreadInfo> thread_info);
};

// Constructor with worker objects
template <typename Worker>
ThreadPool<Worker>::ThreadPool(size_t num_threads, std::vector<std::shared_ptr<Worker>> workers)
    : num_threads_(num_threads)
    , stop_(false)
    , paused_(false)
    , runningTasksCount_(0)
    , isWorkerAssigned_(num_threads, false)
{
    assert(num_threads == workers.size());
    for(size_t i = 0; i < num_threads; ++i)
    {
        auto thread_info = std::make_shared<ThreadInfo>(ThreadInfo{ i, num_threads, workers[i] });
        thread_infos_.emplace_back(thread_info);
        workers_.emplace_back(&ThreadPool::workerThread, this, i, thread_info);
    }
}

// Constructor without worker objects
template <typename Worker>
ThreadPool<Worker>::ThreadPool(size_t num_threads)
    : num_threads_(num_threads)
    , stop_(false)
    , paused_(false)
    , runningTasksCount_(0)
    , isWorkerAssigned_(num_threads, false)
{
    for(size_t i = 0; i < num_threads; ++i)
    {
        auto thread_info = std::make_shared<ThreadInfo>(ThreadInfo{ i, num_threads, nullptr });
        thread_infos_.emplace_back(thread_info);
        workers_.emplace_back(&ThreadPool::workerThread, this, i, thread_info);
    }
}

template <typename Worker>
ThreadPool<Worker>::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for(std::thread& worker : workers_)
        worker.join();
}

template <typename Worker>
template <typename Func, typename... Args>
auto ThreadPool<Worker>::submit(Func&& func, Args&&... args)
    -> std::future<typename std::invoke_result<Func, std::shared_ptr<ThreadInfo>, Args...>::type>
{
    using return_type =
        typename std::invoke_result<Func, std::shared_ptr<ThreadInfo>, Args...>::type;
    auto task = std::make_shared<std::packaged_task<return_type(std::shared_ptr<ThreadInfo>)>>(
        std::bind(std::forward<Func>(func), std::placeholders::_1, std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if(stop_)
            KCTERR("submit on stopped ThreadPool");

        condition_.wait(lock, [this] { return runningTasksCount_ < num_threads_ || stop_; });
        if(stop_)
            return res;

        size_t assigned_id = 0;
        for(size_t i = 0; i < num_threads_; ++i)
        {
            if(!isWorkerAssigned_[i])
            {
                assigned_id = i;
                isWorkerAssigned_[i] = true;
                ++runningTasksCount_;
                break;
            }
        }

        tasks_.emplace([task, assigned_id, this]() {
            (*task)(thread_infos_[assigned_id]);
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                isWorkerAssigned_[assigned_id] = false;
                --runningTasksCount_;
            }
            condition_.notify_all();
        });
    }
    condition_.notify_one();
    return res;
}

template <typename Worker>
void ThreadPool<Worker>::waitAll()
{
    std::unique_lock<std::mutex> lock(queue_mutex_);
    condition_.wait(lock, [this] { return tasks_.empty() && runningTasksCount_ == 0; });
}

template <typename Worker>
void ThreadPool<Worker>::pause(bool blocking)
{
    paused_ = true;
    if(blocking)
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        condition_.wait(lock, [this] { return tasks_.empty() && runningTasksCount_ == 0; });
    }
}

template <typename Worker>
void ThreadPool<Worker>::resume()
{
    paused_ = false;
    condition_.notify_all();
}

template <typename Worker>
void ThreadPool<Worker>::setWorkers(std::vector<std::shared_ptr<Worker>> workers)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        condition_.wait(lock, [this] { return tasks_.empty() && runningTasksCount_ == 0; });
        if(!paused_)
        {
            KCTERR("Cannot set workers while the pool is running");
        }
        assert(workers.size() == thread_infos_.size());
        for(size_t i = 0; i < workers.size(); ++i)
        {
            thread_infos_[i]->worker = workers[i];
        }
    }
}

template <typename Worker>
void ThreadPool<Worker>::workerThread(size_t id, std::shared_ptr<ThreadInfo> thread_info)
{
    while(true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] { return stop_ || (!paused_ && !tasks_.empty()); });
            if(stop_ && tasks_.empty())
                return;
            if(!paused_ && !tasks_.empty())
            {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }
        if(task)
            task();
    }
}

} // namespace KCT::io
