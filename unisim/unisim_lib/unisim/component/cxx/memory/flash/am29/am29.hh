/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_AM29_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29_AM29_HH__

#include <unisim/component/cxx/memory/flash/am29/types.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/endian/endian.hh>
#include <string>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29 {

using unisim::service::interfaces::Memory;
using unisim::kernel::logger::Logger;
using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::util::endian::endian_type;

static const uint32_t K = 1024;
static const uint32_t M = 1024 * K;
static const uint32_t G = 1024 * M;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
class AM29 :
	public Service<Memory<typename CONFIG::ADDRESS> >
{
public:
	static const unsigned int NUM_CHIPS = BYTESIZE / CONFIG::BYTESIZE;
	static const unsigned int CHIP_IO_WIDTH = IO_WIDTH / NUM_CHIPS;
	static const uint32_t ADDR_MASK = BYTESIZE - 1; // WARNING! BYTESIZE must be a power of two

	ServiceExport<Memory<typename CONFIG::ADDRESS> > memory_export;

	AM29(const  char *name, Object *parent = 0);
	virtual ~AM29();
	
	virtual void ResetMemory();
	virtual bool BeginSetup();
	virtual bool WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size);
protected:
	bool FSM(COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	
	// debug stuff
	unisim::kernel::logger::Logger logger;
	
	bool IsVerbose() const;
private:
	unsigned int config_addr_shift; // shift (right) amount to apply to addresses in CONFIG when matching addresses
	unsigned int addr_shift;      // shift (right) amount to apply to a byte address in order to obtain the actual address provided to the flash chips
	typename CONFIG::ADDRESS org;
	typename CONFIG::ADDRESS bytesize;
	endian_type endian;
	typename CONFIG::STATE state[NUM_CHIPS];
	bool sector_protect[CONFIG::NUM_SECTORS];
	uint8_t *storage;
	bool verbose;
	std::string fsm_to_graphviz_output_filename;
	
	typename CONFIG::ADDRESS page[NUM_CHIPS]; // Page in write-to-buffer page mode
	unsigned int word_count[NUM_CHIPS]; // Word count in write buffer
	unsigned int word_index[NUM_CHIPS]; // Current word index in write buffer
	struct
	{
		typename CONFIG::ADDRESS addr; // address
		uint8_t data[CHIP_IO_WIDTH]; // data to program
	}
	write_buffer[NUM_CHIPS][CONFIG::PAGE_SIZE];
	WRITE_STATUS write_status[NUM_CHIPS];
	
	Parameter<bool> param_verbose;
	Parameter<typename CONFIG::ADDRESS> param_org;
	Parameter<typename CONFIG::ADDRESS> param_bytesize;
	Parameter<endian_type> param_endian;
	ParameterArray<bool> param_sector_protect;
	Parameter<std::string> param_fsm_to_graphviz_output_filename;

	bool ReverseCompare(const uint8_t *data1, const uint8_t *data2, uint32_t size);
	void ReverseCopy(uint8_t *dest, const uint8_t *source, uint32_t size);
	void Combine(uint8_t *dest, const uint8_t *source, uint32_t size);
	int GetSector(typename CONFIG::ADDRESS addr);
	int GetPage(typename CONFIG::ADDRESS addr);
	void FSM(unsigned int chip_num, COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void ResetFSM(unsigned int chip_num);
	void Read(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void ReadWriteStatus(unsigned int chip_num, uint8_t *data, uint32_t size);
	void ReadAutoselect(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void CFIQuery(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void Program(unsigned int chip_num, typename CONFIG::ADDRESS addr, const uint8_t *data, uint32_t size);
	void OpenPage(unsigned int chip_num, typename CONFIG::ADDRESS addr);
	void LoadWordCount(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void WriteToBuffer(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size);
	void ProgramBufferToFlash(unsigned int chip_num, typename CONFIG::ADDRESS addr);
	void ChipErase(unsigned int chip_num);
	void SectorErase(unsigned int chip_num, typename CONFIG::ADDRESS addr);
	void SecuredSiliconSectorEntry(unsigned int chip_num);
	void SecuredSiliconSectorExit(unsigned int chip_num);
	void AbortWriteToBuffer(unsigned int chip_num);
	
	const char *GetStateName(typename CONFIG::STATE state) const;
	const char *GetCommandName(COMMAND command) const;
	const char *GetActionName(ACTION action) const;
	void FSMToGraphviz();
	void PrintData(std::ostream& os, const uint8_t *data, unsigned int size);
};

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_HH__
