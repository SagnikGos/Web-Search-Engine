#include "crawler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Crawler::Crawler(const CrawlConfig& config)
    : config_(config), thread_pool_(config.num_threads) {
    EnsureOutputDir();
    int max_id = -1;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(config_.output_dir)) {
            if (entry.path().extension() == ".json") {
                std::string stem = entry.path().stem().string();
                try {
                    int id = std::stoi(stem);
                    if (id > max_id) max_id = id;
                } catch(...) {}
            }
        }
    } catch(...) {}
    next_doc_id_ = max_id + 1;
}

void Crawler::EnsureOutputDir() {
    std::filesystem::create_directories(config_.output_dir);
}

void Crawler::Start() {
    EnsureOutputDir();
    
    std::cout << "Starting crawler with " << config_.num_threads 
              << " threads, max " << config_.max_pages << " pages" << std::endl;
              
    for (const auto& url : config_.seed_urls) {
        url_queue_.Push(url);
    }
    
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < config_.num_threads; ++i) {
        futures.push_back(thread_pool_.Enqueue([this]() {
            std::string url;
            while (pages_crawled_.load() < static_cast<int>(config_.max_pages)) {
                if (!url_queue_.Pop(url)) {
                    break;
                }
                if (pages_crawled_.load() >= static_cast<int>(config_.max_pages)) {
                    break;
                }
                CrawlPage(url);
            }
        }));
    }
    
    int empty_checks = 0;
    while (pages_crawled_.load() < static_cast<int>(config_.max_pages)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\rProgress: " << pages_crawled_.load() << "/" << config_.max_pages 
                  << " pages | Queue: " << url_queue_.QueueSize() << " URLs    " << std::flush;
                  
        if (url_queue_.QueueSize() == 0 && url_queue_.VisitedCount() > 0) {
            empty_checks++;
            if (empty_checks >= 10) {
                std::cout << "\nQueue empty for 10 seconds, stopping." << std::endl;
                break;
            }
        } else {
            empty_checks = 0;
        }
    }
    
    url_queue_.Shutdown();
    for (auto& f : futures) {
        f.get();
    }
    
    std::cout << "\nCrawl complete! Total pages: " << pages_crawled_.load() << std::endl;
}

void Crawler::CrawlPage(const std::string& url) {
    auto response = fetcher_.Fetch(url);
    if (!response.success) {
        std::cerr << "\nFailed to fetch: " << url << std::endl;
        return;
    }
    
    auto parsed_data = parser_.Parse(response.body, url);
    int doc_id = next_doc_id_.fetch_add(1);
    
    SavePage(doc_id, url, parsed_data.title, parsed_data.body_text);
    pages_crawled_.fetch_add(1);
    
    int links_found = 0;
    for (const auto& link : parsed_data.links) {
        if (link.find("http://") == 0 || link.find("https://") == 0) {
            url_queue_.Push(link);
            links_found++;
        }
    }
    
    // Optional print for debug
    // std::cout << "[" << doc_id << "] Crawled: " << url << " (found " << links_found << " links)\n";
}

void Crawler::SavePage(int doc_id, const std::string& url,
                       const std::string& title, const std::string& text) {
    json j = {
        {"doc_id", doc_id},
        {"url", url},
        {"title", title},
        {"body", text}
    };
    
    std::string filename = std::to_string(doc_id) + ".json";
    std::filesystem::path file_path = std::filesystem::path(config_.output_dir) / filename;
    
    std::lock_guard<std::mutex> lock(save_mutex_);
    std::ofstream out(file_path);
    if (out.is_open()) {
        out << j.dump(4);
    }
}

size_t Crawler::PagesCrawled() const {
    return pages_crawled_.load();
}
