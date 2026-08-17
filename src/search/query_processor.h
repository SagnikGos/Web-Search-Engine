#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include <string>
#include <vector>
#include "indexer/tokenizer.h"
#include "indexer/stop_words.h"
#include "indexer/inverted_index.h"
#include "indexer/document_store.h"
#include "search/tf_idf_ranker.h"

// QueryProcessor: the full search pipeline
// Takes a raw user query string and returns ranked results
// Pipeline: raw query -> tokenize -> remove stop words -> rank via TF-IDF
class QueryProcessor {
public:
    QueryProcessor(const InvertedIndex& index, const DocumentStore& doc_store,
                   const Tokenizer& tokenizer, const StopWordFilter& stop_filter);

    // Process a raw query string and return ranked results
    std::vector<SearchResult> Search(const std::string& raw_query,
                                     size_t top_k = 10) const;

    // Get the processed query terms (for debugging/display)
    std::vector<std::string> ProcessQuery(const std::string& raw_query) const;

private:
    const InvertedIndex& index_;
    const DocumentStore& doc_store_;
    const Tokenizer& tokenizer_;
    const StopWordFilter& stop_filter_;
    TfIdfRanker ranker_;
};

#endif // QUERY_PROCESSOR_H
