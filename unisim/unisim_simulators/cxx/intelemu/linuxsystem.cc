/*
 *  Copyright (c) 2017,
 *  Commissariat a l'Energie Atomique (CEA)
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

#include <linuxsystem.hh>
#include <unisim/util/os/linux_os/linux.tcc>
#include <unisim/util/os/linux_os/calls.tcc>

LinuxOS::LinuxOS( std::ostream& log,
         unisim::service::interfaces::Registers *regs_if,
         unisim::service::interfaces::Memory<uint32_t> *mem_if,
         unisim::service::interfaces::MemoryInjection<uint32_t> *mem_inject_if
         )
  : unisim::service::interfaces::LinuxOS()
  , linux_impl( log, log, log, regs_if, mem_if, mem_inject_if )
  , exited( false )
  , app_ret_status( -1 )
{}

void
LinuxOS::Setup( std::vector<std::string> const& simargs, std::vector<std::string> const& envs )
{
  // Set up the different linuxlib parameters
  linux_impl.SetVerbose(false);
  
  if (not linux_impl.SetCommandLine(simargs))
    throw 0;
    
  // Set the linuxlib option to set the target environment with the
  // host environment
  linux_impl.SetApplyHostEnvironment(false);
  linux_impl.SetEnvironment(envs);
    
  // Set the binary that will be simulated in the target simulator
  if (not linux_impl.AddLoadFile( simargs[0].c_str() ))
    throw 0;
  
  // Set the system type of the target simulator (should be the same than the
  // binary)
  auto i386_target = new unisim::util::os::linux_os::I386TS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> >( linux_impl );
  linux_impl.SetTargetSystem(i386_target);
    
  linux_impl.SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
  linux_impl.SetStackBase( 0x40000000UL );
  linux_impl.SetMemoryPageSize( 0x1000UL );
  linux_impl.SetUname("Linux" /* sysname */,
                      "localhost" /* nodename */,
                      "3.14.43-unisim" /* release */,
                      "#1 SMP Fri Mar 12 05:23:09 UTC 2010" /* version */,
                      "i386" /* machine */,
                      "localhost" /* domainname */);
  // linux_impl.SetStdinPipeFilename(stdin_pipe_filename.c_str());
  // linux_impl.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
  // linux_impl.SetStderrPipeFilename(stderr_pipe_filename.c_str());

  // now it is time to try to run the initialization of the linuxlib
  if (not linux_impl.Load())
    throw 0;
  
  if (!linux_impl.SetupTarget())
    throw 0;
}

void
LinuxOS::ExecuteSystemCall( int id )
{
  linux_impl.ExecuteSystemCall( id, exited, app_ret_status );
}
