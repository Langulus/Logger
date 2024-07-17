///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include "Logger.hpp"
#include <type_traits>
#include <syncstream>
#include <chrono>
#include <fmt/chrono.h>


using Clock = ::std::chrono::system_clock;

namespace Langulus::Logger
{

   Interface Instance {};
   MessageSink MessageSinkInstance {};

   void AttachDuplicator(A::Interface* d) noexcept {
      Instance.AttachDuplicator(d);
   }

   void DettachDuplicator(A::Interface* d) noexcept {
      Instance.DettachDuplicator(d);
   }

   void AttachRedirector(A::Interface* r) noexcept {
      Instance.AttachRedirector(r);
   }

   void DettachRedirector(A::Interface* r) noexcept {
      Instance.DettachRedirector(r);
   }

} // namespace Langulus::Logger

using namespace Langulus;
using namespace Langulus::Logger;

   
/// When using fmt::print(style, mask, ...), the style will be reset after    
/// message has been written, and I don't want that to happen                 
///   @param style - the style to set                                         
LANGULUS(INLINED)
void FmtPrintStyle(const Style& style) {
   // Always reset before a style change                                
   fmt::print("{}", "\x1b[0m");
      
   if (style.has_emphasis()) {
      const auto e = fmt::detail::make_emphasis<Letter>(style.get_emphasis());
      fmt::print("{}", e.begin());
   }

   if (style.has_foreground()) {
      const auto f = fmt::detail::make_foreground_color<Letter>(style.get_foreground());
      fmt::print("{}", f.begin());
   }

   if (style.has_background()) {
      const auto b = fmt::detail::make_background_color<Letter>(style.get_background());
      fmt::print("{}", b.begin());
   }
}

/// Scoped tabulator destruction                                              
ScopedTabs::~ScopedTabs() noexcept {
   while (mTabs > 0) {
      --mTabs;
      Instance.RunCommand(Command::Untab);
   }
}

/// Logger construction                                                       
Interface::Interface() {
   mStyleStack.push(DefaultStyle);
}

/// Logger copy-construction                                                  
Interface::Interface(const Interface& other)
   : mStyleStack {other.mStyleStack} {}

/// Logger destruction                                                        
Interface::~Interface() {}

/// Generate an exhaustive timestamp in the current system time zone          
///   @return the timestamp text as {:%F %T %Z}                               
Text Logger::A::Interface::GetAdvancedTime() noexcept {
   try {
      const auto now = Clock::to_time_t(Clock::now());
      return fmt::format("{:%F %T %Z}", fmt::localtime(now));
   }
   catch (...) { return "<advanced time error>"; }
}

/// Generate a short timestamp in the current system time zone                
///   @return the timestamp text as {:%T}                                     
Text Logger::A::Interface::GetSimpleTime() noexcept {
   try {
      const auto now = Clock::to_time_t(Clock::now());
      return fmt::format("{:%T}", fmt::localtime(now));
   }
   catch (...) { return "<simple time error>"; }
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

   try { fmt::print("{}", stdString); }
   catch (...) { Logger::Append("<logger error>"); }

   // Dispatch to duplicators                                           
   for (auto attachment : mDuplicators)
      attachment->Write(stdString);
}

/// Change the style                                                          
///   @param s - the style                                                    
void Interface::Write(Style s) const noexcept {
   // Dispatch to redirectors                                           
   if (not mRedirectors.empty()) {
      for (auto attachment : mRedirectors)
         attachment->Write(s);

      // The presence of a redirector blocks console printing           
      return;
   }

   FmtPrintStyle(s);

   // Dispatch to duplicators                                           
   for (auto attachment : mDuplicators)
      attachment->Write(s);
}

/// Add a new line, tabulating properly, but continuing the previous style    
void Interface::NewLine() const noexcept {
   // Dispatch to redirectors                                           
   if (not mRedirectors.empty()) {
      for (auto attachment : mRedirectors)
         attachment->NewLine();

      // The presence of a redirector blocks console printing           
      return;
   }

   // Clear formatting, add new line, simple time stamp, and tabs       
   fmt::print("\n");
   FmtPrintStyle(TimeStampStyle);
   fmt::print("{}| ", GetSimpleTime());

   if (mTabulator) {
      auto tabs = mTabulator;
      FmtPrintStyle(TabStyle);
      while (tabs) {
         fmt::print("{}", TabString);
         --tabs;
      }
   }

   FmtPrintStyle(mStyleStack.top());

   // Dispatch to duplicators                                           
   for (auto attachment : mDuplicators) {
      attachment->NewLine();
      attachment->Write(mStyleStack.top());
   }
}

/// Clear the entire log (clear the console window or file)                   
void Interface::Clear() const noexcept {
   // Dispatch to redirectors                                           
   if (not mRedirectors.empty()) {
      for (auto attachment : mRedirectors)
         attachment->Clear();

      // The presence of a redirector blocks console printing           
      return;
   }

   // Clear the window                                                  
   fmt::print("{}", "\x1b[2J");

   // Dispatch to duplicators                                           
   for (auto attachment : mDuplicators)
      attachment->Clear();
}

/// Execute a logger command                                                  
///   @param c - the command to execute                                       
void Interface::RunCommand(Command c) noexcept {
   switch (c) {
   case Command::Clear:
      Clear();
      break;
   case Command::NewLine:
      NewLine();
      break;
   case Command::Invert:
      SetEmphasis(Emphasis::Reverse);
      Write(mStyleStack.top());
      break;
   case Command::Reset:
      while (not mStyleStack.empty())
         mStyleStack.pop();
      mStyleStack.push(DefaultStyle);
      Write(mStyleStack.top());
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
      Write(mStyleStack.top());
      break;
   case Command::Push:
      mStyleStack.push(mStyleStack.top());
      break;
   case Command::PopAndPush:
      if (mStyleStack.size() > 1)
         mStyleStack.pop();
      mStyleStack.push(mStyleStack.top());
      break;
   case Command::Stylize:
      Write(mStyleStack.top());
      break;
   case Command::Tab:
      ++mTabulator;
      break;
   case Command::Untab:
      if (mTabulator > 0)
         --mTabulator;
      break;
   }
}
   
/// Change the foreground/background color                                    
///   @param c_with_flags - the color with optional mixing flags              
///   @return the last style, with coloring applied                           
const Style& Interface::SetColor(Color c_with_flags) noexcept {
   if (static_cast<unsigned>(c_with_flags)
   &   static_cast<unsigned>(Color::PreviousColor)) {
      // We have to pop                                                 
      if (mStyleStack.size() > 1)
         mStyleStack.pop();
   }

   if (static_cast<unsigned>(c_with_flags)
   &   static_cast<unsigned>(Color::NextColor)) {
      // We have to push                                                
      mStyleStack.push(mStyleStack.top());
   }

   // Strip the mixing bits from the color                              
   const Color c = static_cast<Color>(
      static_cast<unsigned>(c_with_flags) & (~(
          static_cast<unsigned>(Color::PreviousColor)
        | static_cast<unsigned>(Color::NextColor)
      ))
   );

   // Mix...                                                            
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
   or       (c >= Color::DarkGray and c < Color::DarkGrayBgr)) {
      // Create a new foreground color style                            
      style = fmt::fg(static_cast<fmt::terminal_color>(c));
      if (oldStyle.has_background())
         style |= fmt::bg(oldStyle.get_background());
   }
   else {
      // Create a new background color style                            
      style = fmt::bg(static_cast<fmt::terminal_color>(static_cast<uint8_t>(c) - 10));
      if (oldStyle.has_foreground())
         style |= fmt::fg(oldStyle.get_foreground());
   }

   if (oldStyle.has_emphasis())
      style |= oldStyle.get_emphasis();
   return style;
}

/// Change the emphasis                                                       
///   @param c - the color                                                    
const Style& Interface::SetEmphasis(Emphasis e) noexcept {
   auto& style = mStyleStack.top();
   style |= static_cast<fmt::emphasis>(e);
   return style;
}

/// Change the style                                                          
///   @param s - the style                                                    
const Style& Interface::SetStyle(Style s) noexcept {
   mStyleStack.top() = s;
   return mStyleStack.top();
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

/// Does nothing, but allows for grouping logging statements in ()            
/// Example: Logger::Error() << "yeah" << (Logger::Info() << "no")            
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (Logger::A::Interface&) noexcept {
   return *this;
}

/// Push a command                                                            
///   @param c - the command to push                                          
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (Command c) noexcept {
   Instance.RunCommand(c);
   return *this;
}

/// Push a foreground color                                                   
///   @param c - the command to push                                          
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (Color c) noexcept {
   Instance.SetColor(c);
   Instance.RunCommand(Command::Stylize);
   return *this;
}

/// Push an emphasis                                                          
///   @param e - the emphasis to push                                         
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (Emphasis e) noexcept {
   Instance.SetEmphasis(e);
   Instance.RunCommand(Command::Stylize);
   return *this;
}

/// Push a foreground and background color                                    
///   @param c - the state to push                                            
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (Style c) noexcept {
   Instance.SetStyle(c);
   Instance.RunCommand(Command::Stylize);
   return *this;
}

/// Write string views                                                        
///   @param t - text to write                                                
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (const TextView& t) noexcept {
   Instance.Write(t);
   return *this;
}

/// Write a nullptr as "null"                                                 
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (::std::nullptr_t) noexcept {
   Instance.Write("null");
   return *this;
}

/// Push a number of tabs                                                     
///   @param t - the tabs to push                                             
///   @return a reference to the logger for chaining                          
Logger::A::Interface& Logger::A::Interface::operator << (const Tabs& t) noexcept {
   auto tabs = ::std::max(1, t.mTabs);
   while (tabs) {
      Instance.RunCommand(Command::Tab);
      --tabs;
   }

   return *this;
}

/// Push a number of tabs                                                     
/// Keeps track of the number of tabs that have been pushed, and then         
/// automatically untabs when the Tabs object is destroyed                    
///   @param t - [in/out] the tabs to push                                    
///   @return a reference to the logger for chaining                          
ScopedTabs Logger::A::Interface::operator << (Tabs&& t) noexcept {
   auto tabs = ::std::max(1, t.mTabs);
   while (tabs) {
      Instance.RunCommand(Command::Tab);
      --tabs;
   }

   ++t.mTabs;
   return ScopedTabs {t.mTabs};
}
