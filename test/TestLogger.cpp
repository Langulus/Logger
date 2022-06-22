///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#include "Main.hpp"
#include <catch2/catch.hpp>

SCENARIO("Logging to console", "[logger]") {
	GIVEN("An initialized logger") {
		WHEN("Calling Logger::Line()") {
			Logger::Line("This should be line #1");
			Logger::Line("This should be line #2");
			Logger::Line("This should be line #3");
		}

		WHEN("Calling Logger::Line()") {
			Logger::Line("This should be line #4");
			Logger::Append(", and this should be appended #1");
			Logger::Append(", and this should be appended #2, too");
		}

		WHEN("Calling Logger::Section()") {
			Logger::Line("This should be a line before a section");
			{
				auto scope = Logger::Section("This should be the section title");
				Logger::Line("This should be a line inside a section #1");
				Logger::Line("This should be a line inside a section #2");
				Logger::Line("This should be a line inside a section #3");
				Logger::Append(", and this should be appended to it");
			}
			Logger::Line("This should be a line after a section");
		}

		WHEN("Calling Logger::Fatal()") {
			Logger::Fatal("This should be a fatal error and should be prefixed with FATAL ERROR");
		}

		WHEN("Calling Logger::Error()") {
			Logger::Error("This should be a non-fatal error, and should be prefixed with ERROR");
		}

		WHEN("Calling Logger::Warning()") {
			Logger::Warning("This should be a warning, and should be prefixed with WARNING");
		}

		WHEN("Calling Logger::Verbose()") {
			Logger::Verbose("This is a verbose info message");
		}

		WHEN("Calling Logger::Info()") {
			Logger::Info("This is an info message");
		}

		WHEN("Calling Logger::Message()") {
			Logger::Message("This is a message directed towards the user");
		}

		WHEN("Calling Logger::Special()") {
			Logger::Special("This is a special message for a special user, like you");
		}

		WHEN("Calling Logger::Flow()") {
			Logger::Flow("This is a flow control message");
		}

		WHEN("Calling Logger::Input()") {
			Logger::Input("This is an input event message");
		}

		WHEN("Calling Logger::Network()") {
			Logger::Network("This is a network message");
		}

		WHEN("Calling Logger::OS()") {
			Logger::OS("This is an OS event message");
		}

		WHEN("Calling Logger::Prompt()") {
			Logger::Prompt("This is an input prompt, that blocks execution and waits for console input (TODO)");
		}
	}
}

SCENARIO("Logging to an html log file", "[logger]") {
	GIVEN("An initialized logger with an HTML attachment") {
		WHEN("TODO") {
			THEN("TODO") {
				REQUIRE(true);
			}
		}
	}
}

SCENARIO("Logging to a benchmark file", "[logger]") {
	GIVEN("An initialized logger") {
		WHEN("TODO") {
			THEN("TODO") {
				REQUIRE(true);
			}
		}
	}
}