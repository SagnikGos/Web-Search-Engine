#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <string>
#include <vector>

struct ParsedPage {
    std::string title;
    std::string body_text;
    std::vector<std::string> links;  // Absolute URLs
};

class HtmlParser {
public:
    ParsedPage Parse(const std::string& html, const std::string& base_url);

private:
    // Forward declare GumboNode to avoid exposing gumbo in header
    // Implementation uses gumbo internally
    void ExtractText(void* node, std::string& output);
    void ExtractLinks(void* node, const std::string& base_url, std::vector<std::string>& links);
    std::string ExtractTitle(void* node);
    std::string ResolveUrl(const std::string& base, const std::string& relative);
    std::string NormalizeUrl(const std::string& url);
};

#endif
