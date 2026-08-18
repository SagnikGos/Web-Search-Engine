#include "server/web_server.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>

using json = nlohmann::json;

WebServer::WebServer(SearchFunc search_fn, CrawlFunc crawl_fn, const std::string& frontend_dir)
    : search_fn_(std::move(search_fn)), crawl_fn_(std::move(crawl_fn)), frontend_dir_(frontend_dir), svr_(std::make_unique<httplib::Server>()) {
    SetupRoutes();
}

WebServer::~WebServer() {
    Stop();
}

void WebServer::SetupRoutes() {
    // API route for searching
    svr_->Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
        std::string query_str;
        if (req.has_param("q")) {
            query_str = req.get_param_value("q");
        }

        try {
            auto response_json = search_fn_(query_str, 20);

            res.set_content(response_json.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
        } catch (const std::exception& e) {
            std::cerr << "[WebServer] Exception in search_fn_: " << e.what() << std::endl;
            json error_json = {{"error", e.what()}};
            res.set_content(error_json.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
        } catch (...) {
            std::cerr << "[WebServer] Unknown exception in search_fn_" << std::endl;
            json error_json = {{"error", "Unknown exception"}};
            res.set_content(error_json.dump(), "application/json");
            res.set_header("Access-Control-Allow-Origin", "*");
            res.status = 500;
        }
    });

    // API route for crawling new sites
    svr_->Post("/api/crawl", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        
        try {
            json body = json::parse(req.body);
            std::string url = body.value("url", "");
            int max_pages = body.value("max_pages", 10);
            
            if (url.empty()) {
                res.status = 400;
                res.set_content(R"({"error": "url is required"})", "application/json");
                return;
            }
            
            bool success = crawl_fn_(url, max_pages);
            if (success) {
                res.set_content(R"({"success": true})", "application/json");
            } else {
                res.status = 500;
                res.set_content(R"({"error": "Crawler failed or indexing error"})", "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid JSON body"})", "application/json");
        }
    });

    // Handle OPTIONS requests for CORS
    svr_->Options("/api/crawl", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204;
    });

    // Serve static files
    if (!frontend_dir_.empty()) {
        svr_->set_mount_point("/", frontend_dir_);
    }
}

void WebServer::Start(int port) {
    std::cout << "Starting server on port " << port << "..." << std::endl;
    if (!svr_->listen("0.0.0.0", port)) {
        std::cerr << "Error: Could not start server on port " << port << std::endl;
    }
}

void WebServer::Stop() {
    if (svr_ && svr_->is_running()) {
        svr_->stop();
    }
}
