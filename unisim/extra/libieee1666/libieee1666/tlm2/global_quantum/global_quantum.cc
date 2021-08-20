#include "tlm2/global_quantum/global_quantum.h"

namespace tlm {

tlm_global_quantum& tlm_global_quantum::instance()
{
	static tlm_global_quantum singleton;
	return singleton;
}

tlm_global_quantum::~tlm_global_quantum()
{
}

void tlm_global_quantum::set(const sc_core::sc_time& t)
{
	global_quantum = t;
}

const sc_core::sc_time& tlm_global_quantum::get() const
{
	return global_quantum;
}

sc_core::sc_time tlm_global_quantum::compute_local_quantum()
{
	if(global_quantum != sc_core::SC_ZERO_TIME)
	{
		sc_dt::uint64 time_stamp_tr = sc_core::sc_time_stamp().value();
		sc_dt::uint64 global_quantum_tr = sc_core::sc_time_stamp().value();
		sc_dt::uint64 local_quantum_tr = global_quantum_tr - (time_stamp_tr % global_quantum_tr);
		sc_core::sc_time local_quantum(sc_core::sc_get_time_resolution());
		local_quantum *= local_quantum_tr;
		return local_quantum;
	}
	
	return sc_core::SC_ZERO_TIME;
}

tlm_global_quantum::tlm_global_quantum()
	: global_quantum(sc_core::SC_ZERO_TIME)
{
}

} // end of namespace tlm
