cmake_minimum_required(VERSION 3.22)
project(Langulus.Logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if this project is built as standalone, or a part of something else 
if(PROJECT_IS_TOP_LEVEL)
    if(LANGULUS_BUILD_SHARED_LIBRARIES)
        add_compile_definitions(LANGULUS_EXPORT_ALL)
        set(LANGULUS_LIBRARY_TYPE SHARED)
    else()
        set(LANGULUS_LIBRARY_TYPE STATIC)
    endif()

    include(LangulusUtilities.cmake)
    langulus_init_git_submodule(cmake)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(FetchLangulusModule)
    include(HandleLangulusFeatures)

    handle_langulus_features()
    fetch_langulus_module(Core)

    add_compile_definitions(LANGULUS_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()

# Configure {fmt} library                                                   
download_project(
	PROJ                fmt
	GIT_REPOSITORY      https://github.com/fmtlib/fmt.git
	GIT_TAG             master
	UPDATE_DISCONNECTED 1
)
add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})

# Build and install Logger library                                          
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source LANGULUS_LOGGER_SOURCES)
add_library(Langulus.Logger ${LANGULUS_LIBRARY_TYPE} ${LANGULUS_LOGGER_SOURCES})

target_include_directories(Langulus.Logger PUBLIC include)
target_link_libraries(Langulus.Logger PUBLIC Langulus.Core fmt)
