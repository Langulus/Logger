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
ToHTML::ToHTML(const std::string& filename) : mFilename {filename} {
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   if (not mFile)
      throw std::runtime_error {"Can't open log file"};
   WriteHeader();
}

ToHTML::~ToHTML() {
   mFile.close();
}

/// Write text                                                                
///   @param text - the text to append to the file                            
void ToHTML::Write(const TextView& text) const noexcept {
   mFile << text;
}

/// Apply some style                                                          
///   @param style - the style to set                                         
void ToHTML::Write(const Style& style) const noexcept {
   // Always reset before a style change                                
   Write(" </strong></em></u></blink></del></span>");

   if (style.has_emphasis()) {
      const auto em = static_cast<uint8_t>(style.get_emphasis());
      if (em & static_cast<uint8_t>(fmt::emphasis::bold))
         Write("<strong>");
      //if (em & static_cast<uint8_t>(fmt::emphasis::faint))
      //   ;
      if (em & static_cast<uint8_t>(fmt::emphasis::italic))
         Write("<em>");
      if (em & static_cast<uint8_t>(fmt::emphasis::underline))
         Write("<u>");
      if (em & static_cast<uint8_t>(fmt::emphasis::blink))
         Write("<blink>");
      //if (em & static_cast<uint8_t>(fmt::emphasis::reverse))
      //   ;
      //if (em & static_cast<uint8_t>(fmt::emphasis::conceal))
      //   ;
      if (em & static_cast<uint8_t>(fmt::emphasis::strikethrough))
         Write("<del>");
   }

   if (style.has_foreground()) {
      const auto fg = static_cast<fmt::terminal_color>(
         style.get_foreground().value.term_color);

      switch (fg) {
      case fmt::terminal_color::black:
         Write("<span style = \"color: black;\">");
         break;
      case fmt::terminal_color::red:
         Write("<span style = \"color: DarkRed;\">");
         break;
      case fmt::terminal_color::green:
         Write("<span style = \"color: ForestGreen;\">");
         break;
      case fmt::terminal_color::yellow:
         Write("<span style = \"color: DarkOrange;\">");
         break;
      case fmt::terminal_color::blue:
         Write("<span style = \"color: blue;\">");
         break;
      case fmt::terminal_color::magenta:
         Write("<span style = \"color: DarkMagenta;\">");
         break;
      case fmt::terminal_color::cyan:
         Write("<span style = \"color: DarkCyan;\">");
         break;
      case fmt::terminal_color::white:
         Write("<span style = \"color: LightGray;\">");
         break;
      case fmt::terminal_color::bright_black:
         Write("<span style = \"color: gray;\">");
         break;
      case fmt::terminal_color::bright_red:
         Write("<span style = \"color: Red;\">");
         break;
      case fmt::terminal_color::bright_green:
         Write("<span style = \"color: GreenYellow;\">");
         break;
      case fmt::terminal_color::bright_yellow:
         Write("<span style = \"color: Gold;\">");
         break;
      case fmt::terminal_color::bright_blue:
         Write("<span style = \"color: LightSkyBlue;\">");
         break;
      case fmt::terminal_color::bright_magenta:
         Write("<span style = \"color: magenta;\">");
         break;
      case fmt::terminal_color::bright_cyan:
         Write("<span style = \"color: cyan;\">");
         break;
      case fmt::terminal_color::bright_white:
         Write("<span style = \"color: white;\">");
         break;
      }
   }
   
   if (style.has_background()) {
      const auto fg = static_cast<fmt::terminal_color>(
         style.get_background().value.term_color);

      switch (fg) {
      case fmt::terminal_color::black:
         Write("<span style = \"background-color: black;\">");
         break;
      case fmt::terminal_color::red:
         Write("<span style = \"background-color: DarkRed;\">");
         break;
      case fmt::terminal_color::green:
         Write("<span style = \"background-color: ForestGreen;\">");
         break;
      case fmt::terminal_color::yellow:
         Write("<span style = \"background-color: DarkOrange;\">");
         break;
      case fmt::terminal_color::blue:
         Write("<span style = \"background-color: blue;\">");
         break;
      case fmt::terminal_color::magenta:
         Write("<span style = \"background-color: DarkMagenta;\">");
         break;
      case fmt::terminal_color::cyan:
         Write("<span style = \"background-color: DarkCyan;\">");
         break;
      case fmt::terminal_color::white:
         Write("<span style = \"background-color: LightGray;\">");
         break;
      case fmt::terminal_color::bright_black:
         Write("<span style = \"background-color: gray;\">");
         break;
      case fmt::terminal_color::bright_red:
         Write("<span style = \"background-color: Red;\">");
         break;
      case fmt::terminal_color::bright_green:
         Write("<span style = \"background-color: GreenYellow;\">");
         break;
      case fmt::terminal_color::bright_yellow:
         Write("<span style = \"background-color: Gold;\">");
         break;
      case fmt::terminal_color::bright_blue:
         Write("<span style = \"background-color: LightSkyBlue;\">");
         break;
      case fmt::terminal_color::bright_magenta:
         Write("<span style = \"background-color: magenta;\">");
         break;
      case fmt::terminal_color::bright_cyan:
         Write("<span style = \"background-color: cyan;\">");
         break;
      case fmt::terminal_color::bright_white:
         Write("<span style = \"background-color: white;\">");
         break;
      }
   }
}

/// Remove formatting, add a new line, add a timestamp and tabulate           
///   @attention top of the style stack is not applied                        
void ToHTML::NewLine() const noexcept {
   Write("<br>");
   Write(Instance.TimeStampStyle);
   Write(GetSimpleTime());
   Write("| ");

   auto tabs = Instance.GetTabs();
   if (tabs) {
      Write(Instance.TabStyle);
      while (tabs) {
         Write(Instance.TabString);
         --tabs;
      }
   }
}

/// Clear the log file                                                        
void ToHTML::Clear() const noexcept {
   mFile.close();
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   WriteHeader();
}

/// Write file header - general HTML styling options, etc.                    
void ToHTML::WriteHeader() const {
   Write("<body style = \"color: gray; background-color: black; font-family: monospace; font-size: 14px;\">");
   Write("<h1>Log started - ");
   Write(GetAdvancedTime());
   Write("</h1>");
}
