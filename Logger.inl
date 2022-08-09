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
	inline ScopedTabs::~ScopedTabs() noexcept {
		while (mTabs > 0) {
			--mTabs;
			Instance << Command::Untab;
		}
	}

	/// Analyzes text returned by LANGULUS(FUNCTION) in order to isolate the	
	/// name part for logging																	
	///	@param text - the text to scan													
	///	@return the interesting part														
	constexpr Token A::Interface::GetFunctionName(const Token& text, const Token& omit) noexcept {
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
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (A::Interface&) noexcept {
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
		Write(c);
		return *this;
	}

	/// Push an emphasis																			
	///	@param e - the emphasis to push													
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Emphasis& e) noexcept {
		Write(e);
		return *this;
	}

	/// Push a foreground and background color											
	///	@param c - the state to push														
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Style& c) noexcept {
		Write(c);
		return *this;
	}

	/// Push a number of tabs																	
	///	@param t - the tabs to push														
	///	@return a reference to the logger for chaining								
	inline A::Interface& A::Interface::operator << (const Tabs& t) noexcept {
		auto tabs = ::std::max(1, t.mTabs);
		while (tabs) {
			Write(Command::Tab);
			--tabs;
		}

		return *this;
	}

	/// Push a number of tabs																	
	/// Keeps track of the number of tabs that have been pushed, and then		
	/// automatically untabs when the Tabs object is destroyed						
	///	@param t - [in/out] the tabs to push											
	///	@return a reference to the logger for chaining								
	inline ScopedTabs A::Interface::operator << (Tabs&& t) noexcept {
		auto tabs = ::std::max(1, t.mTabs);
		while (tabs) {
			Write(Command::Tab);
			--tabs;
		}

		++t.mTabs;
		return ScopedTabs {t.mTabs};
	}

	/// Stringify anything that has a valid std::formatter							
	///	@param anything - type type to stringify										
	///	@return a reference to the logger for chaining								
	template<Formattable T>
	A::Interface& A::Interface::operator << (const T& anything) noexcept {
		Write(fmt::format("{}", anything));
		return *this;
	}

	/// A general new-line write function with color									
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Line(T&&...arguments) noexcept {
		if constexpr (sizeof...(arguments) > 0) {
			Instance.NewLine();
			return (Instance << ... << ::std::forward<T>(arguments));
		}
		else return Instance;
	}

	/// A general same-line write function with color									
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Append(T&&...arguments) noexcept {
		if constexpr (sizeof...(arguments) > 0)
			(Instance << ... << ::std::forward<T>(arguments));
		return Instance;
	}

	/// Write a section on a new line, tab all consecutive lines, bold it,		
	/// and return the scoped tabs, that will be	untabbed automatically at the	
	/// scope's end																				
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a scoped tab																	
	template<class... T>
	ScopedTabs Section(T&&...arguments) noexcept {
		return Line(Color::White, Emphasis::Bold, ::std::forward<T>(arguments)..., Tabs {});
	}

	/// Write a new-line fatal error															
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Fatal(T&&...arguments) noexcept {
		return Line(Color::DarkRed, "FATAL ERROR: ", ::std::forward<T>(arguments)...);
	}

	/// Write a new-line error																	
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Error(T&&...arguments) noexcept {
		return Line(Color::Red, "ERROR: ", ::std::forward<T>(arguments)...);
	}

	/// Write a new-line warning																
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Warning(T&&...arguments) noexcept {
		return Line(Color::DarkYellow, "WARNING: ", ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with verbose information										
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Verbose(T&&...arguments) noexcept {
		return Line(Color::DarkGray, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with information													
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Info(T&&...arguments) noexcept {
		return Line(Color::Gray, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with a personal message											
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Message(T&&...arguments) noexcept {
		return Line(Color::White, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with special text													
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Special(T&&...arguments) noexcept {
		return Line(Color::Purple, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with flow information											
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Flow(T&&...arguments) noexcept {
		return Line(Color::DarkCyan, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line on user input														
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Input(T&&...arguments) noexcept {
		return Line(Color::Blue, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with network message												
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Network(T&&...arguments) noexcept {
		return Line(Color::Yellow, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with a message from OS											
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) OS(T&&...arguments) noexcept {
		return Line(Color::DarkBlue, ::std::forward<T>(arguments)...);
	}

	/// Write a new-line with an input prompt												
	///	@tparam ...T - a sequence of elements to log (deducible)					
	///	@return a reference to the logger for chaining								
	template<class... T>
	decltype(auto) Prompt(T&&...arguments) noexcept {
		return Line(Color::Green, ::std::forward<T>(arguments)...);
	}

} // namespace Langulus::Logger
