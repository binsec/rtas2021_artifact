/*
 * BFI.hh
 *
 *  Created on: 9 juil. 2014
 *      Author: houssem
 */

#ifndef BFI_HH_
#define BFI_HH_


#include "unisim/service/interfaces/Fault_Injector.hh"
#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "systemc.h"


namespace unisim{
namespace service{
namespace fault{

using namespace sc_core;
using namespace sc_dt;

using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::Fault_Injector;
using unisim::service::interfaces::BFault_Injector;
using unisim::kernel::variable::Parameter;

template<class PHYSICAL_ADDR>
class Bus_Fault_Injector: public Service<unisim::service::interfaces::BFault_Injector<PHYSICAL_ADDR> >

{
public:

	Bus_Fault_Injector(const char *name, Object *parent=0);
	virtual ~Bus_Fault_Injector();

	ServiceExport<BFault_Injector<PHYSICAL_ADDR> > bus_fault_export;

	/********BFI method************/

   void fault_injection(uint8_t* data_ptr, sc_dt::uint64& address, unsigned int& lenght);



	////******* fault injector methods**********//////

	virtual float evalPe(int X, int B);
	virtual void compute_fault(uint8_t* val,uint8_t size, uint8_t* &error_value);
	virtual void error_provoq(uint8_t* content, PHYSICAL_ADDR address,  uint8_t size);


private:

	bool address_fault;
	bool data_fault;
	Parameter<bool> param_data_fault;
	Parameter<bool> param_address_fault;


};






} // end of fault
} // end of service
} // end of unisim



#endif /* BFI_HH_ */
