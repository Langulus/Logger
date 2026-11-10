add_langulus_test(LangulusLoggerTest
	SOURCES		TestLogger.cpp
	LIBRARIES	LangulusLogger
)
configure_file("canonical_html_test.htm" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canonical_html_test.htm" COPYONLY)
configure_file("canonical_txt_test.txt"  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canonical_txt_test.txt"  COPYONLY)
