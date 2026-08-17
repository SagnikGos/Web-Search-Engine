#include "document_store.h"
#include <stdexcept>

std::string DocumentStore::GenerateSnippet(const std::string& body_text, size_t max_length) const {
    if (body_text.size() <= max_length) {
        return body_text;
    }
    
    // Find the last space within the first max_length chars
    size_t last_space = body_text.rfind(' ', max_length);
    if (last_space != std::string::npos) {
        return body_text.substr(0, last_space) + "...";
    }
    
    // If no space found, just truncate at max_length and append "..."
    return body_text.substr(0, max_length) + "...";
}

int DocumentStore::AddDocument(const std::string& url, const std::string& title,
                               const std::string& body_text, int total_terms) {
    int doc_id = next_id_++;
    DocumentInfo info;
    info.doc_id = doc_id;
    info.url = url;
    info.title = title;
    info.snippet = GenerateSnippet(body_text);
    info.total_terms = total_terms;
    
    documents_.push_back(info);
    return doc_id;
}

const DocumentInfo& DocumentStore::GetDocument(int doc_id) const {
    if (doc_id < 0 || doc_id >= static_cast<int>(documents_.size())) {
        throw std::out_of_range("Invalid document ID");
    }
    return documents_[doc_id];
}

bool DocumentStore::HasDocument(int doc_id) const {
    return doc_id >= 0 && doc_id < static_cast<int>(documents_.size());
}

size_t DocumentStore::TotalDocuments() const {
    return documents_.size();
}

const std::vector<DocumentInfo>& DocumentStore::GetAllDocuments() const {
    return documents_;
}
