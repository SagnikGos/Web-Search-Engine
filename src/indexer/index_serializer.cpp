#include "index_serializer.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

const std::string IndexSerializer::kIndexFilename = "index.json";
const std::string IndexSerializer::kDocumentsFilename = "documents.json";

bool IndexSerializer::Save(const std::string& output_dir,
                           const InvertedIndex& index,
                           const DocumentStore& doc_store) {
    try {
        fs::create_directories(output_dir);

        // Save inverted index
        {
            json j;
            j["doc_count"] = index.DocumentCount();
            j["term_count"] = index.TermCount();

            json terms_obj = json::object();
            for (const auto& [term, postings] : index.GetIndex()) {
                json postings_arr = json::array();
                for (const auto& p : postings) {
                    postings_arr.push_back({
                        {"d", p.doc_id},
                        {"f", p.term_frequency}
                    });
                }
                terms_obj[term] = postings_arr;
            }
            j["terms"] = terms_obj;

            std::ofstream file(fs::path(output_dir) / kIndexFilename);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open " << kIndexFilename << " for writing.\n";
                return false;
            }
            file << j.dump();
            file.close();
        }

        // Save document store
        {
            json j;
            json docs_arr = json::array();
            for (const auto& doc : doc_store.GetAllDocuments()) {
                docs_arr.push_back({
                    {"id", doc.doc_id},
                    {"url", doc.url},
                    {"title", doc.title},
                    {"body", doc.body},
                    {"total_terms", doc.total_terms}
                });
            }
            j["documents"] = docs_arr;
            j["total"] = doc_store.TotalDocuments();

            std::ofstream file(fs::path(output_dir) / kDocumentsFilename);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open " << kDocumentsFilename << " for writing.\n";
                return false;
            }
            file << j.dump();
            file.close();
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving index: " << e.what() << "\n";
        return false;
    }
}

bool IndexSerializer::LoadIndex(const std::string& input_dir,
                                InvertedIndex& index) {
    try {
        std::ifstream file(fs::path(input_dir) / kIndexFilename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << kIndexFilename << "\n";
            return false;
        }

        json j = json::parse(file);
        file.close();

        const auto& terms = j["terms"];
        
        // REVISED APPROACH: Reconstruct document-by-document
        // Group postings by doc_id across all terms
        std::unordered_map<int, std::vector<std::string>> doc_tokens;
        for (auto it = terms.begin(); it != terms.end(); ++it) {
            const std::string& term = it.key();
            for (const auto& posting : it.value()) {
                int doc_id = posting["d"].get<int>();
                int freq = posting["f"].get<int>();
                for (int i = 0; i < freq; i++) {
                    doc_tokens[doc_id].push_back(term);
                }
            }
        }

        // Sort by doc_id and add documents in order
        std::vector<int> doc_ids;
        for (const auto& [id, _] : doc_tokens) {
            doc_ids.push_back(id);
        }
        std::sort(doc_ids.begin(), doc_ids.end());

        for (int id : doc_ids) {
            index.AddDocument(id, doc_tokens[id]);
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading index: " << e.what() << "\n";
        return false;
    }
}

bool IndexSerializer::LoadDocumentStore(const std::string& input_dir,
                                        DocumentStore& doc_store) {
    try {
        std::ifstream file(fs::path(input_dir) / kDocumentsFilename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << kDocumentsFilename << "\n";
            return false;
        }

        json j = json::parse(file);
        file.close();

        // We need to add documents in order. DocumentStore::AddDocument
        // generates a snippet from body_text, but we already have snippets.
        // We need a way to load pre-computed data.
        // Since DocumentStore only has AddDocument which regenerates snippets,
        // we need to add a LoadDocument method.
        // For now, we use AddDocument with snippet as body (since snippet IS
        // the truncated body, it will just return as-is if <= 200 chars).
        for (const auto& doc : j["documents"]) {
            doc_store.AddDocument(
                doc["url"].get<std::string>(),
                doc["title"].get<std::string>(),
                doc.value("body", doc.value("snippet", "")),
                doc["total_terms"].get<int>()
            );
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading document store: " << e.what() << "\n";
        return false;
    }
}

bool IndexSerializer::IndexExists(const std::string& dir) {
    return fs::exists(fs::path(dir) / kIndexFilename) &&
           fs::exists(fs::path(dir) / kDocumentsFilename);
}
