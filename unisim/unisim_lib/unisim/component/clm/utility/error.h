/***************************************************************************
   error.h  -  Defines INFO and ERROR macros
 ***************************************************************************/

/* *****************************************************************************  
                                    BSD LICENSE  
********************************************************************************  
Copyright (c) 2006, INRIA
Authors: Sylvain Girbal (sylvain.girbal@inria.fr)
Foundings: Partly founded with HiPEAC foundings
All rights reserved.  
  
Redistribution and use in source and binary forms, with or without modification,   
are permitted provided that the following conditions are met:  
  
 - Redistributions of source code must retain the above copyright notice, this   
   list of conditions and the following disclaimer.   
  
 - Redistributions in binary form must reproduce the above copyright notice,   
   this list of conditions and the following disclaimer in the documentation   
   and/or other materials provided with the distribution.   
   
 - Neither the name of the UNISIM nor the names of its contributors may be   
   used to endorse or promote products derived from this software without   
   specific prior written permission.   
     
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE   
DISCLAIMED.   
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,   
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY   
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING   
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,   
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
***************************************************************************** */

#ifndef __BASE_ERROR_H__
#define __BASE_ERROR_H__

#include <iostream>
#include <iomanip>

#define error( format, ...) error_func( __LINE__, __FUNCTION__, __FILE__, format, ## __VA_ARGS__)
/*
void error_func(const int line, const char *funcname, const char *filename, const char *format, ...)
{ static char buffer[1024];
  va_list arg;
  va_start(arg, format);
  vsprintf( buffer, format, arg);
  fprintf(stderr, "%s:%s:%d\tERROR:%s", filename, funcname, line, buffer);
  fflush(stderr);
  va_end(arg);
  exit(-1);
}
*/
ofstream info_stream;

// Message macros for modules
//#define INFO if(!info_stream.is_open()) info_stream.open("info.log",ios_base::trunc); else info_stream << left << setw(3) << timestamp() << " " << setw(8) << name() << ": " 
#define INFO cerr << left << setw(3) << timestamp() << " " << setw(10) << fsc_object::name() << ": " 
#define ERROR cerr << __FILE__ << ":" << __LINE__ << ": \e[1;31mRuntime error in module \e[1;36m" << fsc_object::name() << "\e[1;31m at timestamp \e[1;36m" << timestamp() << "\e[0m.\nERROR: "

#endif
