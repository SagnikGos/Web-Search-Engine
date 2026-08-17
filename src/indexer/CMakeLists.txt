add_library(indexer_lib STATIC
    tokenizer.cpp
    stop_words.cpp
    document_store.cpp
    inverted_index.cpp
)

target_include_directories(indexer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(indexer_lib PUBLIC nlohmann_json::nlohmann_json)
