#include "document_store.h"
#include <stdexcept>


int DocumentStore::AddDocument(const std::string& url, const std::string& title,
                               const std::string& body_text, int total_terms) {
    int doc_id = next_id_++;
    DocumentInfo info;
    info.doc_id = doc_id;
    info.url = url;
    info.title = title;
    info.body = body_text;
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
