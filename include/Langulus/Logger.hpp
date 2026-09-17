///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "LoggerStateless.hpp"
#include <stack>
#include <list>
#include <string_view>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_LOGGER)
   #define LANGULUS_API_LOGGER() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_LOGGER() LANGULUS_IMPORT()
#endif

/// Make the rest of the code aware, that Langulus::Logger has been included  
#define LANGULUS_LIBRARY_LOGGER() 1

namespace Langulus::Logger
{
   /// MARK: CommandExt                                                       
   /// Additional commands                                                    
   enum class CommandExt : uint8_t {
      Pop,	   // Pop the style, and apply previous style               
      Push,		// Push the current style (don't stylize)                
      Tab,		// Tab once on a new line after this command             
      Untab,	// Untab once, again on a new line after this command    
      Reset    // Reset the state (color stack, tabulation, intent)     
   };
   using enum CommandExt;
   
   /// MARK: ColorExt                                                         
   /// Colors combined with pushes and pop commands, for shorter code         
   enum class ColorExt : unsigned {
      // Bits that dictate how to mix the colors                        
      // Color is always mixed with the currently set one, unless the   
      // 'PreviousColor' bit is on, in which case the style is popped   
      // before applying color                                          
      PreviousColor           = 128,

      // Color is always mixed with the currently set one, unless the   
      // 'NextColor' bit is on, in which case the style is pushed       
      // before applying color                                          
      NextColor               = 256,

      // Colors that mix with the previous color                        
      PopNoForeground         = static_cast<unsigned>(NoForeground ) | PreviousColor,
      PopNoBackground         = static_cast<unsigned>(NoBackground ) | PreviousColor,
       
      PopBlack                = static_cast<unsigned>(Black        ) | PreviousColor,
      PopDarkRed              = static_cast<unsigned>(DarkRed      ) | PreviousColor,
      PopDarkGreen            = static_cast<unsigned>(DarkGreen    ) | PreviousColor,
      PopDarkYellow           = static_cast<unsigned>(DarkYellow   ) | PreviousColor,
      PopDarkBlue             = static_cast<unsigned>(DarkBlue     ) | PreviousColor,
      PopDarkPurple           = static_cast<unsigned>(DarkPurple   ) | PreviousColor,
      PopDarkCyan             = static_cast<unsigned>(DarkCyan     ) | PreviousColor,
      PopGray                 = static_cast<unsigned>(Gray         ) | PreviousColor,
      
      PopBlackBgr             = static_cast<unsigned>(BlackBgr     ) | PreviousColor,
      PopDarkRedBgr           = static_cast<unsigned>(DarkRedBgr   ) | PreviousColor,
      PopDarkGreenBgr         = static_cast<unsigned>(DarkGreenBgr ) | PreviousColor,
      PopDarkYellowBgr        = static_cast<unsigned>(DarkYellowBgr) | PreviousColor,
      PopDarkBlueBgr          = static_cast<unsigned>(DarkBlueBgr  ) | PreviousColor,
      PopDarkPurpleBgr        = static_cast<unsigned>(DarkPurpleBgr) | PreviousColor,
      PopDarkCyanBgr          = static_cast<unsigned>(DarkCyanBgr  ) | PreviousColor,
      PopGrayBgr              = static_cast<unsigned>(GrayBgr      ) | PreviousColor,
      
      PopDarkGray             = static_cast<unsigned>(DarkGray     ) | PreviousColor,
      PopRed                  = static_cast<unsigned>(Red          ) | PreviousColor,
      PopGreen                = static_cast<unsigned>(Green        ) | PreviousColor,
      PopYellow               = static_cast<unsigned>(Yellow       ) | PreviousColor,
      PopBlue                 = static_cast<unsigned>(Blue         ) | PreviousColor,
      PopPurple               = static_cast<unsigned>(Purple       ) | PreviousColor,
      PopCyan                 = static_cast<unsigned>(Cyan         ) | PreviousColor,
      PopWhite                = static_cast<unsigned>(White        ) | PreviousColor,
      
      PopDarkGrayBgr          = static_cast<unsigned>(DarkGrayBgr  ) | PreviousColor,
      PopRedBgr               = static_cast<unsigned>(RedBgr       ) | PreviousColor,
      PopGreenBgr             = static_cast<unsigned>(GreenBgr     ) | PreviousColor,
      PopYellowBgr            = static_cast<unsigned>(YellowBgr    ) | PreviousColor,
      PopBlueBgr              = static_cast<unsigned>(BlueBgr      ) | PreviousColor,
      PopPurpleBgr            = static_cast<unsigned>(PurpleBgr    ) | PreviousColor,
      PopCyanBgr              = static_cast<unsigned>(CyanBgr      ) | PreviousColor,
      PopWhiteBgr             = static_cast<unsigned>(WhiteBgr     ) | PreviousColor,

      // Colors that mix with the next color                            
      PushNoForeground        = static_cast<unsigned>(NoForeground ) | NextColor,
      PushNoBackground        = static_cast<unsigned>(NoBackground ) | NextColor,
      
      PushBlack               = static_cast<unsigned>(Black        ) | NextColor,
      PushDarkRed             = static_cast<unsigned>(DarkRed      ) | NextColor,
      PushDarkGreen           = static_cast<unsigned>(DarkGreen    ) | NextColor,
      PushDarkYellow          = static_cast<unsigned>(DarkYellow   ) | NextColor,
      PushDarkBlue            = static_cast<unsigned>(DarkBlue     ) | NextColor,
      PushDarkPurple          = static_cast<unsigned>(DarkPurple   ) | NextColor,
      PushDarkCyan            = static_cast<unsigned>(DarkCyan     ) | NextColor,
      PushGray                = static_cast<unsigned>(Gray         ) | NextColor,
      
      PushBlackBgr            = static_cast<unsigned>(BlackBgr     ) | NextColor,
      PushDarkRedBgr          = static_cast<unsigned>(DarkRedBgr   ) | NextColor,
      PushDarkGreenBgr        = static_cast<unsigned>(DarkGreenBgr ) | NextColor,
      PushDarkYellowBgr       = static_cast<unsigned>(DarkYellowBgr) | NextColor,
      PushDarkBlueBgr         = static_cast<unsigned>(DarkBlueBgr  ) | NextColor,
      PushDarkPurpleBgr       = static_cast<unsigned>(DarkPurpleBgr) | NextColor,
      PushDarkCyanBgr         = static_cast<unsigned>(DarkCyanBgr  ) | NextColor,
      PushGrayBgr             = static_cast<unsigned>(GrayBgr      ) | NextColor,
      
      PushDarkGray            = static_cast<unsigned>(DarkGray     ) | NextColor,
      PushRed                 = static_cast<unsigned>(Red          ) | NextColor,
      PushGreen               = static_cast<unsigned>(Green        ) | NextColor,
      PushYellow              = static_cast<unsigned>(Yellow       ) | NextColor,
      PushBlue                = static_cast<unsigned>(Blue         ) | NextColor,
      PushPurple              = static_cast<unsigned>(Purple       ) | NextColor,
      PushCyan                = static_cast<unsigned>(Cyan         ) | NextColor,
      PushWhite               = static_cast<unsigned>(White        ) | NextColor,
      
      PushDarkGrayBgr         = static_cast<unsigned>(DarkGrayBgr  ) | NextColor,
      PushRedBgr              = static_cast<unsigned>(RedBgr       ) | NextColor,
      PushGreenBgr            = static_cast<unsigned>(GreenBgr     ) | NextColor,
      PushYellowBgr           = static_cast<unsigned>(YellowBgr    ) | NextColor,
      PushBlueBgr             = static_cast<unsigned>(BlueBgr      ) | NextColor,
      PushPurpleBgr           = static_cast<unsigned>(PurpleBgr    ) | NextColor,
      PushCyanBgr             = static_cast<unsigned>(CyanBgr      ) | NextColor,
      PushWhiteBgr            = static_cast<unsigned>(WhiteBgr     ) | NextColor,

      // Colors that reset to previous color, push and mix              
      PopAndPushNoForeground  = static_cast<unsigned>(NoForeground ) | NextColor | PreviousColor,
      PopAndPushNoBackground  = static_cast<unsigned>(NoBackground ) | NextColor | PreviousColor,

      PopAndPushBlack         = static_cast<unsigned>(Black        ) | NextColor | PreviousColor,
      PopAndPushDarkRed       = static_cast<unsigned>(DarkRed      ) | NextColor | PreviousColor,
      PopAndPushDarkGreen     = static_cast<unsigned>(DarkGreen    ) | NextColor | PreviousColor,
      PopAndPushDarkYellow    = static_cast<unsigned>(DarkYellow   ) | NextColor | PreviousColor,
      PopAndPushDarkBlue      = static_cast<unsigned>(DarkBlue     ) | NextColor | PreviousColor,
      PopAndPushDarkPurple    = static_cast<unsigned>(DarkPurple   ) | NextColor | PreviousColor,
      PopAndPushDarkCyan      = static_cast<unsigned>(DarkCyan     ) | NextColor | PreviousColor,
      PopAndPushGray          = static_cast<unsigned>(Gray         ) | NextColor | PreviousColor,

      PopAndPushBlackBgr      = static_cast<unsigned>(BlackBgr     ) | NextColor | PreviousColor,
      PopAndPushDarkRedBgr    = static_cast<unsigned>(DarkRedBgr   ) | NextColor | PreviousColor,
      PopAndPushDarkGreenBgr  = static_cast<unsigned>(DarkGreenBgr ) | NextColor | PreviousColor,
      PopAndPushDarkYellowBgr = static_cast<unsigned>(DarkYellowBgr) | NextColor | PreviousColor,
      PopAndPushDarkBlueBgr   = static_cast<unsigned>(DarkBlueBgr  ) | NextColor | PreviousColor,
      PopAndPushDarkPurpleBgr = static_cast<unsigned>(DarkPurpleBgr) | NextColor | PreviousColor,
      PopAndPushDarkCyanBgr   = static_cast<unsigned>(DarkCyanBgr  ) | NextColor | PreviousColor,
      PopAndPushGrayBgr       = static_cast<unsigned>(GrayBgr      ) | NextColor | PreviousColor,

      PopAndPushDarkGray      = static_cast<unsigned>(DarkGray     ) | NextColor | PreviousColor,
      PopAndPushRed           = static_cast<unsigned>(Red          ) | NextColor | PreviousColor,
      PopAndPushGreen         = static_cast<unsigned>(Green        ) | NextColor | PreviousColor,
      PopAndPushYellow        = static_cast<unsigned>(Yellow       ) | NextColor | PreviousColor,
      PopAndPushBlue          = static_cast<unsigned>(Blue         ) | NextColor | PreviousColor,
      PopAndPushPurple        = static_cast<unsigned>(Purple       ) | NextColor | PreviousColor,
      PopAndPushCyan          = static_cast<unsigned>(Cyan         ) | NextColor | PreviousColor,
      PopAndPushWhite         = static_cast<unsigned>(White        ) | NextColor | PreviousColor,

      PopAndPushDarkGrayBgr   = static_cast<unsigned>(DarkGrayBgr  ) | NextColor | PreviousColor,
      PopAndPushRedBgr        = static_cast<unsigned>(RedBgr       ) | NextColor | PreviousColor,
      PopAndPushGreenBgr      = static_cast<unsigned>(GreenBgr     ) | NextColor | PreviousColor,
      PopAndPushYellowBgr     = static_cast<unsigned>(YellowBgr    ) | NextColor | PreviousColor,
      PopAndPushBlueBgr       = static_cast<unsigned>(BlueBgr      ) | NextColor | PreviousColor,
      PopAndPushPurpleBgr     = static_cast<unsigned>(PurpleBgr    ) | NextColor | PreviousColor,
      PopAndPushCyanBgr       = static_cast<unsigned>(CyanBgr      ) | NextColor | PreviousColor,
      PopAndPushWhiteBgr      = static_cast<unsigned>(WhiteBgr     ) | NextColor | PreviousColor
   };
   using enum ColorExt;
   
   /// MARK: Tabs                                                             
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
   struct Scope : Tabs {
      using Tabs::Tabs;
      constexpr Scope(Scope&& other) noexcept
         : Tabs {LglsFwd(other)} {}
      constexpr ~Scope() noexcept;
   };

   /// Returned from disabled scoped functions, should be optimized-out       
   struct [[maybe_unused]] UnusedScope {};


   ///                                                                        
   ///   The abstract logger interface - override this to define attachments  
   ///                                                                        
   struct Interface {
      virtual ~Interface() noexcept = default;
      
      Interface& operator = (const Interface&) = delete;
      Interface& operator = (Interface&&) = delete;

      virtual void Write(::std::string_view const&) const noexcept = 0;
      virtual void Write(Style) const noexcept = 0;
      virtual void NewLine() const noexcept = 0;
      virtual void Clear() const noexcept = 0;
      virtual auto GetFilename() const noexcept -> ::std::string_view = 0;

      LANGULUS_API(LOGGER) static ::std::string GetAdvancedTime() noexcept;
      LANGULUS_API(LOGGER) static ::std::string GetSimpleTime()   noexcept;
   };


   /// MARK: State                                                            
   ///                                                                        
   ///   The global logger state                                              
   ///                                                                        
   /// Supports a style stack, can relay messages to a list of attachments    
   struct State final : Interface {
   private:
      // Style stack                                                    
      mutable ::std::stack<Style> mStyleStack;
      // Number of tabulations                                          
      mutable size_t mTabulator = 0;
      // Current intent                                                 
      mutable Intent mCurrentIntent = DefaultIntent;
      // Last written style                                             
      mutable Style mLastWrittenStyle = DefaultStyle;

      // Redirectors                                                    
      ::std::list<Interface*> mRedirectors;
      // Duplicators                                                    
      ::std::list<Interface*> mDuplicators;

   public:
      // Intent style customization point                               
      IntentProperties mIntentStyle[static_cast<int>(Intent::Counter)];

      // Tabulator color and formatting customization                   
      Intent mDefaultIntent = DefaultIntent;
      Style  mDefaultStyle = DefaultStyle;
      ::std::string_view mTabString = "▌  ";

      size_t GetTabs() const noexcept { return mTabulator; }

      LANGULUS_API(LOGGER)  State();
      LANGULUS_API(LOGGER) ~State();

      ///                                                                     
      /// Interface override                                                  
      LANGULUS_API(LOGGER) void Write(::std::string_view const&) const noexcept;
      LANGULUS_API(LOGGER) void Write(::std::wstring_view const&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;
      LANGULUS_API(LOGGER) auto GetFilename() const noexcept -> ::std::string_view;

      /// Additional services                                                 
      void Write(const CT::Loggable auto& anything) const noexcept {
         const auto formatted = fmt::format("{}", anything);
         return Write(::std::string_view(formatted));
      }

      LANGULUS_API(LOGGER) void Write(CommandExt) const noexcept;
      LANGULUS_API(LOGGER) void Write(Color) const noexcept;
      LANGULUS_API(LOGGER) void Write(ColorExt) const noexcept;
      LANGULUS_API(LOGGER) void Write(const Tabs&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Emphasis) const noexcept;
      LANGULUS_API(LOGGER) void Write(Intent) const noexcept;
      LANGULUS_API(LOGGER) auto NewScope() const noexcept -> Scope;

      LANGULUS_API(LOGGER) auto GetCurrentStyle() const noexcept -> Style&;
      LANGULUS_API(LOGGER) int  GetCurrentIntent() const noexcept;

      ///                                                                     
      /// Attachments                                                         
      LANGULUS_API(LOGGER) void AttachDuplicator (Interface*) noexcept;
      LANGULUS_API(LOGGER) void DettachDuplicator(Interface*) noexcept;

      LANGULUS_API(LOGGER) void AttachRedirector (Interface*) noexcept;
      LANGULUS_API(LOGGER) void DettachRedirector(Interface*) noexcept;
   };


   ///                                                                        
   /// The global logger state                                                
   LANGULUS_API(LOGGER) extern State GlobalState;

   constexpr Scope::~Scope() noexcept {
      if not consteval {
         if (mTabs > 0)
            GlobalState.Write(Tabs {-mTabs});
      }
   }

   /// A general new-line write function that continues the last intent/style 
   template<class...T> LANGULUS(INLINED)
   constexpr void Line(T&&...arguments) noexcept {
      #if LANGULUS_FEATURE(LOGGING)
         if not consteval {
            GlobalState.NewLine();
            (GlobalState.Write(LglsFwd(arguments)), ...);
         }
      #else
         LANGULUS(NOOP);
      #endif
   }

   /// A general same-line write function that continues the last style       
   template<class...T> LANGULUS(INLINED)
   constexpr void Append(T&&...arguments) noexcept {
      #if LANGULUS_FEATURE(LOGGING)
         if not consteval {
            (GlobalState.Write(LglsFwd(arguments)), ...);
         }
      #else
         LANGULUS(NOOP);
      #endif
   }

   /// MARK: Section                                                          
   /// Write a section on a new line, tab all consecutive lines, underline it,
   /// and return the scoped tabs, that will be	untabbed automatically at the 
   /// scope's end. This Section continues the last intent/style.             
   /// Use <Intent>Section if you want to change the intent as well.          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto Section(T&&...arguments) noexcept {
      #if LANGULUS_FEATURE(LOGGING)
         if constexpr (sizeof...(arguments) > 0) {
            if not consteval {
               GlobalState.NewLine();
               GlobalState.Write(Push);
               GlobalState.Write(Underline);
               (GlobalState.Write(LglsFwd(arguments)), ...);
               GlobalState.Write(Pop);
               return GlobalState.NewScope();
            }
            else {
               return Scope {0};
            }
         }
         else return UnusedScope {};
      #else
         return UnusedScope {};
      #endif
   }

   /// MARK: Fatal                                                            
   /// Write a new-line fatal error                                           
   template<class...T> LANGULUS(INLINED)
   constexpr void Fatal([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FATALERRORS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::FatalError);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line fatal error and tab all next lines                    
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto FatalScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FATALERRORS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::FatalError);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_FATALERRORS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a fatal error section and tab all next lines                     
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto FatalSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FATALERRORS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::FatalError);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_FATALERRORS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Error                                                            
   /// Write a new-line error                                                 
   template<class...T> LANGULUS(INLINED)
   constexpr void Error([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_ERRORS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Error);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line error and tab all next lines                          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto ErrorScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_ERRORS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Error);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_ERRORS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write an error section and tab all next lines                          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto ErrorSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_ERRORS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Error);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_ERRORS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Warning                                                          
   /// Write a new-line warning                                               
   template<class...T> LANGULUS(INLINED)
   constexpr void Warning([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_WARNINGS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Warning);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line warning and tab all next lines                        
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto WarningScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_WARNINGS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Warning);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_WARNINGS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a warning section and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto WarningSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_WARNINGS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Warning);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_WARNINGS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Verbose                                                          
   /// Write a new-line with verbose information                              
   template<class...T> LANGULUS(INLINED)
   constexpr void Verbose([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_VERBOSE
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Verbose);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line verbose and tab all next lines                        
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto VerboseScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_VERBOSE
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Verbose);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_VERBOSE
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a verbose section and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto VerboseSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_VERBOSE
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Verbose);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_VERBOSE
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Info                                                             
   /// Write a new-line with information                                      
   template<class...T> LANGULUS(INLINED)
   constexpr void Info([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INFOS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Info);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line info and tab all next lines                           
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto InfoScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INFOS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Info);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_INFOS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write an info section and tab all next lines                           
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto InfoSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INFOS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Info);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_INFOS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Message                                                          
   /// Write a new-line with a personal message                               
   template<class...T> LANGULUS(INLINED)
   constexpr void Message([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_MESSAGES
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Message);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line message and tab all next lines                        
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto MessageScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_MESSAGES
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Message);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_MESSAGES
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a message section and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto MessageSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_MESSAGES
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Message);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_MESSAGES
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Special                                                          
   /// Write a new-line with special text                                     
   template<class...T> LANGULUS(INLINED)
   constexpr void Special([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_SPECIALS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Special);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line special and tab all next lines                        
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto SpecialScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_SPECIALS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Special);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_SPECIALS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a special section and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto SpecialSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_SPECIALS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Special);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_SPECIALS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Flow                                                             
   /// Write a new-line with flow information                                 
   template<class...T> LANGULUS(INLINED)
   constexpr void Flow([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FLOWS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Flow);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line flow and tab all next lines                           
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto FlowScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FLOWS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Flow);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_FLOWS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a flow section and tab all next lines                            
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto FlowSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_FLOWS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Flow);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_FLOWS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Input                                                            
   /// Write a new-line on user input                                         
   template<class...T> LANGULUS(INLINED)
   constexpr void Input([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INPUTS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Input);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line input and tab all next lines                          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto InputScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INPUTS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Input);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_INPUTS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write an input section and tab all next lines                          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto InputSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_INPUTS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Input);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_INPUTS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Network                                                          
   /// Write a new-line with network message                                  
   template<class...T> LANGULUS(INLINED)
   constexpr void Network([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_NETWORKS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Network);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line network and tab all next lines                        
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto NetworkScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_NETWORKS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Network);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_NETWORKS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write a network section and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto NetworkSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_NETWORKS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Network);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_NETWORKS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: OS                                                               
   /// Write a new-line with a message from OS                                
   template<class...T> LANGULUS(INLINED)
   constexpr void OS([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_OS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::OS);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }

   /// Write a new-line OS and tab all next lines                             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto OSScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_OS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::OS);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_OS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// Write an OS section and tab all next lines                             
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto OSSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_OS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::OS);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_OS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }

   /// MARK: Prompt                                                           
   /// Write a new-line with an input prompt                                  
   template<class...T> LANGULUS(INLINED)
   constexpr void Prompt([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         LANGULUS(NOOP);
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_PROMPTS
               GlobalState.Write(Intent::Ignore);
            #else
               GlobalState.Write(Intent::Prompt);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
            #endif
         }
      #endif
   }
   
   /// Write a new-line prompt and tab all next lines                         
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto PromptScoped([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_PROMPTS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Prompt);
               GlobalState.NewLine();
               (GlobalState.Write(LglsFwd(arguments)), ...);
               return GlobalState.NewScope();
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_PROMPTS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }
   
   /// Write a prompt section and tab all next lines                          
   ///   @return a scoped tab, that will untab when destroyed                 
   template<class...T> LANGULUS(INLINED)
   constexpr auto PromptSection([[maybe_unused]] T&&...arguments) noexcept {
      #if not LANGULUS_FEATURE(LOGGING)
         return UnusedScope {};
      #else
         if not consteval {
            #ifdef LANGULUS_LOGGER_DISABLE_PROMPTS
               GlobalState.Write(Intent::Ignore);
               return UnusedScope {};
            #else
               GlobalState.Write(Intent::Prompt);
               return Section(LglsFwd(arguments)...);
            #endif
         }
         else {
            #ifdef LANGULUS_LOGGER_DISABLE_PROMPTS
               return UnusedScope {};
            #else
               return Scope {0};
            #endif
         }
      #endif
   }
   
   /// MARK: Attachments                                                      
   inline void AttachDuplicator(Interface* d) noexcept {
      GlobalState.AttachDuplicator(d);
   }

   inline void DettachDuplicator(Interface* d) noexcept {
      GlobalState.DettachDuplicator(d);
   }

   inline void AttachRedirector(Interface* r) noexcept {
      GlobalState.AttachRedirector(r);
   }

   inline void DettachRedirector(Interface* r) noexcept {
      GlobalState.DettachRedirector(r);
   }
}