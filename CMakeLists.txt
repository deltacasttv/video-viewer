include(cmake/version.cmake)

cmake_minimum_required (VERSION 3.16)

git_version(GIT_VERSION)
set_version(${GIT_VERSION})

project (
    "VideoViewer"
    VERSION ${PROJECT_VERSION}
    DESCRIPTION ${PROJECT_NAME}
    LANGUAGES CXX C
)

add_subdirectory ("src")
add_subdirectory ("sample")