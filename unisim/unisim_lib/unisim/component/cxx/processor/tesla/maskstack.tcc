/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MASKSTACK_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MASKSTACK_TCC

#include <unisim/component/cxx/processor/tesla/maskstack.hh>
#include <algorithm>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<size_t Width, size_t MaxDepth>
MaskStack<Width, MaxDepth>::MaskStack() :
	current_depth(0)
{
	std::fill(counter, counter + Width, 0);
}

template<size_t Width, size_t MaxDepth>
void MaskStack<Width, MaxDepth>::push(Mask mask)
{
	assert(current_depth < MaxDepth);
	for(size_t i = 0; i != Width; ++i)
	{
		if(mask[i]) {
			assert(counter[i] == current_depth);	// No zombies allowed!
			++counter[i];
		}
	}
	++current_depth;
}

template<size_t Width, size_t MaxDepth>
void MaskStack<Width, MaxDepth>::pop()
{
	assert(current_depth > 0);
	for(size_t i = 0; i != Width; ++i)
	{
		if(counter[i] == current_depth) {
			--counter[i];
		}
	}
	--current_depth;
}

template<size_t Width, size_t MaxDepth>
typename MaskStack<Width, MaxDepth>::Mask MaskStack<Width, MaxDepth>::top() const
{
	return slice(current_depth);
}

template<size_t Width, size_t MaxDepth>
bool MaskStack<Width, MaxDepth>::empty() const
{
	return current_depth == 0;
}

template<size_t Width, size_t MaxDepth>
size_t MaskStack<Width, MaxDepth>::size() const
{
	return current_depth;
}

template<size_t Width, size_t MaxDepth>
typename MaskStack<Width, MaxDepth>::Mask MaskStack<Width, MaxDepth>::slice(depth_t depth) const
{
	Mask mask;
	for(size_t i = 0; i != Width; ++i)
	{
		assert(counter[i] <= current_depth);
		mask[i] = (counter[i] == current_depth);
	}
	return mask;
}

template<size_t Width, size_t MaxDepth>
void MaskStack<Width, MaxDepth>::dig(Mask mask, size_t depth_abs)
{
	assert(current_depth >= depth_abs);
	for(size_t i = 0; i != Width; ++i)
	{
		if(mask[i] && counter[i] > depth_abs) {
			counter[i] = depth_abs;
		}
	}
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
