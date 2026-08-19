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

// Helper function to generate a dynamic snippet with highlighting
std::string GenerateDynamicSnippet(const std::string& raw_body, const std::vector<std::string>& query_terms) {
    std::string body = raw_body;
    
    // Strip common Wikipedia boilerplate if present at the start
    std::string wiki_boilerplate = "Jump to content Main menu Main menu move to sidebar hide \n\t\tNavigation\n\t Main page Contents Current events Random article About Wikipedia Contact us \n\t\tContribute\n\t Help Learn to edit Community portal";
    if (body.find(wiki_boilerplate) == 0) {
        body = body.substr(wiki_boilerplate.size());
    }
    // Also strip another common variant without newlines
    std::string wiki_boilerplate2 = "Jump to content Main menu Main menu move to sidebar hide Navigation Main page Contents Current events Random article About Wikipedia Contact us Contribute Help Learn to edit Community portal";
    if (body.find(wiki_boilerplate2) == 0) {
        body = body.substr(wiki_boilerplate2.size());
    }
    
    // Clean up leading whitespace/punctuation
    size_t first_valid = 0;
    while (first_valid < body.size() && (std::isspace(body[first_valid]) || body[first_valid] == '.' || body[first_valid] == ',' || body[first_valid] == '|')) {
        first_valid++;
    }
    if (first_valid > 0) {
        body = body.substr(first_valid);
    }

    if (query_terms.empty() || body.empty()) {
        return body.substr(0, std::min(body.size(), (size_t)200)) + "...";
    }

    std::string lower_body = body;
    std::transform(lower_body.begin(), lower_body.end(), lower_body.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    size_t best_pos = std::string::npos;
    std::string matched_term = "";

    // Find the first occurrence of any query term
    for (const auto& term : query_terms) {
        if (term.empty()) continue;
        size_t pos = lower_body.find(term);
        if (pos != std::string::npos && (best_pos == std::string::npos || pos < best_pos)) {
            best_pos = pos;
            matched_term = term;
        }
    }

    if (best_pos == std::string::npos) {
        return body.substr(0, std::min(body.size(), (size_t)200)) + "...";
    }

    // Extract a window around the match
    size_t window_size = 80;
    size_t start = (best_pos > window_size) ? best_pos - window_size : 0;
    size_t length = matched_term.size() + window_size * 2;

    // Adjust start to a valid UTF-8 character boundary (not a continuation byte 10xxxxxx)
    while (start > 0 && start < body.size() && (body[start] & 0xC0) == 0x80) {
        start--;
    }

    // Adjust end to a valid UTF-8 character boundary
    size_t end = start + std::min(length, body.size() - start);
    while (end < body.size() && (body[end] & 0xC0) == 0x80) {
        end++;
    }
    length = end - start;

    std::string snippet = body.substr(start, length);
    
    std::string prefix = (start > 0) ? "..." : "";
    std::string suffix = (end < body.size()) ? "..." : "";

    // Highlight all terms in the snippet
    std::string lower_snippet = snippet;
    std::transform(lower_snippet.begin(), lower_snippet.end(), lower_snippet.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    std::string result = prefix;
    size_t i = 0;
    while (i < snippet.size()) {
        bool matched = false;
        for (const auto& term : query_terms) {
            if (term.empty()) continue;
            if (lower_snippet.compare(i, term.size(), term) == 0) {
                result += "<mark>" + snippet.substr(i, term.size()) + "</mark>";
                i += term.size();
                matched = true;
                break;
            }
        }
        if (!matched) {
            result += snippet[i];
            i++;
        }
    }
    result += suffix;

    return result;
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
        std::string snippet = GenerateDynamicSnippet(doc_info.body, query_terms);
        results.push_back({doc_id, score, doc_info.url, doc_info.title, snippet});
    }

    std::sort(results.begin(), results.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.score > b.score;
    });

    if (results.size() > top_k) {
        results.resize(top_k);
    }

    return results;
}
