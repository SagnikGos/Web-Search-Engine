#ifndef URL_QUEUE_H
#define URL_QUEUE_H

#include <queue>
#include <string>
#include <unordered_set>
#include <mutex>
#include <condition_variable>

class UrlQueue {
public:
    UrlQueue();
    ~UrlQueue() = default;

    // Disallow copy and assignment
    UrlQueue(const UrlQueue&) = delete;
    UrlQueue& operator=(const UrlQueue&) = delete;

    bool Push(const std::string& url);
    bool Pop(std::string& url);
    void Shutdown();
    bool IsEmpty() const;
    size_t VisitedCount() const;
    size_t QueueSize() const;

private:
    std::queue<std::string> queue_;
    std::unordered_set<std::string> visited_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool shutdown_;
};

#endif // URL_QUEUE_H
