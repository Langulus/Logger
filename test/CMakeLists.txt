project(Langulus.Logger.Test)

# Configure Catch2 library													
download_project(
	PROJ                Catch2
	GIT_REPOSITORY      https://github.com/catchorg/Catch2.git
	GIT_TAG             v2.x
	UPDATE_DISCONNECTED 1
)
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} LANGULUS_LOGGER_TEST_SOURCES)
add_executable(Test.Logger ${LANGULUS_LOGGER_TEST_SOURCES})

target_link_libraries(Test.Logger
	PRIVATE		Langulus.Logger 
				Catch2
)

add_test(NAME Test.Logger COMMAND Test.Logger)
