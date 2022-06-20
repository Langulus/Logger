#include "include/PCFW.Logger.hpp"
#include <PCFW.OS.hpp>
#include <ctime>
#include <iomanip>

namespace PCFW::Logger
{

	LoggerSystem* LogInstance = nullptr;
	constexpr LiteralText TabString = "|  ";

	/// Initialize the logger																	
	bool pcInitLogger() {
		static bool guard = false;
		if (guard)
			return true;
		guard = true;
		::PCFW::Logger::LoggerSystem::CreateInstance();
		pcLogVerbose << ccGreen << "PCFW::Logger initialized";
		return true;
	}

	/// Put a character to console output													
	///	@param c - character to print														
	///	@return number of printed characters											
	template<Character T>
	pcptr pcPutC(const T c) {
		if constexpr (Same<T, char> || Same<T, char8>)
			putchar(c);
		else
			putwchar(c);
		return 1;
	}

	/// Put a null-terminated string to console output									
	///	@param c - null terminated string												
	///	@return number of printed characters											
	template<Character T>
	pcptr pcPutS(const T* c) {
		int written = 0;
		if constexpr (Same<T, char> || Same<T, char8>)
			written += fputs(c, stdout);
		else
			written += fputws(c, stdout);
		return written > 0 ? pcptr(written) : 0;
	}

	/// Extract local time																		
	///	@param time - standard time														
	///	@return the localized time															
	std::tm pcLocalTime(const std::time_t* time) {
		#ifdef __STDC_WANT_SECURE_LIB__
			std::tm result;
			localtime_s(&result, time);
			return result;
		#else
			return *std::localtime(time);
		#endif
	}

	/// Print a formatted string to console to output									
	///	@param f - format mask																
	///	@param ... - virtual arguments													
	///	@return number of printed characters											
	template <Character T, typename ... Args>
	pcptr pcPrintF(const T* f, const Args& ... args) {
		int written = 0;
		#ifdef __STDC_WANT_SECURE_LIB__
			if constexpr (Same<T, char> || Same<T, char8>)
				written += printf_s(f, args...);
			else
				written += wprintf_s(f, args...);
		#else
			if constexpr (Same<T, char> || Same<T, char8>)
				written += printf(f, args...);
			else
				written += wprintf(f, args...);
		#endif
		return written > 0 ? pcptr(written) : 0;
	}

	/// Create the singleton																	
	LoggerSystem* LoggerSystem::CreateInstance() {
		if (LogInstance != nullptr)
			return LogInstance;
		static LoggerSystem LocalLogInstance = LoggerSystem();
		return &LocalLogInstance;
	}

	/// Get the singleton																		
	LoggerSystem* LoggerSystem::GetInstance() {
		return LogInstance;
	}

	/// Logger creation																			
	LoggerSystem::LoggerSystem()
		: mColorCache{} {
		LogInstance = this;

		#if LANGULUS_OS_IS(WINDOWS)
			// Enable modern windows terminal ANSI/VT100 escape sequences	
			DWORD currentMode;
			GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &currentMode);
			SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), currentMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		#endif
	}

	/// Scoped tabulator destruction															
	ScopedTab::~ScopedTab() noexcept {
		while (mTabs > 0) {
			--mTabs;
			pcLog << ConsoleCommand::ccUntab;
		}
	}

	/// Remove formatting, add a new line, add a timestamp and tabulate			
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::NewLine() noexcept {
		// Clear formatting																
		Command(ConsoleCommand::ccReset);
		// Add new line																	
		Write('\n');
		// Add timestamp																	
		Command(ConsoleCommand::ccTime);
		Write("| ");
		// Tabulate																			
		Tabulate();
		if (mExtension)
			mExtension->NewLine();
		return *this;
	}

	/// Insert tabs																				
	void LoggerSystem::Tabulate() noexcept {
		auto tabs = mTabulator;
		Write(GetColorCode(ccGray, ccDefaultColor));
		while (tabs > 0) {
			Write(TabString);
			--tabs;
		}
	}

	/// Set a new foreground color to the current or last color cache entry		
	///	@param color - color to set to background										
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::SetForegroundColor(const ConsoleColor color) noexcept {
		auto index = std::min(mColorIndex, ColorCacheSize - 1);
		mColorCache[index].mForeground = color;
		SetColor(index);
		return *this;
	}

	/// Set a new background color to the current or last color cache entry		
	///	@param color - color to set to background										
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::SetBackgroundColor(const ConsoleColor color) noexcept {
		auto index = std::min(mColorIndex, ColorCacheSize - 1);
		mColorCache[index].mBackground = color;
		SetColor(index);
		return *this;
	}

	/// Set a new color to the console, using the internal color cache			
	///	@param index - the index inside the color cache								
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::SetColor(const pcptr index) noexcept {
		Write(GetColorCode(mColorCache[index].mForeground, mColorCache[index].mBackground));
		if (mExtension)
			mExtension->SetColor(index);
		return *this;
	}

	/// Push literal to the logger															
	///	@param text - the text to write													
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::Write(const LiteralText& text) noexcept {
		pcPrintF("%.*s", text.size(), text.begin());
		if (mExtension)
			mExtension->Write(text);
		return *this;
	}

	/// Push null-terminated string to the logger										
	///	@param text - the null-terminated text to write								
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::Write(const char* text) noexcept {
		pcPrintF("%.*s", std::strlen(text), text);
		if (mExtension)
			mExtension->Write(text);
		return *this;
	}

	/// Push a character to the logger														
	///	@param text - the character to write											
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::Write(const char text) noexcept {
		pcPutC(text);
		if (mExtension)
			mExtension->Write(text);
		return *this;
	}
	
	/// Push and execute a new console command											
	///	@param c - the console command to execute										
	///	@return a reference to the logger for chaining								
	LoggerSystem& LoggerSystem::Command(const ConsoleCommand c) noexcept {
		switch (c) {
		case ConsoleCommand::ccClear:
			Write("\033c");
			break;
		case ConsoleCommand::ccInvert:
			Write("\033[7m");
			break;
		case ConsoleCommand::ccBold:
			Write("\033[1m");
			break;
		case ConsoleCommand::ccItalic:
			Write("\033[3m");
			break;
		case ConsoleCommand::ccBlink:
			Write("\033[5m");
			break;
		case ConsoleCommand::ccReset:
			Write("\033[0m");
			break;
		case ConsoleCommand::ccTime: {
			std::time_t result = std::time(nullptr);
			char buffer[256];
			const auto time = pcLocalTime(&result);
			//auto count = std::strftime(buffer, 256, "%D-%T", &time);
			auto count = std::strftime(buffer, 256, "%T", &time);
			Write(LiteralText(buffer, count));
		} break;
		case ConsoleCommand::ccPop:
			if (mColorIndex > 0)
				--mColorIndex;
			SetColor(std::min(mColorIndex, ColorCacheSize - 1));
			break;
		case ConsoleCommand::ccPush:
			++mColorIndex;
			SetColor(std::min(mColorIndex, ColorCacheSize - 1));
			break;
		case ConsoleCommand::ccTab:
			++mTabulator;
			break;
		case ConsoleCommand::ccUntab:
			if (mTabulator > 0)
				--mTabulator;
			break;
		}

		if (mExtension)
			mExtension->Command(c);
		return *this;
	}

	/// Attach a receiver log to the logger. Unsafe.									
	void LoggerSystem::Attach(LoggerSystem* receiver) noexcept {
		if (receiver)
			return;

		auto log = this;
		while (log->mExtension)
			log = log->mExtension;
		
		log->mExtension = receiver;
	}

	/// Dettach a receiver log. Unsafe.														
	void LoggerSystem::Dettach(LoggerSystem* receiver) noexcept {
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
	LoggerSystem& LoggerSystem::operator << (const ConsoleColor item) {
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
	}

} // namespace PCFW::Logger
