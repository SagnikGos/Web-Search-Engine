include(FetchContent)

# cpr - HTTP client
if(WIN32)
    set(CPR_FORCE_WINSSL_BACKEND ON CACHE BOOL "Force Windows SSL Backend" FORCE)
endif()
set(CPR_ENABLE_SSL ON CACHE BOOL "Enable SSL for cpr" FORCE)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.11.1
)
FetchContent_MakeAvailable(cpr)

# nlohmann_json - JSON library
set(JSON_BuildTests OFF CACHE BOOL "Disable tests for JSON library" FORCE)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# gumbo-parser - HTML parser
FetchContent_Declare(
    gumbo_parser
    GIT_REPOSITORY https://github.com/google/gumbo-parser.git
    GIT_TAG        v0.10.1
)

FetchContent_GetProperties(gumbo_parser)
if(NOT gumbo_parser_POPULATED)
    FetchContent_Populate(gumbo_parser)
    add_library(gumbo STATIC
        ${gumbo_parser_SOURCE_DIR}/src/attribute.c
        ${gumbo_parser_SOURCE_DIR}/src/char_ref.c
        ${gumbo_parser_SOURCE_DIR}/src/error.c
        ${gumbo_parser_SOURCE_DIR}/src/parser.c
        ${gumbo_parser_SOURCE_DIR}/src/string_buffer.c
        ${gumbo_parser_SOURCE_DIR}/src/string_piece.c
        ${gumbo_parser_SOURCE_DIR}/src/tag.c
        ${gumbo_parser_SOURCE_DIR}/src/tokenizer.c
        ${gumbo_parser_SOURCE_DIR}/src/utf8.c
        ${gumbo_parser_SOURCE_DIR}/src/util.c
        ${gumbo_parser_SOURCE_DIR}/src/vector.c
    )
    target_include_directories(gumbo PUBLIC ${gumbo_parser_SOURCE_DIR}/src)
endif()

# cpp-httplib - HTTP server
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.15.3
)
FetchContent_MakeAvailable(httplib)
