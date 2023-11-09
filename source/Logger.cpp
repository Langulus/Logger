///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
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
   
   /// When using fmt::print(style, mask, ...), the style will be reset after 
   /// message has been written, and I don't want that to happen              
   ///   @param style - the style to set                                      
   LANGULUS(INLINED)
   void FmtPrintStyle(const Style& style) {
      // Always reset before a style change                             
      fmt::print("{}", "\x1b[0m");
      
      if (style.has_emphasis()) {
         const auto emphasis = fmt::detail::make_emphasis<Letter>(style.get_emphasis());
         fmt::print("{}", emphasis.begin());
      }

      if (style.has_foreground()) {
         const auto foreground = fmt::detail::make_foreground_color<Letter>(style.get_foreground());
         fmt::print("{}", foreground.begin());
      }

      if (style.has_background()) {
         const auto background = fmt::detail::make_background_color<Letter>(style.get_background());
         fmt::print("{}", background.begin());
      }
   }

   /// The global logger instance                                             
   Interface Instance {};

   /// Logger construction                                                    
   Interface::Interface() {
      mStyleStack.push(DefaultStyle);
   }

   /// Generate an exhaustive timestamp in the current system time zone       
   ///   @return the timestamp text as {:%F %T %Z}                            
   Text A::Interface::GetAdvancedTime() noexcept {
      try {
         const auto now = Clock::to_time_t(Clock::now());
         return fmt::format("{:%F %T %Z}", fmt::localtime(now));
      }
      catch (...) {
         return "<time error>";
      }
   }

   /// Generate a short timestamp in the current system time zone             
   ///   @return the timestamp text as {:%T}                                  
   Text A::Interface::GetSimpleTime() noexcept {
      try {
         const auto now = Clock::to_time_t(Clock::now());
         return fmt::format("{:%T}", fmt::localtime(now));
      }
      catch (...) {
         return "<time error>";
      }
   }

   /// Write a string view to stdout                                          
   ///   @param stdString - the text view to write                            
   void Interface::Write(const TextView& stdString) const noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Write(stdString);

         // The presence of a redirector blocks console printing        
         return;
      }

      try {
         fmt::print("{}", stdString);
      }
      catch (...) {
         Logger::Append("<logger error>");
      }

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->Write(stdString);
   }

   /// Change the foreground/background color                                 
   ///   @param c - the color                                                 
   void Interface::Write(const Color& c) noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Write(c);

         // The presence of a redirector blocks console printing        
         return;
      }

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
      else if ((c >= Color::Black    and c < Color::BlackBgr) 
           or  (c >= Color::DarkGray and c < Color::DarkGrayBgr)) {
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

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->Write(c);
   }

   /// Change the emphasis                                                    
   ///   @param e - the emphasis                                              
   void Interface::Write(const Emphasis& e) noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Write(e);

         // The presence of a redirector blocks console printing        
         return;
      }

      auto& style = mStyleStack.top();
      style |= static_cast<fmt::emphasis>(e);
      FmtPrintStyle(style);

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->Write(e);
   }

   /// Change the style                                                       
   ///   @param s - the style                                                 
   void Interface::Write(const Style& s) noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Write(s);

         // The presence of a redirector blocks console printing        
         return;
      }

      auto& style = mStyleStack.top();
      style = s;
      FmtPrintStyle(style);

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->Write(s);
   }

   /// Execute a logger command                                               
   ///   @param c - the command to execute                                    
   void Interface::Write(const Command& c) noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Write(c);

         // The presence of a redirector blocks console printing        
         return;
      }

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
         while (not mStyleStack.empty())
            mStyleStack.pop();
         mStyleStack.push(DefaultStyle);
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

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->Write(c);
   }

   /// Remove formatting, add a new line, add a timestamp and tabulate        
   void Interface::NewLine() const noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->NewLine();

         // The presence of a redirector blocks console printing        
         return;
      }

      // Clear formatting, add new line, simple time stamp, and tabs    
      FmtPrintStyle(TimeStampStyle);
      fmt::print("\n{}| ", GetSimpleTime());
      Tabulate();

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->NewLine();
   }

   /// Insert current tabs and apply last style from the stack                
   void Interface::Tabulate() const noexcept {
      // Dispatch to redirectors                                        
      if (not mRedirectors.empty()) {
         for (auto attachment : mRedirectors)
            attachment->Tabulate();

         // The presence of a redirector blocks console printing        
         return;
      }

      if (mTabulator) {
         auto tabs = mTabulator;
         FmtPrintStyle(TabStyle);
         while (tabs) {
            Write(TabString);
            --tabs;
         }
      }

      // Dispatch to duplicators                                        
      for (auto attachment : mDuplicators)
         attachment->NewLine();
   }

   /// Attach another logger, if no redirectors are attached, any logging     
   /// will be duplicated to the provided interface                           
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param duplicator - the logger to attach                             
   void Interface::AttachDuplicator(A::Interface* duplicator) noexcept {
      mDuplicators.push_back(duplicator);
   }

   /// Dettach a duplicator                                                   
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param duplicator - the duplicator to dettach                        
   void Interface::DettachDuplicator(A::Interface* duplicator) noexcept {
      mDuplicators.remove(duplicator);
   }

   /// Attach another logger, that will receive any logging, but also consume 
   /// it, so that it doesn't reach the console or any attached duplicators   
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param redirector - the logger to attach                             
   void Interface::AttachRedirector(A::Interface* redirector) noexcept {
      mRedirectors.push_back(redirector);
   }

   /// Dettach a redirector                                                   
   ///   @attention the logger doesn't have ownership of the attachment       
   ///   @param redirector - the duplicator to dettach                        
   void Interface::DettachRedirector(A::Interface* redirector) noexcept {
      mRedirectors.remove(redirector);
   }

} // namespace Langulus::Logger
