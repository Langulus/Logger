///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include "Logger.hpp"

using namespace Langulus;
using namespace Langulus::Logger;


/// Create a plain text file duplicator/redirector                            
///   @param filename - the relative filename of the log file                 
ToTXT::ToTXT(const TextView& filename) : mFilename {filename} {
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   if (not mFile)
      throw std::runtime_error {"Can't open log file"};
   WriteHeader();
}

ToTXT::~ToTXT() {
   WriteFooter();
   mFile.close();
}

/// Write text                                                                
///   @param text - the text to append to the file                            
void ToTXT::Write(const TextView& text) const noexcept {
   mFile << text;
   mFile.flush();
}

/// Plain text logging ignores all styles                                     
///   @param style - the style to set                                         
void ToTXT::Write(Style) const noexcept {
   LANGULUS(NOOP);
}

/// Remove formatting, add a new line, add a timestamp and tabulate           
void ToTXT::NewLine() const noexcept {
   Write("\n");
   Write(GetSimpleTime());
   Write("|");
   if (Instance.CurrentIntent != Intent::Ignore)
      Write(Instance.IntentStyle[int(Instance.CurrentIntent)].prefix);
   else
      Write(" ");
   Write("| ");

   auto tabs = Instance.GetTabs();
   if (tabs) {
      while (tabs) {
         Write(Instance.TabString);
         --tabs;
      }
   }
}

/// Clear the log file                                                        
void ToTXT::Clear() const noexcept {
   mFile.close();
   mFile.open(mFilename, std::ios::out | std::ios::trunc);
   WriteHeader();
}

/// Write file header - just a timestamp                                      
void ToTXT::WriteHeader() const {
   Write("Log started - ");
   Write(GetAdvancedTime());
   Write("\n\n");
}

/// Write file footer - just a timestamp                                      
void ToTXT::WriteFooter() const {
   Write("\n\nLog ended - ");
   Write(GetAdvancedTime());
}
