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

	/// Convert a logger color to an ANSI/VT100 escape code							
	///	@param foreground - the foreground color										
	///	@param background - the background color										
	///	@return the combined escape sequence											
	constexpr Token Interface::GetColorCode(Color foreground, Color background) noexcept {
		const Token colorCode[ColorCounter][ColorCounter] = {
			{ // ConsoleColor::None														
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

		return colorCode[foreground][background];
	}

	/// Analyzes text returned by LANGULUS_FUNCTION() in order to isolate the	
	/// relevant part for logging																
	///	@param text - the text to scan													
	///	@return the interesting part														
	constexpr Token Interface::GetFunctionName(const Token& text) noexcept {
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

	/// Log numbers																				
	///	@param item - the number to log													
	///	@return a reference to the logger for chaining								
	/*template<Number T>
	LoggerSystem& LoggerSystem::operator << (const T& item) {
		if constexpr (Dense<T>) {
			// Log a number with minimal memory overhead							
			// This is more of an optimization than anything else				
			if constexpr (RealNumber<T>) {
				// Stringify a real number												
				constexpr auto size = std::numeric_limits<T>::max_digits10 * 2;
				char temp[size];
				auto [lastChar, errorCode] = std::to_chars(
					temp, temp + size, item, std::chars_format::general);
				if (errorCode != std::errc())
					return *this;

				while ((*lastChar == '0' || *lastChar == '.') && lastChar > temp) {
					if (*lastChar == '.')
						break;
					--lastChar;
				}

				const auto copied = pcP2N(lastChar) - pcP2N(temp);
				Write(LiteralText(temp, copied));
			}
			else if constexpr (IntegerNumber<T>) {
				// Stringify an integer													
				constexpr auto size = std::numeric_limits<T>::digits10 * 2;
				char temp[size];
				auto [lastChar, errorCode] = std::to_chars(
					temp, temp + size, item);
				if (errorCode != std::errc())
					return *this;

				const auto copied = pcP2N(lastChar) - pcP2N(temp);
				Write(LiteralText(temp, copied));
			}
			else if constexpr (CustomNumber<T>) {
				// Stringify a custom number											
				LoggerSystem::operator << (item.GetBuiltinNumber());
			}
			else LANGULUS_ASSERT("Unsupported number type");
		}
		else {
			// Log sparse number by dereferencing									
			if (!item)
				return *this << ccPush << ccRed << "null" << ccPop;
			*this << *item;
		}

		return *this;
	}*/

} // namespace Langulus::Logger
