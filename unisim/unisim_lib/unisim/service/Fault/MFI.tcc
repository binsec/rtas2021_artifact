/*
 * MFI.tcc
 *
 *  Created on: 8 juin 2014
 *      Author: houssem
 */


#include "unisim/kernel/kernel.hh"
#include "unisim/service/interfaces/Fault_Injector.hh"
#include "unisim/service/Fault/MFI.hh"

#include <inttypes.h>
#include <cstdio>

namespace unisim{
namespace service{
namespace fault{


using unisim::kernel::Service;


// ********

template<class PHYSICAL_ADDR>
Memory_Fault_Injector<PHYSICAL_ADDR>::Memory_Fault_Injector(const char *_name, Object *parent):
     Object(_name, parent, "Emulate Fault Injection")
	, Service<Fault_Injector<PHYSICAL_ADDR> >(_name, parent)
	, fault_export("fault-export", this)
	,name(_name)
	,age(0)
	,param_age("aging", this, age)
	,fault_report(false)
	,param_fault_report("fault-report", this, fault_report)
	,next_fault(0)
	,fos("trace.xml")


{
	for (int i=0; i<Tab_SIZE; i++) {
		tabTFaut[i] = NULL;
	}
	//std::cout << "je suis constructeur" << std::endl;
	fos << "<Root>" << std::endl;
}

template<class PHYSICAL_ADDR>
Memory_Fault_Injector<PHYSICAL_ADDR>::~Memory_Fault_Injector()
{

	displayAllFault(fos);
	fos << std::endl;

	fos << "</Root>" << std::endl;

	fos.flush();
	fos.close();

	for (int i=0; i<Tab_SIZE; i++) {
		delete tabTFaut[i]; tabTFaut[i] = NULL;
	}

}

template<class PHYSICAL_ADDR>
float Memory_Fault_Injector<PHYSICAL_ADDR>::evalPe(int X, int B)
{
	return  exp(pow (X, B));
}

template<class PHYSICAL_ADDR>
void Memory_Fault_Injector<PHYSICAL_ADDR>::inject_fault(uint8_t* val, uint8_t size)
{
	uint8_t* error_value;
	error_value = new uint8_t[size];
	memset(error_value, 0, size);
	memcpy (val, error_value, size);
}

template<class PHYSICAL_ADDR>
void Memory_Fault_Injector<PHYSICAL_ADDR>::compute_fault(uint8_t* val, uint8_t size, uint8_t*& error_value)
{
	error_value = new uint8_t[size];
	memset(error_value, 0, size);
}

//template<class PHYSICAL_ADDR>
//std::ostream& Memory_Fault_Injector<PHYSICAL_ADDR>::displayTab(std::ostream& os, uint8_t* tab, int size)
//					{
//						os << "0x";
//						for (int i=0; i<size; i++)
//							{
//									os << std::hex << (unsigned int) tab[i];
//							}
//
//								return os;
//					}


template<class PHYSICAL_ADDR>
void Memory_Fault_Injector<PHYSICAL_ADDR>::error_provoq(uint8_t* content, PHYSICAL_ADDR phy_addr, uint8_t size)

{
	const int seuil = 80;

	if (true /* ? est-ce que je suis dans  ? */)
	{
		float pe = evalPe(age , 1);
		if (pe < seuil)
		{
			tabTFaut[next_fault] = new TFaut(this);

			tabTFaut[next_fault]->setPe(pe);

			tabTFaut[next_fault]->setData(new uint8_t[size]);


			memcpy(tabTFaut[next_fault]->getData(), content, size);

			tabTFaut[next_fault]->setAddr(phy_addr);

			tabTFaut[next_fault]->setSize(size);

			// cas v2
			// calcule de la faute

			uint8_t* val_ptr;

			compute_fault(content, size, val_ptr);

			// injection de la faute precedement calculée
			memcpy(content, val_ptr, size);

			tabTFaut[next_fault]->setErrorVal(new uint8_t[size]);

			memcpy(tabTFaut[next_fault]->getErrorVal(), content, size);

			next_fault = next_fault + 1;

			if (next_fault >= Tab_SIZE) {
				// TODO: vider le tableau dans un fichier et (next_fault=0)

				fos << displayAllFault(fos) << std::endl;

				next_fault = 0;
			}

			// fin cas v2

		}
	}

}

// ********








}  // end fault
}  // end service
}  // end unisim
