///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Core/Exceptions.hpp>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_LOGGER)
   #define LANGULUS_API_LOGGER() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_LOGGER() LANGULUS_IMPORT()
#endif

/// Make the rest of the code aware, that Langulus::Logger has been included  
#define LANGULUS_LIBRARY_LOGGER() 1

#include <stack>
#include <list>
#include <string_view>
#include <string>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fstream>


namespace Langulus::Logger
{

   /// Text container used by the logger                                      
   using Text     = ::std::basic_string<Letter>;
   using TextView = ::std::basic_string_view<Letter>;

   template<class...T>
   concept Formattable = CT::Dense<T...>
       and ((::fmt::is_formattable<Deref<T>>::value) and ...);

   /// Color codes, consistent with ANSI/VT100 escapes                        
   /// Also consistent with fmt::terminal_color                               
   enum class Color : unsigned {
      NoForeground = 0,
      NoBackground = 1,

      Black = 30,
      DarkRed,
      DarkGreen,
      DarkYellow,
      DarkBlue,
      DarkPurple,
      DarkCyan,
      Gray,

      BlackBgr = 40,
      DarkRedBgr,
      DarkGreenBgr,
      DarkYellowBgr,
      DarkBlueBgr,
      DarkPurpleBgr,
      DarkCyanBgr,
      GrayBgr,

      DarkGray = 90,
      Red,
      Green,
      Yellow,
      Blue,
      Purple,
      Cyan,
      White,

      DarkGrayBgr = 100,
      RedBgr,
      GreenBgr,
      YellowBgr,
      BlueBgr,
      PurpleBgr,
      CyanBgr,
      WhiteBgr,

      // Bits that dictate how to mix the colors                        
      // Color is always mixed with the currently set one, unless the   
      // 'PreviousColor' bit is on, in which case the style is popped   
      // before applying color                                          
      PreviousColor     = 128,

      // Color is always mixed with the currently set one, unless the   
      // 'NextColor' bit is on, in which case the style is pushed       
      // before applying color                                          
      NextColor         = 256,

      // Colors that mix with the previous color                        
      PopNoForeground   = NoForeground    | PreviousColor,
      PopNoBackground   = NoBackground    | PreviousColor,

      PopBlack          = Black           | PreviousColor,
      PopDarkRed        = DarkRed         | PreviousColor,
      PopDarkGreen      = DarkGreen       | PreviousColor,
      PopDarkYellow     = DarkYellow      | PreviousColor,
      PopDarkBlue       = DarkBlue        | PreviousColor,
      PopDarkPurple     = DarkPurple      | PreviousColor,
      PopDarkCyan       = DarkCyan        | PreviousColor,
      PopGray           = Gray            | PreviousColor,

      PopBlackBgr       = BlackBgr        | PreviousColor,
      PopDarkRedBgr     = DarkRedBgr      | PreviousColor,
      PopDarkGreenBgr   = DarkGreenBgr    | PreviousColor,
      PopDarkYellowBgr  = DarkYellowBgr   | PreviousColor,
      PopDarkBlueBgr    = DarkBlueBgr     | PreviousColor,
      PopDarkPurpleBgr  = DarkPurpleBgr   | PreviousColor,
      PopDarkCyanBgr    = DarkCyanBgr     | PreviousColor,
      PopGrayBgr        = GrayBgr         | PreviousColor,

      PopDarkGray       = DarkGray        | PreviousColor,
      PopRed            = Red             | PreviousColor,
      PopGreen          = Green           | PreviousColor,
      PopYellow         = Yellow          | PreviousColor,
      PopBlue           = Blue            | PreviousColor,
      PopPurple         = Purple          | PreviousColor,
      PopCyan           = Cyan            | PreviousColor,
      PopWhite          = White           | PreviousColor,

      PopDarkGrayBgr    = DarkGrayBgr     | PreviousColor,
      PopRedBgr         = RedBgr          | PreviousColor,
      PopGreenBgr       = GreenBgr        | PreviousColor,
      PopYellowBgr      = YellowBgr       | PreviousColor,
      PopBlueBgr        = BlueBgr         | PreviousColor,
      PopPurpleBgr      = PurpleBgr       | PreviousColor,
      PopCyanBgr        = CyanBgr         | PreviousColor,
      PopWhiteBgr       = WhiteBgr        | PreviousColor,

      // Colors that mix with the next color                            
      PushNoForeground  = NoForeground    | NextColor,
      PushNoBackground  = NoBackground    | NextColor,

      PushBlack         = Black           | NextColor,
      PushDarkRed       = DarkRed         | NextColor,
      PushDarkGreen     = DarkGreen       | NextColor,
      PushDarkYellow    = DarkYellow      | NextColor,
      PushDarkBlue      = DarkBlue        | NextColor,
      PushDarkPurple    = DarkPurple      | NextColor,
      PushDarkCyan      = DarkCyan        | NextColor,
      PushGray          = Gray            | NextColor,

      PushBlackBgr      = BlackBgr        | NextColor,
      PushDarkRedBgr    = DarkRedBgr      | NextColor,
      PushDarkGreenBgr  = DarkGreenBgr    | NextColor,
      PushDarkYellowBgr = DarkYellowBgr   | NextColor,
      PushDarkBlueBgr   = DarkBlueBgr     | NextColor,
      PushDarkPurpleBgr = DarkPurpleBgr   | NextColor,
      PushDarkCyanBgr   = DarkCyanBgr     | NextColor,
      PushGrayBgr       = GrayBgr         | NextColor,

      PushDarkGray      = DarkGray        | NextColor,
      PushRed           = Red             | NextColor,
      PushGreen         = Green           | NextColor,
      PushYellow        = Yellow          | NextColor,
      PushBlue          = Blue            | NextColor,
      PushPurple        = Purple          | NextColor,
      PushCyan          = Cyan            | NextColor,
      PushWhite         = White           | NextColor,

      PushDarkGrayBgr   = DarkGrayBgr     | NextColor,
      PushRedBgr        = RedBgr          | NextColor,
      PushGreenBgr      = GreenBgr        | NextColor,
      PushYellowBgr     = YellowBgr       | NextColor,
      PushBlueBgr       = BlueBgr         | NextColor,
      PushPurpleBgr     = PurpleBgr       | NextColor,
      PushCyanBgr       = CyanBgr         | NextColor,
      PushWhiteBgr      = WhiteBgr        | NextColor,

      // Colors that reset to previous color, push and mix              
      PopAndPushNoForeground  = NoForeground    | NextColor | PreviousColor,
      PopAndPushNoBackground  = NoBackground    | NextColor | PreviousColor,

      PopAndPushBlack         = Black           | NextColor | PreviousColor,
      PopAndPushDarkRed       = DarkRed         | NextColor | PreviousColor,
      PopAndPushDarkGreen     = DarkGreen       | NextColor | PreviousColor,
      PopAndPushDarkYellow    = DarkYellow      | NextColor | PreviousColor,
      PopAndPushDarkBlue      = DarkBlue        | NextColor | PreviousColor,
      PopAndPushDarkPurple    = DarkPurple      | NextColor | PreviousColor,
      PopAndPushDarkCyan      = DarkCyan        | NextColor | PreviousColor,
      PopAndPushGray          = Gray            | NextColor | PreviousColor,

      PopAndPushBlackBgr      = BlackBgr        | NextColor | PreviousColor,
      PopAndPushDarkRedBgr    = DarkRedBgr      | NextColor | PreviousColor,
      PopAndPushDarkGreenBgr  = DarkGreenBgr    | NextColor | PreviousColor,
      PopAndPushDarkYellowBgr = DarkYellowBgr   | NextColor | PreviousColor,
      PopAndPushDarkBlueBgr   = DarkBlueBgr     | NextColor | PreviousColor,
      PopAndPushDarkPurpleBgr = DarkPurpleBgr   | NextColor | PreviousColor,
      PopAndPushDarkCyanBgr   = DarkCyanBgr     | NextColor | PreviousColor,
      PopAndPushGrayBgr       = GrayBgr         | NextColor | PreviousColor,

      PopAndPushDarkGray      = DarkGray        | NextColor | PreviousColor,
      PopAndPushRed           = Red             | NextColor | PreviousColor,
      PopAndPushGreen         = Green           | NextColor | PreviousColor,
      PopAndPushYellow        = Yellow          | NextColor | PreviousColor,
      PopAndPushBlue          = Blue            | NextColor | PreviousColor,
      PopAndPushPurple        = Purple          | NextColor | PreviousColor,
      PopAndPushCyan          = Cyan            | NextColor | PreviousColor,
      PopAndPushWhite         = White           | NextColor | PreviousColor,

      PopAndPushDarkGrayBgr   = DarkGrayBgr     | NextColor | PreviousColor,
      PopAndPushRedBgr        = RedBgr          | NextColor | PreviousColor,
      PopAndPushGreenBgr      = GreenBgr        | NextColor | PreviousColor,
      PopAndPushYellowBgr     = YellowBgr       | NextColor | PreviousColor,
      PopAndPushBlueBgr       = BlueBgr         | NextColor | PreviousColor,
      PopAndPushPurpleBgr     = PurpleBgr       | NextColor | PreviousColor,
      PopAndPushCyanBgr       = CyanBgr         | NextColor | PreviousColor,
      PopAndPushWhiteBgr      = WhiteBgr        | NextColor | PreviousColor
   };

   /// GCC equates templates with enum types as their underlying type, so we  
   /// are forced to define these anums as enum class, and then do using enum 
   using enum Color;

   /// Some formatting styles, consistent with fmt::emphasis                  
   enum class Emphasis : ::std::underlying_type_t<fmt::emphasis> {
      Default     = 0,			
      Bold        = 1,			// Not working on windows                 
      Faint       = 1 << 1,	// Not working on windows                 
      Italic      = 1 << 2,	// Not working on windows                 
      Underline   = 1 << 3,
      Blink       = 1 << 4,	// Not working on windows                 
      Reverse     = 1 << 5,
      Conceal     = 1 << 6,	// Not working on windows                 
      Strike      = 1 << 7,	// Not working on windows                 
   };

   constexpr bool operator & (const Emphasis& lhs, const Emphasis& rhs) noexcept {
      using T = ::std::underlying_type_t<fmt::emphasis>;
      return (static_cast<T>(lhs) & static_cast<T>(rhs)) == static_cast<T>(rhs);
   }

   /// GCC equates templates with enum types as their underlying type, so we  
   /// are forced to define these anums as enum class, and then do using enum 
   using enum Emphasis;

   /// Text style, with background color, foreground color, and emphasis      
   using Style = fmt::text_style;

   /// Console commands                                                       
   enum class Command : uint8_t {
      Clear,		// Clear the console                                  
      NewLine,		// Write a new line, with a timestamp and tabulation  
      Pop,			// Pop the style, and apply previous style            
      Push,			// Push the current style                             
      PopAndPush, // Pop the style and push another, don't stylize yet  
      Invert,		// Inverts background and foreground colors           
      Reset,		// Reset the style                                    
      Stylize,    // Apply the last style                               
      Tab,			// Tab once on a new line after this command          
      Untab,		// Untab once, again on a new line after this command 
      Time,			// Write a short timestamp                            
      ExactTime,	// Write an exhaustive timestamp                      
   };

   /// GCC equates templates with enum types as their underlying type, so we  
   /// are forced to define these anums as enum class, and then do using enum 
   using enum Command;
   
   /// Types of predefined messages, each with its unique style and search    
   /// patterns.                                                              
   enum class Intent {
      FatalError = 0,
      Error,
      Warning,
      Verbose,
      Info,
      Message,
      Special,
      Flow,
      Input,
      Network,
      OS,
      Prompt,

      Counter,
      Ignore
   };

   /// Can be used to specify each intent's style and search patterns         
   struct IntentProperties {
      TextView prefix;
      Style style;
      bool  silenced = false;
   };

   /// Tabulation marker (can be pushed to log)                               
   struct Tabs {
      int mTabs = 0;

      constexpr Tabs() noexcept = default;
      constexpr Tabs(const Tabs&) noexcept = default;
      constexpr Tabs(Tabs&& other) noexcept
         : mTabs {other.mTabs} { other.mTabs = 0; }
      constexpr explicit Tabs(int tabs) noexcept
         : mTabs {tabs} {}
   };

   /// Scoped tabulation marker that restores tabbing when destroyed          
   struct ScopedTabs : Tabs {
      using Tabs::Tabs;
      constexpr ScopedTabs(ScopedTabs&& other) noexcept
         : Tabs {::std::forward<Tabs>(other)} {}
      LANGULUS_API(LOGGER) ~ScopedTabs() noexcept;
   };

   namespace A
   {

      ///                                                                     
      /// The abstract logger interface - override this to define attachments 
      ///                                                                     
      struct Interface {
         Interface& operator = (const Interface&) = delete;
         Interface& operator = (Interface&&) = delete;

         NOD() static constexpr TextView GetFunctionName(
            const TextView&,
            const TextView& omit = "Langulus::"
         ) noexcept;
         
         NOD() LANGULUS_API(LOGGER)
         static Text GetAdvancedTime() noexcept;
         NOD() LANGULUS_API(LOGGER)
         static Text GetSimpleTime() noexcept;

         virtual void Write(const TextView&) const noexcept = 0;
         virtual void Write(Style) const noexcept = 0;
         virtual void NewLine() const noexcept = 0;
         virtual void Clear() const noexcept = 0;

         /// Implicit bool operator in order to use log in 'if' statements    
         /// Example: if (condition && Logger::Info("stuff"))                 
         ///   @return true                                                   
         constexpr explicit operator bool() const noexcept {
            return true;
         }

         LANGULUS_API(LOGGER) Interface& operator << (A::Interface&) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (const TextView&) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (Command) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (Color) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (Emphasis) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (Style) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (::std::nullptr_t) noexcept;
         LANGULUS_API(LOGGER) Interface& operator << (Intent) noexcept;

         LANGULUS_API(LOGGER) Interface& operator << (const Tabs&) noexcept;
         LANGULUS_API(LOGGER) ScopedTabs operator << (Tabs&&) noexcept;

         Interface& operator << (const CT::Sparse auto&) noexcept;
         template<class T, size_t N>
         Interface& operator << (const ::std::array<T, N>&) noexcept;
         Interface& operator << (const ::Langulus::Logger::Formattable auto&) noexcept;
         Interface& operator << (const char8_t&) noexcept;
      };

   } // namespace Langulus::Logger::A


   ///                                                                        
   ///   The main logger interface                                            
   ///                                                                        
   /// Supports colors, formatting commands, and can relay messages to a      
   /// list of attachments                                                    
   ///                                                                        
   class Interface final : public A::Interface {
   private:
      // Color stack                                                    
      ::std::stack<Style> mStyleStack;
      // Number of tabulations                                          
      size_t mTabulator = 0;

      // Redirectors                                                    
      ::std::list<A::Interface*> mRedirectors;
      // Duplicators                                                    
      ::std::list<A::Interface*> mDuplicators;

   public:
      // Current intent                                                 
      Intent CurrentIntent = Intent::Info;

      // Intent style customization point                               
      IntentProperties IntentStyle[int(Intent::Counter)] = {
         {"F", fmt::fg(fmt::terminal_color::red)},             // FatalError  
         {"E", fmt::fg(fmt::terminal_color::bright_red)},      // Error       
         {"W", fmt::fg(fmt::terminal_color::yellow)},          // Warning     
         {"V", fmt::fg(fmt::terminal_color::bright_black)},    // Verbose     
         {"I", fmt::fg(fmt::terminal_color::white)},           // Info        
         {"M", fmt::fg(fmt::terminal_color::bright_white)},    // Message     
         {"S", fmt::fg(fmt::terminal_color::bright_magenta)},  // Special     
         {"L", fmt::fg(fmt::terminal_color::cyan)},            // Flow        
         {"N", fmt::fg(fmt::terminal_color::bright_blue)},     // Input       
         {"T", fmt::fg(fmt::terminal_color::bright_yellow)},   // Network     
         {"O", fmt::fg(fmt::terminal_color::blue)},            // OS          
         {"P", fmt::fg(fmt::terminal_color::bright_green)}     // Prompt      
      };

      // Tabulator color and formatting customization                   
      Intent DefaultIntent = Intent::Info;
      Style TabStyle = fmt::fg(fmt::terminal_color::bright_black);
      Style TimeStampStyle = TabStyle;
      TextView TabString = "|  ";

      size_t GetTabs() const noexcept { return mTabulator; }

      LANGULUS_API(LOGGER)  Interface();
      LANGULUS_API(LOGGER)  Interface(const Interface&);
      LANGULUS_API(LOGGER) ~Interface();

      ///                                                                     
      /// Interface override                                                  
      ///                                                                     
      LANGULUS_API(LOGGER) void Write(const TextView&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;

      ///                                                                     
      /// State changers                                                      
      ///                                                                     
      LANGULUS_API(LOGGER) void RunCommand(Command) noexcept;
      LANGULUS_API(LOGGER) auto GetCurrentStyle() const noexcept -> Style;
      LANGULUS_API(LOGGER) auto SetStyle(Style) noexcept -> const Style&;
      LANGULUS_API(LOGGER) auto SetColor(Color) noexcept -> const Style&;
      LANGULUS_API(LOGGER) auto SetEmphasis(Emphasis) noexcept -> const Style&;

      ///                                                                     
      /// Attachments                                                         
      ///                                                                     
      LANGULUS_API(LOGGER) void AttachDuplicator(A::Interface*) noexcept;
      LANGULUS_API(LOGGER) void DettachDuplicator(A::Interface*) noexcept;

      LANGULUS_API(LOGGER) void AttachRedirector(A::Interface*) noexcept;
      LANGULUS_API(LOGGER) void DettachRedirector(A::Interface*) noexcept;
   };


   ///                                                                        
   /// The global logger instance                                             
   ///                                                                        
   LANGULUS_API(LOGGER) extern Interface Instance;

   template<class...T>
   decltype(auto) Line(T&&...) noexcept;
   template<class...T>
   decltype(auto) Append(T&&...) noexcept;

   template<class...T>
   decltype(auto) Section(T&&...) noexcept;

   template<class...T>
   decltype(auto) Fatal(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs FatalTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Error(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs ErrorTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Warning(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs WarningTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Verbose(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs VerboseTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Info(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs InfoTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Message(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs MessageTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Special(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs SpecialTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Flow(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs FlowTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Input(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs InputTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Network(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs NetworkTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) OS(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs OSTab(T&&...) noexcept;

   template<class...T>
   decltype(auto) Prompt(T&&...) noexcept;
   template<class...T>
   NOD() ScopedTabs PromptTab(T&&...) noexcept;

   LANGULUS_API(LOGGER) void AttachDuplicator(A::Interface*) noexcept;
   LANGULUS_API(LOGGER) void DettachDuplicator(A::Interface*) noexcept;

   LANGULUS_API(LOGGER) void AttachRedirector(A::Interface*) noexcept;
   LANGULUS_API(LOGGER) void DettachRedirector(A::Interface*) noexcept;


   ///                                                                        
   /// Helpful redirectors and duplicators                                    
   ///                                                                        

   ///                                                                        
   /// Consumes all logging messages, so that they don't interfere with       
   /// rendering inside the console.                                          
   /// Use it like this:                                                      
   ///    Logger::AttachRedirector(&Logger::MessageSinkInstance);             
   ///    <suppresses all logging in console>                                 
   ///    Logger::DettachRedirector(&Logger::MessageSinkInstance);            
   ///    <you can log once again>                                            
   ///                                                                        
   struct MessageSink final : Logger::A::Interface {
      void Write(const TextView&) const noexcept {}
      void Write(Style) const noexcept {}
      void NewLine() const noexcept {}
      void Clear() const noexcept {}
   };

   LANGULUS_API(LOGGER) extern MessageSink MessageSinkInstance;

   ///                                                                        
   /// Generates HTML code from logging messages. Can be used both as         
   /// duplicator or redirector. Colors and styles are consistent with        
   /// console output. Use it like this:                                      
   ///    Logger::ToHTML logRedirect("outputfile.htm");                       
   ///    Logger::AttachRedirector(&logRedirect);                             
   ///    <redirect all logging to an HTML file>                              
   ///    Logger::DettachRedirector(&logRedirect);                            
   ///    <you can log once again in the console>                             
   ///                                                                        
   struct ToHTML final : Logger::A::Interface {
   private:
      std::string mFilename;
      mutable std::ofstream mFile;

      void WriteHeader() const;
      void WriteFooter() const;

   public:
      LANGULUS_API(LOGGER)  ToHTML(const TextView&);
      LANGULUS_API(LOGGER) ~ToHTML();

      LANGULUS_API(LOGGER) void Write(const TextView&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;
   };

   ///                                                                        
   /// Generates plain text file from logging messages. Can be used both as   
   /// duplicator or redirector. Strips and and all styling. Use it like this:
   ///    Logger::ToTXT logRedirect("outputfile.htm");                        
   ///    Logger::AttachRedirector(&logRedirect);                             
   ///    <redirect all logging to a plain text file>                         
   ///    Logger::DettachRedirector(&logRedirect);                            
   ///    <you can log once again in the console>                             
   ///                                                                        
   struct ToTXT final : Logger::A::Interface {
   private:
      std::string mFilename;
      mutable std::ofstream mFile;

      void WriteHeader() const;
      void WriteFooter() const;

   public:
      LANGULUS_API(LOGGER)  ToTXT(const TextView&);
      LANGULUS_API(LOGGER) ~ToTXT();

      LANGULUS_API(LOGGER) void Write(const TextView&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;
   };

   /// Generate hexadecimal string from a given value                         
   ///   @param format - the template string                                  
   ///   @param args... - the arguments                                       
   ///   @return the instantiated template                                    
   auto Hex(const auto& from) {
      ::std::array<char, sizeof(from) * 2> result {};
      auto from_bytes = reinterpret_cast<const std::byte*>(&from);
      auto to_bytes = result.data();
      for (Offset i = 0; i < sizeof(from); ++i)
         fmt::format_to_n(to_bytes + i * 2, 2, "{:02X}", from_bytes[i]);
      return result;
   }

} // namespace Langulus::Logger

#define LANGULUS_FUNCTION_NAME() \
   (::Langulus::Logger::A::Interface::GetFunctionName(LANGULUS_FUNCTION()))

#include "Logger.inl"

namespace fmt
{
   
   ///                                                                        
   /// Extend FMT to be capable of logging any exception                      
   ///                                                                        
   template<::Langulus::CT::Exception T>
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) {
         return ctx.begin();
      }

      template<class CONTEXT> LANGULUS(INLINED)
      auto format(T const& e, CONTEXT& ctx) const {
         #if LANGULUS(DEBUG)
            return ::fmt::format_to(ctx.out(), "{}({} at {})",
               e.GetName(), e.GetMessage(), e.GetLocation());
         #else
            return ::fmt::format_to(ctx.out(), "{}", e.GetName());
         #endif
      }
   };

   ///                                                                        
   /// Extend FMT to be capable of logging byte sizes                         
   ///                                                                        
   template<>
   struct formatter<::Langulus::Size> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) {
         return ctx.begin();
      }

      template<class CONTEXT>
      auto format(::Langulus::Size const& bs, CONTEXT& ctx) const {
         double f;
         if (bs < 1'024LL)
            f = static_cast<float>(bs);
         else if (bs < 1'048'576LL)
            f = bs * 1. / 1'024LL;
         else if (bs < 1'073'741'824LL)
            f = bs * 1. / 1'048'576LL;
         else if constexpr (sizeof(bs) > 4) {
            if (bs < 1'099'511'627'776LL)
               f = bs * 1. / 1'073'741'824LL;
            else if (bs < 1'125'899'906'842'624LL)
               f = bs * 1. / 1'099'511'627'776LL;
            else
               f = bs * 1. / 1'125'899'906'842'624LL;
         }
         else f = bs * 1. / 1'073'741'824LL;

         double intpart;
         if (::std::modf(f, &intpart) < 0.001)
            return ::fmt::format_to(ctx.out(), "{} {}", static_cast<::std::size_t>(f), bs.GetSuffix());
         else
            return ::fmt::format_to(ctx.out(), "{:.2f} {}", f, bs.GetSuffix());
      }
   };

} // namespace fmt