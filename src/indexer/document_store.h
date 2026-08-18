#ifndef DOCUMENT_STORE_H
#define DOCUMENT_STORE_H

#include <string>
#include <vector>
#include <unordered_map>

// Metadata for a single indexed document
struct DocumentInfo {
    int doc_id = -1;
    std::string url;
    std::string title;
    std::string body;        // Full body text for dynamic snippets
    int total_terms = 0;     // Total number of terms after tokenization (for TF calculation)
};

// DocumentStore: manages the mapping between document IDs and their metadata
// Provides O(1) lookup by doc_id
class DocumentStore {
public:
    // Add a document and return its assigned doc_id
    int AddDocument(const std::string& url, const std::string& title,
                    const std::string& body_text, int total_terms);

    // Retrieve document info by ID. Throws std::out_of_range if not found.
    const DocumentInfo& GetDocument(int doc_id) const;

    // Check if a document ID exists
    bool HasDocument(int doc_id) const;

    // Total number of documents in the store
    size_t TotalDocuments() const;

    // Get all documents (for iteration/serialization)
    const std::vector<DocumentInfo>& GetAllDocuments() const;

private:
    std::vector<DocumentInfo> documents_;
    int next_id_ = 0;
};

#endif // DOCUMENT_STORE_H
