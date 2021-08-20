//STF

/* *************************************************************************
 *
 *  The contents of this file are subject to New BSD Licence; This license
 *  is available in the LICENSE file in the toplevel directory, or at
 *  http://www.opensource.org/licenses/bsd-license.php
 *
 * *************************************************************************
 */

#ifndef __UNISIM_COMPONENT_CLM_DEBUG_SVG_MEMREQ_INTERFACE_HH__
#define __UNISIM_COMPONENT_CLM_DEBUG_SVG_MEMREQ_INTERFACE_HH__

#include <string>
#include <inttypes.h>
//#include "memreq.h"
#include <unisim/component/clm/interfaces/memreq.hh>

//class Instruction;

//namespace full_system {
//namespace plugins {
namespace unisim {
namespace component {
namespace clm {
namespace debug {


  using unisim::component::clm::interfaces::memreq_dataless;


template<class INSTRUCTION>
class SVGmemreqInterface
{public:
  
  //  --- 

  virtual void register_cpu(const string & name) = 0;
  virtual void register_cache(const string & name) = 0;
  virtual void register_bus(const string & name) = 0;
  virtual void register_mem(const string & name) = 0;
  
  //  --- 

  virtual void add_cache_to_cpu(uint64_t cycle, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  virtual void add_mem_to_bus(uint64_t cycle, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  virtual void add_cac_to_bus(uint64_t cycle, int cpu_number, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  virtual void add_bus_to_mem(uint64_t cycle, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  virtual void add_bus_to_cac(uint64_t cycle, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  virtual void add_cpu_to_cac(uint64_t cycle, const string &target, const string &label, const memreq_dataless<INSTRUCTION> &mr) = 0;
  
  virtual void add_box_info(uint64_t cycle, const string &name, const string &info) = 0;
  
  //  --- 

  virtual void set_mincycle(int value) = 0; 
  virtual void set_step(int value) = 0;
  virtual void set_skewing(bool value) = 0;
  virtual void monitor_address(uint32_t address) = 0;

};


} // end of namespace debug
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif

//EOSTF
