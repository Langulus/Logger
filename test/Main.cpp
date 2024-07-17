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
   Logger::ToHTML logFile1 {"logfile.htm"};
   Logger::AttachDuplicator(&logFile1);

   // Duplicate any logging messages to an external txt file            
   Logger::ToTXT logFile2 {"logfile.txt"};
   Logger::AttachDuplicator(&logFile2);

   Catch::Session session;
   return session.run(argc, argv);
}
