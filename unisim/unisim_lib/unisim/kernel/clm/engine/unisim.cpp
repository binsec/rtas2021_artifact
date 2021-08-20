/******************************************************************************* 
 *                                   BSD LICENSE 
 *******************************************************************************
 *  Copyright (c) 2006, CEA, INRIA and Universite Paris Sud
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without modification,  
 *  are permitted provided that the following conditions are met: 
 *  
 *   - Redistributions of source code must retain the above copyright notice, this  
 *     list of conditions and the following disclaimer.  
 *  
 *   - Redistributions in binary form must reproduce the above copyright notice,  
 *     this list of conditions and the following disclaimer in the documentation  
 *     and/or other materials provided with the distribution.  
 *  
 *   - Neither the name of CEA, INRIA and Universite Paris Sud nor the names of its 
 *     contributors may be used to endorse or promote products derived from this 
 *     software without specific prior written permission.  
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 *  DISCLAIMED.  
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY  
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *  
 ****************************************************************************** */ 

#include "unisim.h"


/*obsoleted by recent changes?*/
#if 0 
std::list<fraternite_module*> fraternite_module::module_hierarchy; 

void fraternite_module::run_module_body() 
{ for(std::list<fraternite_module*>::iterator i = module_hierarchy.begin(); i!= module_hierarchy.end(); i++)
  { (*i)->module_body();
  }
}
#endif

module *Unisim_Sensitive_Array::__current_array_module = NULL;
int Unisim_Sensitive_Array::__current_array_size = 0;

/*
template <>
Unisim_Prim_Out<bool>& Unisim_Prim_Out<bool>::operator =(Signal_Status &s)
{ if (s.known)
  { (*(fsc_prim_out<bool> *)this) = (bool)s.value;
  }
  return *this;
}
*/
