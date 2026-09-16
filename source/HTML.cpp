///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Logger/HTML.hpp>
#include <map>

using namespace Langulus;
using namespace Langulus::Logger;


/// Create an HTML file duplicator/redirector                                 
///   @param filename - the relative filename of the log file                 
ToHTML::ToHTML(::std::string_view const& filename) : mFilename {filename} {
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
void ToHTML::Write(::std::string_view const& text) const noexcept {
   if (mLastWrittenStyle != GlobalState.GetCurrentStyle())
      Write(GlobalState.GetCurrentStyle());

   mFile << text;
   mFile.flush();
}

/// Apply some style                                                          
///   @param style - the style to set                                         
void ToHTML::Write(Style const style) const noexcept {
   if (mScopeOpened) {
      mFile << "</span>";
      mScopeOpened = false;
   }

   bool needs_separator = false;
   if (style.has_foreground()) {
      const uint8_t fg = style.get_foreground().value();
      if (not GlobalState.mDefaultStyle.has_foreground()
      or fg != GlobalState.mDefaultStyle.get_foreground().value()) {
         if (not mScopeOpened) {
            mFile << "<span class=\"";
            mScopeOpened = true;
         }

         const auto hex = Hex(fg);
         mFile << "f" << hex[0] << hex[1];
         needs_separator = true;
      }
   }

   if (style.has_background()) {
      const uint8_t bg = style.get_background().value();
      if (not GlobalState.mDefaultStyle.has_background()
      or bg != GlobalState.mDefaultStyle.get_background().value()) {
         if (not mScopeOpened) {
            mFile << "<span class=\"";
            mScopeOpened = true;
         }

         const auto hex = Hex(bg);
         if (needs_separator)
            mFile << " b";
         else
            mFile << "b";

         mFile << hex[0] << hex[1];
         needs_separator = true;
      }
   }

   if (style.has_emphasis()) {
      const auto em = style.get_emphasis();
      if (not GlobalState.mDefaultStyle.has_emphasis()
      or em != GlobalState.mDefaultStyle.get_emphasis()) {
         if (not mScopeOpened) {
            mFile << "<span class=\"";
            mScopeOpened = true;
         }

         const auto hex = Hex(em);
         if (needs_separator)
            mFile << " e";
         else
            mFile << "e";

         mFile << hex[0] << hex[1];
      }
   }

   if (mScopeOpened)
      mFile << "\">";
   mLastWrittenStyle = GlobalState.GetCurrentStyle();
}

/// Remove formatting, add a new line, add a timestamp and tabulate           
///   @attention top of the style stack is not applied                        
void ToHTML::NewLine() const noexcept {
   if (mScopeOpened) {
      mFile << "</span>";
      mScopeOpened = false;
   }

   mFile << "</p><p>\n";
   mFile << GlobalState.GetSimpleTime();
   mFile << GlobalState.mIntentStyle[GlobalState.GetCurrentIntent()].prefix;

   auto tabs = GlobalState.GetTabs();
   if (tabs) {
      Write(GlobalState.GetCurrentStyle());
      while (tabs) {
         mFile << GlobalState.mTabString;
         --tabs;
      }
   }
   else mLastWrittenStyle = GlobalState.mDefaultStyle;
}

/// Clear the log file                                                        
void ToHTML::Clear() const noexcept {
   mFile.close();
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   WriteHeader();
}

/// Returns the output filename                                               
auto ToHTML::GetFilename() const noexcept -> ::std::string_view {
   return mFilename;
}

/// Write file header - general HTML styling options, etc.                    
void ToHTML::WriteHeader() const {
   mFile << "<!DOCTYPE html><html>\n";
   mFile << "<head><style>\n";
   mFile << "   @keyframes blink {\n"
            "        0 % {opacity : 1;}\n"
            "       50 % {opacity : 0;}\n"
            "      100 % {opacity : 1;}\n"
            "   }\n";

   // Predeclare all combinations of styles for shorter tags            
   std::map<fmt::terminal_color, std::string> foregroundColors;
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

   std::map<fmt::terminal_color, std::string> backgroundColors;
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

   std::map<fmt::emphasis, std::string> emphasee;
   emphasee[fmt::emphasis::blink]         = "animation: blink 1s infinite; ";
   emphasee[fmt::emphasis::bold]          = "font-weight: bold; ";
   emphasee[fmt::emphasis::conceal]       = "visibility: hidden; ";
   emphasee[fmt::emphasis::faint]         = "text-opacity: 50%; ";
   emphasee[fmt::emphasis::italic]        = "font-style: italic; ";
   emphasee[fmt::emphasis::reverse]       = "mix-blend-mode: difference; ";
   emphasee[fmt::emphasis::strikethrough] = "text-decoration: line-through; ";
   emphasee[fmt::emphasis::underline]     = "text-decoration: underline; ";
   for (uint combination = 0; combination <= 255; ++combination) {
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

      if (GlobalState.mDefaultStyle.has_foreground()) {
         uint8_t c = GlobalState.mDefaultStyle.get_foreground().value();
         mFile << "      " << foregroundColors[static_cast<fmt::terminal_color>(c)] << "\n";
      }

      if (GlobalState.mDefaultStyle.has_background()) {
         uint8_t c = GlobalState.mDefaultStyle.get_background().value();
         mFile << "      " << backgroundColors[static_cast<fmt::terminal_color>(c)] << "\n";
      }

      if (GlobalState.mDefaultStyle.has_emphasis()) {
         auto e = GlobalState.mDefaultStyle.get_emphasis();
         for (auto em : emphasee) {
            if (static_cast<uint8_t>(e) & static_cast<uint8_t>(em.first))
               mFile << "      " << em.second;
         }
      }

      mFile << "   }\n";
   }

   // Prepare for messages                                              
   mFile << "</style></head>\n";
   mFile << "<body style = \"color: LightGray; background-color: black; font-family: monospace; font-size: 14px; white-space: pre; \">\n";
   mFile << "<h2>Log started - ";
   mFile << GetAdvancedTime();
   mFile << "</h2><p>\n";

   mScopeOpened = false;
   mLastWrittenStyle = GlobalState.mDefaultStyle;
}

/// Write file footer - just the official shutdown timestamp                  
void ToHTML::WriteFooter() const {
   if (mScopeOpened) {
      mFile << "</span>";
      mScopeOpened = false;
   }

   mFile << "</p>\n<h2>Log ended - ";
   mFile << GetAdvancedTime();
   mFile << "</h2></body></html>";
}
