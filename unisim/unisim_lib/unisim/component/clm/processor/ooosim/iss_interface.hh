/***************************************************************************
                iss_interface.h  -  interface to the ISS
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_ISS_INTERFACE_HH__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_ISS_INTERFACE_HH__

#include <unisim/component/cxx/processor/powerpc/powerpc.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/config.hh>
#include <unisim/component/clm/processor/ooosim/config.hh>

/*
namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim {
*/
//  using unisim::component::cxx::processor::powerpc::MPC7447AConfig;

  using unisim::component::cxx::processor::powerpc::Operation;
  using unisim::component::cxx::processor::powerpc::CPU;
  using unisim::component::cxx::processor::powerpc::Decoder;


  using unisim::component::cxx::processor::powerpc::operand_type_t;
  using unisim::component::cxx::processor::powerpc::function_t;

  using unisim::component::cxx::processor::powerpc::GPR_T;
  using unisim::component::cxx::processor::powerpc::FPR_T;
  using unisim::component::cxx::processor::powerpc::CRF_T;
  using unisim::component::cxx::processor::powerpc::CRB_T;
  //using unisim::component::cxx::processor::powerpc::FPSCRF_T;
  //using unisim::component::cxx::processor::powerpc::SR_T;
  //using unisim::component::cxx::processor::powerpc::BO_T;
  //using unisim::component::cxx::processor::powerpc::BD_T;
  //using unisim::component::cxx::processor::powerpc::IMM_T;
  using unisim::component::cxx::processor::powerpc::CR_T;
  using unisim::component::cxx::processor::powerpc::FPSCR_T;
  using unisim::component::cxx::processor::powerpc::LR_T;
  using unisim::component::cxx::processor::powerpc::CTR_T;
  using unisim::component::cxx::processor::powerpc::XER_T;
  using unisim::component::cxx::processor::powerpc::NONE_REGISTER;


  typedef unisim::component::clm::processor::ooosim::MyPowerPcConfig CPU_CONFIG;

  /*
using full_system::processors::powerpc::Operation;
using full_system::processors::powerpc::CPU;
using full_system::processors::powerpc::Decoder;
  */

/*
} // end of namespace ooosim
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim
*/
#endif
