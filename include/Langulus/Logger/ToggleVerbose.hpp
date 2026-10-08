///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Logger.hpp>

#if defined(LglsVerbose) or defined(LglsVerboseScoped)
#error "Verbosity has already been enabled, did you forget to include <Langulus/Logger/DisableVerbose.hpp> at end of file, where <Langulus/Logger/EnableVerbose.hpp> was included?"
#endif

#if LglsVerboseEnabled == 1
   #define LglsVerbose(STYLE,...)         ::Langulus::Logger::STYLE(__VA_ARGS__)
   #define LglsVerboseScoped(STYLE,...)   const auto _ = ::Langulus::Logger::STYLE##Scoped(__VA_ARGS__)
   #define LglsVerboseSection(STYLE,...)  const auto _ = ::Langulus::Logger::STYLE##Section(__VA_ARGS__)
#else
   #define LglsVerbose(...)         LANGULUS(NOOP)
   #define LglsVerboseScoped(...)   LANGULUS(NOOP)
   #define LglsVerboseSection(...)  LANGULUS(NOOP)
#endif