///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Logger.hpp"


namespace Langulus::Logger
{

   /// Scoped tabulator destruction                                           
   LANGULUS(INLINED)
   ScopedTabs::~ScopedTabs() noexcept {
      while (mTabs > 0) {
         --mTabs;
         Instance.RunCommand(Command::Untab);
      }
   }

   /// Analyzes text returned by LANGULUS(FUNCTION) in order to isolate the   
   /// name part for logging                                                  
   ///   @param text - the text to scan                                       
   ///   @return the interesting part                                         
   constexpr TextView A::Interface::GetFunctionName(
      const TextView& text,
      const TextView& omit
   ) noexcept {
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
            if (scopes == 0 and not anticipatingName) {
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
            if (anticipatingName and templates == 0) {
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
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (A::Interface&) noexcept {
      return *this;
   }

   /// Push a command                                                         
   ///   @param c - the command to push                                       
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Command& c) noexcept {
      Instance.RunCommand(c);
      return *this;
   }

   /// Push a foreground color                                                
   ///   @param c - the command to push                                       
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Color& c) noexcept {
      Instance.SetColor(c);
      return *this;
   }

   /// Push an emphasis                                                       
   ///   @param e - the emphasis to push                                      
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Emphasis& e) noexcept {
      Instance.SetEmphasis(e);
      return *this;
   }

   /// Push a foreground and background color                                 
   ///   @param c - the state to push                                         
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Style& c) noexcept {
      Instance.SetStyle(c);
      return *this;
   }

   /// Push a number of tabs                                                  
   ///   @param t - the tabs to push                                          
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Tabs& t) noexcept {
      auto tabs = ::std::max(1, t.mTabs);
      while (tabs) {
         Instance.RunCommand(Command::Tab);
         --tabs;
      }

      return *this;
   }

   /// Write string views                                                     
   ///   @param t - text to write                                             
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const TextView& t) noexcept {
      Instance.Write(t);
      return *this;
   }

   /// Write a nullptr as "null"                                              
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const ::std::nullptr_t&) noexcept {
      Instance.Write("null");
      return *this;
   }

   /// Dereference anything sparse, and route it through the logger again     
   ///   @param anything - pointer to stringify                               
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const CT::Sparse auto& sparse) noexcept {
      using T = Deref<decltype(sparse)>;
      if constexpr (CT::String<T>) {
         Instance.Write(sparse);
         return *this;
      }
      else {
         using DT = Deptr<T>;
         static_assert(CT::Sparse<DT> or ::Langulus::Logger::Formattable<DT>,
            "Dereferenced pointer is not Formattable, you have to declare "
            "a (dense) fmt::formatter for it");
         if (sparse == nullptr)
            return operator << (nullptr);
         else
            return operator << (*sparse);
      }
   }

   /// Stringify anything that has a valid fmt formatter                      
   ///   @param anything - type to stringify                                  
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const ::Langulus::Logger::Formattable auto& anything) noexcept {
      Instance.Write(fmt::format("{}", anything));
      return *this;
   }

   /// Push a number of tabs                                                  
   /// Keeps track of the number of tabs that have been pushed, and then      
   /// automatically untabs when the Tabs object is destroyed                 
   ///   @param t - [in/out] the tabs to push                                 
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   ScopedTabs A::Interface::operator << (Tabs&& t) noexcept {
      auto tabs = ::std::max(1, t.mTabs);
      while (tabs) {
         Instance.RunCommand(Command::Tab);
         --tabs;
      }

      ++t.mTabs;
      return ScopedTabs {t.mTabs};
   }

   /// A general new-line write function with color                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
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
   template<class...T> LANGULUS(INLINED)
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
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Section(T&&...arguments) noexcept {
      if constexpr (sizeof...(arguments) > 0) {
         Instance.NewLine();
         Instance << Color::White << Emphasis::Bold;
         (Instance << ... << ::std::forward<T>(arguments));
         return (Instance << Tabs {});
      }
      else return (Instance);
   }

   /// Write a new-line fatal error                                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Fatal([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FATALERRORS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::DarkRed;
            Instance << "FATAL ERROR: ";
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line error                                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Error([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_ERRORS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Red;
            Instance << "ERROR: ";
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line warning                                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Warning([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_WARNINGS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::DarkYellow;
            Instance << "WARNING: ";
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with verbose information                              
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Verbose([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_VERBOSE
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::DarkGray;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with information                                      
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Info([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INFOS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Gray;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with a personal message                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Message([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_MESSAGES
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::White;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with special text                                     
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Special([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_SPECIALS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Purple;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with flow information                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Flow([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FLOWS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::DarkCyan;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line on user input                                         
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Input([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INPUTS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Blue;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with network message                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Network([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_NETWORKS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Yellow;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with a message from OS                                
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) OS([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_OS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::DarkBlue;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

   /// Write a new-line with an input prompt                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Prompt([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_PROMPTS
         if constexpr (sizeof...(arguments) > 0) {
            Instance.NewLine();
            Instance << Color::Green;
            return (Instance << ... << ::std::forward<T>(arguments));
         }
         else return (Instance);
      #else
         return (Instance);
      #endif
   }

} // namespace Langulus::Logger
