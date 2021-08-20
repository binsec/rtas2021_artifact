/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef SYS_GAISLERSYSTEM_HPP
#define SYS_GAISLERSYSTEM_HPP

#include <sys/gaislersystem.hh>
#include <sys/elfloader.hh>
#include <utils/cfmt.hh>
#include <utils/trace.hh>
#include <iostream>
#include <fstream>

namespace SSv8 {
  
  template <class t_Arch_t>
  void
  GaislerSystem<t_Arch_t>::load( char const* _imgpath ) {
    // Loading image
    Trace::chan("sys") << "\n*** Image load ***" << endl;
    {
      ifstream imgsrc( _imgpath );
      if( not imgsrc.good() ) {
        std::cerr << "Can't open image file: " << _imgpath << endl;
        throw OpenError;
      }
      ptrdiff_t size = imgsrc.seekg( 0, ios::end ).tellg();
      uint8_t text[size];
      imgsrc.seekg( 0, ios::beg ).read( (char*)text, size );
      
      SSv8::Image image( text, size );
      
      Trace::chan("sys") << "\nProgramHeaders:\n";
      for( uint32_t idx = 0; idx < image.phnum(); ++idx ) {
        SSv8::ProgramHeader programhdr( image, idx );
        switch( programhdr.type() ) {
        case SSv8::ProgramHeader::null: break;
        case SSv8::ProgramHeader::load: {
          Trace::chan("sys") << CFmt( "*%2d {offset: %#x", idx, programhdr.offset() );
          Trace::chan("sys") << CFmt( ", vaddr: %#x", programhdr.vaddr() );
          Trace::chan("sys") << CFmt( ", paddr: %#x", programhdr.paddr() );
          Trace::chan("sys") << CFmt( ", filesz: %#x", programhdr.filesz() );
          Trace::chan("sys") << CFmt( ", memsz: %#x", programhdr.filesz() );
          Trace::chan("sys") << CFmt( ", flags: %#x", programhdr.flags() );
          Trace::chan("sys") << CFmt( ", align: %#x }\n", programhdr.align() );
          
          SSv8::BigEndianReader prog( image, programhdr.offset(), programhdr.filesz() );
          m_arch.memcpy( programhdr.vaddr(), prog.m_base, prog.m_size );
        } break;
        default:
          cerr << "Not a statically compiled program (p_type: " << programhdr.type() << ")." << endl;
          throw ImageError;
        }
      }
      
      uint32_t textbase = image.entrypoint();
      Trace::chan("sys") << CFmt( "Entrypoint, trapbase: %#08x\n", textbase );
      m_arch.pc( textbase );
      m_arch.m_tbr = textbase;
    
      // Setup (equivalent to prom.S)
      m_arch.impl()    = 15;
      m_arch.ver()     = 3;
      m_arch.icc()     = 0;
      m_arch.ec()      = 0;
      m_arch.ef()      = 1;
      m_arch.pil()     = 0;
      m_arch.s()       = 1;
      m_arch.ps()      = 1;
      m_arch.et()      = 1;
      m_arch.cwp()     = 0;
    
      m_arch.m_wim     = 2;
    
      m_arch.m_gpr[30] = Loc::RAMSTART + Loc::RAMSIZE - 0x20;
      m_arch.m_gpr[14] = m_arch.m_gpr[30] - 0x60;
    }
  }  

};

#endif // SYS_GAISLERSYSTEM_HPP
