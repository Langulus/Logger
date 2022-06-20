cmake_minimum_required(VERSION 3.20)
project(Langulus_Logger)
include(cmake/DownloadProject.cmake)

# Check if this project is built as standalone, or a part of something else
if(PROJECT_IS_TOP_LEVEL)
    # We're building it as standalone
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    enable_testing()
endif()

# Build and install Logger library
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
add_library(Langulus.Logger ${SOURCE_FILES})

target_include_directories(Langulus.Logger
	PRIVATE   ${CMAKE_CURRENT_SOURCE_DIR}
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
    target_compile_options(Langulus.Logger PRIVATE /utf-8)
endif()

# Testing
add_subdirectory(test)
add_test(NAME Test.Logger COMMAND Test.Logger)
