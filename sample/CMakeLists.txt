
cmake_minimum_required(VERSION 3.16)

add_executable (videoviewer_sample)

target_sources(videoviewer_sample PRIVATE
	main.cpp
	colorbar.cpp
)

target_link_libraries(videoviewer_sample PRIVATE video-viewer)

if(UNIX)
	target_sources(videoviewer_sample PRIVATE
		../dep/keyboard.cpp
		../dep/keyboard.h
	)
	target_link_libraries(videoviewer_sample PRIVATE
		pthread
	)
endif()

target_compile_features(videoviewer_sample PUBLIC cxx_std_11)
set_target_properties(videoviewer_sample PROPERTIES CXX_EXTENSIONS OFF)
 