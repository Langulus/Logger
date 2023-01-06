///                                                                           
/// Langulus::Logger                                                          
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Logger.hpp"
#include <type_traits>
#include <syncstream>
#include <chrono>
#include <fmt/chrono.h>

using Clock = ::std::chrono::system_clock;

namespace Langulus::Logger
{

   /// The global handle shall point to the buffer                            
   Interface Instance {};

   /// Logger construction                                                    
   Interface::Interface() {
      mStyleStack.push(DefaultStyle);
   }

   /// Generate an exhaustive timestamp in the current system time zone       
   ///   @return the timestamp text                                           
   Text A::Interface::GetAdvancedTime() noexcept {
      try {
         return fmt::format("{:%F %T %Z}", fmt::localtime(Clock::to_time_t(Clock::now())));
      }
      catch (...) {
         return "<unable to extract time>";
      }
   }

   /// Generate a short timestamp in the current system time zone             
   ///   @return the timestamp text                                           
   Text A::Interface::GetSimpleTime() noexcept {
      try {
         return fmt::format("{:%T}", fmt::localtime(Clock::to_time_t(Clock::now())));
      }
      catch (...) {
         return "<unable to extract time>";
      }
   }

   /// Write a character to stdout                                            
   ///   @param character - the character to write                            
   void Interface::Write(const Letter& character) const noexcept {
      try {
         fmt::print("{}", character);
      }
      catch (...) {
         Logger::Error() << "Logger exception";
      }
   }

   /// Write a string view to stdout                                          
   ///   @param literalText - the text to write                               
   void Interface::Write(const TextView& stdString) const noexcept {
      try {
         fmt::print("{}", stdString);
      }
      catch (...) {
         Logger::Error() << "Logger exception";
      }
   }
   
   /// When using fmt::print(style, mask, ...), the style will be reset after 
   /// message has been written, and I don't want that to happen              
   ///   @param style - the style to set                                      
   LANGULUS(ALWAYSINLINE)
   void FmtPrintStyle(const Style& style) {
      // Always reset before a style change                             
      fmt::print("{}", "\x1b[0m");
      
      if (style.has_emphasis()) {
         auto emphasis = fmt::detail::make_emphasis<Letter>(style.get_emphasis());
         fmt::print("{}", emphasis);
      }

      if (style.has_foreground()) {
         auto foreground = fmt::detail::make_foreground_color<Letter>(style.get_foreground());
         fmt::print("{}", foreground);
      }

      if (style.has_background()) {
         auto background = fmt::detail::make_background_color<Letter>(style.get_background());
         fmt::print("{}", background);
      }
   }

   /// Change the foreground/background color                                 
   ///   @param c - the color                                                 
   void Interface::Write(const Color& c) noexcept {
      auto& style = mStyleStack.top();
      const auto oldStyle = style;
      if (c == Color::NoForeground) {
         // Reset the foreground color                                  
         style = {};
         if (oldStyle.has_background())
            style |= fmt::bg(oldStyle.get_background());
      }
      else if (c == Color::NoBackground) {
         // Reset the background color                                  
         style = {};
         if (oldStyle.has_foreground())
            style |= fmt::fg(oldStyle.get_foreground());
      }
      else if ((c >= Color::Black && c < Color::BlackBgr) || (c >= Color::DarkGray && c < Color::DarkGrayBgr)) {
         // Create a new foreground color style                         
         style = fmt::fg(static_cast<fmt::terminal_color>(c));
         if (oldStyle.has_background())
            style |= fmt::bg(oldStyle.get_background());
      }
      else {
         // Create a new background color style                         
         style = fmt::fg(static_cast<fmt::terminal_color>(c));
         if (oldStyle.has_background())
            style |= fmt::bg(oldStyle.get_background());
      }

      if (oldStyle.has_emphasis())
         style |= oldStyle.get_emphasis();
      FmtPrintStyle(style);
   }

   /// Change the emphasis                                                    
   ///   @param e - the emphasis                                              
   void Interface::Write(const Emphasis& e) noexcept {
      auto& style = mStyleStack.top();
      style |= static_cast<fmt::emphasis>(e);
      FmtPrintStyle(style);
   }

   /// Change the style                                                       
   ///   @param s - the style                                                 
   void Interface::Write(const Style& s) noexcept {
      auto& style = mStyleStack.top();
      style = s;
      FmtPrintStyle(style);
   }

   /// Execute a logger command                                               
   ///   @param c - the command to execute                                    
   void Interface::Write(const Command& c) noexcept {
      switch (c) {
      case Command::Clear:
         fmt::print("{}", "\x1b[2J");
         break;
      case Command::NewLine:
         NewLine();
         break;
      case Command::Invert:
         Write(Emphasis::Reverse);
         break;
      case Command::Reset:
         mStyleStack.top() = DefaultStyle;
         FmtPrintStyle(mStyleStack.top());
         break;
      case Command::Time:
         Write(GetSimpleTime());
         break;
      case Command::ExactTime:
         Write(GetAdvancedTime());
         break;
      case Command::Pop:
         if (mStyleStack.size() > 1)
            mStyleStack.pop();
         else
            mStyleStack.top() = DefaultStyle;
         FmtPrintStyle(mStyleStack.top());
         break;
      case Command::Push:
         mStyleStack.push(mStyleStack.top());
         break;
      case Command::Tab:
         ++mTabulator;
         break;
      case Command::Untab:
         if (mTabulator > 0)
            --mTabulator;
         break;
      }

      // Dispatch                                                       
      for (auto attachment : mAttachments)
         attachment->Write(c);
   }

   /// Remove formatting, add a new line, add a timestamp and tabulate        
   void Interface::NewLine() const noexcept {
      // Clear formatting, add new line, add a simple time stamp        
      FmtPrintStyle(TimeStampStyle);

      try {
         fmt::print("\n{:%T}| ", fmt::localtime(Clock::to_time_t(Clock::now())));
      }
      catch (...) {
         Logger::Error() << "Logger exception";
      }

      // Tabulate                                                       
      Tabulate();

      // Dispatch                                                       
      for (auto attachment : mAttachments)
         attachment->NewLine();
   }

   /// Insert current tabs and apply last style from the stack                
   void Interface::Tabulate() const noexcept {
      if (!mTabulator) {
         FmtPrintStyle(mStyleStack.top());
         return;
      }

      auto tabs = mTabulator;
      FmtPrintStyle(TabStyle);
      while (tabs) {
         Write(TabString);
         --tabs;
      }
      FmtPrintStyle(mStyleStack.top());
   }

   /// Attach another logger, such as an html file                            
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param receiver - the logger to attach                               
   void Interface::Attach(A::Interface* receiver) noexcept {
      mAttachments.push_back(receiver);
   }

   /// Dettach a logger                                                       
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param receiver - the logger to dettach                              
   void Interface::Dettach(A::Interface* receiver) noexcept {
      mAttachments.remove(receiver);
   }

} // namespace Langulus::Logger
