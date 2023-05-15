project(LangulusLoggerTest)

add_executable(TestLogger
	Main.cpp
	TestLogger.cpp
)

target_link_libraries(TestLogger
	PRIVATE		LangulusLogger 
				Catch2
)

add_test(
	NAME		TestLogger
	COMMAND		TestLogger
)

langulus_copy_dlls(TestLogger ON TestLogger)