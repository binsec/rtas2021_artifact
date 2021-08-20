/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_UTIL_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_UTIL_HH__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

#if defined(__GNUC__) && (__GNUC__ >= 3)
template <typename SCALAR> inline bool HasOverlap(SCALAR l1, SCALAR h1, SCALAR l2, SCALAR h2) __attribute__((always_inline));
#endif

// Some helper function to determine if two intervals [l1,h1] and [l2,h2] overlap
template <typename SCALAR> inline bool HasOverlap(SCALAR l1, SCALAR h1, SCALAR l2, SCALAR h2)
{
	return ((h1 < h2) ? h1 : h2) >= ((l1 < l2) ? l2 : l1);
}

// Some helper function to determine if two intervals [l1,h1[ and [l2,h2[ overlap
template <typename SCALAR> inline bool HasOverlapEx(SCALAR l1, SCALAR h1, SCALAR l2, SCALAR h2)
{
	return ((h1 < h2) ? h1 : h2) > ((l1 < l2) ? l2 : l1);
}

// Some helper function to determine if interval [l1,h1] includes interval [l2,h2] (as well as if interval [l1,h1[ includes interval [l2,h2[ )
template <typename SCALAR> inline bool Includes(SCALAR l1, SCALAR h1, SCALAR l2, SCALAR h2)
{
	return (l2 >= l1) && (h2 <= h1);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_UTIL_HH__
