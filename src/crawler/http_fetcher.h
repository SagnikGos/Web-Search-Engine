#ifndef HTTP_FETCHER_H
#define HTTP_FETCHER_H

#include <string>

struct FetchResult {
    int status_code = 0;
    std::string body;
    std::string final_url;      // After redirects
    std::string content_type;
    bool success = false;
    std::string error_message;
};

class HttpFetcher {
public:
    HttpFetcher();
    FetchResult Fetch(const std::string& url);

private:
    static constexpr int kTimeoutSeconds = 10;
    static constexpr int kMaxRedirects = 5;
    static const std::string kUserAgent;
};

#endif
