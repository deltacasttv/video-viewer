cmake_minimum_required(VERSION 3.16)

include(cmake/version.cmake)

git_version(GIT_VERSION)
set_version(${GIT_VERSION})

project(
    "VideoViewer"
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "VideoViewer"
    LANGUAGES CXX C
)

add_subdirectory ("src")
add_subdirectory ("sample")