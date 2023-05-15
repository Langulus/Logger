project(Langulus::Logger::Test)

add_executable(LangulusLoggerTest 
	Main.cpp
	TestLogger.cpp
)

target_link_libraries(LangulusLoggerTest
	PRIVATE		LangulusLogger 
				Catch2
)

add_test(
	NAME		Langulus::Logger::Test
	COMMAND		LangulusLoggerTest
)

langulus_copy_dlls(LangulusLoggerTest ON LangulusLoggerTest)