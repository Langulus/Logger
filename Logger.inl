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
	ScopedTabs::~ScopedTabs() noexcept {
		while (mTabs > 0) {
			--mTabs;
			Instance << Untab;
		}
	}

	/// Convert a logger color to an ANSI/VT100 escape code							
	///	@param foreground - the foreground color										
	///	@param background - the background color										
	///	@return the combined escape sequence											
	constexpr Token A::Interface::GetColorCode(const ColorState& state) noexcept {
		const Token colorCode[ColorCounter][ColorCounter] = {
			{ // ConsoleColor::Defaul													
				"\033[[1;39;49m",
				"\033[[1;39;40m", "\033[[1;39;44m", "\033[[1;39;42m", "\033[[1;39;46m",
				"\033[[1;39;41m", "\033[[1;39;45m", "\033[[1;39;43m", "\033[[1;39;47m",
				"\033[[1;39;47m", "\033[[1;39;44m", "\033[[1;39;42m", "\033[[1;39;46m",
				"\033[[1;39;41m", "\033[[1;39;45m", "\033[[1;39;43m", "\033[[1;39;47m"
			}, 
			{ // ConsoleColor::Black													
				"\033[[0;30;49m",
				"\033[[0;30;40m", "\033[[0;30;44m", "\033[[0;30;42m", "\033[[0;30;46m",
				"\033[[0;30;41m", "\033[[0;30;45m", "\033[[0;30;43m", "\033[[0;30;47m",
				"\033[[0;30;47m", "\033[[0;30;44m", "\033[[0;30;42m", "\033[[0;30;46m",
				"\033[[0;30;41m", "\033[[0;30;45m", "\033[[0;30;43m", "\033[[0;30;47m"
			}, 
			{ // ConsoleColor::DarkBlue												
				"\033[[0;34;49m",
				"\033[[0;34;40m", "\033[[0;34;44m", "\033[[0;34;42m", "\033[[0;34;46m",
				"\033[[0;34;41m", "\033[[0;34;45m", "\033[[0;34;43m", "\033[[0;34;47m",
				"\033[[0;34;47m", "\033[[0;34;44m", "\033[[0;34;42m", "\033[[0;34;46m",
				"\033[[0;34;41m", "\033[[0;34;45m", "\033[[0;34;43m", "\033[[0;34;47m"
			},
			{ // ConsoleColor::DarkGreen												
				"\033[[0;32;49m",
				"\033[[0;32;40m", "\033[[0;32;44m", "\033[[0;32;42m", "\033[[0;32;46m",
				"\033[[0;32;41m", "\033[[0;32;45m", "\033[[0;32;43m", "\033[[0;32;47m",
				"\033[[0;32;47m", "\033[[0;32;44m", "\033[[0;32;42m", "\033[[0;32;46m",
				"\033[[0;32;41m", "\033[[0;32;45m", "\033[[0;32;43m", "\033[[0;32;47m"
			},
			{ // ConsoleColor::DarkCyan												
				"\033[[0;36;49m",
				"\033[[0;36;40m", "\033[[0;36;44m", "\033[[0;36;42m", "\033[[0;36;46m",
				"\033[[0;36;41m", "\033[[0;36;45m", "\033[[0;36;43m", "\033[[0;36;47m",
				"\033[[0;36;47m", "\033[[0;36;44m", "\033[[0;36;42m", "\033[[0;36;46m",
				"\033[[0;36;41m", "\033[[0;36;45m", "\033[[0;36;43m", "\033[[0;36;47m"
			},
			{ // ConsoleColor::DarkRed													
				"\033[[0;31;49m",
				"\033[[0;31;40m", "\033[[0;31;44m", "\033[[0;31;42m", "\033[[0;31;46m",
				"\033[[0;31;41m", "\033[[0;31;45m", "\033[[0;31;43m", "\033[[0;31;47m",
				"\033[[0;31;47m", "\033[[0;31;44m", "\033[[0;31;42m", "\033[[0;31;46m",
				"\033[[0;31;41m", "\033[[0;31;45m", "\033[[0;31;43m", "\033[[0;31;47m"
			},
			{ // ConsoleColor::DarkPurple												
				"\033[[0;35;49m",
				"\033[[0;35;40m", "\033[[0;35;44m", "\033[[0;35;42m", "\033[[0;35;46m",
				"\033[[0;35;41m", "\033[[0;35;45m", "\033[[0;35;43m", "\033[[0;35;47m",
				"\033[[0;35;47m", "\033[[0;35;44m", "\033[[0;35;42m", "\033[[0;35;46m",
				"\033[[0;35;41m", "\033[[0;35;45m", "\033[[0;35;43m", "\033[[0;35;47m"
			},
			{ // ConsoleColor::DarkYellow												
				"\033[[0;33;49m",
				"\033[[0;33;40m", "\033[[0;33;44m", "\033[[0;33;42m", "\033[[0;33;46m",
				"\033[[0;33;41m", "\033[[0;33;45m", "\033[[0;33;43m", "\033[[0;33;47m",
				"\033[[0;33;47m", "\033[[0;33;44m", "\033[[0;33;42m", "\033[[0;33;46m",
				"\033[[0;33;41m", "\033[[0;33;45m", "\033[[0;33;43m", "\033[[0;33;47m"
			},
			{ // ConsoleColor::DarkWhite												
				"\033[[0;37;49m",
				"\033[[0;37;40m", "\033[[0;37;44m", "\033[[0;37;42m", "\033[[0;37;46m",
				"\033[[0;37;41m", "\033[[0;37;45m", "\033[[0;37;43m", "\033[[0;37;47m",
				"\033[[0;37;47m", "\033[[0;37;44m", "\033[[0;37;42m", "\033[[0;37;46m",
				"\033[[0;37;41m", "\033[[0;37;45m", "\033[[0;37;43m", "\033[[0;37;47m"
			},
			{ // ConsoleColor::Gray														
				"\033[[1;30;49m",
				"\033[[1;30;40m", "\033[[1;30;44m", "\033[[1;30;42m", "\033[[1;30;46m",
				"\033[[1;30;41m", "\033[[1;30;45m", "\033[[1;30;43m", "\033[[1;30;47m",
				"\033[[1;30;47m", "\033[[1;30;44m", "\033[[1;30;42m", "\033[[1;30;46m",
				"\033[[1;30;41m", "\033[[1;30;45m", "\033[[1;30;43m", "\033[[1;30;47m"
			},
			{ // ConsoleColor::Blue														
				"\033[[1;34;49m",
				"\033[[1;34;40m", "\033[[1;34;44m", "\033[[1;34;42m", "\033[[1;34;46m",
				"\033[[1;34;41m", "\033[[1;34;45m", "\033[[1;34;43m", "\033[[1;34;47m",
				"\033[[1;34;47m", "\033[[1;34;44m", "\033[[1;34;42m", "\033[[1;34;46m",
				"\033[[1;34;41m", "\033[[1;34;45m", "\033[[1;34;43m", "\033[[1;34;47m"
			},
			{ // ConsoleColor::Green													
				"\033[[1;32;49m",
				"\033[[1;32;40m", "\033[[1;32;44m", "\033[[1;32;42m", "\033[[1;32;46m",
				"\033[[1;32;41m", "\033[[1;32;45m", "\033[[1;32;43m", "\033[[1;32;47m",
				"\033[[1;32;47m", "\033[[1;32;44m", "\033[[1;32;42m", "\033[[1;32;46m",
				"\033[[1;32;41m", "\033[[1;32;45m", "\033[[1;32;43m", "\033[[1;32;47m"
			},
			{ // ConsoleColor::Cyan														
				"\033[[1;36;49m",
				"\033[[1;36;40m", "\033[[1;36;44m", "\033[[1;36;42m", "\033[[1;36;46m",
				"\033[[1;36;41m", "\033[[1;36;45m", "\033[[1;36;43m", "\033[[1;36;47m",
				"\033[[1;36;47m", "\033[[1;36;44m", "\033[[1;36;42m", "\033[[1;36;46m",
				"\033[[1;36;41m", "\033[[1;36;45m", "\033[[1;36;43m", "\033[[1;36;47m"
			},
			{ // ConsoleColor::Red														
				"\033[[1;31;49m",
				"\033[[1;31;40m", "\033[[1;31;44m", "\033[[1;31;42m", "\033[[1;31;46m",
				"\033[[1;31;41m", "\033[[1;31;45m", "\033[[1;31;43m", "\033[[1;31;47m",
				"\033[[1;31;47m", "\033[[1;31;44m", "\033[[1;31;42m", "\033[[1;31;46m",
				"\033[[1;31;41m", "\033[[1;31;45m", "\033[[1;31;43m", "\033[[1;31;47m"
			},
			{ // ConsoleColor::Purple													
				"\033[[1;35;49m",
				"\033[[1;35;40m", "\033[[1;35;44m", "\033[[1;35;42m", "\033[[1;35;46m",
				"\033[[1;35;41m", "\033[[1;35;45m", "\033[[1;35;43m", "\033[[1;35;47m",
				"\033[[1;35;47m", "\033[[1;35;44m", "\033[[1;35;42m", "\033[[1;35;46m",
				"\033[[1;35;41m", "\033[[1;35;45m", "\033[[1;35;43m", "\033[[1;35;47m"
			},
			{ // ConsoleColor::Yellow													
				"\033[[1;33;49m",
				"\033[[1;33;40m", "\033[[1;33;44m", "\033[[1;33;42m", "\033[[1;33;46m",
				"\033[[1;33;41m", "\033[[1;33;45m", "\033[[1;33;43m", "\033[[1;33;47m",
				"\033[[1;33;47m", "\033[[1;33;44m", "\033[[1;33;42m", "\033[[1;33;46m",
				"\033[[1;33;41m", "\033[[1;33;45m", "\033[[1;33;43m", "\033[[1;33;47m"
			},
			{ // ConsoleColor::White													
				"\033[[1;37;49m",
				"\033[[1;37;40m", "\033[[1;37;44m", "\033[[1;37;42m", "\033[[1;37;46m",
				"\033[[1;37;41m", "\033[[1;37;45m", "\033[[1;37;43m", "\033[[1;37;47m",
				"\033[[1;37;47m", "\033[[1;37;44m", "\033[[1;37;42m",	"\033[[1;37;46m",
				"\033[[1;37;41m", "\033[[1;37;45m", "\033[[1;37;43m", "\033[[1;37;47m"
			},
		};

		return colorCode[state.mForeground][state.mBackground];
	}

	/// Analyzes text returned by LANGULUS_FUNCTION() in order to isolate the	
	/// relevant part for logging																
	///	@param text - the text to scan													
	///	@return the interesting part														
	constexpr Token A::Interface::GetFunctionName(const Token& text) noexcept {
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

		constexpr Token mNamespaceFilter = "Langulus::";
		if (text.starts_with(mNamespaceFilter))
			start += mNamespaceFilter.size();

		return text.substr(start, end - start);
	}

	/// Does nothing, but allows for grouping logging statements in ()			
	/// Example: Logger::Error() << "yeah" << (Logger::Info() << "no")			
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Interface&) noexcept {
		return *this;
	}

	/// Push a command																			
	///	@param c - the command to push													
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Command& c) noexcept {
		Write(c);
		return *this;
	}

	/// Push a foreground color																
	///	@param c - the command to push													
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Color& c) noexcept {
		SetForegroundColor(c);
		return *this;
	}

	/// Push a foreground and background color											
	///	@param c - the state to push														
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const ColorState& c) noexcept {
		SetForegroundColor(c.mForeground);
		SetBackgroundColor(c.mBackground);
		return *this;
	}

	/// Push a number of tabs, scoped or not												
	///	@param t - the tabs to push														
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Tabs& t) noexcept {
		Command(Tab);
		return *this;
	}

	/// Push a number of tabs, scoped or not												
	/// Keeps track of the number of tabs that have been pushed, and then		
	/// automatically untabs when the Tabs object is destroyed						
	///	@param t - [in/out] the tabs to push											
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (Tabs& t) noexcept {
		Command(Tab);
		++t.mTabs;
		return *this;
	}

	/// Stringify anything that has a valid std::formatter							
	///	@param anything - type type to stringify										
	///	@return a reference to the logger for chaining								
	template<CT::Formattable T>
	A::Interface& A::Interface::operator << (const T& anything) const noexcept {
		try {
			Write(::std::format("{}", anything));
		}
		catch (...) {}
		return *this;
	}

	/// A general new-line write function with color									
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<ColorState COLOR, class... T>
	Interface& Line(T&&...arguments) noexcept {
		if constexpr (sizeof...(arguments) > 0)
			Instance << NewLine << COLOR << (... << ::std::forward<T>(arguments));
		return *this;
	}

	/// A general same-line write function with color									
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	Interface& Append(T&&...arguments) noexcept {
		if constexpr (sizeof...(arguments) > 0)
			Instance << ... << ::std::forward<T>(arguments);
		return *this;
	}

	/// Write a section on a new line, tab all consecutive lines, bold it,		
	/// and return the scoped tabs, that will be	untabbed automatically at the	
	/// scope's end																				
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a scoped tab																	
	template<class... T>
	ScopedTabs Section(T&&...arguments) noexcept {
		ScopedTabs tabs;
		Interface << Bold << Info(arguments...) << tabs;
		return Move(tabs);
	}


	template<class... T>
	Interface& Error(T&&...) noexcept {

	}

	template<class... T>
	Interface& Warning(T&&...) noexcept {

	}

	template<class... T>
	Interface& Verbose(T&&...) noexcept {

	}

	template<class... T>
	Interface& Info(T&&...) noexcept {

	}

	template<class... T>
	Interface& Message(T&&...) noexcept {

	}

	template<class... T>
	Interface& Special(T&&...) noexcept {

	}

	template<class... T>
	Interface& Flow(T&&...) noexcept {

	}

	template<class... T>
	Interface& Input(T&&...) noexcept {

	}

	template<class... T>
	Interface& TCP(T&&...) noexcept {

	}

	template<class... T>
	Interface& UDP(T&&...) noexcept {

	}

	template<class... T>
	Interface& OS(T&&...) noexcept {

	}

	template<class... T>
	Interface& Prompt(T&&...) noexcept {

	}

} // namespace Langulus::Logger
