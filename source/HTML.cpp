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
   // Always reset before a style change                                
   Write("\n</code></strong></em></u></blink></del></span><code>");

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

   if (not style.has_foreground() and not style.has_background())
      return;

   std::string style_string = "<span style = \"";
   if (style.has_foreground()) {
      const auto fg = static_cast<fmt::terminal_color>(
         style.get_foreground().value.term_color);

      switch (fg) {
      case fmt::terminal_color::black:
         style_string += "color: black; ";
         break;
      case fmt::terminal_color::red:
         style_string += "color: DarkRed; ";
         break;
      case fmt::terminal_color::green:
         style_string += "color: ForestGreen; ";
         break;
      case fmt::terminal_color::yellow:
         style_string += "color: DarkOrange; ";
         break;
      case fmt::terminal_color::blue:
         style_string += "color: blue; ";
         break;
      case fmt::terminal_color::magenta:
         style_string += "color: DarkMagenta; ";
         break;
      case fmt::terminal_color::cyan:
         style_string += "color: DarkCyan; ";
         break;
      case fmt::terminal_color::white:
         style_string += "color: LightGray; ";
         break;
      case fmt::terminal_color::bright_black:
         style_string += "color: gray; ";
         break;
      case fmt::terminal_color::bright_red:
         style_string += "color: Red; ";
         break;
      case fmt::terminal_color::bright_green:
         style_string += "color: GreenYellow; ";
         break;
      case fmt::terminal_color::bright_yellow:
         style_string += "color: Gold; ";
         break;
      case fmt::terminal_color::bright_blue:
         style_string += "color: royalblue; ";
         break;
      case fmt::terminal_color::bright_magenta:
         style_string += "color: magenta; ";
         break;
      case fmt::terminal_color::bright_cyan:
         style_string += "color: cyan; ";
         break;
      case fmt::terminal_color::bright_white:
         style_string += "color: white; ";
         break;
      }
   }
   
   if (style.has_background()) {
      const auto bg = static_cast<fmt::terminal_color>(
         style.get_background().value.term_color);

      switch (bg) {
      case fmt::terminal_color::black:
         style_string += "background-color: black; ";
         break;
      case fmt::terminal_color::red:
         style_string += "background-color: DarkRed; ";
         break;
      case fmt::terminal_color::green:
         style_string += "background-color: ForestGreen; ";
         break;
      case fmt::terminal_color::yellow:
         style_string += "background-color: DarkOrange; ";
         break;
      case fmt::terminal_color::blue:
         style_string += "background-color: blue; ";
         break;
      case fmt::terminal_color::magenta:
         style_string += "background-color: DarkMagenta; ";
         break;
      case fmt::terminal_color::cyan:
         style_string += "background-color: DarkCyan; ";
         break;
      case fmt::terminal_color::white:
         style_string += "background-color: LightGray; ";
         break;
      case fmt::terminal_color::bright_black:
         style_string += "background-color: gray; ";
         break;
      case fmt::terminal_color::bright_red:
         style_string += "background-color: Red; ";
         break;
      case fmt::terminal_color::bright_green:
         style_string += "background-color: GreenYellow; ";
         break;
      case fmt::terminal_color::bright_yellow:
         style_string += "background-color: Gold; ";
         break;
      case fmt::terminal_color::bright_blue:
         style_string += "background-color: royalblue; ";
         break;
      case fmt::terminal_color::bright_magenta:
         style_string += "background-color: magenta; ";
         break;
      case fmt::terminal_color::bright_cyan:
         style_string += "background-color: cyan; ";
         break;
      case fmt::terminal_color::bright_white:
         style_string += "background-color: white; ";
         break;
      }
   }

   style_string += "\">\n";
   Write(style_string);
}

/// Remove formatting, add a new line, add a timestamp and tabulate           
///   @attention top of the style stack is not applied                        
void ToHTML::NewLine() const noexcept {
   Write("<br>");
   Write(Instance.TimeStampStyle);
   Write(GetSimpleTime());
   Write("|");
   if (Instance.CurrentIntent != Intent::Ignore)
      Write(Instance.IntentStyle[int(Instance.CurrentIntent)].prefix);
   else
      Write(" ");
   Write("| ");

   auto tabs = Instance.GetTabs();
   if (tabs) {
      Write(Instance.TabStyle);
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
   Write("<!DOCTYPE html><html>\n");
   Write("<body style = \"color: LightGray; background-color: black; font-family: monospace; font-size: 14px;\">\n");
   Write("<h2>Log started - ");
   Write(GetAdvancedTime());
   Write("</h2><code>\n");
}

/// Write file footer - just the official shutdown timestamp                  
void ToHTML::WriteFooter() const {
   Write("</strong></em></u></blink></del></span><h2>Log ended - ");
   Write(GetAdvancedTime());
   Write("</h2></code></body></html>");
}
