cmake_minimum_required(VERSION 3.22)
include(cmake/FetchLangulusModule.cmake)
project(Langulus_Logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if this project is built as standalone, or a part of something else
if(PROJECT_IS_TOP_LEVEL)
    fetch_langulus_module(Core)

	enable_testing()
	add_subdirectory(test)
endif()

# Configure {fmt} library
message(STATUS "Configuring external project: fmt...")
download_project(
	PROJ                fmt
	GIT_REPOSITORY      https://github.com/fmtlib/fmt.git
	GIT_TAG             master
	GIT_PROGRESS        TRUE
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