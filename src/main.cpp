#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "crawler/crawler.h"
#include "indexer/tokenizer.h"
#include "indexer/stop_words.h"
#include "indexer/document_store.h"
#include "indexer/inverted_index.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void PrintUsage() {
    std::cout << "Usage: search_engine <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  crawl <seed_url> [--max-pages N] [--threads N] [--output-dir DIR]" << std::endl;
    std::cout << "    Crawl web pages starting from the seed URL." << std::endl;
    std::cout << std::endl;
    std::cout << "  index [--input-dir DIR] [--stop-words FILE]" << std::endl;
    std::cout << "    Build an inverted index from crawled pages." << std::endl;
    std::cout << std::endl;
    std::cout << "  search <query> [--input-dir DIR] [--stop-words FILE] [--top-k N]" << std::endl;
    std::cout << "    Search the index for matching documents. (Phase 3)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  search_engine crawl https://en.wikipedia.org/wiki/C++ --max-pages 100" << std::endl;
    std::cout << "  search_engine index --input-dir ./data/crawled_pages/" << std::endl;
}

// ========================
// CRAWL Command
// ========================
int RunCrawl(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: crawl requires a seed URL." << std::endl;
        PrintUsage();
        return 1;
    }

    CrawlConfig config;
    config.seed_urls.push_back(argv[2]);

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
    return 0;
}

// ========================
// INDEX Command
// ========================
int RunIndex(int argc, char* argv[]) {
    std::string input_dir = "./data/crawled_pages/";
    std::string stop_words_file = "./data/stop_words.txt";

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--input-dir" && i + 1 < argc) {
            input_dir = argv[++i];
        } else if (arg == "--stop-words" && i + 1 < argc) {
            stop_words_file = argv[++i];
        }
    }

    std::cout << "=== Web Search Engine Indexer ===" << std::endl;
    std::cout << "Input directory: " << input_dir << std::endl;
    std::cout << "Stop words file: " << stop_words_file << std::endl;
    std::cout << std::endl;

    // Initialize components
    Tokenizer tokenizer;
    StopWordFilter stop_filter(stop_words_file);
    DocumentStore doc_store;
    InvertedIndex index;

    std::cout << "Loaded " << stop_filter.Size() << " stop words." << std::endl;

    // Collect all JSON files
    std::vector<fs::path> json_files;
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.path().extension() == ".json") {
            json_files.push_back(entry.path());
        }
    }

    // Sort by filename for consistent ordering
    std::sort(json_files.begin(), json_files.end());

    std::cout << "Found " << json_files.size() << " documents to index." << std::endl;
    std::cout << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Process each document
    int processed = 0;
    for (const auto& file_path : json_files) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << file_path << std::endl;
            continue;
        }

        try {
            json j = json::parse(file);
            std::string url = j.value("url", "");
            std::string title = j.value("title", "");
            std::string body = j.value("body", "");

            // Tokenize and filter
            auto tokens = tokenizer.Tokenize(body);
            auto filtered_tokens = stop_filter.Filter(tokens);

            // Add to document store and index
            int doc_id = doc_store.AddDocument(url, title, body,
                                               static_cast<int>(filtered_tokens.size()));
            index.AddDocument(doc_id, filtered_tokens);

            processed++;
            if (processed % 10 == 0 || processed == static_cast<int>(json_files.size())) {
                std::cout << "\rIndexed: " << processed << "/" << json_files.size()
                          << " documents" << std::flush;
            }
        } catch (const json::parse_error& e) {
            std::cerr << "Warning: JSON parse error in " << file_path << ": " << e.what() << std::endl;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Print statistics
    std::cout << "\n\n=== Indexing Complete ===" << std::endl;
    std::cout << "Documents indexed: " << doc_store.TotalDocuments() << std::endl;
    std::cout << "Unique terms: " << index.TermCount() << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;
    std::cout << std::endl;

    // Sample lookups to demonstrate the index works
    std::cout << "=== Sample Index Lookups ===" << std::endl;
    std::vector<std::string> sample_terms = {"search", "engine", "google", "web", "algorithm"};
    for (const auto& term : sample_terms) {
        const auto& postings = index.GetPostings(term);
        if (!postings.empty()) {
            std::cout << "  '" << term << "' -> found in " << postings.size()
                      << " document(s):";
            // Show first 5 doc IDs
            int shown = 0;
            for (const auto& p : postings) {
                if (shown >= 5) {
                    std::cout << " ...";
                    break;
                }
                std::cout << " [doc" << p.doc_id << ":" << p.term_frequency << "x]";
                shown++;
            }
            std::cout << std::endl;
        } else {
            std::cout << "  '" << term << "' -> not found" << std::endl;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "crawl") {
        return RunCrawl(argc, argv);
    } else if (command == "index") {
        return RunIndex(argc, argv);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }
}
