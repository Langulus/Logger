///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Logger.hpp"


namespace Langulus::Logger
{

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

   /// Dereference anything sparse, and route it through the logger again     
   ///   @param anything - pointer to stringify                               
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const CT::Sparse auto& sparse) noexcept {
      using T = Deref<decltype(sparse)>;
      if constexpr (CT::String<T>)
         return operator << (TextView {sparse});
      else {
         using DT = Deptr<T>;
         static_assert(CT::Sparse<DT> or Formattable<DT>,
            "Dereferenced pointer is not Formattable, you have to declare "
            "a (dense) fmt::formatter for it");
         if (sparse == nullptr)
            return operator << (nullptr);
         else
            return operator << (*sparse);
      }
   }

   /// Log any character array                                                
   template<class T, size_t N> LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const ::std::array<T, N>& a) noexcept {
      return operator << (TextView {a.data(), N});
   }

   /// Stringify anything that has a valid fmt formatter                      
   ///   @param anything - type to stringify                                  
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const Formattable auto& anything) noexcept {
      const auto formatted = fmt::format("{}", anything);
      return operator << (TextView {formatted});
   }
   
   /// Stringify char8_t                                                      
   ///   @param c - char                                                      
   ///   @return a reference to the logger for chaining                       
   LANGULUS(INLINED)
   A::Interface& A::Interface::operator << (const char8_t& c) noexcept {
      const auto formatted = fmt::format("{}", reinterpret_cast<const char&>(c));
      return operator << (TextView {formatted});
   }

   /// A general new-line write function that continues the last intent/style 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Line(T&&...arguments) noexcept {
      Instance.NewLine();

      if constexpr (sizeof...(arguments) > 0)
         return (Instance << ... << ::std::forward<T>(arguments));
      else
         return (Instance);
   }

   /// A general same-line write function that continues the last style/intent
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
   /// scope's end. Section color is context dependent on the current style   
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab                                                 
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Section(T&&...arguments) noexcept {
      if constexpr (sizeof...(arguments) > 0) {
         const auto currentStyle = Instance.GetCurrentStyle();
         Instance.NewLine();
         Instance << Command::Push
                  << Instance.TabStyle << "┌─ "
                  << currentStyle
                  << Command::Pop;
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
         Instance << Intent::FatalError;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line fatal error and tab all next lines                    
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs FatalTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FATALERRORS
         Fatal(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line error                                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Error([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_ERRORS
         Instance << Intent::Error;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line error and tab all next lines                          
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs ErrorTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_ERRORS
         Error(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line warning                                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Warning([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_WARNINGS
         Instance << Intent::Warning;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line warning and tab all next lines                        
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs WarningTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_WARNINGS
         Warning(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with verbose information                              
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Verbose([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_VERBOSE
         Instance << Intent::Verbose;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line vernose and tab all next lines                        
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs VerboseTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_VERBOSE
         Verbose(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with information                                      
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Info([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INFOS
         Instance << Intent::Info;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line info and tab all next lines                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs InfoTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INFOS
         Info(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with a personal message                               
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Message([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_MESSAGES
         Instance << Intent::Message;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line message and tab all next lines                        
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs MessageTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_MESSAGES
         Message(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with special text                                     
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Special([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_SPECIALS
         Instance << Intent::Special;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line special and tab all next lines                        
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs SpecialTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_SPECIALS
         Special(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with flow information                                 
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Flow([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FLOWS
         Instance << Intent::Flow;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line flow and tab all next lines                           
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs FlowTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_FLOWS
         Flow(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line on user input                                         
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Input([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INPUTS
         Instance << Intent::Input;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line input and tab all next lines                          
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs InputTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_INPUTS
         Input(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with network message                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Network([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_NETWORKS
         Instance << Intent::Network;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line network and tab all next lines                        
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs NetworkTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_NETWORKS
         Network(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with a message from OS                                
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) OS([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_OS
         Instance << Intent::OS;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }

   /// Write a new-line OS and tab all next lines                             
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs OSTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_OS
         OS(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

   /// Write a new-line with an input prompt                                  
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a reference to the logger for chaining                       
   template<class...T> LANGULUS(INLINED)
   decltype(auto) Prompt([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_PROMPTS
         Instance << Intent::Prompt;
         Instance.NewLine();

         if constexpr (sizeof...(arguments) > 0)
            return (Instance << ... << ::std::forward<T>(arguments));
         else
            return (Instance);
      #else
         Instance << Intent::Ignore;
         return (Instance);
      #endif
   }
   
   /// Write a new-line prompt and tab all next lines                         
   ///   @tparam ...T - a sequence of elements to log (deducible)             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   ScopedTabs PromptTab([[maybe_unused]] T&&...arguments) noexcept {
      #ifdef LANGULUS_LOGGER_ENABLE_PROMPTS
         Prompt(::std::forward<T>(arguments)...);
         return (Instance << Tabs {});
      #else
         Instance << Intent::Ignore;
         return ScopedTabs {0};
      #endif
   }

} // namespace Langulus::Logger
