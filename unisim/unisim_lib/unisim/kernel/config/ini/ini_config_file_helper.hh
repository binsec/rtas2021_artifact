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

#ifndef __UNISIM_KERNEL_CONFIG_INI_CONFIG_FILE_HELPER_HH__
#define __UNISIM_KERNEL_CONFIG_INI_CONFIG_FILE_HELPER_HH__

#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace kernel {
namespace config {
namespace ini {

class INIConfigFileHelper : public unisim::kernel::ConfigFileHelper
{
public:
	INIConfigFileHelper(unisim::kernel::Simulator *simulator);
	virtual ~INIConfigFileHelper();
	
	virtual const char *GetName() const;
	virtual bool SaveVariables(const char *filename, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool SaveVariables(std::ostream& os, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool LoadVariables(const char *filename, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	virtual bool LoadVariables(std::istream& is, unisim::kernel::VariableBase::Type type = unisim::kernel::VariableBase::VAR_VOID);
	
private:
	unisim::kernel::Simulator *simulator;
	
	void SaveVariables(std::ostream& os, unisim::kernel::Object *object, unisim::kernel::VariableBase::Type type);
	void Assign(const std::string& section, const std::string& key, const std::string& value);
};

} // end of namespace ini
} // end of namespace config
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_CONFIG_INI_CONFIG_FILE_HELPER_HH__
