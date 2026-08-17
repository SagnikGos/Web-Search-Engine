#include "tokenizer.h"
#include <algorithm>
#include <cctype>

std::vector<std::string> Tokenizer::Tokenize(const std::string& text) const {
    std::string lower_text = ToLowerCase(text);
    std::vector<std::string> tokens;
    std::string current_token;

    for (char c : lower_text) {
        if (IsAlphanumeric(c)) {
            current_token += c;
        } else {
            if (current_token.length() >= kMinTokenLength) {
                tokens.push_back(current_token);
            }
            current_token.clear();
        }
    }
    
    // Handle the last token after the loop ends
    if (current_token.length() >= kMinTokenLength) {
        tokens.push_back(current_token);
    }

    return tokens;
}

std::string Tokenizer::ToLowerCase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

bool Tokenizer::IsAlphanumeric(char c) const {
    return std::isalnum(static_cast<unsigned char>(c));
}
