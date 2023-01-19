///                                                                           
/// Langulus::Logger                                                          
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Logger.hpp"

namespace Langulus::Logger
{

   /// Scoped tabulator destruction                                           
   LANGULUS(ALWAYSINLINE)
   ScopedTabs::~ScopedTabs() noexcept {
      while (mTabs > 0) {
         --mTabs;
         Instance << Command::Untab;
      }
   }

   /// Analyzes text returned by LANGULUS(FUNCTION) in order to isolate the   
   /// name part for logging                                                  
   ///   @param text - the text to scan                                       
   ///   @return the interesting part                                         
   constexpr TextView A::Interface::GetFunctionName(const TextView& text, const TextView& omit) noexcept {
      size_t length = text.size();
      size_t start = 0;
      size_t end = 0;
      size_t scopes = 0;
      size_t templates = 0;
      bool anticipatingName = false;
      while (length > 0) {
         bool done = false;
         switch (text[length - 1]) {
         case ')':
            ++scopes;
            start = end = 0;
            break;
         case '(':
            --scopes;
            if (scopes == 0 && !anticipatingName) {
               start = end = length - 1;
               anticipatingName = true;
            }
            break;
         case '>':
            ++templates;
            break;
         case '<':
            --templates;
            break;
         case ' ': case '\t':
            if (anticipatingName && templates == 0) {
               start = length;
               done = true;
            }
            break;
         }

         if (done)
            break;

         --length;
      }

      const auto nodecorations = text.substr(start, end - start);
      if (nodecorations.starts_with(omit))
         return nodecorations.substr(omit.size());

      return nodecorations;
   }

   /// Does nothing, but allows for grouping logging statements in ()         
   /// Example: Logger::Error() << "yeah" << (Logger::Info() << "no")         
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (A::Interface&) noexcept {
      return *this;
   }

   /// Push a command                                                         
   ///   @param c - the command to push                                       
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Command& c) noexcept {
      Write(c);
      return *this;
   }

   /// Push a foreground color                                                
   ///   @param c - the command to push                                       
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Color& c) noexcept {
      Write(c);
      return *this;
   }

   /// Push an emphasis                                                       
   ///   @param e - the emphasis to push                                      
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Emphasis& e) noexcept {
      Write(e);
      return *this;
   }

   /// Push a foreground and background color                                 
   ///   @param c - the state to push                                         
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Style& c) noexcept {
      Write(c);
      return *this;
   }

   /// Push a number of tabs                                                  
   ///   @param t - the tabs to push                                          
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Tabs& t) noexcept {
      auto tabs = ::std::max(1, t.mTabs);
      while (tabs) {
         Write(Command::Tab);
         --tabs;
      }

      return *this;
   }

   /// Write string views                                                     
   ///   @param t - text to write                                             
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const TextView& t) noexcept {
      Write(t);
      return *this;
   }

   /// Write a nullptr as "null"                                              
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const nullptr_t&) noexcept {
      Write("null");
      return *this;
   }

   /// Dereference anything sparse, and route it through the logger again     
   ///   @param anything - type type to stringify                             
   ///   @return a reference to the logger for chaining                       
   template<CT::Sparse T>
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const T& sparse) noexcept {
      if constexpr (CT::BuiltinCharacter<T>) {
         Write(sparse);
         return *this;
      }
      else {
         using DT = Deptr<T>;
         static_assert(CT::Sparse<DT> || Formattable<DT>,
            "Dereferenced pointer is not Formattable, you have to declare "
            "a (dense) fmt::formatter for it");
         if (sparse == nullptr)
            return operator << (nullptr);
         else
            return operator << (*sparse);
      }
   }

   /// Stringify anything that has a valid std::formatter                     
   ///   @param anything - type type to stringify                             
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   A::Interface& A::Interface::operator << (const Formattable auto& anything) noexcept {
      Write(fmt::format("{}", anything));
      return *this;
   }

   /// Push a number of tabs                                                  
   /// Keeps track of the number of tabs that have been pushed, and then      
   /// automatically untabs when the Tabs object is destroyed                 
   ///   @param t - [in/out] the tabs to push                                 
   ///   @return a reference to the logger for chaining                       
   LANGULUS(ALWAYSINLINE)
   ScopedTabs A::Interface::operator << (Tabs&& t) noexcept {
      auto tabs = ::std::max(1, t.mTabs);
      while (tabs) {
         Write(Command::Tab);
         --tabs;
      }

      ++t.mTabs;
      return ScopedTabs {t.mTabs};
   }

   /// A general new-line write function with color                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Line(T&&...arguments) noexcept {
      if constexpr (sizeof...(arguments) > 0) {
         Instance.NewLine();
         return (Instance << ... << ::std::forward<T>(arguments));
      }
      else return (Instance);
   }

   /// A general same-line write function with color                          
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Append(T&&...arguments) noexcept {
      if constexpr (sizeof...(arguments) > 0)
         (Instance << ... << ::std::forward<T>(arguments));
      return (Instance);
   }

   /// Write a section on a new line, tab all consecutive lines, bold it,     
   /// and return the scoped tabs, that will be	untabbed automatically at the 
   /// scope's end                                                            
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab                                                 
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   ScopedTabs Section(T&&...arguments) noexcept {
      return Line(Color::White, Emphasis::Bold, ::std::forward<T>(arguments)..., Tabs {});
   }

   /// Write a new-line fatal error                                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Fatal([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FATALERRORS
         return Line(Color::DarkRed, "FATAL ERROR: ", ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line error                                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Error([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_ERRORS
         return Line(Color::Red, "ERROR: ", ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line warning                                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Warning([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_WARNINGS
         return Line(Color::DarkYellow, "WARNING: ", ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with verbose information                              
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Verbose([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_VERBOSE
         return Line(Color::DarkGray, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with information                                      
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Info([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INFOS
         return Line(Color::Gray, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with a personal message                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Message([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_MESSAGES
         return Line(Color::White, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with special text                                     
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   decltype(auto) Special([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_SPECIALS
         return Line(Color::Purple, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with flow information                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Flow([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FLOWS
         return Line(Color::DarkCyan, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line on user input                                         
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Input([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INPUTS
         return Line(Color::Blue, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with network message                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Network([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_NETWORKS
         return Line(Color::Yellow, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with a message from OS                                
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) OS([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_OS
         return Line(Color::DarkBlue, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with an input prompt                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class... T>
   LANGULUS(ALWAYSINLINE)
   decltype(auto) Prompt([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_PROMPTS
         return Line(Color::Green, ::std::forward<T>(arguments)...);
      #else
         return (Instance);
      #endif
   }

} // namespace Langulus::Logger
