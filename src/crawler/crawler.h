#ifndef CRAWLER_H
#define CRAWLER_H

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include "thread_pool.h"
#include "url_queue.h"
#include "http_fetcher.h"
#include "html_parser.h"

struct CrawlConfig {
    std::vector<std::string> seed_urls;
    size_t max_pages = 1000;
    size_t num_threads = 8;
    std::string output_dir = "./data/crawled_pages/";
};

class Crawler {
public:
    explicit Crawler(const CrawlConfig& config);
    void Start();  // Blocks until crawl completes
    size_t PagesCrawled() const;

private:
    void CrawlPage(const std::string& url);
    void SavePage(int doc_id, const std::string& url,
                  const std::string& title, const std::string& text);
    void EnsureOutputDir();

    CrawlConfig config_;
    UrlQueue url_queue_;
    ThreadPool thread_pool_;
    HttpFetcher fetcher_;  // Each thread will create its own or we can use one (cpr is thread-safe per-request)
    HtmlParser parser_;
    std::atomic<int> pages_crawled_{0};
    std::atomic<int> next_doc_id_{0};
    std::mutex save_mutex_;  // Protect file I/O
    std::mutex fetch_mutex_; // Protect fetcher if needed
};

#endif // CRAWLER_H
