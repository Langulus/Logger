cmake_minimum_required(VERSION 3.20)
include(cmake/DownloadProject.cmake)

project(Langulus_Logger)

# Check if this project is built as standalone, or a part of something else
if(PROJECT_IS_TOP_LEVEL)
    # We're building it as standalone
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    enable_testing()

    # Configure Langulus.Core
    message(STATUS "Configuring external project: Langulus.Core...")
    download_project(
	    PROJ                Langulus_Core
	    GIT_REPOSITORY      https://github.com/Langulus/Core.git
	    GIT_TAG             main
	    GIT_PROGRESS        TRUE
	    UPDATE_DISCONNECTED 1
    )
    include_directories(${Langulus_Core_SOURCE_DIR}/include)
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
add_subdirectory(${fmt_SOURCE_DIR})

# Build and install Logger library
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
add_library(Langulus.Logger ${SOURCE_FILES})

target_include_directories(Langulus.Logger
	PRIVATE   ${CMAKE_CURRENT_SOURCE_DIR}
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Langulus.Logger
	PUBLIC   fmt
)

if (MSVC)
    target_compile_options(Langulus.Logger PRIVATE /utf-8)
endif()

# Testing
add_subdirectory(test)
add_test(NAME Test.Logger COMMAND Test.Logger)
