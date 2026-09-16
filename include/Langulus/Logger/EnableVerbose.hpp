///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Logger.hpp>

#if defined(LglsVerboseEnabled) or defined(LglsVerbose) or defined(LglsVerboseScoped)
#error "Verbosity has already been enabled, did you forget to include <Langulus/Logger/DisableVerbose.hpp> at end of file, where <Langulus/Logger/EnableVerbose.hpp> was included?"
#endif

#define LglsVerboseEnabled 1
#define LglsVerbose(...) ::Langulus::Logger::Info(__VA_ARGS__)
#define LglsVerboseScoped(...) const auto scope = ::Langulus::Logger::InfoScoped(__VA_ARGS__)
