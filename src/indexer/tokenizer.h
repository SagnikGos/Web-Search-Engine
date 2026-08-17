#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

// Tokenizer: breaks raw text into clean, lowercase tokens
// Removes punctuation, splits on whitespace, filters short tokens
class Tokenizer {
public:
    // Tokenize raw text into a vector of clean lowercase words
    std::vector<std::string> Tokenize(const std::string& text) const;

private:
    // Convert a string to lowercase
    std::string ToLowerCase(const std::string& str) const;

    // Check if character is alphanumeric
    bool IsAlphanumeric(char c) const;

    // Minimum token length to keep (default: 2)
    static constexpr int kMinTokenLength = 2;
};

#endif // TOKENIZER_H
