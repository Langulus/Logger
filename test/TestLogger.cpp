///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
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
            {
               auto scope2 = Logger::Section("This should be a subsection title");
               Logger::Line("This should be a line inside a subsection #1");
               Logger::Line("This should be a line inside a subsection #2");
               Logger::Line("This should be a line inside a subsection #3");
               Logger::Append(", and this should be appended to it");
            }
            Logger::Line("This should be a line inside a section #3");
            Logger::Append(", and this should be appended to it");
         }
         Logger::Line("This should be a line after a section");
      }

      WHEN("Writing all colors") {
         {
            auto scope = Logger::Section("Now testing foreground colors: ");
            Logger::Line(Logger::Black, "Black, ", Logger::Blue, "Blue, ", Logger::Cyan, "Cyan, ", Logger::DarkBlue, "Dark blue, ");
            Logger::Line(Logger::DarkCyan, "Dark cyan, ", Logger::DarkGray, "Dark gray, ", Logger::DarkGreen, "Dark green, ", Logger::DarkPurple, "Dark purple, ");
            Logger::Line(Logger::DarkRed, "Dark red, ", Logger::DarkYellow, "Dark yellow, ", Logger::Gray, "Gray, ", Logger::Green, "Green, ");
            Logger::Line(Logger::Purple, "Purple, ", Logger::Red, "Red, ", Logger::White, "White, ", Logger::Yellow, "Yellow, ");
         }
         {
            auto scope = Logger::Section("Now testing background colors: ");
            Logger::Line(Logger::BlackBgr, "Black, ", Logger::BlueBgr, "Blue, ", Logger::CyanBgr, "Cyan, ", Logger::DarkBlueBgr, "Dark blue, ");
            Logger::Line(Logger::DarkCyanBgr, "Dark cyan, ", Logger::DarkGrayBgr, "Dark gray, ", Logger::DarkGreenBgr, "Dark green, ", Logger::DarkPurpleBgr, "Dark purple, ");
            Logger::Line(Logger::DarkRedBgr, "Dark red, ", Logger::DarkYellowBgr, "Dark yellow, ", Logger::GrayBgr, "Gray, ", Logger::GreenBgr, "Green, ");
            Logger::Line(Logger::PurpleBgr, "Purple, ", Logger::RedBgr, "Red, ", Logger::WhiteBgr, "White, ", Logger::YellowBgr, "Yellow, ");
         }
         Logger::Line(Logger::Reset, "Done testing colors");
      }

      WHEN("Calling Logger::Fatal()") {
         Logger::Fatal("This should be a fatal error and should be prefixed with FATAL ERROR");
         Logger::Line("This should be a continued fatal error on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Error()") {
         Logger::Error("This should be a non-fatal error, and should be prefixed with ERROR");
         Logger::Line("This should be a continued non-fatal error on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Warning()") {
         Logger::Warning("This should be a warning, and should be prefixed with WARNING");
         Logger::Line("This should be a continued warning on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Verbose()") {
         Logger::Verbose("This is a verbose info message");
         Logger::Line("This should be a continued verbose on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Info()") {
         Logger::Info("This is an info message");
         Logger::Line("This should be a continued info on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Message()") {
         Logger::Message("This is a message directed towards the user");
         Logger::Line("This should be a continued message on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Special()") {
         Logger::Special("This is a special message for a special user, like you");
         Logger::Line("This should be a continued special message on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Flow()") {
         Logger::Flow("This is a flow control message");
         Logger::Line("This should be a continued flow on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Input()") {
         Logger::Input("This is an input event message");
         Logger::Line("This should be a continued input on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Network()") {
         Logger::Network("This is a network message");
         Logger::Line("This should be a continued network on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::OS()") {
         Logger::OS("This is an OS event message");
         Logger::Line("This should be a continued OS event on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Calling Logger::Prompt()") {
         Logger::Prompt("This is an input prompt, that blocks execution and waits for console input (TODO)");
         Logger::Line("This should be a continued Prompt on a new line, without any prefix");
         Logger::Append(", and this should be appended");
      }

      WHEN("Logging numbers, characters and booleans") {
         Logger::Special() << "The answer is " << 42 << '!' << " It's " << true << ", I tell ya!";
      }

      WHEN("Pushing and popping styles") {
         Logger::Warning("This is a warning, ",
            Logger::Push, Logger::Underline, "but now we underline it, ",
               Logger::Push, Logger::RedBgr, "then we even change color, ",
               Logger::Pop, "but then we return to underlined warning, ",
            Logger::Pop, "and finally, back to warning, ",
         Logger::Pop, "but if we actually pop once more, we return to default Logger style\n\n");
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