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

#include "derivative.h" /* include peripheral declarations */
#include "compiler_api.h"
#include "cache.h"
#include "mpu.h"
#include "intc.h"
#include "stm.h"
#include "swt.h"
#include "pit.h"
#include "linflexd.h"
#include "dmamux.h"
#include "edma.h"
#include "dspi.h"
#include "siul2.h"
#include "ebi.h"
#include "pbridge.h"
#include "xbar.h"
#include "smpu.h"
#include "m_can.h"
#include "sema4.h"
#include "console.h"
#include "ramdisk.h"
#include "lfs.h"

// newlib headers
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <utime.h>

#undef errno
extern int  errno;

#define MAX_FILE_DESCRIPTORS 16

#define PIT_1_CLK_FREQ_MHZ 80  /* Mhz */
#define TICKS_PER_SECONDS 1000 /* increase this value for better precision */

extern char __FLASH;
extern char __FLASH_END;
extern char __SRAM;
extern char __SRAM_END;
extern char __LOCAL_IMEM;
extern char __LOCAL_IMEM_END;
extern char __LOCAL_DMEM;
extern char __LOCAL_DMEM_END;
extern char __RAMDISK;
extern char __RAMDISK_END;
extern char __SHARE;
extern char __SHARE_END;

volatile char boot_flag __attribute__ ((section (".share")));

enum io_backend_id_t
{
	NO_BACK_END      = 0,
	CONSOLE_BACK_END = 1,
	LFS_BACK_END     = 2
};

struct file_t
{
	enum io_backend_id_t io_backend_id;
	union
	{
		lfs_file_t *lf;
		struct con_t *con;
		void *opaque;
	} io_backend;
	char *path;
	int flags;
	int mode;
};

static struct lfs_config ramdisk_lfs_cfg;

static lfs_t *root_fs;

static struct file_t files[MAX_FILE_DESCRIPTORS];

static int convert_flags(int libc_flags)
{
	int lfs_flags = 0;
	if((libc_flags & O_ACCMODE) == O_RDONLY) lfs_flags = LFS_O_RDONLY;
	else if((libc_flags & O_ACCMODE) == O_WRONLY) lfs_flags = LFS_O_WRONLY;
	else if((libc_flags & O_ACCMODE) == O_RDWR) lfs_flags = LFS_O_RDWR;
	if(libc_flags & O_CREAT ) lfs_flags |= LFS_O_CREAT ;
	if(libc_flags & O_EXCL  ) lfs_flags |= LFS_O_EXCL  ;
	if(libc_flags & O_TRUNC ) lfs_flags |= LFS_O_TRUNC ;
	if(libc_flags & O_APPEND) lfs_flags |= LFS_O_APPEND;
	
	return lfs_flags;
}

static int convert_errno(int err)
{
	switch(err)
	{
		case LFS_ERR_IO      : return EIO;
		case LFS_ERR_CORRUPT : return EIO;
		case LFS_ERR_NOENT   : return ENOENT;
		case LFS_ERR_EXIST   : return EEXIST;
		case LFS_ERR_NOTDIR  : return ENOTDIR;
		case LFS_ERR_ISDIR   : return EISDIR;
		case LFS_ERR_NOTEMPTY: return ENOTEMPTY;
		case LFS_ERR_INVAL   : return EINVAL;
		case LFS_ERR_NOSPC   : return ENOSPC;
		case LFS_ERR_NOMEM   : return ENOMEM;
	}
	assert(false);
	return 0;
}

static unsigned int sys_get_core_id()
{
	return MFSPR(286); // read PIR: our processor number
}

void _fini()
{
}

void sys_init()
{
	unsigned int core_id = sys_get_core_id();
	
	icache_invalidate();
	icache_enable();

	mpu_entry_t m_text_mpu_entry;
	memset(&m_text_mpu_entry, 0, sizeof(m_text_mpu_entry));
	m_text_mpu_entry.mas0.b.esel = 0;
	m_text_mpu_entry.mas0.b.shd = 1;
	mpu_read_entry(&m_text_mpu_entry);
	m_text_mpu_entry.mas0.b.valid = 1;
	m_text_mpu_entry.mas0.b.sx_sr = 1;
	m_text_mpu_entry.mas3.b.lower_bound = (uint32_t) &__FLASH;
	m_text_mpu_entry.mas2.b.upper_bound = (uint32_t) &__FLASH_END;
	mpu_write_entry(&m_text_mpu_entry);

	mpu_entry_t local_imem_mpu_entry;
	memset(&local_imem_mpu_entry, 0, sizeof(local_imem_mpu_entry));
	local_imem_mpu_entry.mas0.b.esel = 0;
	local_imem_mpu_entry.mas0.b.inst = 1;
	local_imem_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&local_imem_mpu_entry);
	local_imem_mpu_entry.mas0.b.valid = 1;
	local_imem_mpu_entry.mas0.b.sx_sr = 1;
	local_imem_mpu_entry.mas3.b.lower_bound = (uint32_t) &__LOCAL_IMEM;
	local_imem_mpu_entry.mas2.b.upper_bound = (uint32_t) &__LOCAL_IMEM_END;
	mpu_write_entry(&local_imem_mpu_entry);

	mpu_entry_t sram_mpu_entry;
	memset(&sram_mpu_entry, 0, sizeof(sram_mpu_entry));
	sram_mpu_entry.mas0.b.esel = 0;
	sram_mpu_entry.mas0.b.inst = 0;
	sram_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&sram_mpu_entry);
	sram_mpu_entry.mas0.b.valid = 1;
	sram_mpu_entry.mas0.b.sx_sr = 0;
	sram_mpu_entry.mas0.b.sw = 1;
	sram_mpu_entry.mas3.b.lower_bound = (uint32_t) &__SRAM;
	sram_mpu_entry.mas2.b.upper_bound = (uint32_t) &__SRAM_END;
	mpu_write_entry(&sram_mpu_entry);

	mpu_entry_t share_mpu_entry;
	memset(&share_mpu_entry, 0, sizeof(share_mpu_entry));
	share_mpu_entry.mas0.b.esel = 1;
	share_mpu_entry.mas0.b.inst = 0;
	share_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&share_mpu_entry);
	share_mpu_entry.mas0.b.valid = 1;
	share_mpu_entry.mas0.b.sx_sr = 0;
	share_mpu_entry.mas0.b.sw = 1;
	share_mpu_entry.mas0.b.i = 1;
	share_mpu_entry.mas3.b.lower_bound = (uint32_t) &__SHARE;
	share_mpu_entry.mas2.b.upper_bound = (uint32_t) &__SHARE_END;
	mpu_write_entry(&share_mpu_entry);

	mpu_entry_t local_dmem_mpu_entry;
	memset(&local_dmem_mpu_entry, 0, sizeof(local_dmem_mpu_entry));
	local_dmem_mpu_entry.mas0.b.esel = 2;
	local_dmem_mpu_entry.mas0.b.inst = 0;
	local_dmem_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&local_dmem_mpu_entry);
	local_dmem_mpu_entry.mas0.b.valid = 1;
	local_dmem_mpu_entry.mas0.b.sx_sr = 0;
	local_dmem_mpu_entry.mas0.b.sw = 1;
	local_dmem_mpu_entry.mas3.b.lower_bound = (uint32_t) &__LOCAL_DMEM;
	local_dmem_mpu_entry.mas2.b.upper_bound = (uint32_t) &__LOCAL_DMEM_END;
	mpu_write_entry(&local_dmem_mpu_entry);
	
	mpu_entry_t m_ramdisk_mpu_entry;
	memset(&m_ramdisk_mpu_entry, 0, sizeof(m_ramdisk_mpu_entry));
	m_ramdisk_mpu_entry.mas0.b.esel = 3;
	m_ramdisk_mpu_entry.mas0.b.inst = 0;
	m_ramdisk_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&m_ramdisk_mpu_entry);
	m_ramdisk_mpu_entry.mas0.b.valid = 1;
	m_ramdisk_mpu_entry.mas0.b.sx_sr = 0;
	m_ramdisk_mpu_entry.mas0.b.sw = 1;
	m_ramdisk_mpu_entry.mas3.b.lower_bound = (uint32_t) &__RAMDISK;
	m_ramdisk_mpu_entry.mas2.b.upper_bound = (uint32_t) &__RAMDISK_END;
	mpu_write_entry(&m_ramdisk_mpu_entry);

	mpu_entry_t peripheral_mpu_entry;
	memset(&peripheral_mpu_entry, 0, sizeof(peripheral_mpu_entry));
	peripheral_mpu_entry.mas0.b.esel = 4;
	peripheral_mpu_entry.mas0.b.inst = 0;
	peripheral_mpu_entry.mas0.b.shd = 0;
	mpu_read_entry(&peripheral_mpu_entry);
	peripheral_mpu_entry.mas0.b.valid = 1;
	peripheral_mpu_entry.mas0.b.sx_sr = 0;
	peripheral_mpu_entry.mas0.b.sw = 1;
	peripheral_mpu_entry.mas0.b.i = 1;
	peripheral_mpu_entry.mas3.b.lower_bound = 0xf0000000;
	peripheral_mpu_entry.mas2.b.upper_bound = 0xffffffff;
	mpu_write_entry(&peripheral_mpu_entry);
	
	mpu_enable();
	
	switch(core_id)
	{
		case 0:
		case 1:
			dcache_invalidate();
			dcache_enable();
	}
	
	
	intc_drv_init();
	dmamux_drv_init();
	edma_drv_init();
	stm_drv_init();
	swt_drv_init();
	pit_drv_init();
	linflexd_drv_init();
	dspi_drv_init();
	siul2_drv_init();
	ebi_drv_init();
	pbridge_drv_init();
	xbar_drv_init();
	smpu_drv_init();
	m_can_drv_init();
	sema4_drv_init();
	
	intc_init();      // initialize interrupt controller
	edma_init(0);     // initialize eDMA_0
	edma_init(1);     // initialize eDMA_1
	intc_enable_external_interrupt(); // Enable processor external interrupts
	
	switch(core_id)
	{
		case 0:
		case 1:
			while(!boot_flag);
			break;
			
		case 2:
			xbar_init(0);     // initialize XBAR_0
			xbar_init(1);     // initialize XBAR_1
			smpu_init(0);     // initialize SMPU_0
			smpu_init(1);     // initialize SMPU_1
			pbridge_init(0);  // initialize PBRIDGE_A
			pbridge_init(1);  // initialize PBRIDGE_B
			swt_init(2);      // initialize SWT_2
			stm_init(2);      // initialize STM_2
			pit_init(0);      // initialize PIT_0
			pit_init(1);      // initialize PIT_1
			siul2_init();     // initialize SIUL2
			ebi_init();       // initialize EBI
			sema4_init();
			
			// check that semaphores work
			sema4_lock_gate(0);
			sema4_reset_all_gates();
			
			ebi_enable_module();
			
			stm_set_channel_irq_priority(2, 0, 63);          // STM_2: set STM_2 channel #0 IRQ priority level to 63
			stm_select_channel_irq_for_processor(2, 0, 2);  // STM_2: select STM_2 channel #0 IRQ for processor #2

			pit_set_timer_irq_priority(0, 0, 62);            // PIT_0: set PIT_0 timer #0 IRQ priority level to 62
			pit_select_timer_irq_for_processor(0, 0, 2);    // PIT_0: select PIT_0 timer #0 IRQ for Processor #2
			
			dspi_set_irq_priority(0, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_CMDFFF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_CMDTCF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_SPEF, 61);
			dspi_set_irq_priority(0, DSPI_REQ_TFUF_RFOF_TFIWF, 61);
			
			dspi_set_irq_priority(1, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_CMDFFF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_CMDTCF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_SPEF, 61);
			dspi_set_irq_priority(1, DSPI_REQ_TFUF_RFOF_TFIWF, 61);

			dspi_set_irq_priority(2, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_CMDFFF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_CMDTCF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_SPEF, 61);
			dspi_set_irq_priority(2, DSPI_REQ_TFUF_RFOF_TFIWF, 61);

			dspi_set_irq_priority(3, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_CMDFFF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_CMDTCF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_SPEF, 61);
			dspi_set_irq_priority(3, DSPI_REQ_TFUF_RFOF_TFIWF, 61);

			dspi_set_irq_priority(4, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_DDIF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_TFUF_RFOF_TFIWF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_SPITCF_CMDTCF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_DSITCF_CMDFFF, 61);
			dspi_set_irq_priority(4, DSPI_REQ_SPEF_DPEF, 61);

			dspi_set_irq_priority(5, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_DDIF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_TFUF_RFOF_TFIWF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_SPITCF_CMDTCF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_DSITCF_CMDFFF, 61);
			dspi_set_irq_priority(5, DSPI_REQ_SPEF_DPEF, 61);

			dspi_set_irq_priority(6, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_DDIF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_TFUF_RFOF_TFIWF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_SPITCF_CMDTCF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_DSITCF_CMDFFF, 61);
			dspi_set_irq_priority(6, DSPI_REQ_SPEF_DPEF, 61);

			dspi_set_irq_priority(12, DSPI_REQ_EOQF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_TFFF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_CMDFFF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_TCF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_CMDTCF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_RFDF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_SPEF, 61);
			dspi_set_irq_priority(12, DSPI_REQ_TFUF_RFOF_TFIWF, 61);

			dspi_select_irq_for_processor(0, DSPI_REQ_EOQF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_TFFF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_CMDFFF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_TCF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_CMDTCF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_RFDF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_SPEF, 0);
			dspi_select_irq_for_processor(0, DSPI_REQ_TFUF_RFOF_TFIWF, 0);
			
			dspi_select_irq_for_processor(1, DSPI_REQ_EOQF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_TFFF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_CMDFFF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_TCF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_CMDTCF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_RFDF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_SPEF, 1);
			dspi_select_irq_for_processor(1, DSPI_REQ_TFUF_RFOF_TFIWF, 1);

			dspi_select_irq_for_processor(2, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_CMDFFF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_CMDTCF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_SPEF, 2);
			dspi_select_irq_for_processor(2, DSPI_REQ_TFUF_RFOF_TFIWF, 2);

			dspi_select_irq_for_processor(3, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_CMDFFF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_CMDTCF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_SPEF, 2);
			dspi_select_irq_for_processor(3, DSPI_REQ_TFUF_RFOF_TFIWF, 2);

			dspi_select_irq_for_processor(4, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_DDIF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_TFUF_RFOF_TFIWF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_SPITCF_CMDTCF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_DSITCF_CMDFFF, 2);
			dspi_select_irq_for_processor(4, DSPI_REQ_SPEF_DPEF, 2);

			dspi_select_irq_for_processor(5, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_DDIF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_TFUF_RFOF_TFIWF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_SPITCF_CMDTCF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_DSITCF_CMDFFF, 2);
			dspi_select_irq_for_processor(5, DSPI_REQ_SPEF_DPEF, 2);

			dspi_select_irq_for_processor(6, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_DDIF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_TFUF_RFOF_TFIWF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_SPITCF_CMDTCF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_DSITCF_CMDFFF, 2);
			dspi_select_irq_for_processor(6, DSPI_REQ_SPEF_DPEF, 2);

			dspi_select_irq_for_processor(12, DSPI_REQ_EOQF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_TFFF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_CMDFFF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_TCF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_CMDTCF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_RFDF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_SPEF, 2);
			dspi_select_irq_for_processor(12, DSPI_REQ_TFUF_RFOF_TFIWF, 2);

			m_can_set_irq_priority(1, M_CAN_INT0, 60);
			m_can_set_irq_priority(1, M_CAN_INT1, 60);

			m_can_set_irq_priority(2, M_CAN_INT0, 60);
			m_can_set_irq_priority(2, M_CAN_INT1, 60);

			m_can_set_irq_priority(3, M_CAN_INT0, 60);
			m_can_set_irq_priority(3, M_CAN_INT1, 60);

			m_can_set_irq_priority(4, M_CAN_INT0, 60);
			m_can_set_irq_priority(4, M_CAN_INT1, 60);

			m_can_select_irq_for_processor(1, M_CAN_INT0, 0);
			m_can_select_irq_for_processor(1, M_CAN_INT1, 0);

			m_can_select_irq_for_processor(2, M_CAN_INT0, 1);
			m_can_select_irq_for_processor(2, M_CAN_INT1, 1);

			m_can_select_irq_for_processor(3, M_CAN_INT0, 2);
			m_can_select_irq_for_processor(3, M_CAN_INT1, 2);

			unsigned int chan;
			for(chan = 0; chan < 64; chan++)
			{
				//edma_enable_master_id_replication(0, chan);                // eDMA_0: enable Master ID replication
				//edma_enable_master_id_replication(1, chan);                // eDMA_1: enable Master ID replication
				
				edma_set_channel_arbitration_priority(0, chan, chan & 15); // eDMA_0: set channel priority in group (lower to higher)
				edma_set_channel_arbitration_priority(1, chan, chan & 15); // eDMA_1: set channel priority in group (lower to higher)
			}
			
			
			// lifetime
			pit_set_timer_load_value(1, 0, ((PIT_1_CLK_FREQ_MHZ * 1000000) / TICKS_PER_SECONDS) - 1);      // PIT_1: timer #0 down counter to zero every ticks
			pit_set_timer_load_value(1, 1, 0xffffffff);  // PIT_1: timer #1 down counter by 1 every ticks
			pit_chain_timer(1, 1);                       // PIT_1: chain timer #0 and timer #1
			pit_enable_timer(1, 0);                      // PIT_1: enable timer #0
			pit_enable_timer(1, 1);                      // PIT_1: enable timer #1
			pit_enable_timers_clock(1);                  // PIT_1: enable PIT_1 timers clock
			
			// System memory protection
			smpu_region_descriptor_set_start_address(1, 0, 0x0);                            // SMPU_0: region descriptor #0, start address <- 0x0
			smpu_region_descriptor_set_end_address(1, 0, 0xffffffff);                       // SMPU_0: region descriptor #0, end address <- 0xffffffff
			smpu_region_descriptor_set_master_permission(1, 0, 0, SMPU_MP_READ_WRITE);      // SMPU_0: region descriptor #0, master #0, permission <- read/write (Main_Core_0)
			smpu_region_descriptor_set_master_permission(1, 0, 1, SMPU_MP_READ_WRITE);      // SMPU_0: region descriptor #0, master #1, permission <- read/write (Main_Core_1)
			smpu_region_descriptor_set_master_permission(1, 0, 2, SMPU_MP_READ_WRITE);      // SMPU_0: region descriptor #0, master #2, permission <- read/write (Peripheral_Core_2)
			smpu_region_descriptor_set_master_permission(1, 0, 3, SMPU_MP_READ);            // SMPU_0: region descriptor #0, master #3, permission <- read only (eDMA_0)
			smpu_region_descriptor_set_valid_flag(1, 0, 1);                                 // SMPU_0: region descriptor #0, valid <- 1
			
			smpu_region_descriptor_set_start_address(1, 1, (uint32_t) &LINFlexD_0);         // SMPU_0: region descriptor #1, start address <- LINFlexD_0(start)
			smpu_region_descriptor_set_end_address(1, 1, (uint32_t) (&LINFlexD_0 + 1) - 1); // SMPU_0: region descriptor #1, end address <- LINFlexD_0(end)
			smpu_region_descriptor_set_master_permission(1, 1, 3, SMPU_MP_WRITE);           // SMPU_0: region descriptor #1, master #3, permission <- write (eDMA_0)
			smpu_region_descriptor_set_valid_flag(1, 1, 1);                                 // SMPU_0: region descriptor #1, valid <- 1
			                                                                                
			smpu_region_descriptor_set_start_address(1, 2, (uint32_t) &LINFlexD_1);         // SMPU_0: region descriptor #2, start address <- LINFlexD_1(start)
			smpu_region_descriptor_set_end_address(1, 2, (uint32_t) (&LINFlexD_1 + 1) - 1); // SMPU_0: region descriptor #2, end address <- LINFlexD_1(end)
			smpu_region_descriptor_set_master_permission(1, 2, 3, SMPU_MP_WRITE);           // SMPU_0: region descriptor #2, master #3, permission <- write (eDMA_0)
			smpu_region_descriptor_set_valid_flag(1, 2, 1);                                 // SMPU_0: region descriptor #2, valid <- 1
			
			smpu_region_descriptor_set_start_address(1, 3, (uint32_t) &LINFlexD_2);         // SMPU_0: region descriptor #3, start address <- LINFlexD_2(start)
			smpu_region_descriptor_set_end_address(1, 3, (uint32_t) (&LINFlexD_2 + 1) - 1); // SMPU_0: region descriptor #3, end address <- LINFlexD_2(end)
			smpu_region_descriptor_set_master_permission(1, 3, 3, SMPU_MP_WRITE);           // SMPU_0: region descriptor #3, master #3, permission <- write (eDMA_0)
			smpu_region_descriptor_set_valid_flag(1, 3, 1);                                 // SMPU_0: region descriptor #3, valid <- 1
			
			smpu_enable(1);                                                                 // SMPU_0: enable
	
			boot_flag = 1;
			break;
	}
	
	unsigned int fd;
		
	for(fd = 0; fd < MAX_FILE_DESCRIPTORS; fd++)
	{
		files[fd].io_backend_id = NO_BACK_END;
		files[fd].io_backend.opaque = 0;
	}
	
	assert(STDIN_FILENO < MAX_FILE_DESCRIPTORS);
	assert(STDOUT_FILENO < MAX_FILE_DESCRIPTORS);
	assert(STDERR_FILENO < MAX_FILE_DESCRIPTORS);
	
	assert(con_init(core_id) != 0); // using core ID as console ID

	assert(open("/dev/tty", O_RDONLY, 0) == STDIN_FILENO);
	assert(open("/dev/tty", O_WRONLY, 0) == STDOUT_FILENO);
	assert(open("/dev/tty", O_WRONLY, 0) == STDERR_FILENO);
	
	unsigned int ramdisk_ebi_bank = core_id;
	
	ebi_set_bank_port_size(ramdisk_ebi_bank, EBI_PORT_SIZE_32);         // EBI: word addressing
	ebi_set_bank_base_address(ramdisk_ebi_bank, (uint32_t) &__RAMDISK); // EBI: base address
	ebi_set_bank_address_mask(ramdisk_ebi_bank, 0xfc000000);            // EBI: 64MB segments
	ebi_set_bank_valid_flag(ramdisk_ebi_bank, 1);                       // EBI: enable bank

	ramdisk_init(&ramdisk_lfs_cfg);
	
	/* check if there's a littlefs file system in ramdisk */
	char magic[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	if(ramdisk_lfs_cfg.read(&ramdisk_lfs_cfg, 0, 40, magic, 8) == 0)
	{
		if(memcmp(magic, "littlefs", 8) == 0)
		{
			root_fs = (lfs_t *) malloc(sizeof(lfs_t));
			assert(root_fs != 0);
			/* mount littlefs root file system in ramdisk */
			int err = lfs_mount(root_fs, &ramdisk_lfs_cfg);
			
			assert(err == LFS_ERR_OK);
		}
	}
}

int mkdir(const char *path, mode_t mode)
{
	if(root_fs)
	{
		int err = lfs_mkdir(root_fs, path);
		
		if(err == LFS_ERR_OK)
		{
			return 0;
		}
		else
		{
			errno = convert_errno(err);
		}
	}
	else
	{
		errno = ENOENT;
	}
	
	return -1;
}

int rmdir(const char *path)
{
	if(root_fs)
	{
		struct lfs_info info;
		memset(&info, 0, sizeof(struct lfs_info));
		int err = lfs_stat(root_fs, path, &info);
		
		if(err == LFS_ERR_OK)
		{
			if(info.type == LFS_TYPE_DIR)
			{
				err = lfs_remove(root_fs, path);
				
				if(err == LFS_ERR_OK)
				{
					return 0;
				}
				else
				{
					errno = convert_errno(err);
				}
			}
			else
			{
				errno = ENOENT;
			}
		}
		else
		{
			errno = convert_errno(err);
		}
	}
	else
	{
		errno = ENOENT;
	}
	
	return -1;
}

int close(int fd)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if(files[fd].io_backend_id == CONSOLE_BACK_END)
			{
				free(file->path);
				return 0;
			}
			else if(files[fd].io_backend_id == LFS_BACK_END)
			{
				assert(root_fs != 0);
				int err = lfs_file_close(root_fs, file->io_backend.lf);
				
				if(err == LFS_ERR_OK)
				{
					free(file->path);
					file->io_backend_id = NO_BACK_END;
					return 0;
				}
				else
				{
					errno = convert_errno(err);
				}
			}
			else
			{
				errno = EBADF;
			}
		}
		else
		{
			errno = EBADF;
		}
	}
	else
	{
		errno = EBADF;
	}
	
	return -1;
}
 
int fstat(int fd, struct stat *buf)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if(file->io_backend_id == CONSOLE_BACK_END)
			{
				buf->st_mode = file->mode & S_IFMT;
				return 0;
			}
			else if(file->io_backend_id == LFS_BACK_END)
			{
				assert(root_fs != 0);
				struct lfs_info info;
				memset(&info, 0, sizeof(struct lfs_info));
				int err = lfs_stat(root_fs, file->path, &info);
				
				if(err == LFS_ERR_OK)
				{
					buf->st_mode = (info.type == LFS_TYPE_DIR) ? S_IFDIR : S_IFREG;
					buf->st_size = info.size;
					buf->st_blksize = RAMDISK_BLOCK_SIZE;
					buf->st_blocks = (info.size + RAMDISK_BLOCK_SIZE - 1) / RAMDISK_BLOCK_SIZE;
					return 0;
				}
				else
				{
					errno = convert_errno(err);
				}
			}
		}
		else
		{
			errno = EBADF;
		}
	}
	else
	{
		errno = EBADF;
	}
	
	return -1;
}
 
int isatty(int fd)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			return file->io_backend_id == CONSOLE_BACK_END;
		}
		else
		{
			errno = EBADF;
		}
	}
	else
	{
		errno = EBADF;
	}
	
	return -1;
}
 
off_t lseek(int fd, off_t offset, int whence)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if((whence == SEEK_SET) || (whence == SEEK_CUR) || (whence == SEEK_END))
			{
				if(file->io_backend_id == CONSOLE_BACK_END)
				{
					errno = ESPIPE;
				}
				else if(file->io_backend_id == LFS_BACK_END)
				{
					assert(root_fs != 0);
					int lfs_whence = 0;
					switch(whence)
					{
						case SEEK_SET: lfs_whence = LFS_SEEK_SET; break;
						case SEEK_CUR: lfs_whence = LFS_SEEK_CUR; break;
						case SEEK_END: lfs_whence = LFS_SEEK_END; break;
					}
					
					int err = lfs_file_seek(root_fs, file->io_backend.lf, offset, lfs_whence);
					
					if(err >= 0)
					{
						return err;
					}
					else
					{
						errno = convert_errno(err);
					}
				}
				else
				{
					errno = EBADF;
				}
			}
			else
			{
				errno = EINVAL;
			}
		}
		else
		{
			errno = EBADF;
		}
	}
	else
	{
		errno = EBADF;
	}
	
	return -1;
}

int open(const char *path, int flags, ...)
{
	va_list args;
	
	va_start(args, flags);
	
	unsigned int fd;
	
	for(fd = 0; fd < MAX_FILE_DESCRIPTORS; fd++)
	{
		if(files[fd].io_backend_id == NO_BACK_END) break;
	}
	
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if((strcmp("/dev/tty", path) == 0))
		{
			file->io_backend_id = CONSOLE_BACK_END;
			struct con_t *con = con_find(sys_get_core_id());
			
			if(con)
			{
				file->io_backend.con = con;
				file->path = strdup(path);
				
				if(file->path)
				{
					file->flags = flags;
					file->mode = (va_arg(args, mode_t) & ~S_IFMT) | S_IFCHR;
					return fd;
				}
				else
				{
					errno = ENOMEM;
				}
			}
			else
			{
				errno = ENOENT;
			}
		}
		else if(root_fs)
		{
			lfs_file_t *lf = (lfs_file_t *) malloc(sizeof(lfs_file_t));
			int err = lfs_file_open(root_fs, lf, path, convert_flags(flags));
			
			if(err == LFS_ERR_OK)
			{
				file->io_backend_id = LFS_BACK_END;
				file->io_backend.lf = lf;
				file->path = strdup(path);
				file->flags = flags;
				file->mode = (va_arg(args, mode_t) & ~S_IFMT) | S_IFREG;
				file->io_backend.lf = lf;
				return fd;
			}
			else
			{
				free(lf);
				errno = convert_errno(err);
			}
		}
		else
		{
			errno = ENOENT;
		}
	}
	else
	{
		errno = EMFILE;
	}

	va_end(args);
	return -1;
}
 
int read(int fd, void *buf, size_t nbytes)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if(((file->flags & O_ACCMODE) == O_RDONLY) || ((file->flags & O_ACCMODE) == O_RDWR))
			{
				if(file->io_backend_id == CONSOLE_BACK_END)
				{
					if(file->flags & O_NONBLOCK)
					{
						int err = con_read(file->io_backend.con, buf, nbytes, 1 /* nonblock */);
						if(err >= 0)
						{
							return err;
						}
						errno = EIO;
						return -1;
					}
					else
					{
						size_t i;
						char *p = (char *) buf;
						
						int err = 0;
						for(i = 0; i < nbytes; i++)
						{
							err = con_read(file->io_backend.con, &p[i], 1, 0 /* nonblock */);
							
							if(err >= 0)
							{
								if(p[i] == '\n')
								{
									i++;
									break;
								}
							}
							else
							{
								errno = EIO;
								break;
							}
						}
						
						return (err >= 0) ? i : -1;
					}
					
				}
				else if(file->io_backend_id == LFS_BACK_END)
				{
					assert(root_fs != 0);
					int err = lfs_file_read(root_fs, file->io_backend.lf, buf, nbytes);
					
					if(err >= 0)
					{
						return err;
					}
					else
					{
						errno = convert_errno(err);
					}
				}
			}
			else
			{
				errno = EINVAL;
			}
		}
		else
		{
			errno = EINVAL;
		}
	}
    else
	{
		errno = EBADF;
	}
	
	return -1;
}

int write(int fd, const void *buf, size_t nbytes)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if(((file->flags & O_ACCMODE) == O_WRONLY) || ((file->flags & O_ACCMODE) == O_RDWR))
			{
				if(file->io_backend_id == CONSOLE_BACK_END)
				{
					int err = con_write(file->io_backend.con, buf, nbytes);
					return (err >= 0) ? err : -1;
				}
				else if(file->io_backend_id == LFS_BACK_END)
				{
					assert(root_fs != 0);
					int err = lfs_file_write(root_fs, file->io_backend.lf, buf, nbytes);
					
					if(err >= 0)
					{
						return err;
					}
					else
					{
						errno = convert_errno(err);
					}
				}
			}
			else
			{
				errno = EINVAL;
			}
		}
		else
		{
			errno = EINVAL;
		}
    }
    else
	{
		errno = EBADF;
	}
	
	return -1;
}

static char *heap_end = 0;
void *sbrk(ptrdiff_t incr)
{
	extern char __HEAP;
	extern char __HEAP_END;
	char *prev_heap_end;

	if(heap_end == 0)
	{
		heap_end = &__HEAP;
	}
	prev_heap_end = heap_end;

	if((heap_end + incr) > &__HEAP_END)
	{
		/* Heap and stack collision */
		return 0;
	}

	heap_end += incr;
	
	return prev_heap_end;
}

unsigned int alarm(unsigned int seconds)
{
	return 0;
}

int utime(const char *filename, const struct utimbuf *times)
{
	errno = ENOSYS;
	return -1;
}

int chmod(const char *pathname, mode_t mode)
{
	errno = ENOSYS;
	return -1;
}

int fchmod(int fd, mode_t mode)
{
	errno = ENOSYS;
	return -1;
}

int chown(const char *pathname, uid_t owner, gid_t group)
{
	errno = ENOSYS;
	return -1;
}

int fchown(int fd, uid_t owner, gid_t group)
{
	errno = ENOSYS;
	return -1;
}

int lchown(const char *pathname, uid_t owner, gid_t group)
{
	errno = ENOSYS;
	return -1;
}

int truncate(const char *path, off_t length)
{
	errno = ENOSYS;
	return -1;
}

int ftruncate(int fd, off_t length)
{
	if(fd < MAX_FILE_DESCRIPTORS)
	{
		struct file_t *file = &files[fd];
		
		if(file->io_backend_id != NO_BACK_END)
		{
			if(((file->flags & O_ACCMODE) == O_WRONLY) || ((file->flags & O_ACCMODE) == O_RDWR))
			{
				if(file->io_backend_id == LFS_BACK_END)
				{
					assert(root_fs != 0);
					int err = lfs_file_truncate(root_fs, file->io_backend.lf, length);
					
					if(err >= 0)
					{
						return err;
					}
					else
					{
						errno = convert_errno(err);
					}
				}
			}
			else
			{
				errno = EINVAL;
			}
		}
		else
		{
			errno = EINVAL;
		}
    }
    else
	{
		errno = EBADF;
	}
	
	return -1;
}

long sysconf(int name)
{
	switch(name)
	{
		case _SC_CLK_TCK:
			return TICKS_PER_SECONDS; // number of ticks per second
			break;
	}
	
	errno = EINVAL;
	return -1;
}

static clock_t ticks()
{
	return 0xffffffff - (pit_get_lifetime(1) >> 32); // read PIT_1 lifetime (high 32-bit counts down ticks)
}

clock_t times(struct tms *buffer)
{
	if(buffer)
	{
		clock_t t = ticks();
		buffer->tms_utime = t;
		buffer->tms_stime = 0;
		buffer->tms_cutime = 0;
		buffer->tms_cstime = 0;
		return t;
	}
	
	errno = EINVAL;
	return -1;
}

int gettimeofday(struct timeval *tv, void *tz)
{
	if(tv)
	{
		clock_t t = ticks();
		tv->tv_sec = t / TICKS_PER_SECONDS;
		tv->tv_usec = t % TICKS_PER_SECONDS;
	}
	
	return 0;
}
