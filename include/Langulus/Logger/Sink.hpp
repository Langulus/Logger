///                                                                           
/// Langulus::Logger                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Logger.hpp"


namespace Langulus::Logger
{
   ///                                                                        
   /// Consumes all logging messages, so that they don't interfere with       
   /// rendering inside the console.                                          
   /// Use it like this:                                                      
   ///    Logger::AttachRedirector(&MessageSinkInstance);                     
   ///    <suppresses all logging in console>                                 
   ///    Logger::DettachRedirector(&MessageSinkInstance);                    
   ///    <you can log once again>                                            
   ///                                                                        
   struct MessageSink final : Interface {
      void Write(::std::string_view const&) const noexcept {}
      void Write(Style) const noexcept {}
      void NewLine() const noexcept {}
      void Clear() const noexcept {}
      auto GetFilename() const noexcept -> ::std::string_view;
   };
}