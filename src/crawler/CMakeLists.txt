add_library(crawler_lib STATIC
    thread_pool.cpp
    url_queue.cpp
    http_fetcher.cpp
    html_parser.cpp
    crawler.cpp
)

target_include_directories(crawler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(crawler_lib PUBLIC cpr::cpr gumbo nlohmann_json::nlohmann_json)

if(WIN32)
    target_link_libraries(crawler_lib PUBLIC ws2_32)
endif()
