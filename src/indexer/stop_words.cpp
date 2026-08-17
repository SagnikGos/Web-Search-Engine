#include "stop_words.h"
#include <fstream>
#include <iostream>
#include <algorithm>

StopWordFilter::StopWordFilter() {
    LoadDefaults();
}

StopWordFilter::StopWordFilter(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open stop words file: " << filepath << ". Falling back to default stop words.\n";
        LoadDefaults();
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace from line
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (!line.empty()) {
            stop_words_.insert(line);
        }
    }
}

bool StopWordFilter::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> StopWordFilter::Filter(const std::vector<std::string>& tokens) const {
    std::vector<std::string> filtered;
    filtered.reserve(tokens.size()); // Pre-allocate to avoid reallocations
    for (const auto& token : tokens) {
        if (!IsStopWord(token)) {
            filtered.push_back(token);
        }
    }
    return filtered;
}

size_t StopWordFilter::Size() const {
    return stop_words_.size();
}

void StopWordFilter::LoadDefaults() {
    std::vector<std::string> defaults = {
        "a", "an", "and", "are", "as", "at", "be", "been", "but", "by", 
        "can", "do", "does", "for", "from", "had", "has", "have", "he", 
        "her", "his", "how", "i", "if", "in", "into", "is", "it", "its", 
        "just", "me", "my", "no", "nor", "not", "of", "on", "or", "our", 
        "she", "so", "some", "than", "that", "the", "their", "them", 
        "then", "there", "these", "they", "this", "to", "too", "us", 
        "very", "was", "we", "were", "what", "when", "which", "while", 
        "who", "will", "with", "would", "you", "your"
    };
    for (const auto& word : defaults) {
        stop_words_.insert(word);
    }
}
