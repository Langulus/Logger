///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Logger/TXT.hpp>

using namespace Langulus;
using namespace Langulus::Logger;


/// Create a plain text file duplicator/redirector                            
///   @param filename - the relative filename of the log file                 
ToTXT::ToTXT(::std::string_view const& filename) : mFilename {filename} {
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
void ToTXT::Write(::std::string_view const& text) const noexcept {
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
   Write(GlobalState.mIntentStyle[GlobalState.GetCurrentIntent()].prefix);

   auto tabs = GlobalState.GetTabs();
   if (tabs) {
      while (tabs) {
         Write(GlobalState.mTabString);
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

/// Returns the output filename                                               
auto ToTXT::GetFilename() const noexcept -> ::std::string_view {
   return mFilename;
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
