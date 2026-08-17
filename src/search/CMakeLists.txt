add_library(search_lib STATIC
    tf_idf_ranker.cpp
    query_processor.cpp
)

target_include_directories(search_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(search_lib PUBLIC indexer_lib)
