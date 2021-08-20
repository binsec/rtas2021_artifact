/*
 * BFI.tcc
 *
 *  Created on: 11 juil. 2014
 *      Author: houssem
 */

#include "unisim/service/Fault/BFI.hh"
#include <cstring>

namespace unisim{
namespace service{
namespace fault{


template<class PHYSICAL_ADDR>
Bus_Fault_Injector<PHYSICAL_ADDR>::Bus_Fault_Injector(const char *name, Object *parent):
Object(name, parent)
,Service<unisim::service::interfaces::BFault_Injector<PHYSICAL_ADDR> >(name, parent)
,bus_fault_export("bus_fault_export", this)
, address_fault(false)
, data_fault(false)
, param_data_fault("param-data-fault", this, data_fault)
, param_address_fault("param-address-fault", this, address_fault)


{
}

template<class PHYSICAL_ADDR>
Bus_Fault_Injector<PHYSICAL_ADDR>::~Bus_Fault_Injector()
{
}


template<class PHYSICAL_ADDR>
void Bus_Fault_Injector<PHYSICAL_ADDR>::fault_injection(uint8_t* data_ptr, sc_dt::uint64& address, unsigned int& lenght)
{

	if (data_fault)
	{
		uint8_t *error_val = new uint8_t[lenght];

		memcpy(error_val, data_ptr, lenght);

		int const mask = 0b00000011;

		*error_val = (*data_ptr) & mask;

		memcpy(data_ptr, error_val, lenght);
	}


	if (address_fault)
	{
		if ((address >= (sc_dt::uint64)0x0400) && (address <= (sc_dt::uint64)0x040F))
		{
			address = 0x040F | 0xF000 ;
		}

		if((address >= (sc_dt::uint64)0x000800) && (address <= (sc_dt::uint64)0xFFFF))
		{
			address = 0x0402;
		}
	}

}

template<class PHYSICAL_ADDR>
float Bus_Fault_Injector<PHYSICAL_ADDR>::evalPe(int X, int)
{
	float n = 0;
	return n;
}

template<class PHYSICAL_ADDR>
void Bus_Fault_Injector<PHYSICAL_ADDR>::compute_fault(uint8_t *val, uint8_t size, uint8_t* &error_value)
{

}

template<class PHYSICAL_ADDR>
void Bus_Fault_Injector<PHYSICAL_ADDR>::error_provoq(uint8_t* content, PHYSICAL_ADDR adress, uint8_t size)
{


}



}//end of unisim
}//end of service
}//end of fault



