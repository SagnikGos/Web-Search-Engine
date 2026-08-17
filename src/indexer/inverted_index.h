#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <string>
#include <vector>
#include <unordered_map>

// A posting entry: records that a specific term appeared in a specific document
struct Posting {
    int doc_id;
    int term_frequency;  // How many times this term appears in this document
};

// InvertedIndex: the core data structure of the search engine
// Maps each unique term to a list of documents (postings) that contain it
// Provides O(1) average-case term lookup via hash map
class InvertedIndex {
public:
    // Add a document's tokens to the index
    // Counts term frequencies and creates posting entries
    void AddDocument(int doc_id, const std::vector<std::string>& tokens);

    // Look up a term — returns its posting list (O(1) average)
    // Returns empty vector reference if term not found
    const std::vector<Posting>& GetPostings(const std::string& term) const;

    // Check if a term exists in the index
    bool Contains(const std::string& term) const;

    // Number of distinct terms in the index
    size_t TermCount() const;

    // Number of documents that have been indexed
    size_t DocumentCount() const;

    // Get the entire index (for serialization)
    const std::unordered_map<std::string, std::vector<Posting>>& GetIndex() const;

private:
    // The inverted index: term -> list of (doc_id, term_frequency) pairs
    std::unordered_map<std::string, std::vector<Posting>> index_;

    // Track number of unique documents indexed
    size_t doc_count_ = 0;

    // Empty postings list returned for terms not in the index
    static const std::vector<Posting> kEmptyPostings;
};

#endif // INVERTED_INDEX_H
