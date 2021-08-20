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

#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/debug/profiler/profiler.tcc>

namespace unisim {
namespace service {
namespace debug {
namespace profiler {

template class AddressProfile<uint32_t, signed char>;
template class AddressProfile<uint32_t, short>;
template class AddressProfile<uint32_t, int>;
template class AddressProfile<uint32_t, long>;
template class AddressProfile<uint32_t, long long>;
template class AddressProfile<uint32_t, unsigned char>;
template class AddressProfile<uint32_t, unsigned short>;
template class AddressProfile<uint32_t, unsigned int>;
template class AddressProfile<uint32_t, unsigned long>;
template class AddressProfile<uint32_t, unsigned long long>;
template class AddressProfile<uint32_t, double>;
template class AddressProfile<uint32_t, sc_core::sc_time>;
template class FunctionInstructionProfile<uint32_t, signed char>;
template class FunctionInstructionProfile<uint32_t, short>;
template class FunctionInstructionProfile<uint32_t, int>;
template class FunctionInstructionProfile<uint32_t, long>;
template class FunctionInstructionProfile<uint32_t, long long>;
template class FunctionInstructionProfile<uint32_t, unsigned char>;
template class FunctionInstructionProfile<uint32_t, unsigned short>;
template class FunctionInstructionProfile<uint32_t, unsigned int>;
template class FunctionInstructionProfile<uint32_t, unsigned long>;
template class FunctionInstructionProfile<uint32_t, unsigned long long>;
template class FunctionInstructionProfile<uint32_t, double>;
template class FunctionNameLocationConversion<uint32_t>;
template class SourceCodeProfile<uint32_t, signed char>;
template class SourceCodeProfile<uint32_t, short>;
template class SourceCodeProfile<uint32_t, int>;
template class SourceCodeProfile<uint32_t, long>;
template class SourceCodeProfile<uint32_t, long long>;
template class SourceCodeProfile<uint32_t, unsigned char>;
template class SourceCodeProfile<uint32_t, unsigned short>;
template class SourceCodeProfile<uint32_t, unsigned int>;
template class SourceCodeProfile<uint32_t, unsigned long>;
template class SourceCodeProfile<uint32_t, unsigned long long>;
template class SourceCodeProfile<uint32_t, double>;
template class AnnotatedSourceCodeFile<uint32_t, signed char>;
template class AnnotatedSourceCodeFile<uint32_t, short>;
template class AnnotatedSourceCodeFile<uint32_t, int>;
template class AnnotatedSourceCodeFile<uint32_t, long>;
template class AnnotatedSourceCodeFile<uint32_t, long long>;
template class AnnotatedSourceCodeFile<uint32_t, unsigned char>;
template class AnnotatedSourceCodeFile<uint32_t, unsigned short>;
template class AnnotatedSourceCodeFile<uint32_t, unsigned int>;
template class AnnotatedSourceCodeFile<uint32_t, unsigned long>;
template class AnnotatedSourceCodeFile<uint32_t, unsigned long long>;
template class AnnotatedSourceCodeFile<uint32_t, double>;
template class AnnotatedSourceCodeFileSet<uint32_t, signed char>;
template class AnnotatedSourceCodeFileSet<uint32_t, short>;
template class AnnotatedSourceCodeFileSet<uint32_t, int>;
template class AnnotatedSourceCodeFileSet<uint32_t, long>;
template class AnnotatedSourceCodeFileSet<uint32_t, long long>;
template class AnnotatedSourceCodeFileSet<uint32_t, unsigned char>;
template class AnnotatedSourceCodeFileSet<uint32_t, unsigned short>;
template class AnnotatedSourceCodeFileSet<uint32_t, unsigned int>;
template class AnnotatedSourceCodeFileSet<uint32_t, unsigned long>;
template class AnnotatedSourceCodeFileSet<uint32_t, unsigned long long>;
template class AnnotatedSourceCodeFileSet<uint32_t, double>;
template class Profiler<uint32_t>;

} // end of namespace profiler
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim
