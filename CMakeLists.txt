cmake_minimum_required(VERSION 3.22)
include(cmake/FetchLangulusModule.cmake)
project(Langulus.Logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure the features you require, all these are disabled by default
option(LANGULUS_ENABLE_SAFE_MODE 
    "Overrides additional error checking and sanity checks, \
    incurs a serious runtime overhead")

option(LANGULUS_ENABLE_DEBUGGING 
    "Explicitly define debug mode, in the case that default flags are \
    not detected by some odd reason")

if(LANGULUS_ENABLE_SAFE_MODE)
    add_compile_definitions(LANGULUS_ENABLE_SAFE_MODE)
endif()
if(LANGULUS_ENABLE_DEBUGGING)
    add_compile_definitions(LANGULUS_ENABLE_DEBUGGING)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(STATUS "Debug mode, all assertions are enabled")
	add_compile_definitions(LANGULUS_ENABLE_ASSERTION_LEVEL=1000)
    add_compile_definitions(LANGULUS_ENABLE_DEBUGGING)
endif()

# Check if this project is built as standalone, or a part of something else
if(PROJECT_IS_TOP_LEVEL)
    fetch_langulus_module(Core)

    add_compile_definitions(LANGULUS_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()

# Configure {fmt} library
message(STATUS "Configuring external project: fmt...")
download_project(
	PROJ                fmt
	GIT_REPOSITORY      https://github.com/fmtlib/fmt.git
	GIT_TAG             master
	UPDATE_DISCONNECTED 1
)
add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})

# Build and install Logger library
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
add_library(Langulus.Logger ${SOURCE_FILES})

target_include_directories(Langulus.Logger PUBLIC include)
target_link_libraries(Langulus.Logger PUBLIC Langulus.Core fmt)

if (MSVC)
    target_compile_options(Langulus.Logger PRIVATE /utf-8)
endif()