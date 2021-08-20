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

#ifndef __UNISIM_COMPONENT_TLM2_OPERATORS_ASSOCIATIVE_OPERATOR_HH__
#define __UNISIM_COMPONENT_TLM2_OPERATORS_ASSOCIATIVE_OPERATOR_HH__

#include <systemc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace operators {

template <typename T> struct BinLogicalAndOperator { static T Compute(const T& v1, const T& v2) { return v1 && v2; } };
template <typename T> struct BinLogicalOrOperator  { static T Compute(const T& v1, const T& v2) { return v1 || v2; } };
template <typename T> struct BinBitwiseAndOperator { static T Compute(const T& v1, const T& v2) { return v1 & v2; } };
template <typename T> struct BinBitwiseOrOperator  { static T Compute(const T& v1, const T& v2) { return v1 | v2; } };
template <typename T> struct BinBitwiseXorOperator { static T Compute(const T& v1, const T& v2) { return v1 ^ v2; } };

template <typename BINARY_ASSOCIATIVE_OPERATOR, typename T, unsigned int N>
struct AssociativeOperator : sc_core::sc_module
{
	sc_core::sc_vector<sc_core::sc_in<T> > in;
	sc_core::sc_out<T> out;
	
	AssociativeOperator(const sc_core::sc_module_name& name) : in("in", N), out("out")
	{
		SC_HAS_PROCESS(AssociativeOperator);
		
		SC_METHOD(Process);
		unsigned int i;
		for(i = 0; i < in.size(); i++)
		{
			sensitive << in[i];
		}
	}
	
private:
	void Process()
	{
		T value = T();
		unsigned int i;
		for(i = 0; i < N; i++)
		{
			value = BINARY_ASSOCIATIVE_OPERATOR::Compute(value, (const T&) in[i]);
		}
		out = value;
	}
};

template <typename T, unsigned int N>
struct LogicalAndOperator : AssociativeOperator<BinLogicalAndOperator<T>, T, N>
{
	LogicalAndOperator(const sc_core::sc_module_name& name) : AssociativeOperator<BinLogicalAndOperator<T>, T, N>(name) {}
};

template <typename T, unsigned int N>
struct LogicalOrOperator : AssociativeOperator<BinLogicalOrOperator<T>, T, N>
{
	LogicalOrOperator(const sc_core::sc_module_name& name) : AssociativeOperator<BinLogicalOrOperator<T>, T, N>(name) {}
};

template <typename T, unsigned int N>
struct BitwiseAndOperator : AssociativeOperator<BinBitwiseAndOperator<T>, T, N>
{
	BitwiseAndOperator(const sc_core::sc_module_name& name) : AssociativeOperator<BinBitwiseAndOperator<T>, T, N>(name) {}
};

template <typename T, unsigned int N>
struct BitwiseOrOperator : AssociativeOperator<BinBitwiseOrOperator<T>, T, N>
{
	BitwiseOrOperator(const sc_core::sc_module_name& name) : AssociativeOperator<BinBitwiseOrOperator<T>, T, N>(name) {}
};

template <typename T, unsigned int N>
struct BitwiseXorOperator : AssociativeOperator<BinBitwiseXorOperator<T>, T, N>
{
	BitwiseXorOperator(const sc_core::sc_module_name& name) : AssociativeOperator<BinBitwiseXorOperator<T>, T, N>(name) {}
};


} // end of namespace operators
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_OPERATORS_ASSOCIATIVE_OPERATOR_HH__
