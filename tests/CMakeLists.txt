cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(test_colorbar
    test_colorbar.cpp
    ${PROJECT_SOURCE_DIR}/sample/colorbar.cpp
)

target_include_directories(test_colorbar PRIVATE
    ${PROJECT_SOURCE_DIR}/sample
)

target_link_libraries(test_colorbar PRIVATE Catch2::Catch2WithMain)

add_test(NAME test_colorbar COMMAND test_colorbar)
