add_library(server_lib STATIC web_server.cpp)
target_include_directories(server_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(server_lib PUBLIC httplib::httplib search_lib)
