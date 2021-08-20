/*
 * MFI.hh
 *
 *  Created on: 5 juin 2014
 *      Author: houssem
 */

#ifndef MFI_HH_
#define MFI_HH_


#include "unisim/service/interfaces/Fault_Injector.hh"
#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include <cmath>
#include <inttypes.h>
#include <fstream>

namespace unisim{
namespace service{
namespace fault{


using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::Fault_Injector;
using unisim::kernel::variable::Parameter;

//template <class PHYSICAL_ADDR> class Memory_Fault_Injector;

template <class PHYSICAL_ADDR>

class Memory_Fault_Injector : public  Service<Fault_Injector<PHYSICAL_ADDR> >
	{

		public:
		Memory_Fault_Injector (const char *name, Object *parent = 0);

		virtual ~Memory_Fault_Injector();

		ServiceExport<unisim::service::interfaces::Fault_Injector<PHYSICAL_ADDR> > fault_export;

		virtual float evalPe(int X, int B);
		virtual void inject_fault(uint8_t* val,uint8_t size);
		virtual void compute_fault(uint8_t* val,uint8_t size, uint8_t* &error_value);
		virtual void error_provoq(uint8_t* content,PHYSICAL_ADDR address, uint8_t size);


private:
		const char* name;

		//uint8_t* recov_data;
		//PHYSICAL_ADDR recov_addr;

		int age;
		Parameter<int> param_age;
		bool fault_report;
		Parameter<bool> param_fault_report;

		std::ostream& displayTab(std::ostream& os, uint8_t* tab, int size) {
			os << "0x";
			for (int i=0; i<size; i++)
				{
						os << std::hex << (unsigned int) tab[i];
				}

					return os;

		}

		static const int Tab_SIZE = 100;

		class TFaut {
		public:
			TFaut(const Memory_Fault_Injector* _parent): parent(_parent), addr(0), size(0), data(NULL), error_val(NULL), pe(0) {}
			~TFaut() {
				if(data) {delete data; data = NULL; }
				if(error_val) { delete error_val; error_val = NULL; }
			}

			std::ostream& displayFault(std::ostream& os)
			{
				os << "  <Fault addr=\"0x" << std::hex << addr << "\" size=\"" << std::dec << (unsigned int) size << "\" Pe=\"" << pe << "\">" << std::endl;
				os << "    <InitialData>";
				((Memory_Fault_Injector*) parent)->displayTab(os, data, size);
				os << "</InitialData>";
				os << std::endl;
				os << "    <CorruptedData>";
				((Memory_Fault_Injector*) parent)->displayTab(os, error_val, size);
				os << "</CorruptedData>";
				os << std::endl;
				os << "  </Fault>";

				return os;
			}

			PHYSICAL_ADDR getAddr() const {
				return addr;
			}

			void setAddr(PHYSICAL_ADDR addr) {
				this->addr = addr;
			}

			uint8_t* getData() const {
				return data;
			}

			void setData(uint8_t* data) {
				this->data = data;
			}

			uint8_t* getErrorVal() const {
				return error_val;
			}

			void setErrorVal(uint8_t* errorVal) {
				error_val = errorVal;
			}

			double getPe() const {
				return pe;
			}

			void setPe(double pe) {
				this->pe = pe;
			}

			uint8_t getSize() const {
				return size;
			}

			void setSize(uint8_t size) {
				this->size = size;
			}



		private:
			const Memory_Fault_Injector* parent;
			uint8_t* data;
			uint8_t* error_val;
			PHYSICAL_ADDR addr;
			uint8_t size;
			double pe;

		};

		struct TFaut* tabTFaut[Tab_SIZE];

		int next_fault;
		std::ofstream fos;

		std::ostream& displayAllFault(std::ostream& os)
		{


			for (int i=0; i<next_fault; i++)
			{
				(tabTFaut[i])->displayFault(os);
				os << std::endl;
			}


			return os;
		}
 };

}  // end fault
}  // end service
}  // end unisim


#endif /* MFI_HH_ */
