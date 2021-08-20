/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef UNISIM_SERVICE_LOADER_S19_LOADER_HH_
#define UNISIM_SERVICE_LOADER_S19_LOADER_HH_

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>

#include <unisim/kernel/variable/variable.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/kernel.hh>

#include <iostream>
#include <fstream>

#include <inttypes.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

namespace unisim {
namespace service {
namespace loader {
namespace s19_loader {

typedef uint32_t s19_address_t;

// using namespace std;
using unisim::service::interfaces::Memory;
using namespace unisim::util::endian;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::service::interfaces::Loader;

#define S_RECORD_SIZE			515		// s2_record_size = 2+2+255*2 +1 ("+1" is for \0 char)
#define CPU12_RESET_ADDR		0xFFFE 	//is the local Reset Vector Address for the CPU12
#define GLOBAL_CPU12_RESET_ADDR	0xFFBFFE 	//is the global Reset Vector Address offset for the CPU12

template <class MEMORY_ADDR>
class S19_Loader :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader>
{
public:

	enum {ERR_NOFILE, ERR_BADREC, ERR_NOSUPPORT, ERR_BADADDR, ERR_BADCHKSUM, ERR_BADFILENAME, ERR_IO};

/* ********* S-Record Types ***********************
 * Record	Description		Address Bytes	Data Sequence
 * ------	-----------		-------------	-------------
 * S0		Block header	2				Yes
 * S1		Data Sequence	2				Yes
 * S2		Data Sequence	3				Yes
 * S3		Data Sequence	4				Yes
 * S5		Record Count	2				Yes
 * S7		End of Block	4				No
 * S8		End of Block	3				No
 * S9		End of Block	2				No
 */
	enum {S0='0', S1='1', S2='2', S3='3', S5='5', S7='7', S8='8', S9='9'};

	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceExport<Loader> loader_export;

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
	virtual void Reset();
	virtual MEMORY_ADDR GetEntryPoint() const;
	virtual MEMORY_ADDR GetTopAddr() const;
	virtual MEMORY_ADDR GetStackBase() const;
	virtual bool Load(const char *filename);

	S19_Loader(char const *name, Object *parent = 0);
	virtual ~S19_Loader();

private:
	std::string				filename;
	uint32_t	entry_point;

	Parameter<std::string>	param_filename;

	bool				isFirstDataRec;

	bool	ProcessRecord(int linenum, char srec[S_RECORD_SIZE]);
	void	ShowError(int  errnum, int linenum, char srec[S_RECORD_SIZE]);
	bool	memWrite(uint32_t addr, const void *buffer, uint32_t size);
	bool	Load();

};



} // end UNISIM namespace
} // end SERVICE namespace
} // end LOADER namespace
} // end S19_LOADER

#endif /*UNISIM_SERVICE_LOADER_S19_LOADER_HH_*/
