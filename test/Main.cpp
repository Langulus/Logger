///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include "Main.hpp"
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>


int main(int argc, char* argv[]) {
   // Duplicate any logging messages to an external HTML file           
   Logger::ToHTML logFile {"logfile.htm"};
   Logger::AttachDuplicator(&logFile);

   Catch::Session session;
   return session.run(argc, argv);
}
