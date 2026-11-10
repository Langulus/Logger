///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing.hpp>
#include <Langulus/Logger.hpp>
#include <Langulus/Logger/HTML.hpp>
#include <Langulus/Logger/TXT.hpp>
#include <fstream>

using namespace Langulus;


SCENARIO("Logging to console (stateless)") {
   Logger::LineRaw("\n\nTESTING STATELESS LOGGING\n");
   
   Logger::LineRaw("This should be line #1");
   Logger::LineRaw("This should be line #2");
   Logger::LineRaw("This should be line #3");

   Logger::LineRaw("This should be line #4");
   Logger::AppendRaw(", and this should be appended #1");
   Logger::AppendRaw(", and this should be appended #2, too");

   Logger::LineRaw(Logger::Black, "Black, ", Logger::Blue, "Blue, ", Logger::Cyan, "Cyan, ", Logger::DarkBlue, "Dark blue, ");
   Logger::LineRaw(Logger::DarkCyan, "Dark cyan, ", Logger::DarkGray, "Dark gray, ", Logger::DarkGreen, "Dark green, ", Logger::DarkPurple, "Dark purple, ");
   Logger::LineRaw(Logger::DarkRed, "Dark red, ", Logger::DarkYellow, "Dark yellow, ", Logger::Gray, "Gray, ", Logger::Green, "Green, ");
   Logger::LineRaw(Logger::Purple, "Purple, ", Logger::Red, "Red, ", Logger::White, "White, ", Logger::Yellow, "Yellow, ");

   Logger::LineRaw(Logger::BlackBgr, "Black, ", Logger::BlueBgr, "Blue, ", Logger::CyanBgr, "Cyan, ", Logger::DarkBlueBgr, "Dark blue, ");
   Logger::LineRaw(Logger::DarkCyanBgr, "Dark cyan, ", Logger::DarkGrayBgr, "Dark gray, ", Logger::DarkGreenBgr, "Dark green, ", Logger::DarkPurpleBgr, "Dark purple, ");
   Logger::LineRaw(Logger::DarkRedBgr, "Dark red, ", Logger::DarkYellowBgr, "Dark yellow, ", Logger::GrayBgr, "Gray, ", Logger::GreenBgr, "Green, ");
   Logger::LineRaw(Logger::PurpleBgr, "Purple, ", Logger::RedBgr, "Red, ", Logger::WhiteBgr, "White, ", Logger::YellowBgr, "Yellow, ");


   Logger::FatalRaw("This should be a fatal error and should be prefixed with |F|");
   Logger::LineRaw("This should be a continued fatal error on a new line, with |F| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::ErrorRaw("This should be a non-fatal error, and should be prefixed with |E|");
   Logger::LineRaw("This should be a continued non-fatal error on a new line, with |E| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::WarningRaw("This should be a warning, and should be prefixed with |W|");
   Logger::LineRaw("This should be a continued warning on a new line, with |W| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::VerboseRaw("This is a verbose info message, and should be prefixed with |V|");
   Logger::LineRaw("This should be a continued verbose on a new line, with |V| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::InfoRaw("This is an info message, and should be prefixed with |I|");
   Logger::LineRaw("This should be a continued info on a new line, with |I| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::MessageRaw("This is a message directed towards the user, and should be prefixed with |M|");
   Logger::LineRaw("This should be a continued message on a new line, with |M| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::SpecialRaw("This is a special message for a special user, like you, and should be prefixed with |S|");
   Logger::LineRaw("This should be a continued special message on a new line, with |S| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::FlowRaw("This is a flow control message, and should be prefixed with |L|");
   Logger::LineRaw("This should be a continued flow on a new line, with |L| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::InputRaw("This is an input event message, and should be prefixed with |N|");
   Logger::LineRaw("This should be a continued input on a new line, with |N| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::NetworkRaw("This is a network message, and should be prefixed with |T|");
   Logger::LineRaw("This should be a continued network on a new line, with |T| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::OSRaw("This is an OS event message, and should be prefixed with |O|");
   Logger::LineRaw("This should be a continued OS event on a new line, with |O| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::PromptRaw("This is an input prompt, that blocks execution and waits for console input (TODO), and should be prefixed with |P|");
   Logger::LineRaw("This should be a continued Prompt on a new line, with |P| prefix");
   Logger::AppendRaw(", and this should be appended");

   Logger::SpecialRaw("#2 The answer is ", 42, '!', " It's ", true, ", I tell ya!");
}

SCENARIO("Logging to console (stateful)") {
   // Duplicate any logging messages to an external HTML file           
   Logger::ToHTML html_test {"html_test.htm"};
   Logger::AttachDuplicator(&html_test);
   Logger::ToTXT txt_test {"txt_test.txt"};
   Logger::AttachDuplicator(&txt_test);

   Logger::Info("TESTING STATEFUL LOGGING");
   
   Logger::Line("This should be line #1");
   Logger::Line("This should be line #2");
   Logger::Line("This should be line #3");

   Logger::Line("This should be line #4");
   Logger::Append(", and this should be appended #1");
   Logger::Append(", and this should be appended #2, too");

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

   {
      Logger::Fatal("This should be a fatal error and should be prefixed with |F|");
      Logger::Line("This should be a continued fatal error on a new line, with |F| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a fatal error");
      Logger::Line("This should be a line inside a section, inside a fatal error");
   }

   {
      Logger::Error("This should be a non-fatal error, and should be prefixed with |E|");
      Logger::Line("This should be a continued non-fatal error on a new line, with |E| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside an error");
      Logger::Line("This should be a line inside a section, inside an error");
   }

   {
      Logger::Warning("This should be a warning, and should be prefixed with |W|");
      Logger::Line("This should be a continued warning on a new line, with |W| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a warning");
      Logger::Line("This should be a line inside a section, inside a warning");
   }

   {
      Logger::Verbose("This is a verbose info message, and should be prefixed with |V|");
      Logger::Line("This should be a continued verbose on a new line, with |V| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a verbose message");
      Logger::Line("This should be a line inside a section, inside a verbose message");
   }

   {
      Logger::Info("This is an info message, and should be prefixed with |I|");
      Logger::Line("This should be a continued info on a new line, with |I| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside an info message");
      Logger::Line("This should be a line inside a section, inside an info message");
   }

   {
      Logger::Message("This is a message directed towards the user, and should be prefixed with |M|");
      Logger::Line("This should be a continued message on a new line, with |M| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a message");
      Logger::Line("This should be a line inside a section, inside a message");
   }

   {
      Logger::Special("This is a special message for a special user, like you, and should be prefixed with |S|");
      Logger::Line("This should be a continued special message on a new line, with |S| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a special message");
      Logger::Line("This should be a line inside a section, inside a special message");
   }

   {
      Logger::Flow("This is a flow control message, and should be prefixed with |L|");
      Logger::Line("This should be a continued flow on a new line, with |L| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a flow message");
      Logger::Line("This should be a line inside a section, inside a flow message");
   }

   {
      Logger::Input("This is an input event message, and should be prefixed with |N|");
      Logger::Line("This should be a continued input on a new line, with |N| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside an input message");
      Logger::Line("This should be a line inside a section, inside an input message");
   }

   {
      Logger::Network("This is a network message, and should be prefixed with |T|");
      Logger::Line("This should be a continued network on a new line, with |T| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a network message");
      Logger::Line("This should be a line inside a section, inside a network message");
   }

   {
      Logger::OS("This is an OS event message, and should be prefixed with |O|");
      Logger::Line("This should be a continued OS event on a new line, with |O| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside an OS message");
      Logger::Line("This should be a line inside a section, inside an OS message");
   }

   {
      Logger::Prompt("This is an input prompt, that blocks execution and waits for console input (TODO), and should be prefixed with |P|");
      Logger::Line("This should be a continued Prompt on a new line, with |P| prefix");
      Logger::Append(", and this should be appended");
      auto scope = Logger::Section("This should be a section inside a prompt");
      Logger::Line("This should be a line inside a section, inside a prompt");
      auto scope2 = Logger::OSSection("This should be an OSSection inside a prompt section");
      Logger::Network("This is a network message inside two sections");
   }

   Logger::Special("#2 The answer is ", 42, '!', " It's ", true, ", I tell ya!");

   {
      Logger::Line("#2 You shouldn't see the following (color should be reset to default intent): ", Logger::Intent::Ignore, 42, '!', " It's ", true, ", I tell ya!");
      auto scope1 = Logger::Section("This section should be invisible, too");
      Logger::Line("#3 You shouldn't see this line AT ALL: ", 42, '!', " It's ", true, ", I tell ya!");
      auto scope2 = Logger::Section("As well as this one");
   }

   {
      Logger::Warning(Logger::Color::Cyan, "This is actually a warning, ",
         Logger::Push, Logger::Underline, "but now we underline it, ",
            Logger::PushRedBgr, "then we even change color, ",
            Logger::Pop, "but then we return to underlined warning, ",
         Logger::Pop, "and finally, back to warning, ",
      Logger::Pop, "but if we actually pop once more, we return to the original intent",
      Logger::Pop, ", and any subsequent pop shouldn't change anything");
      Logger::Line("^ just checking the above statement\n\n");
   }

   Logger::DettachDuplicator(&html_test);
   Logger::DettachDuplicator(&txt_test);

   std::locale::global(std::locale::classic());

   WHEN("HTML is compared with the canonical") {
      std::ifstream f1(          "html_test.htm");
      std::ifstream f2("canonical_html_test.htm");
      REQUIRE(f1);
      REQUIRE(f2);

      [[maybe_unused]] size_t line_counter = 0;
      std::string line1, line2;
      do
      {
         std::getline(f1, line1);
         std::getline(f2, line2);
         ++line_counter;

         // Skip log start/log end messages due to unique timestamps    
         if (line1.starts_with("<h2>") and line2.starts_with("<h2>"))
            continue;

         // When comparing lines, skip the timestamp                    
         if (not line1.empty() and not line2.empty()
         and line1[2] == ':' and line1[5] == ':' and line1[8] == '|'
         and line2[2] == ':' and line2[5] == ':' and line2[8] == '|') {
            REQUIRE(line1.substr(8) == line2.substr(8));
         }
         else {
            REQUIRE(line1 == line2);
         }
      }
      while(not f1.eof() and not f2.eof());
   }

   WHEN("TXT is compared with the canonical") {
      std::ifstream f1(          "txt_test.txt");
      std::ifstream f2("canonical_txt_test.txt");
      REQUIRE(f1);
      REQUIRE(f2);

      [[maybe_unused]] size_t line_counter = 0;
      std::string line1, line2;
      do
      {
         std::getline(f1, line1);
         std::getline(f2, line2);
         ++line_counter;

         // Skip log start/log end messages due to unique timestamps    
         if (line1.starts_with("Log started - ") and line2.starts_with("Log started - "))
            continue;
         if (line1.starts_with("Log ended - ") and line2.starts_with("Log ended - "))
            continue;

         // When comparing lines, skip the timestamp                    
         if (not line1.empty() and not line2.empty()
         and line1[2] == ':' and line1[5] == ':' and line1[8] == '|'
         and line2[2] == ':' and line2[5] == ':' and line2[8] == '|') {
            REQUIRE(line1.substr(8) == line2.substr(8));
         }
         else {
            REQUIRE(line1 == line2);
         }
      }
      while(not f1.eof() and not f2.eof());
   }
}
