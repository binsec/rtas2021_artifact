/*
 * S12XFTMX.cc
 *
 *  Created on: 28 january 2013
 *      Author: rnouacer
 */

#include <unisim/component/tlm2/processor/hcs12x/s12xftmx.hh>
#include <unisim/component/tlm2/processor/hcs12x/s12xftmx.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {

template class S12XFTMX<32, uint32_t, 8, DEFAULT_PAGE_SIZE, false>;
template class S12XFTMX<32, uint64_t, 8, DEFAULT_PAGE_SIZE, false>;
template class S12XFTMX<64, uint32_t, 4, DEFAULT_PAGE_SIZE, false>;
template class S12XFTMX<64, uint64_t, 4, DEFAULT_PAGE_SIZE, false>;
template class S12XFTMX<128, uint32_t, 2, DEFAULT_PAGE_SIZE, false>;
template class S12XFTMX<128, uint64_t, 2, DEFAULT_PAGE_SIZE, false>;


} // end of hcs12x
} // end of processor
} // end of tlm2
} // end of component
} // end of unisim
