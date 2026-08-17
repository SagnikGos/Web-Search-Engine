#include "search/tf_idf_ranker.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>

TfIdfRanker::TfIdfRanker(const InvertedIndex& index, const DocumentStore& doc_store)
    : index_(index), doc_store_(doc_store) {}

double TfIdfRanker::ComputeIDF(const std::string& term) const {
    size_t N = doc_store_.TotalDocuments();
    auto postings = index_.GetPostings(term);
    size_t df = postings.size();
    if (df == 0) {
        return 0.0;
    }
    return std::log(static_cast<double>(N) / static_cast<double>(df));
}

double TfIdfRanker::ComputeTF(int term_frequency, int total_terms) const {
    if (total_terms == 0) {
        return 0.0;
    }
    return static_cast<double>(term_frequency) / static_cast<double>(total_terms);
}

std::vector<SearchResult> TfIdfRanker::Rank(const std::vector<std::string>& query_terms, size_t top_k) const {
    std::unordered_map<int, double> doc_scores;

    for (const auto& query_term : query_terms) {
        double idf = ComputeIDF(query_term);
        if (idf == 0.0) {
            continue;
        }

        auto postings = index_.GetPostings(query_term);
        for (const auto& posting : postings) {
            auto doc_info = doc_store_.GetDocument(posting.doc_id);
            double tf = ComputeTF(posting.term_frequency, doc_info.total_terms);
            doc_scores[posting.doc_id] += tf * idf;
        }
    }

    std::vector<SearchResult> results;
    results.reserve(doc_scores.size());
    for (const auto& [doc_id, score] : doc_scores) {
        auto doc_info = doc_store_.GetDocument(doc_id);
        results.push_back({doc_id, score, doc_info.url, doc_info.title, doc_info.snippet});
    }

    std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.score > b.score;
    });

    if (results.size() > top_k) {
        results.resize(top_k);
    }

    return results;
}
