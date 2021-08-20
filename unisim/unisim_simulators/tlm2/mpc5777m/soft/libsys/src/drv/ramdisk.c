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

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include "ramdisk.h"
#include <string.h>

extern char __RAMDISK;
extern char __RAMDISK_END;

static int ramdisk_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	char *data = (char *) buffer;
	char *ptr = &__RAMDISK + (block * RAMDISK_BLOCK_SIZE) + off;
	
	if((ptr + size) <= &__RAMDISK_END)
	{
		memcpy(data, ptr, size);
		return 0;
	}
	
	return -1;
}

static int ramdisk_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	const char *data = (const char *) buffer;
	char *ptr = &__RAMDISK + (block * RAMDISK_BLOCK_SIZE) + off;
	
	if((ptr + size) <= &__RAMDISK_END)
	{
		memcpy(ptr, data, size);
		return 0;
	}
	
	return -1;
}

static int ramdisk_erase(const struct lfs_config *c, lfs_block_t block)
{
	char *ptr = &__RAMDISK + (block * RAMDISK_BLOCK_SIZE);
	
	if((ptr + RAMDISK_BLOCK_SIZE) <= &__RAMDISK_END)
	{
		memset(ptr, 0, RAMDISK_BLOCK_SIZE);
		return 0;
	}
	
	return -1;
}

static int ramdisk_sync(const struct lfs_config *c)
{
	return 0;
}

void ramdisk_init(struct lfs_config *ramdisk_lfs_cfg)
{
	memset(ramdisk_lfs_cfg, 0, sizeof(struct lfs_config));
	
	ramdisk_lfs_cfg->read = ramdisk_read;
	ramdisk_lfs_cfg->prog = ramdisk_prog;
	ramdisk_lfs_cfg->erase = ramdisk_erase;
	ramdisk_lfs_cfg->sync = ramdisk_sync;
	ramdisk_lfs_cfg->read_size = 1;
	ramdisk_lfs_cfg->prog_size = 1;
	ramdisk_lfs_cfg->block_size = RAMDISK_BLOCK_SIZE;
	ramdisk_lfs_cfg->block_count = (&__RAMDISK_END - &__RAMDISK) / RAMDISK_BLOCK_SIZE;
	ramdisk_lfs_cfg->lookahead = 32;
}
