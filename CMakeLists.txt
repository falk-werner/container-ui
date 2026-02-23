cmake_minimum_required(VERSION 3.28)
project(container-ui)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MHD REQUIRED libmicrohttpd IMPORTED_TARGET)

add_executable(container-ui
    src/containerui/static_resource_handler.cpp
    src/containerui/passthrough_handler.cpp
    src/containerui/webserver.cpp
    src/containerui/webserver_context.cpp
    src/containerui/fetch.cpp
    src/main.cpp)
target_include_directories(container-ui PRIVATE src)
target_link_libraries(container-ui PRIVATE PkgConfig::MHD CURL::libcurl)
