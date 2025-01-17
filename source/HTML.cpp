///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include "Logger.hpp"

using namespace Langulus;
using namespace Langulus::Logger;


/// Create an HTML file duplicator/redirector                                 
///   @param filename - the relative filename of the log file                 
ToHTML::ToHTML(const TextView& filename) : mFilename {filename} {
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   if (not mFile)
      throw std::runtime_error {"Can't open log file"};
   WriteHeader();
}

ToHTML::~ToHTML() {
   WriteFooter();
   mFile.close();
}

/// Write text                                                                
///   @param text - the text to append to the file                            
void ToHTML::Write(const TextView& text) const noexcept {
   mFile << text;
   mFile.flush();
}

/// Apply some style                                                          
///   @param style - the style to set                                         
void ToHTML::Write(Style style) const noexcept {
   if ((not style.has_foreground() or style.get_foreground().value.term_color == Instance.DefaultStyle.get_foreground().value.term_color)
   and (not style.has_background() or style.get_background().value.term_color == Instance.DefaultStyle.get_background().value.term_color)
   and (not style.has_emphasis()   or style.get_emphasis()                    == Instance.DefaultStyle.get_emphasis())) {
      mFile << "</span><span>";
      return;
   }

   // Always reset before a style change                                
   mFile << "</span><span class=\"";

   if (style.has_foreground()) {
      const auto hex = Hex(style.get_foreground().value.term_color);
      mFile << " f" << hex[0] << hex[1];
   }

   if (style.has_background()) {
      const auto hex = Hex(style.get_background().value.term_color);
      mFile << " b" << hex[0] << hex[1];
   }

   if (style.has_emphasis()) {
      const auto hex = Hex(style.get_emphasis());
      mFile << " e" << hex[0] << hex[1];
   }

   mFile << "\">";
}

/// Remove formatting, add a new line, add a timestamp and tabulate           
///   @attention top of the style stack is not applied                        
void ToHTML::NewLine() const noexcept {
   mFile << "</span></p><p>\n";
   mFile << GetSimpleTime();
   Write(Instance.IntentStyle[int(Instance.CurrentIntent)].prefix);

   auto tabs = Instance.GetTabs();
   if (tabs) {
      while (tabs) {
         Write(Instance.TabString);
         --tabs;
      }
   }

   Write(Instance.GetCurrentStyle());
}

/// Clear the log file                                                        
void ToHTML::Clear() const noexcept {
   mFile.close();
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   WriteHeader();
}

/// Write file header - general HTML styling options, etc.                    
void ToHTML::WriteHeader() const {
   mFile << "<!DOCTYPE html><html>\n";
   mFile << "<body style = \"color: LightGray; background-color: black; font-family: monospace; font-size: 14px; white-space: pre; \">\n";
   mFile << "<head><style>\n";
   mFile << "   @keyframes blink {\n"
            "        0 % {opacity : 1;}\n"
            "       50 % {opacity : 0;}\n"
            "      100 % {opacity : 1;}\n"
            "   }\n";

   // Predeclare all combinations of styles for shorter tags            
   std::unordered_map<fmt::terminal_color, std::string> foregroundColors;
   foregroundColors[fmt::terminal_color::black         ] = "color: black; ";
   foregroundColors[fmt::terminal_color::red           ] = "color: DarkRed; ";
   foregroundColors[fmt::terminal_color::green         ] = "color: ForestGreen; ";
   foregroundColors[fmt::terminal_color::yellow        ] = "color: DarkOrange; ";
   foregroundColors[fmt::terminal_color::blue          ] = "color: blue; ";
   foregroundColors[fmt::terminal_color::magenta       ] = "color: DarkMagenta; ";
   foregroundColors[fmt::terminal_color::cyan          ] = "color: DarkCyan; ";
   foregroundColors[fmt::terminal_color::white         ] = "color: LightGray; ";
   foregroundColors[fmt::terminal_color::bright_black  ] = "color: gray; ";
   foregroundColors[fmt::terminal_color::bright_red    ] = "color: Red; ";
   foregroundColors[fmt::terminal_color::bright_green  ] = "color: GreenYellow; ";
   foregroundColors[fmt::terminal_color::bright_yellow ] = "color: Gold; ";
   foregroundColors[fmt::terminal_color::bright_blue   ] = "color: royalblue; ";
   foregroundColors[fmt::terminal_color::bright_magenta] = "color: magenta; ";
   foregroundColors[fmt::terminal_color::bright_cyan   ] = "color: cyan; ";
   foregroundColors[fmt::terminal_color::bright_white  ] = "color: white; ";
   for (auto fg : foregroundColors) {
      const auto hex = Hex(fg.first);
      mFile << "   .f" << hex[0] << hex[1] << "{" << fg.second << "}\n";
   }

   std::unordered_map<fmt::terminal_color, std::string> backgroundColors;
   backgroundColors[fmt::terminal_color::black         ] = "background-" + foregroundColors[fmt::terminal_color::black];
   backgroundColors[fmt::terminal_color::red           ] = "background-" + foregroundColors[fmt::terminal_color::red];
   backgroundColors[fmt::terminal_color::green         ] = "background-" + foregroundColors[fmt::terminal_color::green];
   backgroundColors[fmt::terminal_color::yellow        ] = "background-" + foregroundColors[fmt::terminal_color::yellow];
   backgroundColors[fmt::terminal_color::blue          ] = "background-" + foregroundColors[fmt::terminal_color::blue];
   backgroundColors[fmt::terminal_color::magenta       ] = "background-" + foregroundColors[fmt::terminal_color::magenta];
   backgroundColors[fmt::terminal_color::cyan          ] = "background-" + foregroundColors[fmt::terminal_color::cyan];
   backgroundColors[fmt::terminal_color::white         ] = "background-" + foregroundColors[fmt::terminal_color::white];
   backgroundColors[fmt::terminal_color::bright_black  ] = "background-" + foregroundColors[fmt::terminal_color::bright_black];
   backgroundColors[fmt::terminal_color::bright_red    ] = "background-" + foregroundColors[fmt::terminal_color::bright_red];
   backgroundColors[fmt::terminal_color::bright_green  ] = "background-" + foregroundColors[fmt::terminal_color::bright_green];
   backgroundColors[fmt::terminal_color::bright_yellow ] = "background-" + foregroundColors[fmt::terminal_color::bright_yellow];
   backgroundColors[fmt::terminal_color::bright_blue   ] = "background-" + foregroundColors[fmt::terminal_color::bright_blue];
   backgroundColors[fmt::terminal_color::bright_magenta] = "background-" + foregroundColors[fmt::terminal_color::bright_magenta];
   backgroundColors[fmt::terminal_color::bright_cyan   ] = "background-" + foregroundColors[fmt::terminal_color::bright_cyan];
   backgroundColors[fmt::terminal_color::bright_white  ] = "background-" + foregroundColors[fmt::terminal_color::bright_white];
   for (auto bg : backgroundColors) {
      const auto hex = Hex(bg.first);
      mFile << "   .b" << hex[0] << hex[1] << "{" << bg.second << "}\n";
   }

   std::unordered_map<fmt::emphasis, std::string> emphasee;
   emphasee[fmt::emphasis::blink]         = "animation: blink 1s infinite; ";
   emphasee[fmt::emphasis::bold]          = "font-weight: bold; ";
   emphasee[fmt::emphasis::conceal]       = "visibility: hidden; ";
   emphasee[fmt::emphasis::faint]         = "text-opacity: 50%; ";
   emphasee[fmt::emphasis::italic]        = "font-style: italic; ";
   emphasee[fmt::emphasis::reverse]       = "mix-blend-mode: difference; ";
   emphasee[fmt::emphasis::strikethrough] = "text-decoration: line-through; ";
   emphasee[fmt::emphasis::underline]     = "text-decoration: underline; ";
   for (unsigned combination = 0; combination <= 255; ++combination) {
      const auto hex = Hex(static_cast<uint8_t>(combination));
      mFile << "   .e" << hex[0] << hex[1] << "{";
      for (auto em : emphasee) {
         if (combination & static_cast<uint8_t>(em.first))
            mFile << em.second;
      }
      mFile << "}\n";
   }


   // Write the most commonly used new-line style as <p>                
   {
      mFile << "   p {\n";
      mFile << "      margin: 0em;\n";
      mFile << "      padding: 0em;\n";
      mFile << "      line-height: 9px;\n";

      if (Instance.DefaultStyle.has_foreground()) {
         auto c = Instance.DefaultStyle.get_foreground().value.term_color;
         mFile << "      " << foregroundColors[static_cast<fmt::terminal_color>(c)] << "\n";
      }

      if (Instance.DefaultStyle.has_background()) {
         auto c = Instance.DefaultStyle.get_background().value.term_color;
         mFile << "      " << backgroundColors[static_cast<fmt::terminal_color>(c)] << "\n";
      }

      if (Instance.DefaultStyle.has_emphasis()) {
         auto e = Instance.DefaultStyle.get_emphasis();
         for (auto em : emphasee) {
            if (static_cast<uint8_t>(e) & static_cast<uint8_t>(em.first))
               mFile << "      " << em.second;
         }
      }

      mFile << "   }\n";
   }

   // Prepare for messages                                              
   mFile << "</style></head>\n";
   mFile << "<h2>Log started - ";
   mFile << GetAdvancedTime();
   mFile << "</h2><p><span>\n";
}

/// Write file footer - just the official shutdown timestamp                  
void ToHTML::WriteFooter() const {
   mFile << "</span></p><h2>Log ended - ";
   mFile << GetAdvancedTime();
   mFile << "</h2></body></html>";
}
