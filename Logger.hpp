///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include <Langulus.Core.hpp>
#include <stack>
#include <list>
#include <string_view>
#include <string>
#include <fmt/format.h>
#include <fmt/color.h>

namespace Langulus::Logger
{
	using Letter = char;
	using Token = ::std::basic_string_view<Letter>;
	using Text = ::std::basic_string<Letter>;

	/// Check if a type can be used with {fmt} by the logger							
	template<class T>
	concept Formattable = fmt::is_formattable<T, Letter>::value;
}

namespace Langulus::Logger
{

	/// Color codes, consistent with ANSI/VT100 escapes								
	/// Also consistent with fmt::terminal_color											
	enum class Color : ::std::underlying_type_t<fmt::terminal_color> {
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
		WhiteBgr
	};

	using enum Color;

	/// Some formatting styles, consistent with fmt::emphasis						
	enum class Emphasis : ::std::underlying_type_t<fmt::emphasis> {
		Bold = 1,					// Not working on windows
		Faint = 1 << 1,			// Not working on windows
		Italic = 1 << 2,			// Not working on windows
		Underline = 1 << 3,
		Blink = 1 << 4,			// Not working on windows
		Reverse = 1 << 5,
		Conceal = 1 << 6,			// Not working on windows
		Strike = 1 << 7,			// Not working on windows
	};

	using enum Emphasis;

	/// Text style, with background color, foreground color, and emphasis		
	using Style = fmt::text_style;

	/// Console commands																			
	enum class Command : uint8_t {
		Clear,		// Clear the console												
		NewLine,		// Write a new line, with a timestamp and tabulation	
		Pop,			// Pop the color state											
		Push,			// Push the color state											
		Invert,		// Inverts background and foreground colors				
		Reset,		// Reset the color and formatting state					
		Tab,			// Tab once on a new line after this command				
		Untab,		// Untab once, again on a new line after this command	
		Time,			// Write a short timestamp										
		ExactTime,	// Write an exhaustive timestamp								
	};

	using enum Command;

	/// Tabulation marker (can be pushed to log)											
	class Tabs {
	public:
		int mTabs = 0;

		constexpr Tabs() noexcept = default;
		constexpr Tabs(const Tabs&) noexcept = default;
		constexpr Tabs(Tabs&& other) noexcept
			: mTabs {other.mTabs} { other.mTabs = 0; }
		constexpr Tabs(int tabs) noexcept
			: mTabs {tabs} {}
	};

	/// Scoped tabulation marker that restores tabbing when destroyed				
	struct ScopedTabs : public Tabs {
		using Tabs::Tabs;
		constexpr ScopedTabs(ScopedTabs&& other) noexcept
			: Tabs {::std::forward<Tabs>(other)} {}
		~ScopedTabs() noexcept;
	};

	namespace Inner
	{
		/// Static initializer for every translation unit								
		static struct InterfaceInitializer {
			InterfaceInitializer();
			~InterfaceInitializer();
		} InterfaceInitializerInstance;
	}

	namespace A
	{
		///																							
		/// The abstract logger interface - override this to define attachments	
		///																							
		class Interface {
		public:
			static constexpr Token GetFunctionName(const Token&) noexcept;
			static Text GetAdvancedTime() noexcept;
			static Text GetSimpleTime() noexcept;

			virtual void Write(const Letter&) const noexcept = 0;
			virtual void Write(const Token&) const noexcept = 0;
			virtual void Write(const Text&) const noexcept = 0;
			virtual void Write(const Command&) noexcept = 0;
			virtual void Write(const Color&) noexcept = 0;
			virtual void Write(const Emphasis&) noexcept = 0;
			virtual void Write(const Style&) noexcept = 0;
			virtual void NewLine() const noexcept = 0;
			virtual void Tabulate() const noexcept = 0;

			/// Implicit bool operator in order to use log in 'if' statements		
			/// Example: if (condition && Logger::Info("stuff"))						
			///	@return true																	
			constexpr operator bool() const noexcept {
				return true;
			}

			LANGULUS(ALWAYSINLINE) Interface& operator << (A::Interface&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (const Command&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (const Color&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (const Emphasis&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (const Style&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (const Tabs&) noexcept;
			LANGULUS(ALWAYSINLINE) Interface& operator << (Tabs&) noexcept;

			template<Formattable T>
			LANGULUS(ALWAYSINLINE) Interface& operator << (const T&) const noexcept;
		};
	}


	///																								
	///	The main logger interface															
	///																								
	/// Supports colors, formatting commands, and can relay messages to a		
	/// list of attachments																		
	///																								
	class Interface : public A::Interface {
	friend class Inner::InterfaceInitializer;
	private:
		// Tabulator color and formatting											
		static constexpr Style DefaultStyle = {};
		static constexpr Style TabStyle = fmt::fg(fmt::terminal_color::bright_black);
		static constexpr Style TimeStampStyle = TabStyle;

		// A string used instead of \t, when you push a Tab command			
		static constexpr Token TabString = "|  ";

		// Color stack																		
		::std::stack<Style> mStyleStack;
		// Number of tabulations														
		size_t mTabulator = 0;
		// Attachments																		
		::std::list<A::Interface*> mAttachments;

	private:
		Interface();
		~Interface();

	public:
		void Write(const Letter&) const noexcept final;
		void Write(const Token&) const noexcept final;
		void Write(const Text&) const noexcept final;
		void Write(const Color&) noexcept final;
		void Write(const Emphasis&) noexcept final;
		void Write(const Command&) noexcept final;
		void Write(const Style&) noexcept final;
		void NewLine() const noexcept final;
		void Tabulate() const noexcept final;

		void Attach(A::Interface*) noexcept;
		void Dettach(A::Interface*) noexcept;
	};

	///																								
	/// The global logger instance, initialized via a schwarz counter				
	///																								
	extern Interface& Instance;

	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Line(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Append(T&&...) noexcept;

	template<class... T>
	NOD() LANGULUS(ALWAYSINLINE) ScopedTabs Section(T&&...) noexcept;

	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Fatal(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Error(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Warning(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Verbose(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Info(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Message(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Special(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Flow(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Input(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Network(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& OS(T&&...) noexcept;
	template<class... T>
	LANGULUS(ALWAYSINLINE) A::Interface& Prompt(T&&...) noexcept;

} // namespace Langulus::Logger

#include "Logger.inl"