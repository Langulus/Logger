///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#include "Logger.hpp"
#include <type_traits>
#include <syncstream>
#include <chrono>
#include <fmt/chrono.h>

using Clock = ::std::chrono::system_clock;

namespace Langulus::Logger
{


	/// Schwarz counter pattern																
	/// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter			
	/// Zero initialized at load time, guaranteed										
	static int NiftyCounter;

	/// Memory for the logger instance														
	static ::std::aligned_storage_t<sizeof(Interface), alignof(Interface)> InstanceBuffer;

	/// The global handle shall point to the buffer										
	Interface& Instance = reinterpret_cast<Interface&> (InstanceBuffer);

	/// Initialize logger together with the first include of the header			
	Inner::InterfaceInitializer::InterfaceInitializer() {
		if (NiftyCounter++ == 0)
			new (&Instance) Interface();
	}

	/// Destroy logger with the last destruction of the initializer				
	Inner::InterfaceInitializer::~InterfaceInitializer() {
		if (--NiftyCounter == 0)
			Instance.~Interface();
	}

	/// Logger construction																		
	Interface::Interface() {
		mStyleStack.push(DefaultStyle);
	}

	/// Logger destruction																		
	Interface::~Interface() { }

	/// Generate an exhaustive timestamp in the current system time zone			
	///	@return the timestamp text															
	Text A::Interface::GetAdvancedTime() noexcept {
		return fmt::format("{:%F %T %Z}", fmt::localtime(Clock::now()));
	}

	/// Generate a short timestamp in the current system time zone					
	///	@return the timestamp text															
	Text A::Interface::GetSimpleTime() noexcept {
		return fmt::format("{:%T}", fmt::localtime(Clock::now()));
	}

	/// Write a character to stdout															
	///	@param character - the character to write										
	void Interface::Write(const Letter& character) const noexcept {
		fmt::print("{}", character);
	}

	/// Write a string view to stdout														
	///	@param literalText - the text to write											
	void Interface::Write(const Token& literalText) const noexcept {
		fmt::print("{}", literalText);
	}

	/// Write a string view to stdout														
	///	@param literalText - the text to write											
	void Interface::Write(const Text& stdString) const noexcept {
		fmt::print("{}", stdString);
	}
	
	/// When using fmt::print(style, mask, ...), the style will be reset after	
	/// message has been written, and I don't want that to happen					
	///	@param style - the style to set													
	inline void FmtPrintStyle(const Style& style) {
		// Always reset before a style change										
		fmt::print("{}", "\x1b[0m");
		
		if (style.has_emphasis()) {
			auto emphasis = fmt::detail::make_emphasis<Letter>(style.get_emphasis());
			fmt::print("{}", emphasis);
		}

		if (style.has_foreground()) {
			auto foreground = fmt::detail::make_foreground_color<Letter>(style.get_foreground());
			fmt::print("{}", foreground);
		}

		if (style.has_background()) {
			auto background = fmt::detail::make_background_color<Letter>(style.get_background());
			fmt::print("{}", background);
		}
	}

	/// Change the foreground color															
	///	@param c - the color																	
	void Interface::Write(const Color& c) noexcept {
		auto& style = mStyleStack.top();
		const auto oldStyle = style;
		if (c != Color::DefaultColor) {
			// Create a new foreground color style									
			style = fmt::fg(static_cast<fmt::terminal_color>(c));
		}
		else {
			// Reset foreground color													
			style = {};
		}

		// Restore background color and emphasis									
		if (oldStyle.has_background())
			style |= fmt::bg(oldStyle.get_background());
		if (oldStyle.has_emphasis())
			style |= oldStyle.get_emphasis();

		FmtPrintStyle(style);
	}

	/// Change the emphasis																		
	///	@param e - the emphasis																
	void Interface::Write(const Emphasis& e) noexcept {
		#if defined(_WIN32)
			// Boldness is ignored on Windows, because it screws with color
			if (e == Emphasis::Bold)
				return;
		#endif

		auto& style = mStyleStack.top();
		style |= static_cast<fmt::emphasis>(e);
		FmtPrintStyle(style);
	}

	/// Change the style																			
	///	@param s - the style																	
	void Interface::Write(const Style& s) noexcept {
		auto& style = mStyleStack.top();
		style = s;
		FmtPrintStyle(style);
	}

	/// Execute a logger command																
	///	@param c - the command to execute												
	void Interface::Write(const Command& c) noexcept {
		switch (c) {
		case Command::Clear:
			Write(Token {"\033c"});
			break;
		case Command::NewLine:
			NewLine();
			break;
		case Command::Invert:
			Write(Token {"\033[7m"});
			break;
		case Command::Reset:
			Write(Token {"\x1b[0m"});
			break;
		case Command::Time:
			Write(GetSimpleTime());
			break;
		case Command::ExactTime:
			Write(GetAdvancedTime());
			break;
		case Command::Pop:
			if (mStyleStack.size() > 1)
				mStyleStack.pop();
			else
				mStyleStack.top() = DefaultStyle;
			FmtPrintStyle(mStyleStack.top());
			break;
		case Command::Push:
			mStyleStack.push(mStyleStack.top());
			break;
		case Command::Tab:
			++mTabulator;
			break;
		case Command::Untab:
			if (mTabulator > 0)
				--mTabulator;
			break;
		}

		// Dispatch																			
		for (auto attachment : mAttachments)
			attachment->Write(c);
	}

	/// Remove formatting, add a new line, add a timestamp and tabulate			
	void Interface::NewLine() const noexcept {
		// Clear formatting, add new line, add a simple time stamp			
		FmtPrintStyle(TimeStampStyle);
		fmt::print("\n{:%T}| ", fmt::localtime(Clock::now()));

		// Tabulate																			
		Tabulate();

		// Dispatch																			
		for (auto attachment : mAttachments)
			attachment->NewLine();
	}

	/// Insert current tabs and apply last style from the stack						
	void Interface::Tabulate() const noexcept {
		if (!mTabulator) {
			FmtPrintStyle(mStyleStack.top());
			return;
		}

		auto tabs = mTabulator;
		FmtPrintStyle(TabStyle);
		while (tabs) {
			Write(TabString);
			--tabs;
		}
		FmtPrintStyle(mStyleStack.top());
	}

	/// Attach another logger, such as an html file										
	///	@attention the logger doesn't have ownership of the attachment			
	///	@param receiver - the logger to attach											
	void Interface::Attach(A::Interface* receiver) noexcept {
		mAttachments.push_back(receiver);
	}

	/// Dettach a logger 																		
	///	@attention the logger doesn't have ownership of the attachment			
	///	@param receiver - the logger to dettach										
	void Interface::Dettach(A::Interface* receiver) noexcept {
		mAttachments.remove(receiver);
	}

} // namespace Langulus::Logger
