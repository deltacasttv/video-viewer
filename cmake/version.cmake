function(git_version _var)
	find_package(Git)
	if (GIT_FOUND)
		execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			OUTPUT_VARIABLE out
			RESULT_VARIABLE ret
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE)
		
		if(ret)
			set(${_var}
			"0.0.0"
			PARENT_SCOPE)
		else()
			set(${_var}
			"${out}"
			PARENT_SCOPE)
		endif()
		 
	endif()
endfunction()


function(set_version _var)
    # Get partial versions into a list
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(.*)" VERSION_MATCHED "${_var}")
    
    if(VERSION_MATCHED)
        set(PROJECT_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3} PARENT_SCOPE)
    else()
		set(PROJECT_VERSION 0.0.0 PARENT_SCOPE)
    endif()
endfunction()