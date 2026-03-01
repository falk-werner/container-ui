cmake_minimum_required(VERSION 3.28)
project(container-ui)

option(WITHOUT_TEST "Disable unit test" OFF)

set(CMAKE_CXX_STANDARD 20)

find_program(XXD xxd REQUIRED)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MHD REQUIRED libmicrohttpd IMPORTED_TARGET)

add_custom_command(
    OUTPUT  index_html.c style_css.c script_main_js.c script_api_js.c script_login_js.c script_util.js
    COMMAND xxd -n index_html -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/index.html
              ${CMAKE_CURRENT_BINARY_DIR}/index_html.c
    COMMAND xxd -n style_css -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/style.css
              ${CMAKE_CURRENT_BINARY_DIR}/style_css.c
    COMMAND xxd -n script_main_js -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/script/main.js
              ${CMAKE_CURRENT_BINARY_DIR}/script_main_js.c
    COMMAND xxd -n script_api_js -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/script/api.js
              ${CMAKE_CURRENT_BINARY_DIR}/script_api_js.c
    COMMAND xxd -n script_login_js -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/script/login.js
              ${CMAKE_CURRENT_BINARY_DIR}/script_login_js.c
    COMMAND xxd -n script_util_js -i
              ${CMAKE_CURRENT_SOURCE_DIR}/www/script/util.js
              ${CMAKE_CURRENT_BINARY_DIR}/script_util_js.c
    DEPENDS www/index.html www/style.css www/script/main.js www/script/api.js www/script/login.js www/script/util.js
)

add_library(container-ui-static STATIC
    src/containerui/static_resource_handler.cpp
    src/containerui/passthrough_handler.cpp
    src/containerui/passthrough_with_param_handler.cpp
    src/containerui/request.cpp
    src/containerui/webserver.cpp
    src/containerui/webserver_context.cpp
    src/containerui/util/fetch.cpp
    src/containerui/util/url_util.cpp
    src/containerui/util/post_data.cpp
    src/containerui/util/timestamp.cpp
    src/containerui/util/b64.cpp
    src/containerui/util/token.cpp
    src/containerui/util/sha256.cpp
    src/containerui/auth/authorize_handler.cpp
    src/containerui/auth/token_handler.cpp
    src/containerui/auth/authenticator.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/index_html.c
    ${CMAKE_CURRENT_BINARY_DIR}/style_css.c
    ${CMAKE_CURRENT_BINARY_DIR}/script_main_js.c
    ${CMAKE_CURRENT_BINARY_DIR}/script_api_js.c
    ${CMAKE_CURRENT_BINARY_DIR}/script_login_js.c
    ${CMAKE_CURRENT_BINARY_DIR}/script_util_js.c
)
target_include_directories(container-ui-static PUBLIC src)
target_link_libraries(container-ui-static PUBLIC PkgConfig::MHD CURL::libcurl OpenSSL::Crypto)


add_executable(container-ui src/main.cpp)
target_link_libraries(container-ui PRIVATE container-ui-static)

install(TARGETS container-ui)

if(NOT(WITHOUT_TEST))

enable_testing()
include(CTest)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(alltests
    test-src/test_url_util.cpp
    test-src/test_post_data.cpp
    test-src/test_b64.cpp
)

target_link_libraries(alltests PRIVATE container-ui-static GTest::gtest GTest::gtest_main)
gtest_discover_tests(alltests)

endif()