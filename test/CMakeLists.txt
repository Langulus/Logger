file(GLOB_RECURSE
	LANGULUS_LOGGER_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp *.hpp
)

add_executable(LangulusLoggerTest ${LANGULUS_LOGGER_TEST_SOURCES})

target_link_libraries(LangulusLoggerTest
	PRIVATE		LangulusLogger 
				Catch2
)

add_test(
	NAME		LangulusLoggerTest
	COMMAND		LangulusLoggerTest
)