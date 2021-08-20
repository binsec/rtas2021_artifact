#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/cpu.tcc>

#include <unisim/component/clm/processor/ooosim_mc/config.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template class CPU<unisim::component::clm::processor::ooosim::MyPowerPcConfig>;

}//powerpc
}//processor
}//cxx
}//component
}//unisim
