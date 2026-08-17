#include "search/query_processor.h"

QueryProcessor::QueryProcessor(const InvertedIndex& index, const DocumentStore& doc_store,
                               const Tokenizer& tokenizer, const StopWordFilter& stop_filter)
    : index_(index), doc_store_(doc_store), tokenizer_(tokenizer), stop_filter_(stop_filter), ranker_(index, doc_store) {}

std::vector<std::string> QueryProcessor::ProcessQuery(const std::string& raw_query) const {
    auto tokens = tokenizer_.Tokenize(raw_query);
    return stop_filter_.Filter(tokens);
}

std::vector<SearchResult> QueryProcessor::Search(const std::string& raw_query, size_t top_k) const {
    auto query_terms = ProcessQuery(raw_query);
    if (query_terms.empty()) {
        return {};
    }
    return ranker_.Rank(query_terms, top_k);
}
