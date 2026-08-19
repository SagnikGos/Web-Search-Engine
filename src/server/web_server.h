#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using SearchFunc = std::function<json(const std::string&, size_t)>;
using CrawlFunc = std::function<bool(const std::string&, int)>;
using StatsFunc = std::function<json()>;

using ClearFunc = std::function<bool()>;

// Forward declare to avoid including httplib.h in header
namespace httplib { class Server; }

class WebServer {
public:
    WebServer(SearchFunc search_fn, CrawlFunc crawl_fn, StatsFunc stats_fn, ClearFunc clear_fn, const std::string& frontend_dir);
    ~WebServer();

    // Start the server (blocking)
    void Start(int port = 8080);
    
    // Stop the server
    void Stop();

private:
    void SetupRoutes();

    SearchFunc search_fn_;
    CrawlFunc crawl_fn_;
    StatsFunc stats_fn_;
    ClearFunc clear_fn_;
    std::string frontend_dir_;
    std::unique_ptr<httplib::Server> svr_;
};

#endif // WEB_SERVER_H
