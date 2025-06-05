cmake_minimum_required(VERSION 3.16)

add_library(video-viewer STATIC)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(video-viewer PUBLIC glfw glm::glm)

include(FetchContent)

FetchContent_Declare(
  gl3w
  GIT_REPOSITORY https://github.com/skaslev/gl3w.git
  GIT_TAG 5f8d7fd191ba22ff2b60c1106d7135bb9a335533
  SOURCE_DIR "${PROJECT_BINARY_DIR}/extern/gl3w"
)

FetchContent_MakeAvailable(gl3w)


find_package(Python COMPONENTS Interpreter REQUIRED)
message("Executing gl3w_gen.py")
execute_process(COMMAND "${PYTHON_EXECUTABLE}" ${gl3w_SOURCE_DIR}/gl3w_gen.py WORKING_DIRECTORY ${gl3w_BINARY_DIR})

target_sources(video-viewer PRIVATE
    ${PROJECT_SOURCE_DIR}/src/videoviewer_internal.cpp
    ${PROJECT_SOURCE_DIR}/src/videoviewer_internal.hpp
    ${PROJECT_SOURCE_DIR}/src/videoviewer.cpp
    ${PROJECT_SOURCE_DIR}/src/shader.cpp
    ${gl3w_BINARY_DIR}/src/gl3w.c
 )

target_include_directories(video-viewer
   PUBLIC "${gl3w_BINARY_DIR}/include"
   PUBLIC ${PROJECT_SOURCE_DIR}/include
   PUBLIC ${PROJECT_SOURCE_DIR}/src
)

if(UNIX)
    target_link_libraries(video-viewer PUBLIC
        dl
        pthread
    )
endif()

set_target_properties(video-viewer PROPERTIES CXX_EXTENSIONS OFF)

set_target_properties(video-viewer PROPERTIES PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/include/videoviewer/videoviewer.hpp)
set_target_properties(video-viewer PROPERTIES DEBUG_POSTFIX d)

install(TARGETS video-viewer DESTINATION "."
        PUBLIC_HEADER DESTINATION "include/videoviewer"
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
          
