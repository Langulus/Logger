#pragma once
#include "../LoggerSystem.hpp"

namespace PCFW::Logger
{
   PC_API_LOG bool pcInitLogger();
}

#define pcLog                 (*Logger::LoggerSystem::GetInstance())
#define pcLogUnknown          (pcLog.NewLine())
#define pcLogInfo             (pcLog.NewLine() << Logger::ConsoleColor::ccWhite)
#define pcLogVerbose          (pcLog.NewLine() << Logger::ConsoleColor::ccGray)
#define pcLogWarning          (pcLog.NewLine() << Logger::ConsoleColor::ccDarkYellow)
#define pcLogError            (pcLog.NewLine() << Logger::ConsoleColor::ccRed)
#define pcLogMessage          (pcLog.NewLine() << Logger::ConsoleColor::ccCyan)
#define pcLogSpecial          (pcLog.NewLine() << Logger::ConsoleColor::ccPurple)
#define pcLogFlow             (pcLog.NewLine() << Logger::ConsoleColor::ccDarkCyan)
#define pcLogInput            (pcLog.NewLine() << Logger::ConsoleColor::ccDarkGreen)
#define pcLogTCP              (pcLog.NewLine() << Logger::ConsoleColor::ccYellow)
#define pcLogUDP              (pcLog.NewLine() << Logger::ConsoleColor::ccYellow)
#define pcLogOS               (pcLog.NewLine() << Logger::ConsoleColor::ccBlue)
#define pcLogChoice           (pcLog.NewLine() << Logger::ConsoleColor::ccGreen)

#define pcLogFunc					(pcLog << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncUnknown		(pcLogUnknown << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncInfo			(pcLogInfo << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncVerbose		(pcLogVerbose << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncWarning		(pcLogWarning << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncError			(pcLogError << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncMessage		(pcLogMessage << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncSpecial		(pcLogSpecial << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncFlow			(pcLogFlow << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncInput			(pcLogInput << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncTCP				(pcLogTCP << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncUDP				(pcLogUDP << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncOS				(pcLogOS << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
#define pcLogFuncChoice			(pcLogChoice << Logger::LoggerSystem::GetFunctionName(LANGULUS_FUNCTION()) << ": ")
