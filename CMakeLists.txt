cmake_minimum_required(VERSION 3.28)
project(LangulusLogger
    VERSION         1.0.0
    DESCRIPTION     "Langulus logging library, using {fmt} library"
    HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else 
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

    include(LangulusUtilities.cmake)

    # Add Langulus::Core library                                            
    fetch_langulus_module(Core GIT_TAG 35756f11d2f9c475f27b094b8d4c82cd453969fc)
endif()

# Configure {fmt} library                                                   
fetch_external_module(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        22701d5f63fd9ba3ffa35fe94585b5bfcb69238b # master branch
)

# Build and install Logger library                                          
add_langulus_library(LangulusLogger
	source/Logger.cpp
	source/HTML.cpp
	source/TXT.cpp
)

target_compile_definitions(LangulusLogger
    PRIVATE     LANGULUS_EXPORT_ALL
)

target_include_directories(LangulusLogger
    PUBLIC      include
)

target_link_libraries(LangulusLogger
    PUBLIC      LangulusCore
                fmt
)

if (LANGULUS_TESTING)
    enable_testing()
	add_subdirectory(test)
endif()