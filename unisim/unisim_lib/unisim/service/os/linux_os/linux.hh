/*
 *  Copyright (c) 2011,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_HH__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/linux_os.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/util/os/linux_os/linux.hh>

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
class Linux
  : public unisim::kernel::Service< unisim::service::interfaces::LinuxOS >
  , public unisim::kernel::Service< unisim::service::interfaces::Blob<ADDRESS_TYPE> >
  , public unisim::kernel::Client<  unisim::service::interfaces::Memory<ADDRESS_TYPE> >
  , public unisim::kernel::Client<  unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> >
  , public unisim::kernel::Client<  unisim::service::interfaces::Registers>
{
 public:
  
  typedef unisim::util::os::linux_os::Linux<ADDRESS_TYPE, PARAMETER_TYPE> LinuxImpl;
  typedef typename LinuxImpl::TargetSystem TargetSystem;
  
  /* Exported services */
  unisim::kernel::ServiceExport<unisim::service::interfaces::LinuxOS>
      linux_os_export_;
  unisim::kernel::ServiceExport<unisim::service::interfaces::Blob<ADDRESS_TYPE> >
      blob_export_;

  /* Imported services */
  unisim::kernel::ServiceImport<
      unisim::service::interfaces::Memory<ADDRESS_TYPE> > memory_import_;
  unisim::kernel::ServiceImport<
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> >
      memory_injection_import_;
  unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>
      registers_import_;

  /* Constructor/Destructor */
  Linux(const char *name, unisim::kernel::Object *parent = 0);
  virtual ~Linux();

  /* Service methods */
  virtual void OnDisconnect();
  virtual bool BeginSetup();
  virtual bool Setup(unisim::kernel::ServiceExportBase *srv_export);
  virtual bool EndSetup();

  /* Service interface methods */
  virtual void ExecuteSystemCall(int id);
  //virtual bool Load();

  virtual const unisim::util::blob::Blob<ADDRESS_TYPE> *GetBlob() const;
  
  virtual void SetupTargetSystem() = 0;
  
protected:
  /* the logger and its verbose option */
  unisim::kernel::logger::Logger logger_;
  bool verbose_;
  unisim::kernel::variable::Parameter<bool> param_verbose_;
  
  /* DWARF options */
  bool parse_dwarf_;
  bool debug_dwarf_;
  std::string dwarf_to_html_output_directory_;
  std::string dwarf_to_xml_output_filename_;
  unisim::kernel::variable::Parameter<bool> param_parse_dwarf_;
  unisim::kernel::variable::Parameter<bool> param_debug_dwarf_;
  unisim::kernel::variable::Parameter<std::string> param_dwarf_to_html_output_directory_;
  unisim::kernel::variable::Parameter<std::string> param_dwarf_to_xml_output_filename_;
  
  /* stdin/stdout/stderr pipes */
  std::string stdin_pipe_filename;
  std::string stdout_pipe_filename;
  std::string stderr_pipe_filename;
  unisim::kernel::variable::Parameter<std::string> param_stdin_pipe_filename;
  unisim::kernel::variable::Parameter<std::string> param_stdout_pipe_filename;
  unisim::kernel::variable::Parameter<std::string> param_stderr_pipe_filename;
  
  /* the linux library */
  LinuxImpl* linuxlib_;

  unisim::util::endian::endian_type endianness_;
  unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endianness_;
  ADDRESS_TYPE memory_page_size_;
  unisim::kernel::variable::Parameter<ADDRESS_TYPE> param_memory_page_size_;
  ADDRESS_TYPE stack_base_;
  unisim::kernel::variable::Parameter<ADDRESS_TYPE> param_stack_base_;
  std::string binary_;
  unisim::kernel::variable::Parameter<std::string> param_binary_;
  unsigned int argc_;
  unisim::kernel::variable::Parameter<unsigned int> param_argc_;
  std::vector<std::string> argv_;
  std::vector<unisim::kernel::variable::Parameter<std::string> *> param_argv_;
  bool apply_host_cmd_line_;
  unisim::kernel::variable::Parameter<bool> param_apply_host_cmd_line_;
  bool apply_host_environment_;
  unisim::kernel::variable::Parameter<bool> param_apply_host_environment_;
  unsigned int envc_;
  unisim::kernel::variable::Parameter<unsigned int> param_envc_;
  std::vector<std::string> envp_;
  std::vector<unisim::kernel::variable::Parameter<std::string> *> param_envp_;
  std::string utsname_sysname_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_sysname_;
  std::string utsname_nodename_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_nodename_;
  std::string utsname_release_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_release_;
  std::string utsname_version_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_version_;
  std::string utsname_machine_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_machine_;
  std::string utsname_domainname_;
  unisim::kernel::variable::Parameter<std::string> param_utsname_domainname_;
  std::string hwcap_;
  unisim::kernel::variable::Parameter<std::string> param_hwcap_;
};

} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_HH__
