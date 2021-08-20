#include <tlm>
#include "tlm2/interface/direct_memory.h"

namespace tlm {

void tlm_dmi::init()
{
	dmi_ptr = 0;
	start_address = 0;
	end_address = -1;
	granted_access = DMI_ACCESS_NONE;
	read_latency = sc_core::SC_ZERO_TIME;
	write_latency = sc_core::SC_ZERO_TIME;	
}

unsigned char *tlm_dmi::get_dmi_ptr() const
{
	return dmi_ptr;
}

sc_dt::uint64 tlm_dmi::get_start_address() const
{
	return start_address;
}

sc_dt::uint64 tlm_dmi::get_end_address() const
{
	return end_address;
}

sc_core::sc_time tlm_dmi::get_read_latency() const
{
	return read_latency;
}

sc_core::sc_time tlm_dmi::get_write_latency() const
{
	return write_latency;
}

tlm_dmi::dmi_access_e tlm_dmi::get_granted_access() const
{
	return granted_access;
}

bool tlm_dmi::is_none_allowed() const
{
	return granted_access == DMI_ACCESS_NONE;
}

bool tlm_dmi::is_read_allowed() const
{
	return granted_access == DMI_ACCESS_READ;
}

bool tlm_dmi::is_write_allowed() const
{
	return granted_access == DMI_ACCESS_WRITE;
}

bool tlm_dmi::is_read_write_allowed() const
{
	return granted_access == DMI_ACCESS_READ_WRITE;
}

void tlm_dmi::set_dmi_ptr(unsigned char* p)
{
	dmi_ptr = p;
}

void tlm_dmi::set_start_address(sc_dt::uint64 addr)
{
	start_address = addr;
}

void tlm_dmi::set_end_address(sc_dt::uint64 addr)
{
	end_address = addr;
}

void tlm_dmi::set_read_latency(sc_core::sc_time t)
{
	read_latency = t;
}

void tlm_dmi::set_write_latency(sc_core::sc_time t)
{
	write_latency = t;
}

void tlm_dmi::set_granted_access(tlm_dmi::dmi_access_e v)
{
	granted_access = v;
}

void tlm_dmi::allow_none()
{
	granted_access = DMI_ACCESS_NONE;
}

void tlm_dmi::allow_read()
{
	granted_access = (dmi_access_e)(granted_access | DMI_ACCESS_READ);
}

void tlm_dmi::allow_write()
{
	granted_access = (dmi_access_e)(granted_access | DMI_ACCESS_READ_WRITE);
}

void tlm_dmi::allow_read_write()
{
	granted_access = DMI_ACCESS_READ_WRITE;
}

} // end of namespace tlm
