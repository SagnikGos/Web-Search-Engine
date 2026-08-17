#ifndef STOP_WORDS_H
#define STOP_WORDS_H

#include <string>
#include <vector>
#include <unordered_set>

// StopWordFilter: filters out common English words that don't
// contribute to meaningful search (e.g., "the", "is", "and")
class StopWordFilter {
public:
    // Default constructor loads a built-in set of ~175 common stop words
    StopWordFilter();

    // Load stop words from a file (one word per line)
    explicit StopWordFilter(const std::string& filepath);

    // Check if a word is a stop word (O(1) average via hash set)
    bool IsStopWord(const std::string& word) const;

    // Filter a vector of tokens, removing all stop words
    std::vector<std::string> Filter(const std::vector<std::string>& tokens) const;

    // Number of stop words loaded
    size_t Size() const;

private:
    std::unordered_set<std::string> stop_words_;

    // Built-in default stop words
    void LoadDefaults();
};

#endif // STOP_WORDS_H
