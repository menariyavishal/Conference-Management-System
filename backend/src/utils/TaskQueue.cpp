#include "utils/TaskQueue.h"

namespace conference {
namespace utils {

TaskQueue::TaskQueue(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    
                    if(this->stop && this->tasks.empty()) return;
                    
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

void TaskQueue::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if(stop) return;
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

void TaskQueue::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker : workers) {
        if(worker.joinable()) worker.join();
    }
}

TaskQueue::~TaskQueue() {
    shutdown();
}

} // namespace utils
} // namespace conference
