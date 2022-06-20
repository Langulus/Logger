#pragma once
#include <PCFW.Core.hpp>

#if defined(PC_EXPORT_LOGGER)
	#define PC_API_LOG LANGULUS_EXPORT()
#else
	#define PC_API_LOG LANGULUS_IMPORT()
#endif

namespace PCFW::Logger
{

	/// Standart piception color codes, consistent with ANSI/VT100 escapes		
	enum ConsoleColor {
		ccDefaultColor = 0,
		ccBlack,
		ccDarkBlue,
		ccDarkGreen,
		ccDarkCyan,
		ccDarkRed,
		ccDarkPurple,
		ccDarkYellow,
		ccDarkWhite,
		ccGray,
		ccBlue,
		ccGreen,
		ccCyan,
		ccRed,
		ccPurple,
		ccYellow,
		ccWhite,

		ccColorCounter
	};

	/// Console commands																			
	/// You can << these as easily as text													
	enum ConsoleCommand {
		ccClear = ccColorCounter,		// Clear the console						
		ccPop,		// Pop the color state											
		ccPush,		// Push the color state											
		ccInvert,	// Inverts background and foreground colors				
		ccBold,		// Enable bold (not widely supported)						
		ccItalic,	// Enable italic (not widely supported)					
		ccBlink,		// Enable blink													
		ccReset,		// Reset the color and formatting state					
		ccTab,		// Tab once on a new line after this command				
		ccUntab,		// Untab once, again on a new line after this command	
		ccTime,		// Write a timestamp												
	};

	/// Tabulation marker (can be pushed to log)											
	struct Tab {
		constexpr Tab() noexcept = default;
		constexpr Tab(const Tab&) = default;
		constexpr Tab(int tabs) : mTabs(tabs) {}
		int mTabs = 0;
	};

	/// Scoped tabulation marker that restores tabbing when destroyed				
	struct ScopedTab : public Tab {
		using Tab::Tab;
		PC_API_LOG ~ScopedTab() noexcept;
	};


	/// Concept for stuff that is integrated to logger by default					
	template<typename T>
	concept NotLogSpecific = 
		!( Same<T, LoggerSystem> || Same<T, ConsoleColor>
		|| Same<T, ConsoleCommand> || Same<T, LiteralText> || Same<T, std::string>
		|| Same<T, Hash> || Same<T, ByteCount> || Boolean<T> || Character<T>
		|| Same<T, Tab> || Same<T, ScopedTab> || Number<T>
		|| (Same<T, void> && Sparse<T>));


	///																								
	///	LOGGER																					
	///																								
	/// The main logging interface for Piception. Supports colors, formatting	
	/// commands, and can relay messages to the logging module, if one is		
	/// attached.																					
	///																								
	class PC_API_LOG LoggerSystem {
	public:
		static LoggerSystem* CreateInstance();
		static LoggerSystem* GetInstance();
		
		NOD() static constexpr LiteralText GetColorCode(ConsoleColor, ConsoleColor);
		NOD() static constexpr LiteralText GetFunctionName(const LiteralText&);

		virtual LoggerSystem& NewLine() noexcept;
		virtual LoggerSystem& SetForegroundColor(ConsoleColor) noexcept;
		virtual LoggerSystem& SetBackgroundColor(ConsoleColor) noexcept;
		virtual LoggerSystem& SetColor(pcptr) noexcept;
		virtual LoggerSystem& Command(ConsoleCommand) noexcept;
		virtual LoggerSystem& Write(const LiteralText&) noexcept;
		virtual LoggerSystem& Write(const char*) noexcept;
		virtual LoggerSystem& Write(char) noexcept;

		void Attach(LoggerSystem*) noexcept;
		void Dettach(LoggerSystem*) noexcept;

		/// Implicit bool operator in order to use log in IF statements			
		constexpr operator bool() const noexcept { return true; }

		LoggerSystem& operator << (const ConsoleColor);
		LoggerSystem& operator << (const ConsoleCommand);
		LoggerSystem& operator << (const ByteCount&);
		LoggerSystem& operator << (const LiteralText&);
		LoggerSystem& operator << (const std::string&);
		LoggerSystem& operator << (const std::wstring&);
		LoggerSystem& operator << (const char*);
		LoggerSystem& operator << (const wchar_t*);
		LoggerSystem& operator << (const char8*);
		LoggerSystem& operator << (const charw*);
		LoggerSystem& operator << (char);
		LoggerSystem& operator << (wchar_t);
		LoggerSystem& operator << (char8);
		LoggerSystem& operator << (charw);
		LoggerSystem& operator << (bool);
		LoggerSystem& operator << (const Hash&);
		LoggerSystem& operator << (const LoggerSystem&);
		LoggerSystem& operator << (Tab&);
		LoggerSystem& operator << (const void*);
		template<Number T>
		LoggerSystem& operator << (const T&);

		/// This operator is intentionally left undefined - if you want to log	
		/// custom types, you'll have to define it										
		template<NotLogSpecific T>
		LoggerSystem& operator << (const T&);

	private:
		static constexpr pcptr ColorCacheSize = 32;

		// Cached colors																	
		struct ColorState {
			ConsoleColor mForeground = ConsoleColor::ccDefaultColor;
			ConsoleColor mBackground = ConsoleColor::ccDefaultColor;
		};

		ColorState mColorCache[ColorCacheSize] = {};

		// Current position in color cache											
		pcptr mColorIndex = 0;

		// Number of tabulations														
		pcptr mTabulator = 0;

		// Log relay																		
		LoggerSystem* mExtension = nullptr;

		// Will be prepended																
		void Tabulate() noexcept;

	private:
		LoggerSystem();
		~LoggerSystem() = default;
	};

	
	///																								
	///	GLOBAL LOG INSTANCE																	
	///																								
	extern PC_API_LOG LoggerSystem* LogInstance;

} // namespace PCFW::Logger

#include "LoggerSystem.inl"