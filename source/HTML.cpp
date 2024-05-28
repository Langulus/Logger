#include "Logger.hpp"
#include <string>

using namespace Langulus;
using namespace Langulus::Logger;


/// Create an HTML file duplicator/redirector                                 
///   @param filename - the relative filename of the log file                 
ToHTML::ToHTML(const std::string& filename) {
   mFile.open(filename, std::ios::out | std::ios::app | std::ios::ate);
   if (not mFile)
      throw std::runtime_error {"Can't open log file"};
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
   Write("</strong></em></u></blink></del></span>");

   if (style.has_emphasis()) {
      const auto em = static_cast<uint8_t>(style.get_emphasis());
      if (em & static_cast<uint8_t>(fmt::emphasis::bold))
         Write("<strong>");
      if (em & static_cast<uint8_t>(fmt::emphasis::faint))
         ;
      if (em & static_cast<uint8_t>(fmt::emphasis::italic))
         Write("<em>");
      if (em & static_cast<uint8_t>(fmt::emphasis::underline))
         Write("<u>");
      if (em & static_cast<uint8_t>(fmt::emphasis::blink))
         Write("<blink>");
      if (em & static_cast<uint8_t>(fmt::emphasis::reverse))
         ;
      if (em & static_cast<uint8_t>(fmt::emphasis::conceal))
         ;
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
         Write("<span style = \"color: red;\">");
         break;
      case fmt::terminal_color::green:
         Write("<span style = \"color: green;\">");
         break;
      case fmt::terminal_color::yellow:
         Write("<span style = \"color: yellow;\">");
         break;
      case fmt::terminal_color::blue:
         Write("<span style = \"color: blue;\">");
         break;
      case fmt::terminal_color::magenta:
         Write("<span style = \"color: magenta;\">");
         break;
      case fmt::terminal_color::cyan:
         Write("<span style = \"color: cyan;\">");
         break;
      case fmt::terminal_color::white:
         Write("<span style = \"color: gray;\">");
         break;
      case fmt::terminal_color::bright_black:
         Write("<span style = \"color: dark gray;\">");
         break;
      case fmt::terminal_color::bright_red:
         Write("<span style = \"color: bright red;\">");
         break;
      case fmt::terminal_color::bright_green:
         Write("<span style = \"color: bright green;\">");
         break;
      case fmt::terminal_color::bright_yellow:
         Write("<span style = \"color: bright yellow;\">");
         break;
      case fmt::terminal_color::bright_blue:
         Write("<span style = \"color: bright blue;\">");
         break;
      case fmt::terminal_color::bright_magenta:
         Write("<span style = \"color: bright magenta;\">");
         break;
      case fmt::terminal_color::bright_cyan:
         Write("<span style = \"color: bright cyan;\">");
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
         Write("<span style = \"background-color: red;\">");
         break;
      case fmt::terminal_color::green:
         Write("<span style = \"background-color: green;\">");
         break;
      case fmt::terminal_color::yellow:
         Write("<span style = \"background-color: yellow;\">");
         break;
      case fmt::terminal_color::blue:
         Write("<span style = \"background-color: blue;\">");
         break;
      case fmt::terminal_color::magenta:
         Write("<span style = \"background-color: magenta;\">");
         break;
      case fmt::terminal_color::cyan:
         Write("<span style = \"background-color: cyan;\">");
         break;
      case fmt::terminal_color::white:
         Write("<span style = \"background-color: gray;\">");
         break;
      case fmt::terminal_color::bright_black:
         Write("<span style = \"background-color: dark gray;\">");
         break;
      case fmt::terminal_color::bright_red:
         Write("<span style = \"background-color: bright red;\">");
         break;
      case fmt::terminal_color::bright_green:
         Write("<span style = \"background-color: bright green;\">");
         break;
      case fmt::terminal_color::bright_yellow:
         Write("<span style = \"background-color: bright yellow;\">");
         break;
      case fmt::terminal_color::bright_blue:
         Write("<span style = \"background-color: bright blue;\">");
         break;
      case fmt::terminal_color::bright_magenta:
         Write("<span style = \"background-color: bright magenta;\">");
         break;
      case fmt::terminal_color::bright_cyan:
         Write("<span style = \"background-color: bright cyan;\">");
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