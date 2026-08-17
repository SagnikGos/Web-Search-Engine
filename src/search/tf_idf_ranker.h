#ifndef TF_IDF_RANKER_H
#define TF_IDF_RANKER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "indexer/inverted_index.h"
#include "indexer/document_store.h"

// A single search result with relevance score
struct SearchResult {
    int doc_id;
    double score;          // TF-IDF relevance score
    std::string url;
    std::string title;
    std::string snippet;
};

// TfIdfRanker: computes TF-IDF scores and ranks documents
// TF  = term_frequency / total_terms_in_document (normalized)
// IDF = log(total_documents / documents_containing_term)
// Score = sum of TF * IDF for each query term
class TfIdfRanker {
public:
    TfIdfRanker(const InvertedIndex& index, const DocumentStore& doc_store);

    // Rank documents for a multi-term query
    // Returns results sorted by descending TF-IDF score
    std::vector<SearchResult> Rank(const std::vector<std::string>& query_terms,
                                   size_t top_k = 10) const;

private:
    // Compute IDF for a single term: log(N / df)
    double ComputeIDF(const std::string& term) const;

    // Compute TF for a term in a document: tf / total_terms
    double ComputeTF(int term_frequency, int total_terms) const;

    const InvertedIndex& index_;
    const DocumentStore& doc_store_;
};

#endif // TF_IDF_RANKER_H
