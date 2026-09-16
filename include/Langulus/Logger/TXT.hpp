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
   /// Generates plain text file from logging messages. Can be used both as   
   /// duplicator or redirector. Strips all styling. Use it like this:        
   ///    Logger::ToTXT logRedirect("outputfile.txt");                        
   ///    Logger::AttachRedirector(&logRedirect);                             
   ///    <redirect all logging to a plain text file>                         
   ///    Logger::DettachRedirector(&logRedirect);                            
   ///    <you can log once again in the console>                             
   ///                                                                        
   struct ToTXT final : Interface {
   private:
      ::std::string mFilename;
      mutable ::std::ofstream mFile;

      void WriteHeader() const;
      void WriteFooter() const;

   public:
      LANGULUS_API(LOGGER)  ToTXT(::std::string_view const& filename);
      LANGULUS_API(LOGGER) ~ToTXT();

      LANGULUS_API(LOGGER) void Write(::std::string_view const&) const noexcept;
      LANGULUS_API(LOGGER) void Write(Style) const noexcept;
      LANGULUS_API(LOGGER) void NewLine() const noexcept;
      LANGULUS_API(LOGGER) void Clear() const noexcept;
      LANGULUS_API(LOGGER) auto GetFilename() const noexcept -> ::std::string_view;
   };
}
