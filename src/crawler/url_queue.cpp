#include "url_queue.h"

UrlQueue::UrlQueue() : shutdown_(false) {}

bool UrlQueue::Push(const std::string& url) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if the URL has already been visited or enqueued
    if (visited_.find(url) != visited_.end()) {
        return false;
    }
    
    // Mark as visited and push to the queue
    visited_.insert(url);
    queue_.push(url);
    condition_.notify_one();
    
    return true;
}

bool UrlQueue::Pop(std::string& url) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    // Wait until the queue is not empty or we are shutting down
    condition_.wait(lock, [this] {
        return !queue_.empty() || shutdown_;
    });
    
    // If shutting down and the queue is empty, return false
    if (shutdown_ && queue_.empty()) {
        return false;
    }
    
    // Pop the front URL
    url = queue_.front();
    queue_.pop();
    
    return true;
}

void UrlQueue::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    shutdown_ = true;
    condition_.notify_all();
}

bool UrlQueue::IsEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

size_t UrlQueue::VisitedCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return visited_.size();
}

size_t UrlQueue::QueueSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}
