function(git_version _var)
	find_package(Git)
	if (GIT_FOUND)
		execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE out
			OUTPUT_STRIP_TRAILING_WHITESPACE)
			
		 set(${_var}
			  "${out}"
			  PARENT_SCOPE)
	endif()
endfunction()


function(set_version _var)
	# Get partial versions into a list
	string(REGEX MATCHALL "-.*$|[0-9]+" PROJECT_PARTIAL_VERSION_LIST
		${_var})

	# Set the version numbers
	list(GET PROJECT_PARTIAL_VERSION_LIST
		0 PROJECT_VERSION_MAJOR)
	list(GET PROJECT_PARTIAL_VERSION_LIST
		1 PROJECT_VERSION_MINOR)
	list(GET PROJECT_PARTIAL_VERSION_LIST
		2 PROJECT_VERSION_PATCH)

	# Unset the list
	unset(PROJECT_PARTIAL_VERSION_LIST)
	
	# Set project version (without the preceding 'v')
	set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} PARENT_SCOPE)

endfunction()
