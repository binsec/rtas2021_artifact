/*
 * s12xeetx.cc
 *
 *  Created on: 10 oct. 2011
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12xeetx.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xeetx.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 32, uint32_t, 8, DEFAULT_PAGE_SIZE, false>;
template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 32, uint64_t, 8, DEFAULT_PAGE_SIZE, false>;
template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 64, uint32_t, 4, DEFAULT_PAGE_SIZE, false>;
template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 64, uint64_t, 4, DEFAULT_PAGE_SIZE, false>;
template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 128, uint32_t, 2, DEFAULT_PAGE_SIZE, false>;
template class S12XEETX<DEFAULT_CMD_PIPELINE_SIZE, 128, uint64_t, 2, DEFAULT_PAGE_SIZE, false>;


} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim
