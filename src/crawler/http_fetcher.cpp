#include "http_fetcher.h"
#include <cpr/cpr.h>
#include <exception>

const std::string HttpFetcher::kUserAgent = "WebSearchEngine/1.0 (C++ Crawler)";

HttpFetcher::HttpFetcher() = default;

FetchResult HttpFetcher::Fetch(const std::string& url) {
    FetchResult result;
    result.final_url = url;

    try {
        cpr::Response r = cpr::Get(
            cpr::Url{url},
            cpr::Timeout{kTimeoutSeconds * 1000},
            cpr::Redirect(kMaxRedirects, true),
            cpr::UserAgent{kUserAgent}
        );

        if (r.error) {
            result.success = false;
            result.error_message = r.error.message;
            return result;
        }

        result.status_code = r.status_code;
        result.body = r.text;
        result.final_url = r.url.str();
        result.content_type = r.header["content-type"];

        if (result.status_code == 200 && result.content_type.find("text/html") != std::string::npos) {
            result.success = true;
        } else {
            result.success = false;
            if (result.status_code != 200) {
                result.error_message = "Status code: " + std::to_string(result.status_code);
            } else {
                result.error_message = "Content type is not text/html";
            }
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = std::string("Exception: ") + e.what();
    } catch (...) {
        result.success = false;
        result.error_message = "Unknown exception occurred.";
    }

    return result;
}
