#include <iostream>
#include <string>
#include <vector>
#include "crawler/crawler.h"

void PrintUsage() {
    std::cout << "Usage: search_engine <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  crawl <seed_url> [--max-pages N] [--threads N] [--output-dir DIR]" << std::endl;
    std::cout << "    Crawl web pages starting from the seed URL." << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  search_engine crawl https://en.wikipedia.org/wiki/C++ --max-pages 100" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "crawl") {
        if (argc < 3) {
            std::cerr << "Error: crawl requires a seed URL." << std::endl;
            PrintUsage();
            return 1;
        }

        CrawlConfig config;
        config.seed_urls.push_back(argv[2]);

        // Parse optional arguments
        for (int i = 3; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--max-pages" && i + 1 < argc) {
                config.max_pages = std::stoul(argv[++i]);
            } else if (arg == "--threads" && i + 1 < argc) {
                config.num_threads = std::stoul(argv[++i]);
            } else if (arg == "--output-dir" && i + 1 < argc) {
                config.output_dir = argv[++i];
            }
        }

        std::cout << "=== Web Search Engine Crawler ===" << std::endl;
        std::cout << "Seed URL: " << config.seed_urls[0] << std::endl;
        std::cout << "Max pages: " << config.max_pages << std::endl;
        std::cout << "Threads: " << config.num_threads << std::endl;
        std::cout << "Output: " << config.output_dir << std::endl;
        std::cout << std::endl;

        Crawler crawler(config);
        crawler.Start();

        std::cout << "\nTotal pages crawled: " << crawler.PagesCrawled() << std::endl;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }

    return 0;
}
