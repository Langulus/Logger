///																									
/// Langulus::Logger																				
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#include "Logger.hpp"
#include <ctime>
#include <iomanip>
#include <new>
#include <type_traits>
#include <syncstream>
#include <iostream>
#include <format>

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

	/// Scoped tabulator destruction															
	ScopedTab::~ScopedTab() noexcept {
		while (mTabs > 0) {
			--mTabs;
			Instance << Untab;
		}
	}


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
	void Interface::Write(const char8_t& character) const noexcept {
		std::osyncstream(std::cout) << character;
	}

	void Interface::Write(const Token& literalText) const noexcept {
		std::osyncstream(std::cout) << literalText;
	}

	void Interface::Write(const ::std::string& stdString) const noexcept {
		std::osyncstream(std::cout) << stdString;
	}

	/// We can use this to convert from UTC to the local time zone as follows	
	auto LocalTime(::std::chrono::system_clock::time_point const tp) {
		return ::std::chrono::zoned_time {::std::chrono::current_zone(), tp};
	}

	/// The zoned_time value can be converted to a string using the new text	
	/// formatting library and the std::format() function								
	inline ::std::string AdvancedTime(auto tp) {
		return ::std::format("{:%F %T %Z}", tp);
	}

	/// The zoned_time value can be converted to a string using the new text	
	/// formatting library and the std::format() function								
	inline ::std::string SimpleTime(auto tp) {
		return ::std::format("{%T}", tp);
	}

	/// Execute a logger command																
	void Interface::Write(const Command& c) noexcept {
		switch (c) {
		case Command::Clear:
			Write(Token {"\033c"});
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
			Write(SimpleTime(LocalTime(::std::chrono::system_clock::now())));
			break;
		case Command::ExactTime:
			Write(AdvancedTime(LocalTime(::std::chrono::system_clock::now())));
			break;
		case Command::Pop:
			mColorStack.pop();
			SetForegroundColor(mColorStack.top().mForeground);
			SetBackgroundColor(mColorStack.top().mBackground);
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

		if (mExtension)
			mExtension->Command(c);
	}

	void Interface::NewLine() const noexcept {

	}

	void Interface::SetForegroundColor(Color) noexcept {

	}

	void Interface::SetBackgroundColor(Color) noexcept {

	}

	void Interface::Tabulate() const noexcept {

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
		Write(Token {"| "});
		// Tabulate																			
		Tabulate();


		if (mExtension)
			mExtension->NewLine();
		return *this;
	}

	/// Insert tabs																				
	void Interface::Tabulate() const noexcept {
		auto tabs = mTabulator;
		Write(GetColorCode(Gray, DefaultColor));
		while (tabs > 0) {
			Write(TabString);
			--tabs;
		}
	}

	/// Attach a receiver log to the logger. Unsafe.									
	void Interface::Attach(Interface* receiver) noexcept {
		if (receiver)
			return;

		auto log = this;
		while (log->mExtension)
			log = log->mExtension;
		
		log->mExtension = receiver;
	}

	/// Dettach a receiver log. Unsafe.														
	void Interface::Dettach(Interface* receiver) noexcept {
		if (receiver)
			return;

		auto log = this;
		while (log->mExtension && log->mExtension != receiver)
			log = log->mExtension;

		if (!log->mExtension)
			return;
	}

	/// Colorize																					
	///	@return a reference to the logger for chaining								
	/*LoggerSystem& LoggerSystem::operator << (const ConsoleColor item) {
		return SetForegroundColor(item);
	}

	/// Push a command																			
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const ConsoleCommand item) {
		return Command(item);
	}

	/// Push a byte count																		
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const ByteCount& item) {
		const auto asLiteral = item.GetLiteral();
		return Write(LiteralText(asLiteral));
	}

	/// Push literal																				
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const LiteralText& item) {
		return Write(item);
	}

	/// Push std::string																			
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const std::string& item) {
		return Write(item.c_str());
	}

	/// Push literal																				
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const char* item) {
		return Write(item);
	}

	/// Push character																			
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const char item) {
		return Write(item);
	}

	/// Push boolean																				
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const bool item) {
		return Write(item? LiteralText("yes") : LiteralText("no"));
	}

	/// Push hash																					
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const Hash& item) {
		return Write(LiteralText(pcToHex(item)));
	}

	/// Operator for using logging calls as push operators, too						
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const LoggerSystem&) {
		return *this;
	}

	/// Push a tabulator. Modifies internal counter for tabulator					
	/// This allows the use of scoped tabulators that untab on function exit	
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (Tab& tabulator) {
		Command(ConsoleCommand::ccTab);
		++tabulator.mTabs;
		return *this;
	}

	/// Log a void pointer by writing a hex												
	///	@param pointer - the pointer to log												
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::operator << (const void* pointer) {
		if (!pointer)
			return (*this) << ccPush << ccRed << "null" << ccPop;
		return (*this) << Hash(pcP2N(pointer));
	}*/

} // namespace Langulus::Logger
