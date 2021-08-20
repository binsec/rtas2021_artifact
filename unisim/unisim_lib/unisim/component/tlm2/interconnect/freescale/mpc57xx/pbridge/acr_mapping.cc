/*
 *  Copyright (c) 2018,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/kernel/variable/variable.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge/pbridge.hh>

namespace unisim {
namespace kernel {
namespace variable {

template <>
Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::Variable(const char *_name, Object *_object, unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping &_storage, Type type, const char *_description)
	: VariableBase(_name, _object, type, _description)
	, storage(&_storage)
{
	Initialize();
}

template <> unsigned int Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::GetBitSize() const { return 0; }

template <> Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator bool () const { return false; }
template <> Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator long long () const { return 0; }
template <> Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator unsigned long long () const { return 0; }
template <> Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator double () const { return 0; }

template <> Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator std::string () const
{ 
	std::stringstream buf;
	buf << (*storage);
	return buf.str();
}

template <> VariableBase& Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator = (bool value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator = (long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator = (unsigned long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator = (double value) { return *this;}

template <> VariableBase& Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::operator = (const char *value)
{ 
	if(IsMutable())
	{
		unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping tmp;
		
		std::string str(value);
		std::size_t first_digit_pos = str.find_first_of("0123456789");
		
		if(first_digit_pos != std::string::npos)
		{
			std::string prefix(str.substr(0, first_digit_pos));
			
			if((prefix == "pacr") || (prefix == "opacr"))
			{
				if(prefix == "opacr") tmp.off_platform = true;
				else if(prefix == "pacr") tmp.off_platform = false;
			
				if(first_digit_pos != std::string::npos)
				{
					std::stringstream value_sstr(str.substr(first_digit_pos));
					if(value_sstr >> tmp.reg_num)
					{
						unsigned int num_acr_regs = tmp.off_platform ? 256 : 64;
						tmp.valid = (tmp.reg_num < num_acr_regs);
					}
				}
			}
		}
		
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> const char *Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::GetDataTypeName() const
{
	return "unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping";
}

template <> VariableBase::DataType Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>::GetDataType() const
{
	return DT_USER;
}

template class Variable<unisim::component::tlm2::interconnect::freescale::mpc57xx::pbridge::AccessControlRegisterMapping>;


} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim
