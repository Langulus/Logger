project(Langulus_Test_Logger)

# Configure Catch2 library
message(STATUS "Configuring external project: Catch2...")
download_project(
	PROJ                Catch2
	GIT_REPOSITORY      https://github.com/catchorg/Catch2.git
	GIT_TAG             v2.x
	GIT_PROGRESS        TRUE
	UPDATE_DISCONNECTED 1
)
add_subdirectory(${Catch2_SOURCE_DIR} ${Catch2_BINARY_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
add_executable(Test.Logger ${SOURCE_FILES})

target_link_libraries(Test.Logger PRIVATE Langulus.Logger Catch2)

add_test(NAME Test.Logger COMMAND Test.Logger)
