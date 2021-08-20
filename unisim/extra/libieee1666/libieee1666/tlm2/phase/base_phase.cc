#include "tlm2/phase/base_phase.h"

namespace tlm {

std::vector<const char *> tlm_phase::extended_phase_name;
unsigned int tlm_phase::last_extended_phase_value = END_RESP;

tlm_phase::tlm_phase()
	: value(UNINITIALIZED_PHASE)
{
}

tlm_phase::tlm_phase(unsigned int v)
	: value((v > END_REQ) ? UNINITIALIZED_PHASE : (tlm_phase_enum) v)
{
}

tlm_phase::tlm_phase(const tlm_phase_enum& v)
	: value(v)
{
}

tlm_phase& tlm_phase::operator = (const tlm_phase_enum& v)
{
	value = v;
	return *this;
}

tlm_phase::operator unsigned int () const
{
	return value;
}

} // end of namespace tlm
