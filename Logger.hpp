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
#include <string>
#include <format>

namespace Langulus::Logger
{
	using Letter = char;
	using Token = ::std::basic_string_view<Letter>;
	using Text = ::std::basic_string<Letter>;
}

namespace Langulus::CT
{
	/// Check if a type can be used with std::format									
	template<class T>
	concept Formattable = ::std::_Has_const_formatter<T, 
		::std::basic_format_context<
			::std::back_insert_iterator<
				::std::_Fmt_buffer<::Langulus::Logger::Letter>>,
			::Langulus::Logger::Letter>>;
}

namespace Langulus::Logger
{

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

	/// A pair of a foreground and background color										
	struct ColorState {
		Color mForeground;
		Color mBackground;
	};

	constexpr ColorState operator | (Color&& fg, Color&& bg) noexcept {
		return { fg, bg };
	}

	/// Console commands																			
	/// You can << these as easily as text													
	enum Command {
		Clear = ColorCounter,			// Clear the console						
		NewLine,		// Write a new line, with a timestamp and tabulation	
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
	struct Tabs {
		int mTabs = 0;

		constexpr Tabs() noexcept = default;
		constexpr Tabs(const Tabs&) noexcept = default;
		constexpr Tabs(Tabs&& other) noexcept
			: mTabs{other.mTabs} { other.mTabs = 0; }
		constexpr Tabs(int tabs) noexcept
			: mTabs{tabs} {}
	};

	/// Scoped tabulation marker that restores tabbing when destroyed				
	struct ScopedTabs : public Tabs {
		using Tabs::Tabs;
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
		///	The abstract logger interface - override this to define an			
		///	attachment																			
		///																							
		class Interface {
		public:
			static constexpr Token GetColorCode(const ColorState&) noexcept;
			static constexpr Token GetFunctionName(const Token&) noexcept;
			static Text GetAdvancedTime() noexcept;
			static Text GetSimpleTime() noexcept;

			virtual void Write(const Letter&) const noexcept = 0;
			virtual void Write(const Token&) const noexcept = 0;
			virtual void Write(const Text&) const noexcept = 0;
			virtual void Write(const Command&) noexcept = 0;
			virtual void NewLine() const noexcept = 0;
			virtual void SetForegroundColor(Color) noexcept = 0;
			virtual void SetBackgroundColor(Color) noexcept = 0;
			virtual void Tabulate() const noexcept = 0;

			/// Implicit bool operator in order to use log in 'if' statements		
			/// Example: if (condition && Logger::Info("stuff"))						
			///	@return true																	
			constexpr operator bool() const noexcept {
				return true;
			}

			Interface& operator << (const Interface&) noexcept;
			Interface& operator << (const Command&) noexcept;
			Interface& operator << (const Color&) noexcept;
			Interface& operator << (const ColorState&) noexcept;
			Interface& operator << (const Tabs&) noexcept;
			Interface& operator << (Tabs&) noexcept;

			template<CT::Formattable T>
			Interface& operator << (const T&) const noexcept;
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
		// Color stack																		
		::std::stack<ColorState> mColorStack;
		// Number of tabulations														
		size_t mTabulator = 0;
		// Tabulator color																
		ColorState mTabColor {Gray, DefaultColor};
		// Attachments																		
		::std::list<A::Interface*> mAttachments;

	private:
		Interface();
		~Interface();

		void Write(const Letter&) const noexcept final;
		void Write(const Token&) const noexcept final;
		void Write(const Text&) const noexcept final;
		void Write(const Command&) noexcept final;
		void NewLine() const noexcept final;
		void SetForegroundColor(Color) noexcept final;
		void SetBackgroundColor(Color) noexcept final;
		void Tabulate() const noexcept final;

	public:
		void Attach(A::Interface*) noexcept;
		void Dettach(A::Interface*) noexcept;
	};

	///																								
	/// The global logger instance, initialized via a schwarz counter				
	///																								
	extern Interface& Instance;

	template<ColorState COLOR, class... T>
	Interface& Line(T&&...) noexcept;
	template<class... T>
	Interface& Append(T&&...) noexcept;

	template<class... T>
	ScopedTabs Section(T&&...) noexcept;

	template<class... T>
	Interface& Error(T&&...) noexcept;
	template<class... T>
	Interface& Warning(T&&...) noexcept;
	template<class... T>
	Interface& Verbose(T&&...) noexcept;
	template<class... T>
	Interface& Info(T&&...) noexcept;
	template<class... T>
	Interface& Message(T&&...) noexcept;
	template<class... T>
	Interface& Special(T&&...) noexcept;
	template<class... T>
	Interface& Flow(T&&...) noexcept;
	template<class... T>
	Interface& Input(T&&...) noexcept;
	template<class... T>
	Interface& TCP(T&&...) noexcept;
	template<class... T>
	Interface& UDP(T&&...) noexcept;
	template<class... T>
	Interface& OS(T&&...) noexcept;
	template<class... T>
	Interface& Prompt(T&&...) noexcept;

} // namespace Langulus::Logger

#include "Logger.inl"