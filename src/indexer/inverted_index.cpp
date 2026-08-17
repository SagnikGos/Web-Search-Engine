#include "inverted_index.h"
#include <unordered_map>

const std::vector<Posting> InvertedIndex::kEmptyPostings = {};

void InvertedIndex::AddDocument(int doc_id, const std::vector<std::string>& tokens) {
    std::unordered_map<std::string, int> term_counts;
    for (const auto& token : tokens) {
        term_counts[token]++;
    }
    
    for (const auto& [term, count] : term_counts) {
        index_[term].push_back(Posting{doc_id, count});
    }
    
    doc_count_++;
}

const std::vector<Posting>& InvertedIndex::GetPostings(const std::string& term) const {
    const auto it = index_.find(term);
    if (it != index_.end()) {
        return it->second;
    }
    return kEmptyPostings;
}

bool InvertedIndex::Contains(const std::string& term) const {
    return index_.count(term) > 0;
}

size_t InvertedIndex::TermCount() const {
    return index_.size();
}

size_t InvertedIndex::DocumentCount() const {
    return doc_count_;
}

const std::unordered_map<std::string, std::vector<Posting>>& InvertedIndex::GetIndex() const {
    return index_;
}
