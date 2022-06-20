///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include <stack>
#include <list>
#include <string_view>

namespace Langulus::Logger
{
	using Token = ::std::string_view;

	/// Color codes, consistent with ANSI/VT100 escapes								
	enum Color {
		DefaultColor = 0,
		Black,
		DarkBlue,
		DarkGreen,
		DarkCyan,
		DarkRed,
		DarkPurple,
		DarkYellow,
		DarkWhite,
		Gray,
		Blue,
		Green,
		Cyan,
		Red,
		Purple,
		Yellow,
		White,

		ColorCounter
	};

	/// Console commands																			
	/// You can << these as easily as text													
	enum Command {
		Clear = ColorCounter,			// Clear the console						
		Pop,			// Pop the color state											
		Push,			// Push the color state											
		Invert,		// Inverts background and foreground colors				
		Bold,			// Enable bold (not widely supported)						
		Italic,		// Enable italic (not widely supported)					
		Blink,		// Enable blink													
		Reset,		// Reset the color and formatting state					
		Tab,			// Tab once on a new line after this command				
		Untab,		// Untab once, again on a new line after this command	
		Time,			// Write a short timestamp										
		ExactTime,	// Write an exhaustive timestamp								
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
		~ScopedTab() noexcept;
	};

	namespace Inner
	{
		/// Static initializer for every translation unit								
		static struct InterfaceInitializer {
			InterfaceInitializer();
			~InterfaceInitializer();
		} InterfaceInitializerInstance;
	}


	///																								
	///	The main logger interface															
	///																								
	///	Supports colors, formatting commands, and can relay messages to a		
	/// list of attachments																		
	///																								
	class Interface {
	friend class Inner::InterfaceInitializer;
	private:
		// Color stack																		
		struct ColorState {
			Color mForeground;
			Color mBackground;
		};

		::std::stack<ColorState> mColorStack;

		// Number of tabulations														
		size_t mTabulator = 0;

		// Attachments																		
		::std::list<Interface*> mAttachments;

		Interface();
		~Interface();

		static constexpr Token GetColorCode(Color, Color) noexcept;
		static constexpr Token GetFunctionName(const Token&) noexcept;

		void Write(const char8_t&) const noexcept;
		void Write(const Token&) const noexcept;
		void Write(const ::std::string&) const noexcept;
		void Write(const Command&) noexcept;
		void NewLine() const noexcept;
		void SetForegroundColor(Color) noexcept;
		void SetBackgroundColor(Color) noexcept;
		void Tabulate() const noexcept;

	public:
		void Attach(Interface*) noexcept;
		void Dettach(Interface*) noexcept;

		template<class T>
		Interface& operator << (const T&) noexcept;
	};

	///																								
	/// The global logger instance, initialized via a schwarz counter				
	///																								
	extern Interface& Instance;

	template<class... T>
	Interface& Error(T...) noexcept;
	template<class... T>
	Interface& Warning(T...) noexcept;
	template<class... T>
	Interface& Verbose(T...) noexcept;

	/// Concept for stuff that is integrated to logger by default					
	/*template<typename T>
	concept NotLogSpecific = 
		!( Same<T, LoggerSystem> || Same<T, ConsoleColor>
		|| Same<T, ConsoleCommand> || Same<T, LiteralText> || Same<T, std::string>
		|| Same<T, Hash> || Same<T, ByteCount> || Boolean<T> || Character<T>
		|| Same<T, Tab> || Same<T, ScopedTab> || Number<T>
		|| (Same<T, void> && Sparse<T>));


	///																								
	///	LOGGER																					
	///																								
	///																								
	class PC_API_LOG LoggerSystem {
	public:
		static LoggerSystem* CreateInstance();
		static LoggerSystem* GetInstance();
		
		NOD() static constexpr LiteralText GetColorCode(ConsoleColor, ConsoleColor);
		NOD() static constexpr LiteralText GetFunctionName(const LiteralText&);



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

	private:
		LoggerSystem();
		~LoggerSystem() = default;
	};*/

} // namespace Langulus::Logger

#include "Logger.inl"