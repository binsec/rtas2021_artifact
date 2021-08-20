/*
 *  Copyright (c) 2007,
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
 * Authors: 
 *     Gilles Mouchard (gilles.mouchard@cea.fr)
 *     David Parello (david.parello@univ-perp.fr)
 *
 */

/***************************************************************************
                parms.hh  -  superscalar processor parameters
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_PARMETER_HH__
#define __UNISIM_COMPONENT_CLM_PROCESSOR_OOOSIM_PARMETER_HH__

#include <unisim/component/clm/utility/types.hh>
#include <unisim/component/clm/memory/mem_common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace processor {
namespace ooosim {

  using unisim::component::clm::memory::ReplacementPolicyType;
  using unisim::component::clm::memory::randomReplacementPolicy;

/* ISA Parameters */
//const int nSources = 3;
const int nSources = 32;
const int nDestinations = 1;
typedef UInt64 Word;

const int nCPU = 1;
const int nProg = 1;
// Memory subsystem parameters
const int Snooping = 0;

// BUS parameters
const int BUS_BufferSize = 1;
const int BUS_RequestWidth = 32;
//const int BUS_RequestWidth = 64;
//const int BUS_Snooping = 0;

// DRAM
const int DRAM_nBanks = 4;			// number of banks (typically 2 or 4)
const int DRAM_nRows = 8192;		// number of rows (typically 2048, 4096, or 8192)
const int DRAM_nCols = 1024;		// number of columns (typically 256, 512 or 1024)
const int DRAM_TRRD = 2;			// Delay Time, ACTV command in one bank to ACTV command in another bank
const int DRAM_TRAS = 5;			// Delay Time, ACTV command to PRE or PREALL
const int DRAM_TRCD = 2;			// Delay Time, ACTV command to READ, READP, WRT or WRTP command (for READP and WRTP mTRCD <= mTRAS)
const int DRAM_CL = 2;				// Delay Time, CAS Latency, time to wait before Data Out
const int DRAM_TRP = 2;				// Delay Time, PRE or PREALL command to ACTV, MRS, REFR, or SLFR command
const int DRAM_TRC = 7;				// Delay Time, ACTV, REFR or SLFR command to ACTV, MRS, REFR, or SLFR command
const UInt64 DRAM_TREF = 10000000000LL;		// Delay Time, Refresh Interval
//const int DRAM_nDataPathSize = 8;	// Data Bus width in Bytes (typically 8)
//const int DRAM_nCacheLineSize = 64;	// Cache Line Size, i.e. Burst size in bytes
const int DRAM_nDataPathSize = 32;	// Data Bus width in Bytes (typically 8)
//const int DRAM_nDataPathSize = 64;	// Data Bus width in Bytes (typically 8)
const int DRAM_nCacheLineSize = 32;	// Cache Line Size, i.e. Burst size in bytes
const int DRAM_nClockMultiplier = 10; // Clock Multiplier (typically 8 to 16)
//const int DRAM_nCtrlQueueSize = 8;			// Memory Controller queue size
const int DRAM_nCtrlQueueSize = 16;			// Memory Controller queue size

/* L1 Data Cache */
const ReplacementPolicyType DL1_replacementPolicy = randomReplacementPolicy;
//const int DL1_nPorts = 4;
const int DL1_nPorts = 1;
const int DL1_nCPUtoCacheDataPathSize = 8;
const int DL1_nCachetoCPUDataPathSize = 8;
const int DL1_nCPULineSize = 0;
//const int DL1_nMemtoCacheDataPathSize = 64;
//const int DL1_nCachetoMemDataPathSize = 64;
const int DL1_nMemtoCacheDataPathSize = 32;
const int DL1_nCachetoMemDataPathSize = 32;
const int DL1_nMSHR = 8;
const int DL1_nMSHRRead = 8;
//const int DL1_nLineSize = 64;
const int DL1_nLineSize = 32;
const int DL1_nCacheLines = 512;
//const int DL1_nCacheLines = 128;
const int DL1_nAssociativity = 8;
const int DL1_nDelay = 1;
const int DL1_nStages = 1;

/* L1 Instruction Cache */
const ReplacementPolicyType IL1_replacementPolicy = randomReplacementPolicy;
const int IL1_nPorts = 1;
//const int IL1_nCPUtoCacheDataPathSize = 64;
//const int IL1_nCachetoCPUDataPathSize = 64;
const int IL1_nCPUtoCacheDataPathSize = 32;
const int IL1_nCachetoCPUDataPathSize = 32;
//const int IL1_nCachetoCPUDataPathSize = 128;
const int IL1_nCPULineSize = 0;
//const int IL1_nMemtoCacheDataPathSize = 64;
//const int IL1_nCachetoMemDataPathSize = 64;
const int IL1_nMemtoCacheDataPathSize = 32;
const int IL1_nCachetoMemDataPathSize = 32;
//const int IL1_nLineSize = 64;
const int IL1_nLineSize = 32;
const int IL1_nCacheLines = 512;
const int IL1_nAssociativity = 8;
const int IL1_nDelay = 1;
const int IL1_nStages = 1;

// unused
/* L2 Cache */
/*
const ReplacementPolicyType L2_replacementPolicy = randomReplacementPolicy;
const int L2_nPorts = 2;
const int L2_nCPUtoCacheDataPathSize = 64;
const int L2_nCachetoCPUDataPathSize = 64;
const int L2_nCPULineSize = 64;
const int L2_nMemtoCacheDataPathSize = 8;
const int L2_nCachetoMemDataPathSize = 8;
const int L2_nLineSize = 64;
const int L2_nCacheLines = 16384;
const int L2_nAssociativity = 8;
const int L2_nDelay = 1;
const int L2_nStages = 10;
const int L2_nMSHR = 8;
const int L2_nMSHRRead = 8;
*/
/* L2 Cache */
const ReplacementPolicyType L2_replacementPolicy = randomReplacementPolicy;
//const int L2_nPorts = 4;
const int L2_nPorts = 1;
const int L2_nCPUtoCacheDataPathSize = 32;
const int L2_nCachetoCPUDataPathSize = 32;
const int L2_nCPULineSize = 0;
//const int L2_nMemtoCacheDataPathSize = 64;
//const int L2_nCachetoMemDataPathSize = 64;
const int L2_nMemtoCacheDataPathSize = 32;
const int L2_nCachetoMemDataPathSize = 32;
const int L2_nMSHR = 8;
const int L2_nMSHRRead = 8;
//const int L2_nLineSize = 64;
  const int L2_nLineSize = 32; // ORIGINAL
//  const int L2_nLineSize = 64; // BUGGED (Fetch is stalling)
const int L2_nCacheLines = 4096;
//const int L2_nCacheLines = 128;
const int L2_nAssociativity = 8;
const int L2_nDelay = 1;
const int L2_nStages = 1;




/************************** Fetch *********************************/
const int Degree = 4;
/* Fetcher */
//const int fetchWidth = 4;
const int fetchWidth = Degree;
const int InstructionQueueSize = 32;
const int InstructionSize = 4;
const int fetchMaxPendingRequests = 1;
const int MaxBranches = 32;

/* Branch History Table */
const int BHT_Size = 16384;
const int BHT_nLevels = 4;
const int BHT_nHistorySize = 4;

/* Branch Target Buffer */
const int BTB_nlines = 1024;
const int BTB_associativity = 8;

/* Return Address Stack */
const int RAS_Size = 64;

/************************* Decode/Allocate/Rename ************************/

const int allocateRenameWidth = Degree;
const int nAllocateRenameStages = 1;

/*********************** Schedule/Issue **************************/

/* Integer issue queue */
const int IntegerIssueQueueSize = 16;
const int IntegerIssueQueueIssueWidth = Degree;
const int nIntegerIssueQueueWritePorts = Degree;

/* Floating point issue queue */
const int FloatingPointIssueQueueSize = 16;
const int FloatingPointIssueQueueIssueWidth = Degree;
const int nFloatingPointIssueQueueWritePorts = Degree;

/* Load Store Issue Queue */
const int LoadStoreIssueQueueSize = 16;
const int LoadStoreIssueQueueIssueWidth = Degree;
const int nLoadStoreIssueQueueWritePorts = Degree;

/* scheduler */
const int IssueWidth = Degree*3;

/*********************** Register Read ***************************/

/* Registers */
const int nIntegerArchitecturalRegisters = 32;
const int nFloatingPointArchitecturalRegisters = 32;
//
const int nConditionArchitecturalRegisters = 8;
const int nCountArchitecturalRegisters = 1;
const int nFPSCArchitecturalRegisters = 1;
const int nLinkArchitecturalRegisters = 1;
const int nXERArchitecturalRegisters = 4;

const int nIntegerRegisters = 128;
const int nFloatingPointRegisters = 128;
//
const int nConditionRegisters = 32;
const int nCountRegisters = 16;
const int nFPSCRegisters = 20;
const int nLinkRegisters = 16;
const int nXERRegisters = 24;

/* Register file */
const int nReadRegisterStages = 1;
const int IntegerReadRegisterWidth = Degree;
const int FloatingPointReadRegisterWidth = Degree;
const int LoadStoreReadRegisterWidth = Degree;

/************************* Execute ******************************/
/* Integer Units */
const int nIntegerUnits = Degree;
const int integerPipelineDepth = 1;

/* Floating Point units */
const int nFloatingPointUnits = Degree;
const int floatingPointPipelineDepth = 1;

/* Load/Store Unit */
const int nAddressGenerationUnits = Degree;
const int addressGenerationPipelineDepth = 1;
const int loadQueueSize = 32;
const int storeQueueSize = 16;
//const int LSQ_nCDBPorts = Degree;
const int LSQ_nCDBPorts = 1;

/*************************** Write Back **************************/
const int commonDataBusArbiterPorts = nIntegerUnits + nFloatingPointUnits + nAddressGenerationUnits + LSQ_nCDBPorts;

const int nWriteBackStages = 1;
const int WriteBackWidth = commonDataBusArbiterPorts;


/***************************** Retire *****************************/

const int reorderBufferSize = 128;
//const int retireWidth = 32;
const int retireWidth = Degree;
//const int retireWidth = commonDataBusArbiterPorts;

} // end of namespace ooosim
} // end of namespace processor
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
