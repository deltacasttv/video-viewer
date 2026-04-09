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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_EXAMPLES "Build samples" ON)

add_subdirectory("src")
if(BUILD_EXAMPLES)
    add_subdirectory("sample")
endif()

if(BUILD_TESTING)
enable_testing()
include(CTest)
add_subdirectory("tests")
endif()