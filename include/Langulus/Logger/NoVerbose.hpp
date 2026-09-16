///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#if defined(LglsVerboseEnabled) or defined(LglsVerbose) or defined(LglsVerboseScoped)
#error "Verbosity has already been defined, did you forget to include <Langulus/Logger/DisableVerbose.hpp> at end of file, where <Langulus/Logger/EnableVerbose.hpp> was included?"
#endif

#define LglsVerboseEnabled 0
#define LglsVerbose(...)
#define LglsVerboseScoped(...)
