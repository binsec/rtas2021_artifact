/*
 *  Copyright (c) 2011,
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
 * 
 */

#ifndef __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_HH__
#define __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_HH__

#include <inttypes.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/service/interfaces/stmt_lookup.hh"

namespace unisim {
namespace service {
namespace tee {
namespace stmt_lookup {

using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::StatementLookup;

template <class ADDRESS, unsigned int MAX_IMPORTS = 16>
class Tee :
	public Client<StatementLookup<ADDRESS> >,
	public Service<StatementLookup<ADDRESS> >
{
public:
	ServiceExport<StatementLookup<ADDRESS> > stmt_lookup_export;
	ServiceImport<StatementLookup<ADDRESS> > *stmt_lookup_import[MAX_IMPORTS];
	
	Tee(const char *name, Object *parent = 0);
	virtual ~Tee();

	virtual void GetStatements(std::multimap<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& stmts) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const char *filename, unsigned int lineno, unsigned int colno) const;
};

} // end of namespace stmt_lookup
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_STMT_LOOKUP_TEE_HH__
