#include "html_parser.h"
#include "gumbo.h"
#include <iostream>

ParsedPage HtmlParser::Parse(const std::string& html, const std::string& base_url) {
    ParsedPage page;
    if (html.empty()) return page;

    GumboOutput* output = gumbo_parse(html.c_str());
    if (!output) return page;

    page.title = ExtractTitle(output->root);
    ExtractText(output->root, page.body_text);
    ExtractLinks(output->root, base_url, page.links);

    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return page;
}

void HtmlParser::ExtractText(void* node_ptr, std::string& output) {
    GumboNode* node = static_cast<GumboNode*>(node_ptr);
    if (!node) return;

    if (node->type == GUMBO_NODE_TEXT) {
        output += std::string(node->v.text.text) + " ";
    } else if (node->type == GUMBO_NODE_ELEMENT) {
        GumboTag tag = node->v.element.tag;
        if (tag == GUMBO_TAG_SCRIPT || tag == GUMBO_TAG_STYLE || 
            tag == GUMBO_TAG_NOSCRIPT || tag == GUMBO_TAG_HEAD) {
            return;
        }

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            ExtractText(children->data[i], output);
        }
    }
}

void HtmlParser::ExtractLinks(void* node_ptr, const std::string& base_url, std::vector<std::string>& links) {
    GumboNode* node = static_cast<GumboNode*>(node_ptr);
    if (!node) return;

    if (node->type == GUMBO_NODE_ELEMENT) {
        if (node->v.element.tag == GUMBO_TAG_A) {
            GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
            if (href) {
                std::string relative = href->value;
                std::string absolute = ResolveUrl(base_url, relative);
                if (!absolute.empty()) {
                    std::string normalized = NormalizeUrl(absolute);
                    if (!normalized.empty()) {
                        links.push_back(normalized);
                    }
                }
            }
        }

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            ExtractLinks(children->data[i], base_url, links);
        }
    }
}

std::string HtmlParser::ExtractTitle(void* node_ptr) {
    GumboNode* node = static_cast<GumboNode*>(node_ptr);
    if (!node) return "";

    if (node->type == GUMBO_NODE_ELEMENT) {
        if (node->v.element.tag == GUMBO_TAG_TITLE) {
            GumboVector* children = &node->v.element.children;
            if (children->length > 0) {
                GumboNode* title_text = static_cast<GumboNode*>(children->data[0]);
                if (title_text->type == GUMBO_NODE_TEXT || title_text->type == GUMBO_NODE_WHITESPACE) {
                    return title_text->v.text.text;
                }
            }
            return "";
        }

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            std::string title = ExtractTitle(children->data[i]);
            if (!title.empty()) {
                return title;
            }
        }
    }
    return "";
}

std::string HtmlParser::ResolveUrl(const std::string& base, const std::string& relative) {
    if (relative.empty()) return "";
    if (relative.find("javascript:") == 0 || relative.find("mailto:") == 0 || relative.find("#") == 0) {
        return "";
    }
    if (relative.find("http://") == 0 || relative.find("https://") == 0) {
        return relative;
    }

    size_t scheme_end = base.find("://");
    if (scheme_end == std::string::npos) return ""; // Invalid base URL
    std::string scheme = base.substr(0, scheme_end + 3);
    
    if (relative.find("//") == 0) {
        return scheme.substr(0, scheme_end) + ":" + relative;
    }

    size_t host_end = base.find("/", scheme_end + 3);
    std::string host = (host_end == std::string::npos) ? base : base.substr(0, host_end);

    if (relative[0] == '/') {
        return host + relative;
    }

    std::string path_dir;
    size_t last_slash = base.find_last_of('/');
    if (last_slash != std::string::npos && last_slash >= scheme_end + 3) {
        path_dir = base.substr(0, last_slash + 1);
    } else {
        path_dir = host + "/";
    }

    return path_dir + relative;
}

std::string HtmlParser::NormalizeUrl(const std::string& url) {
    std::string normalized = url;
    
    // Strip fragment
    size_t hash_pos = normalized.find('#');
    if (hash_pos != std::string::npos) {
        normalized = normalized.substr(0, hash_pos);
    }

    // Strip trailing slash if present (except if it's just scheme://host/)
    if (!normalized.empty() && normalized.back() == '/') {
        size_t scheme_end = normalized.find("://");
        if (scheme_end != std::string::npos) {
            size_t first_slash_after_host = normalized.find("/", scheme_end + 3);
            if (first_slash_after_host != std::string::npos && first_slash_after_host != normalized.length() - 1) {
                normalized.pop_back();
            }
        }
    }

    return normalized;
}
