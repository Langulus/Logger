///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#include "Logger.hpp"
#include <new>
#include <type_traits>
#include <syncstream>
#include <iostream>
#include <chrono>

using Clock = ::std::chrono::system_clock;
using TimePoint = typename Clock::time_point;
using ZonedTime = ::std::chrono::zoned_time<typename Clock::duration>;

#ifdef _WIN32
	// Shave it..																			
	#define NOGDICAPMASKS
	#define NOVIRTUALKEYCODES
	#define NOWINMESSAGES
	#define NOWINSTYLES
	#define NOSYSMETRICS
	#define NOMENUS
	#define NOICONS
	#define NOKEYSTATES
	#define NOSYSCOMMANDS
	#define NORASTEROPS
	#define NOSHOWWINDOW
	#define OEMRESOURCE
	#define NOATOM
	#define NOCLIPBOARD
	#define NOCOLOR
	#define NOCTLMGR
	#define NODRAWTEXT
	#define NOGDI
	#define NOKERNEL
	#define NOUSER
	//#define NONLS // needed for CP_UTF8
	#define NOMB
	#define NOMEMMGR
	#define NOMETAFILE
	#define NOMINMAX
	#define NOMSG
	#define NOOPENFILE
	#define NOSCROLL
	#define NOSERVICE
	#define NOSOUND
	#define NOTEXTMETRIC
	#define NOWH
	#define NOWINOFFSETS
	#define NOCOMM
	#define NOKANJI
	#define NOHELP
	#define NOPROFILER
	#define NODEFERWINDOWPOS
	#define NOMCX
	#include <Windows.h>
#endif

namespace Langulus::Logger
{

	/// A string used instead of \t, when you push Tab command						
	/// You can still use \t when writing it as string									
	constexpr Token TabString = "|  ";


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
			new (&InstanceBuffer) Interface();
	}

	/// Destroy logger with the last destruction of the initializer				
	Inner::InterfaceInitializer::~InterfaceInitializer() {
		if (--NiftyCounter == 0)
			reinterpret_cast<Interface&>(InstanceBuffer).~Interface();
	}

	/// Logger construction																		
	Interface::Interface() {
		mColorStack.push({DefaultColor, DefaultColor});
		#ifdef _WIN32
			// Enable modern windows terminal ANSI/VT100 escape sequences	
			// A bit slower, unfortunately											
			DWORD currentMode;
			GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &currentMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), currentMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
			// Enable UTF8 code page for windows									
			// Also, don't forget to build this logger with /utf-8			
			SetConsoleOutputCP(CP_UTF8);
			// On Windows 10+ we need buffering or console will get 1 byte	
			// at a time (screwing up utf-8 encoding)								
			// Unfortunately this also means that there is still a chance	
			// of screwing up your output if you fill buffer completely		
			// before next flush. Thankfully, osyncstream flushes anyway	
			setvbuf(stderr, NULL, _IOFBF, 1024);
			setvbuf(stdout, NULL, _IOFBF, 1024);
		#endif
	}

	/// Logger destruction																		
	Interface::~Interface() { }

	/// A thread-safe write to std::cout													
	/// https://www.modernescpp.com/index.php/synchronized-outputstreams			
	void Interface::Write(const Letter& character) const noexcept {
		::std::osyncstream(::std::cout) << character;
	}

	/// A thread-safe write to std::cout													
	/// https://www.modernescpp.com/index.php/synchronized-outputstreams			
	void Interface::Write(const Token& literalText) const noexcept {
		::std::osyncstream(::std::cout) << literalText;
	}

	/// A thread-safe write to std::cout													
	/// https://www.modernescpp.com/index.php/synchronized-outputstreams			
	void Interface::Write(const Text& stdString) const noexcept {
		::std::osyncstream(::std::cout) << stdString;
	}

	/// Generate an exhaustive timestamp in the current system time zone			
	///	@return the timestamp text															
	Text A::Interface::GetAdvancedTime() noexcept {
		try {
			return ::std::format("{:%F %T %Z}", ZonedTime {
				::std::chrono::current_zone(), Clock::now()
			});
		}
		catch (...) {
			return {};
		}
	}

	/// Generate a short timestamp in the current system time zone					
	///	@return the timestamp text															
	Text A::Interface::GetSimpleTime() noexcept {
		try {
			return ::std::format("{:%T}", ZonedTime {
				::std::chrono::current_zone(), Clock::now()
			});
		}
		catch (...) {
			return {};
		}
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
		case Command::Bold:
			Write(Token {"\033[1m"});
			break;
		case Command::Italic:
			Write(Token {"\033[3m"});
			break;
		case Command::Blink:
			Write(Token {"\033[5m"});
			break;
		case Command::Reset:
			Write(Token {"\033[0m"});
			break;
		case Command::Time:
			Write(GetSimpleTime());
			break;
		case Command::ExactTime:
			Write(GetAdvancedTime());
			break;
		case Command::Pop:
			if (mColorStack.size() > 1) {
				mColorStack.pop();
				SetForegroundColor(mColorStack.top().mForeground);
				SetBackgroundColor(mColorStack.top().mBackground);
			}
			break;
		case Command::Push:
			mColorStack.push(mColorStack.top());
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
		// Clear formatting																
		Command(Reset);
		// Add new line																	
		Write('\n');
		// Add timestamp																	
		Command(Time);
		// Add a separator for the timestamp										
		Write(TabString);
		// Tabulate																			
		Tabulate();

		// Dispatch																			
		for (auto attachment : mAttachments)
			attachment->NewLine();
	}

	/// Set foreground color																	
	///	@param c - the color to set														
	void Interface::SetForegroundColor(Color c) noexcept {
		mColorStack.top().mForeground = c;
		Write(GetColorCode(mColorStack.top()));
	}

	/// Set background color																	
	///	@param c - the color to set														
	void Interface::SetBackgroundColor(Color c) noexcept {
		mColorStack.top().mBackground = c;
		Write(GetColorCode(mColorStack.top()));
	}

	/// Insert current tabs																		
	void Interface::Tabulate() const noexcept {
		auto tabs = mTabulator;
		Write(GetColorCode(mTabColor));
		while (tabs > 0) {
			Write(TabString);
			--tabs;
		}
		Write(GetColorCode(mColorStack.top()));
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
