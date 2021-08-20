/*
 *  Copyright (c) 2017,
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
#include <unisim/util/netstreamer/netstreamer.hh>
#include <string>

namespace unisim {
namespace kernel {
namespace variable {

using unisim::util::netstreamer::NetStreamerProtocol;
using unisim::util::netstreamer::NETSTREAMER_PROTOCOL_RAW;
using unisim::util::netstreamer::NETSTREAMER_PROTOCOL_TELNET;

template <> Variable<NetStreamerProtocol>::Variable(const char *_name, Object *_object, NetStreamerProtocol& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Initialize();
	AddEnumeratedValue("raw");
	AddEnumeratedValue("telnet");
}

template <>
const char *Variable<NetStreamerProtocol>::GetDataTypeName() const
{
	return "netstreamer-protocol";
}

template <>
VariableBase::DataType Variable<NetStreamerProtocol>::GetDataType() const
{
	return DT_USER;
}

template <>
unsigned int Variable<NetStreamerProtocol>::GetBitSize() const
{
	return 1;
}

template <> Variable<NetStreamerProtocol>::operator bool () const { return *storage != NETSTREAMER_PROTOCOL_TELNET; }
template <> Variable<NetStreamerProtocol>::operator long long () const { return *storage; }
template <> Variable<NetStreamerProtocol>::operator unsigned long long () const { return *storage; }
template <> Variable<NetStreamerProtocol>::operator double () const { return (double)(*storage); }
template <> Variable<NetStreamerProtocol>::operator std::string () const
{
	switch(*storage)
	{
		case NETSTREAMER_PROTOCOL_TELNET: return std::string("telnet");
		case NETSTREAMER_PROTOCOL_RAW: return std::string("raw");
	}
	return std::string("?");
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (bool value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch((unsigned int) value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (long long value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch(value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch(value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (double value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch((unsigned int) value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (const char *value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		if(std::string(value) == std::string("telnet")) tmp = NETSTREAMER_PROTOCOL_TELNET;
		else if(std::string(value) == std::string("raw")) tmp = NETSTREAMER_PROTOCOL_RAW;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<NetStreamerProtocol>;

} // end of variable namespace
} // end of kernel namespace
} // end of unisim namespace
