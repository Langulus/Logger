project(PCLogger)

add_definitions(-DPC_EXPORT_LOGGER)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

add_library(PCLogger SHARED ${SOURCE_FILES})

target_include_directories(PCLogger
	PRIVATE		${CMAKE_CURRENT_SOURCE_DIR}
	INTERFACE	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(PCLogger
	PUBLIC		PCCore
)

install(TARGETS PCLogger DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})