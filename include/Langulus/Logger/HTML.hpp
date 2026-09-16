///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Logger.hpp"
#include <fstream>


namespace Langulus::Logger
{
   ///                                                                        
   /// Generates HTML code from logging messages. Can be used both as         
   /// duplicator or redirector. Colors and styles are consistent with        
   /// console output. Use it like this:                                      
   ///    Logger::ToHTML logRedirect("outputfile.htm");                       
   ///    Logger::AttachRedirector(&logRedirect);                             
   ///    <redirect all logging to an HTML file>                              
   ///    Logger::DettachRedirector(&logRedirect);                            
   ///    <you can log once again in the console>                             
   ///                                                                        
   struct ToHTML final : Interface {
   private:
      ::std::string mFilename;
      mutable ::std::ofstream mFile;
      mutable bool mScopeOpened;
      mutable Style mLastWrittenStyle;

      void WriteHeader() const;
      void WriteFooter() const;

   public:
      LANGULUS_API(LOGGER)  ToHTML(::std::string_view const& filename);
      LANGULUS_API(LOGGER) ~ToHTML();

      LANGULUS_API(LOGGER) void Write(::std::string_view const&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;
      LANGULUS_API(LOGGER) auto GetFilename() const noexcept -> ::std::string_view;
   };
}
