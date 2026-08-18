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
    std::cout << "Examples:" << std::endl;
    std::cout << "  search_engine crawl https://en.wikipedia.org/wiki/C++ --max-pages 100" << std::endl;
    std::cout << "  search_engine index" << std::endl;
    std::cout << "  search_engine search \"web search algorithm\"" << std::endl;
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
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }
}
