/*
 * Fault_Injector.hh
 *
 *  Created on: 5 juin 2014
 *      Author: houssem
 */

#ifndef FAULT_INJECTOR_HH_
#define FAULT_INJECTOR_HH_


#include <unisim/service/interfaces/interface.hh>
#include <inttypes.h>
#include "systemc.h"


namespace unisim{
namespace service{
namespace interfaces{

using namespace sc_dt;

 template <class ADRESS>
class Fault_Injector : public ServiceInterface
{
	public:

	virtual float evalPe(int X, int B) = 0;
	virtual void compute_fault(uint8_t* val,uint8_t size, uint8_t* &error_value)= 0;
	virtual void error_provoq(uint8_t* content,ADRESS address,  uint8_t size) = 0;

};

 template <class ADRESS>
class BFault_Injector : public Fault_Injector<ADRESS>
{
	public:

	 virtual void fault_injection(uint8_t* data_ptr, sc_dt::uint64& address, unsigned int& lenght) = 0;

};


}
}
}




#endif /* FAULT_INJECTOR_HH_ */
