cmake_minimum_required(VERSION 3.28)
project(container-ui)

option(WITHOUT_TEST "Disable unit test" OFF)

set(CMAKE_CXX_STANDARD 20)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MHD REQUIRED libmicrohttpd IMPORTED_TARGET)

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
    src/containerui/auth/authorize_handler.cpp
    src/containerui/auth/authenticator.cpp
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