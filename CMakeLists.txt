cmake_minimum_required(VERSION 3.20)
project(WebSearchEngine VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Windows-specific defines
if(WIN32)
    add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

# Fetch all dependencies
include(cmake/FetchDependencies.cmake)

# Add source modules
add_subdirectory(src/crawler)

# Main executable
add_executable(search_engine src/main.cpp)
target_link_libraries(search_engine PRIVATE crawler_lib)
