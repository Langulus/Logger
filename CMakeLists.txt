cmake_minimum_required(VERSION 3.30)
project(LangulusLogger
   VERSION         1.0.0
   DESCRIPTION     "Langulus logging library, using {fmt} library"
   HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else 
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
   include(LangulusUtilities.cmake)
   fetch_langulus_module(Core GIT_TAG wip)
   fetch_langulus_module(CT   GIT_TAG wip)
endif()

# Configure {fmt} library                                                   
fetch_external_module(
   fmt
   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
   GIT_TAG        12.2.0
)

# Build and install Logger library                                          
add_langulus_library(LangulusLogger
   SOURCES  source/Logger.cpp
            source/HTML.cpp
            source/TXT.cpp
)

target_compile_definitions(LangulusLogger
   PRIVATE  LANGULUS_EXPORT_ALL
)

target_include_directories(LangulusLogger
   PUBLIC   include
)

target_link_libraries(LangulusLogger
   PUBLIC   LangulusCore
            LangulusCT
            $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:fmt>
)

if(LANGULUS_OPTION_TESTING)
   enable_testing()
   add_subdirectory(test)
endif()