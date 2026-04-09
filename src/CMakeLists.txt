cmake_minimum_required(VERSION 3.16)

add_library(video-viewer STATIC)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad REQUIRED)
target_link_libraries(video-viewer PUBLIC glfw glm::glm glad::glad)

target_sources(video-viewer PRIVATE
    ${PROJECT_SOURCE_DIR}/src/videoviewer_internal.cpp
    ${PROJECT_SOURCE_DIR}/src/videoviewer_internal.hpp
    ${PROJECT_SOURCE_DIR}/src/videoviewer.cpp
    ${PROJECT_SOURCE_DIR}/src/shader.cpp
 )

target_include_directories(video-viewer
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

target_compile_features(video-viewer PUBLIC cxx_std_14)

install(TARGETS video-viewer DESTINATION "."
        PUBLIC_HEADER DESTINATION "include/videoviewer"
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )