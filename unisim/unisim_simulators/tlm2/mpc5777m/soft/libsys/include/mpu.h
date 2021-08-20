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

#include "typedefs.h"

typedef struct mpu_entry
{
	union
	{
		struct
		{
			unsigned int valid : 1;
			unsigned int iprot : 1;
			unsigned int sel : 2;
			unsigned int reserved1 : 1;
			unsigned int ro : 1;
			unsigned int debug : 1;
			unsigned int inst : 1;
			unsigned int shd : 1;
			unsigned int reserved2 : 3;
			unsigned int esel : 4;
			unsigned int reserved3 : 1;
			unsigned int uamsk : 3;
			unsigned int uw : 1;
			unsigned int sw : 1;
			unsigned int ux_ur : 1;
			unsigned int sx_sr : 1;
			unsigned int iovr : 1;
			unsigned int govr : 1;
			unsigned int vle : 1;
			unsigned int w : 1;
			unsigned int i : 1;
			unsigned int m : 1;
			unsigned int g : 1;
			unsigned int e : 1;
		}
		b;
		
		uint32_t r;
	}
	mas0;
	
	union
	{
		struct
		{
			unsigned int reserved1 : 8;
			unsigned int tid : 8;
			unsigned int reserved2 : 8;
			unsigned int tidmsk : 8;
		}
		b;
		
		uint32_t r;
	}
	mas1;
	
	union
	{
		struct
		{
			uint32_t upper_bound;
		}
		b;
		
		uint32_t r;
	} mas2;
	
	union
	{
		struct
		{
			uint32_t lower_bound;
		}
		b;
		
		uint32_t r;
	} mas3;
} mpu_entry_t;

void mpu_enable();
void mpu_write_entry(mpu_entry_t *e);
void mpu_read_entry(mpu_entry_t *e);
