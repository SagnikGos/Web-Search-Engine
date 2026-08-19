#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <memory>
#include <nlohmann/json.hpp>
#include "crawler/crawler.h"
#include "indexer/tokenizer.h"
#include "indexer/stop_words.h"
#include "indexer/document_store.h"
#include "indexer/inverted_index.h"
#include "indexer/index_serializer.h"
#include "search/query_processor.h"
#include "server/web_server.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void PrintUsage() {
    std::cout << "Usage: search_engine <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  crawl <seed_url> [--max-pages N] [--threads N] [--output-dir DIR]" << std::endl;
    std::cout << "    Crawl web pages starting from the seed URL." << std::endl;
    std::cout << std::endl;
    std::cout << "  index [--input-dir DIR] [--stop-words FILE] [--index-dir DIR]" << std::endl;
    std::cout << "    Build and save an inverted index from crawled pages." << std::endl;
    std::cout << std::endl;
    std::cout << "  search <query> [--index-dir DIR] [--stop-words FILE] [--top-k N]" << std::endl;
    std::cout << "    Search the saved index for matching documents." << std::endl;
    std::cout << std::endl;
    std::cout << "  server [--port N] [--frontend-dir DIR] [--index-dir DIR] [--stop-words FILE]" << std::endl;
    std::cout << "    Start the web search backend server." << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  search_engine crawl https://en.wikipedia.org/wiki/C++ --max-pages 100" << std::endl;
    std::cout << "  search_engine index" << std::endl;
    std::cout << "  search_engine search \"web search algorithm\"" << std::endl;
    std::cout << "  search_engine server --port 8080 --frontend-dir ./frontend" << std::endl;
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
// Helper: Build Index from crawled pages
// ========================
struct IndexBundle {
    Tokenizer tokenizer;
    StopWordFilter stop_filter;
    DocumentStore doc_store;
    InvertedIndex index;

    IndexBundle(const std::string& stop_words_file) : stop_filter(stop_words_file) {}
};

std::unique_ptr<IndexBundle> BuildIndex(const std::string& input_dir,
                                         const std::string& stop_words_file,
                                         bool verbose = true) {
    auto bundle = std::make_unique<IndexBundle>(stop_words_file);

    if (verbose) {
        std::cout << "Loaded " << bundle->stop_filter.Size() << " stop words." << std::endl;
    }

    std::vector<fs::path> json_files;
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.path().extension() == ".json") {
            json_files.push_back(entry.path());
        }
    }
    std::sort(json_files.begin(), json_files.end());

    if (verbose) {
        std::cout << "Found " << json_files.size() << " documents to index." << std::endl;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    int processed = 0;
    for (const auto& file_path : json_files) {
        std::ifstream file(file_path);
        if (!file.is_open()) continue;

        try {
            json j = json::parse(file);
            std::string url = j.value("url", "");
            std::string title = j.value("title", "");
            std::string body = j.value("body", "");

            auto tokens = bundle->tokenizer.Tokenize(body);
            auto filtered_tokens = bundle->stop_filter.Filter(tokens);

            int doc_id = bundle->doc_store.AddDocument(url, title, body,
                                                       static_cast<int>(filtered_tokens.size()));
            bundle->index.AddDocument(doc_id, filtered_tokens);
            processed++;

            if (verbose && (processed % 10 == 0 || processed == static_cast<int>(json_files.size()))) {
                std::cout << "\rIndexed: " << processed << "/" << json_files.size()
                          << " documents" << std::flush;
            }
        } catch (const json::parse_error& e) {
            std::cerr << "Warning: JSON parse error in " << file_path << ": " << e.what() << std::endl;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    if (verbose) {
        std::cout << "\n\nIndexing complete: " << bundle->doc_store.TotalDocuments()
                  << " documents, " << bundle->index.TermCount()
                  << " unique terms (" << duration.count() << " ms)" << std::endl;
    }

    return bundle;
}

// ========================
// INDEX Command (now saves to disk)
// ========================
int RunIndex(int argc, char* argv[]) {
    std::string input_dir = "./data/crawled_pages/";
    std::string stop_words_file = "./data/stop_words.txt";
    std::string index_dir = "./data/index/";

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--input-dir" && i + 1 < argc) {
            input_dir = argv[++i];
        } else if (arg == "--stop-words" && i + 1 < argc) {
            stop_words_file = argv[++i];
        } else if (arg == "--index-dir" && i + 1 < argc) {
            index_dir = argv[++i];
        }
    }

    std::cout << "=== Web Search Engine Indexer ===" << std::endl;
    std::cout << "Input directory: " << input_dir << std::endl;
    std::cout << "Stop words file: " << stop_words_file << std::endl;
    std::cout << "Index output:    " << index_dir << std::endl;
    std::cout << std::endl;

    auto bundle = BuildIndex(input_dir, stop_words_file);

    // Save index to disk
    std::cout << std::endl;
    std::cout << "Saving index to " << index_dir << "..." << std::endl;
    auto save_start = std::chrono::high_resolution_clock::now();

    if (IndexSerializer::Save(index_dir, bundle->index, bundle->doc_store)) {
        auto save_end = std::chrono::high_resolution_clock::now();
        auto save_ms = std::chrono::duration_cast<std::chrono::milliseconds>(save_end - save_start);

        // Calculate file sizes
        auto index_size = fs::file_size(fs::path(index_dir) / "index.json");
        auto docs_size = fs::file_size(fs::path(index_dir) / "documents.json");

        std::cout << "Index saved successfully!" << std::endl;
        std::cout << "  index.json:     " << (index_size / 1024) << " KB" << std::endl;
        std::cout << "  documents.json: " << (docs_size / 1024) << " KB" << std::endl;
        std::cout << "  Save time:      " << save_ms.count() << " ms" << std::endl;
    } else {
        std::cerr << "Error: Failed to save index." << std::endl;
        return 1;
    }

    // Sample lookups
    std::cout << std::endl;
    std::cout << "=== Sample Index Lookups ===" << std::endl;
    std::vector<std::string> sample_terms = {"search", "engine", "google", "web", "algorithm"};
    for (const auto& term : sample_terms) {
        const auto& postings = bundle->index.GetPostings(term);
        if (!postings.empty()) {
            std::cout << "  '" << term << "' -> found in " << postings.size() << " doc(s):";
            int shown = 0;
            for (const auto& p : postings) {
                if (shown >= 5) { std::cout << " ..."; break; }
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

// ========================
// SEARCH Command (now loads from disk)
// ========================
int RunSearch(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: search requires a query string." << std::endl;
        PrintUsage();
        return 1;
    }

    std::string raw_query;
    std::string index_dir = "./data/index/";
    std::string stop_words_file = "./data/stop_words.txt";
    size_t top_k = 10;

    std::vector<std::string> query_parts;
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--index-dir" && i + 1 < argc) {
            index_dir = argv[++i];
        } else if (arg == "--stop-words" && i + 1 < argc) {
            stop_words_file = argv[++i];
        } else if (arg == "--top-k" && i + 1 < argc) {
            top_k = std::stoul(argv[++i]);
        } else {
            query_parts.push_back(arg);
        }
    }

    for (size_t i = 0; i < query_parts.size(); i++) {
        if (i > 0) raw_query += " ";
        raw_query += query_parts[i];
    }

    if (raw_query.empty()) {
        std::cerr << "Error: empty query." << std::endl;
        return 1;
    }

    std::cout << "=== Web Search Engine ===" << std::endl;
    std::cout << std::endl;

    // Load index from disk (fast!) instead of rebuilding
    Tokenizer tokenizer;
    StopWordFilter stop_filter(stop_words_file);
    DocumentStore doc_store;
    InvertedIndex index;

    if (IndexSerializer::IndexExists(index_dir)) {
        std::cout << "Loading index from " << index_dir << "..." << std::endl;
        auto load_start = std::chrono::high_resolution_clock::now();

        bool ok = IndexSerializer::LoadIndex(index_dir, index) &&
                  IndexSerializer::LoadDocumentStore(index_dir, doc_store);

        auto load_end = std::chrono::high_resolution_clock::now();
        auto load_ms = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);

        if (ok) {
            std::cout << "Index loaded: " << doc_store.TotalDocuments()
                      << " documents, " << index.TermCount()
                      << " terms (" << load_ms.count() << " ms)" << std::endl;
        } else {
            std::cerr << "Error: Failed to load index. Run 'index' command first." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: No saved index found at " << index_dir << std::endl;
        std::cerr << "Run 'search_engine index' first to build and save the index." << std::endl;
        return 1;
    }

    std::cout << std::endl;

    // Process query and search
    QueryProcessor processor(index, doc_store, tokenizer, stop_filter);

    auto query_terms = processor.ProcessQuery(raw_query);
    std::cout << "Query: \"" << raw_query << "\"" << std::endl;
    std::cout << "Terms: [";
    for (size_t i = 0; i < query_terms.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << query_terms[i];
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto results = processor.Search(raw_query, top_k);
    auto end = std::chrono::high_resolution_clock::now();
    auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    if (results.empty()) {
        std::cout << "No results found." << std::endl;
    } else {
        std::cout << "Found " << results.size() << " result(s) in "
                  << search_time.count() << " \xC2\xB5s:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;

        for (size_t i = 0; i < results.size(); i++) {
            const auto& r = results[i];
            std::cout << std::endl;
            std::cout << "  " << (i + 1) << ". " << r.title << std::endl;
            std::cout << "     URL:   " << r.url << std::endl;
            std::cout << "     Score: " << std::fixed << std::setprecision(4) << r.score << std::endl;
            std::cout << "     " << r.snippet << std::endl;
        }
    }
    std::cout << std::endl;

    return 0;
}

// ========================
// SERVER Command
// ========================
int RunServer(int argc, char* argv[]) {
    int port = 8080;
    std::string frontend_dir = "./frontend";
    std::string index_dir = "./data/index/";
    std::string stop_words_file = "./data/stop_words.txt";

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--frontend-dir" && i + 1 < argc) {
            frontend_dir = argv[++i];
        } else if (arg == "--index-dir" && i + 1 < argc) {
            index_dir = argv[++i];
        } else if (arg == "--stop-words" && i + 1 < argc) {
            stop_words_file = argv[++i];
        }
    }

    if (!IndexSerializer::IndexExists(index_dir)) {
        std::cerr << "Error: No saved index found at " << index_dir << std::endl;
        std::cerr << "Run 'search_engine index' first." << std::endl;
        return 1;
    }

    std::mutex engine_mutex;
    std::shared_ptr<IndexBundle> current_bundle;
    std::shared_ptr<QueryProcessor> current_processor;

    auto reload_index = [&]() {
        auto bundle = std::make_shared<IndexBundle>(stop_words_file);
        if (!IndexSerializer::LoadIndex(index_dir, bundle->index) ||
            !IndexSerializer::LoadDocumentStore(index_dir, bundle->doc_store)) {
            std::cerr << "Error: Failed to load index." << std::endl;
            return false;
        }
        
        auto processor = std::make_shared<QueryProcessor>(
            bundle->index, bundle->doc_store, bundle->tokenizer, bundle->stop_filter);

        std::lock_guard<std::mutex> lock(engine_mutex);
        current_bundle = bundle;
        current_processor = processor;
        return true;
    };

    std::cout << "Loading index from " << index_dir << "..." << std::endl;
    if (!reload_index()) {
        return 1;
    }
    std::cout << "Index loaded successfully." << std::endl;

    auto search_fn = [&](const std::string& query_str, size_t top_k) -> json {
        std::shared_ptr<QueryProcessor> processor;
        {
            std::lock_guard<std::mutex> lock(engine_mutex);
            processor = current_processor;
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto results = processor->Search(query_str, top_k);
        auto end = std::chrono::high_resolution_clock::now();
        auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        size_t total_terms = 0;
        try {
            total_terms = processor->ProcessQuery(query_str).size();
        } catch(...) {}

        json response_json;
        response_json["time_us"] = time_us;
        response_json["total_terms"] = total_terms;
        
        json results_array = json::array();
        for (const auto& r : results) {
            json item = {
                {"url", r.url},
                {"title", r.title},
                {"snippet", r.snippet},
                {"score", r.score}
            };
            results_array.push_back(item);
        }
        
        response_json["results"] = results_array;
        return response_json;
    };

    // Globals for progress tracking
    static std::atomic<bool> g_is_crawling{false};
    static std::atomic<int> g_crawl_phase{0}; // 0=idle, 1=crawling, 2=indexing, 3=saving
    static std::atomic<int> g_crawled_pages{0};
    static std::atomic<int> g_indexed_pages{0};

    auto crawl_fn = [&](const std::string& url, int max_pages) -> bool {
        std::cout << "\n[API] Starting background crawl for: " << url << " (max " << max_pages << ")" << std::endl;
        
        CrawlConfig config;
        config.seed_urls.push_back(url);
        config.max_pages = max_pages;
        config.num_threads = 4;
        config.output_dir = "./data/crawled_pages/";
        
        try {
            g_is_crawling = true;
            g_crawl_phase = 1;
            g_crawled_pages = 0;
            g_indexed_pages = 0;

            // Simple thread to poll crawler progress by counting files
            std::atomic<bool> stop_poll{false};
            std::thread poller([&]() {
                int initial_count = 0;
                try {
                    for (const auto& entry : fs::directory_iterator(config.output_dir)) {
                        if (entry.path().extension() == ".json") initial_count++;
                    }
                } catch(...) {}

                while (!stop_poll) {
                    int count = 0;
                    try {
                        for (const auto& entry : fs::directory_iterator(config.output_dir)) {
                            if (entry.path().extension() == ".json") count++;
                        }
                    } catch(...) {}
                    g_crawled_pages = count > initial_count ? count - initial_count : 0;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            });

            Crawler crawler(config);
            crawler.Start(); // Blocking crawl
            
            stop_poll = true;
            if (poller.joinable()) poller.join();

            std::cout << "[API] Crawl complete. Rebuilding index..." << std::endl;
            g_crawl_phase = 2;
            auto new_bundle = BuildIndex(config.output_dir, stop_words_file, false);
            // NOTE: BuildIndex itself doesn't easily expose progress without changes.
            // But we know it finished when it moves to phase 3.
            
            std::cout << "[API] Saving new index to disk..." << std::endl;
            g_crawl_phase = 3;
            if (!IndexSerializer::Save(index_dir, new_bundle->index, new_bundle->doc_store)) {
                std::cerr << "[API] Error saving new index" << std::endl;
                g_is_crawling = false;
                g_crawl_phase = 0;
                return false;
            }
            
            auto new_processor = std::make_shared<QueryProcessor>(
                new_bundle->index, new_bundle->doc_store, new_bundle->tokenizer, new_bundle->stop_filter);

            {
                std::lock_guard<std::mutex> lock(engine_mutex);
                current_bundle = std::move(new_bundle);
                current_processor = new_processor;
            }
            
            std::cout << "[API] Hot-swapped new index successfully!" << std::endl;
            g_is_crawling = false;
            g_crawl_phase = 0;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[API] Crawl error: " << e.what() << std::endl;
            g_is_crawling = false;
            g_crawl_phase = 0;
            return false;
        }
    };

    auto stats_fn = [&]() -> json {
        std::shared_ptr<IndexBundle> bundle;
        {
            std::lock_guard<std::mutex> lock(engine_mutex);
            bundle = current_bundle;
        }

        size_t total_docs = bundle->doc_store.TotalDocuments();
        size_t total_chars = 0;
        const auto& docs = bundle->doc_store.GetAllDocuments();
        for (const auto& doc : docs) {
            total_chars += doc.body.length();
        }

        json res;
        res["total_pages"] = total_docs;
        res["total_chars"] = total_chars;
        res["is_crawling"] = (bool)g_is_crawling;
        res["crawl_phase"] = (int)g_crawl_phase;
        res["crawled_pages"] = (int)g_crawled_pages;
        res["index_ready"] = true;
        return res;
    };

    auto clear_fn = [&]() -> bool {
        std::cout << "\n[API] Clearing database..." << std::endl;
        
        {
            std::lock_guard<std::mutex> lock(engine_mutex);
            // Create empty bundle
            auto empty_bundle = std::make_shared<IndexBundle>(stop_words_file);
            empty_bundle->tokenizer = current_bundle->tokenizer;
            empty_bundle->stop_filter = current_bundle->stop_filter;
            
            auto new_processor = std::make_shared<QueryProcessor>(
                empty_bundle->index, empty_bundle->doc_store, empty_bundle->tokenizer, empty_bundle->stop_filter);

            current_bundle = empty_bundle;
            current_processor = new_processor;
        }

        try {
            if (fs::exists("./data/crawled_pages/")) {
                for (const auto& entry : fs::directory_iterator("./data/crawled_pages/")) {
                    fs::remove(entry.path());
                }
            }
            if (fs::exists("./data/index.bin")) fs::remove("./data/index.bin");
            if (fs::exists("./data/doc_store.bin")) fs::remove("./data/doc_store.bin");
        } catch (const std::exception& e) {
            std::cerr << "Error removing files: " << e.what() << std::endl;
            return false;
        }

        return true;
    };

    WebServer server(search_fn, crawl_fn, stats_fn, clear_fn, frontend_dir);

    server.Start(port);
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
    } else if (command == "search") {
        return RunSearch(argc, argv);
    } else if (command == "server") {
        return RunServer(argc, argv);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }
}
