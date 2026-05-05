#pragma once
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>

namespace conference {
namespace utils {

/**
 * @class TaskQueue
 * @brief Simple thread pool for executing background tasks
 */
class TaskQueue {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

public:
    TaskQueue(size_t threads = 4);
    ~TaskQueue();

    /**
     * @brief Add a task to the queue
     */
    void enqueue(std::function<void()> task);

    /**
     * @brief Stop all worker threads
     */
    void shutdown();
};

} // namespace utils
} // namespace conference
