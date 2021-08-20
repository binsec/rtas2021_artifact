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
 
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <hw/console.hh>
#include <hw/pnp.hh>
#include <hw/controller.tcc>
#include <sys/gaislersystem.tcc>
#include <utils/cfmt.hh>
#include <utils/trace.hh>
#include <utils/pfxchan.hh>
#include <arch.hh>
#include <options.hh>

using namespace std;
using SSv8::Trace;
using SSv8::CFmt;

void usage( ostream& _sink, int _code );

int
main( int argc, char *argv[] ) {
  // Parsing options
  int aidx;
  try {
    aidx = Star::Options::parse( argc - 1, argv + 1 ) + 1;
  } catch( Star::Options::Exception_t exc ) {
    switch( exc ) {
    case Star::Options::Error: usage( cerr, 1 ); break;
    case Star::Options::Help:  usage( cout, 0 ); break;
    }
  }
  
  SSv8::Controller<Star::Arch> sparcv8;
  Star::Arch& arch = sparcv8.m_arch;
  
  SSv8::GaislerSystem<Star::Arch> grsystem( arch );
  
  grsystem.load( argv[aidx] );
  
  uint32_t startdisasm = 0, exitpoint = 0;
  int64_t maxinsts = -1;

  if( Star::Options::table[Star::Option::startdisasm].defined() ) {
    startdisasm = Star::Options::table[Star::Option::startdisasm];
    Trace::chan("main") << CFmt( "Starting disassembly at: %#08x\n", startdisasm );
  }
  if( Star::Options::table[Star::Option::exitpoint].defined() ) {
    exitpoint = Star::Options::table[Star::Option::exitpoint];
    Trace::chan("main") << CFmt( "Exitpoint: %#08x\n", exitpoint );
  }
  if( Star::Options::table[Star::Option::maxinsts].defined() ) {
    maxinsts = Star::Options::table[Star::Option::maxinsts];
    Trace::chan("main") << CFmt( "Maxinsts: %#08x\n", maxinsts );
  }
  
  SSv8::Console console;
  SSv8::AHBPNP ahbpnp;
  SSv8::APBPNP apbpnp;
  arch.add( console );
  arch.add( ahbpnp );
  arch.add( apbpnp );
  
  SSv8::PFXChan trapchan( "\x1b[31;1m", cerr );
  Trace::initchan( "trap", &trapchan );

  Trace::chan("main") << "\n*** Run ***" << endl;
  
  // {
  //   char const* img_path = "toto.bmp";
  //   uint32_t img_base_addr = 0xb0000000;
    
  //   ifstream img_source( img_path );
    
  //   if( not img_source.good() ) {
  //     std::cerr << "Can't open image file: " << img_path << endl;
  //     throw 0;
  //   }
    
  //   ptrdiff_t size = img_source.seekg( 0, ios::end ).tellg();
  //   uint8_t text[size];
  //   img_source.seekg( 0, ios::beg ).read( (char*)text, size );
  //   arch.memcpy( img_base_addr, text, size );
  // }
  
  while( arch.m_execute_mode ) {
    if( arch.m_pc == startdisasm ) {
      asm volatile( "start_disasm:" );
      sparcv8.m_disasm = true;
    }
    // Some stop conditions
    if( arch.m_pc == exitpoint )
      { Trace::chan("main") << "[Stop] exitpoint." << endl; break; }
    if( arch.m_instcount == maxinsts )
      { Trace::chan("main") << "[Stop] instruction max." << endl; break; }
    sparcv8.step();
  }
  
  Trace::chan("main") << CFmt( "%-24s%lld\n", "Executed instructions:", arch.m_instcount );

  for( uint32_t asi = 0; asi < 256; ++asi ) {
    int64_t count = arch.m_asi_accesses[asi];
    if( count == 0 ) continue;
    Trace::chan("main") << CFmt( "Mem; asi=%x:         %lld\n", asi, count );
  }
  
  return 0;
}
  
void
usage( ostream& _sink, int _code ) {
  _sink << "Usage: sparcv8 [options] program arguments." << endl;
  if( _code == 0 )
    Star::Options::dump( _sink );
  exit( _code );
}
