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
                       load_store_queue.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_LOAD_STORE_QUEUE_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_EXECUTE_LOAD_STORE_QUEUE_HH__

#include <unisim/component/clm/processor/ooosim/parameters.hh>
#include <unisim/component/clm/interfaces/instruction_interface.hh>

#include <unisim/component/clm/interfaces/memreq.hh>

#include <unisim/component/cxx/processor/powerpc/floating.hh>

#include <unisim/component/clm/utility/common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace execute {

  using unisim::component::clm::memory::address_t;
  using unisim::component::clm::memory::endianess_t;
  using unisim::component::clm::memory::ByteArray;

  // Parameters
using unisim::component::clm::processor::ooosim::DL1_nLineSize;

using unisim::component::clm::interfaces::memreq;
using unisim::component::clm::interfaces::memreq_types;

using unisim::component::clm::utility::hexa;
 
using unisim::component::cxx::processor::powerpc::Flags;
using unisim::component::cxx::processor::powerpc::SoftFloat;
using unisim::component::cxx::processor::powerpc::SoftDouble;
  

  //using unisim::service::interfaces::StatisticReporting;


/////////////////////////////////////////
// From AlphaISS alpha/src/alpha_lib.c //
/////////////////////////////////////////
/*
double UInt64toDouble(UInt64 value)
{
	double result;
	//	result = *(Float64_t *) &value;
	result = *(double *) &value;
	return result;
}
*/
/** Transform a double into a single.
		@param double the double to transform
		@return a single into a 32 bits unsigned integer
*/
 /*
UInt32 F32ToLong(double f) {
	double f_double;
	int64_t f_copy;
	UInt32 result;

	f_double = f;
	f_copy = *(int64_t *)&f_double;
	result = (((f_copy >> 32) & 0x0c0000000) | ((f_copy >> 29) & 0x3fffffff));
	return result;
}
 */
/** Transform a single to a double.
		@param val the value to transform
		@return the transformed value
*/
 /*
double LongToF32(UInt32 val) {
    UInt64 val1, val2, val3;
    double f;
    double result;
		
    val1 = val & 0x40000000;
    val2 = (val >> 23) & 0x07f;
    if(val1) {
        if(val2==0x07f)
            val2 = 0x07ff;
        else
            val2 = val2 | 0x0400;
    } else {
        if(val2!=0)
            val2 = val2 | 0x0380;
    }
    val3 = val & 0x0ffffffff;
    val2 = ((val3 & 0x080000000) << 32 ) | val2 << 52 | (val3 & 0x7fffff) << 29;
    f = *(double *)&(val2);
    result = f;
		return result;
}
 */
 /*
UInt64 LongToF32bis(UInt32 val) {

  uint64_t raw_fp64;
  uint32_t raw_fp32 = val;
  
  SoftFloat fp32(raw_fp32);
  Flags flagsConvert;
  SoftDouble fp64(fp32, flagsConvert);
  
  raw_fp64 = fp64.queryValue();
 */
  /*
    UInt64 val1, val2, val3;
    double f;
    double result;
		
    val1 = val & 0x40000000;
    val2 = (val >> 23) & 0x07f;
    if(val1) {
        if(val2==0x07f)
            val2 = 0x07ff;
        else
            val2 = val2 | 0x0400;
    } else {
        if(val2!=0)
            val2 = val2 | 0x0380;
    }
    val3 = val & 0x0ffffffff;
    val2 = ((val3 & 0x080000000) << 32 ) | val2 << 52 | (val3 & 0x7fffff) << 29;
  */
  /*
    f = *(double *)&(val2);
    result = f;
		return result;
  */
  //    return val2;
 /*
  return raw_fp64;
}

*/

INLINE uint64_t LSQMASK(int n)
{
	switch(n)
	{
		case 1: return 0x00000000000000ffLL;
		case 2: return 0x000000000000ffffLL;
		case 3: return 0x0000000000ffffffLL;
		case 4: return 0x00000000ffffffffLL;
		case 5: return 0x000000ffffffffffLL;
		case 6: return 0x0000ffffffffffffLL;
		case 7: return 0x00ffffffffffffffLL;
		case 8: return 0xffffffffffffffffLL;
	}
	exit(-1);
}

enum load_state_t {
	allocated_load,
	issued_load,
	waiting_data_cache_accept_load,
	waiting_data_cache_load,
	finished_hit_sq,
	finished_load,
	flushed_load,
	invalid_load//,
	//	invalid_load // loads which occur on speculative path with highly incorrect address
};

enum store_state_t {
	allocated_store,
	issued_store,
	commited_store,
	waiting_data_cache_accept_store
};

template <class T, int nSources>
class LoadQueueEntry
{
public:
	load_state_t state;
  //InstructionPtr<T, nSources> instruction;
	InstructionPtr instruction;
  int cdbPort;
  int dcachePort;
  bool conflicted;

	friend ostream& operator << (ostream& os, const LoadQueueEntry<T, nSources>& lqe)
	{
		os << lqe.instruction;
		os << ",state=";
		switch(lqe.state)
		{
			case allocated_load:
				os << "allocated_load";
				break;
			case issued_load:
				os << "issued_load";
				break;
			case waiting_data_cache_accept_load:
				os << "waiting_data_cache_accept_load";
				break;
			case waiting_data_cache_load:
				os << "waiting_data_cache_load";
				break;
			case finished_load:
				os << "finished_load";
				break;
			case flushed_load:
				os << "flushed_load";
				break;
			case invalid_load:
				os << "invalid_load";
				break;
		}
		os << ",cdbPort=" << lqe.cdbPort;
		os << ",dcachePort=" << lqe.dcachePort;
		os << ",conflicted=" << lqe.conflicted;
		return os;
	}

	int operator == (const LoadQueueEntry<T, nSources>& entry) const
	{
		return entry.instruction == instruction;
	}
};

template <class T, int nSources>
class StoreQueueEntry
{
public:
	store_state_t state;
  //InstructionPtr<T, nSources> instruction;
	InstructionPtr instruction;
  int cdbPort;
  int dcachePort;
  bool conflicted;

	friend ostream& operator << (ostream& os, const StoreQueueEntry<T, nSources>& sqe)
	{
		os << sqe.instruction;
		os << ",state=";
		switch(sqe.state)
		{
			case allocated_store:
				os << "allocated_store";
				break;
			case issued_store:
				os << "issued_store";
				break;
			case commited_store:
				os << "commited_store";
				break;
			case waiting_data_cache_accept_store:
				os << "waiting_data_cache_accept_store";
				break;
		}
		os << ",cdbPort=" << sqe.cdbPort;
		os << ",dcachePort=" << sqe.dcachePort;
		os << ",conflicted=" << sqe.conflicted;
		return os;
	}

	int operator == (const StoreQueueEntry<T, nSources>& entry) const
	{
		return entry.instruction == instruction;
	}
};

  /*
bool misaligned(InstructionPtr load)
{
  bool res;
  address_t ea = load->ea;
  int size = load->operation->memory_access_size();
  address_t baseaddr = ea & (~((address_t)(DL1_nLineSize - 1)));
  res = (ea + size) > (baseaddr + DL1_nLineSize);
#ifdef DD_DEBUG_MISALIGNED_LOAD
  if (DD_DEBUG_TIMESTAMP < timestamp() )
    {
      if (res)
	{
	  cerr << "[Misaligned] ea           : " << hexa(ea) << endl;
	  cerr << "[Misaligned] baseaddr     : " << hexa(baseaddr) << endl;
	  cerr << "[Misaligned] ea+size      : " << hexa(ea+size) << endl;
	  cerr << "[Misaligned] ba+nLineSize : " << hexa(baseaddr+DL1_nLineSize) << endl;
	  cerr << "[Misaligned] size         : " << size << endl;
	  cerr << "[Misaligned] nLineSize    : " << DL1_nLineSize << endl;
	}
    }
#endif
  return res;
}
*/

template <class T, int nSources, int LoadQueueSize, int StoreQueueSize, int Width, int AllocateWidth, int RetireWidth, int nCPUDataPathSize, int nDataCachePorts, int nCDBPorts>
class LoadStoreQueue : public module
{
public:
	inclock inClock;

	/* From/To Address Generator */
	inport<InstructionPtr> inInstruction[Width];

	/* From/To Allocator/Renamer */
	inport<InstructionPtr> inAllocateLoadInstruction[AllocateWidth];
	inport<InstructionPtr> inAllocateStoreInstruction[AllocateWidth];

	/* From/To L1 Data Cache */
	/*
	ml_out_valid outL1Valid[nDataCachePorts];
	ml_out_enable outL1Enable[nDataCachePorts];
	ml_out_data<InstructionPtr<T, nSources> > outL1Instruction[nDataCachePorts]; //  (for tracing purposes) 
	ml_out_cache_cmd outL1Command[nDataCachePorts];		        //< true if it is a store instruction 
	ml_out_vector<nCPUDataPathSize> outL1Data[nDataCachePorts];	//< the data to write in the memory system 
	ml_out_size outL1Size[nDataCachePorts];		 	       	//< the size of the data to read in bytes 
	ml_out_addr outL1Address[nDataCachePorts]; //< the address of the load/store to execute 
	ml_out_accept outL1Accept[nDataCachePorts];  //< true if load/store unit has accepted the data 
	ml_out_data<int> outL1Tag[nDataCachePorts];

	ml_in_accept inL1Accept[nDataCachePorts];    //< true if data has been accepted 
	ml_in_data<InstructionPtr<T, nSources> > inL1Instruction[nDataCachePorts]; //< load/store instruction 
	ml_in_vector<nCPUDataPathSize> inL1Data[nDataCachePorts]; //< load result 
	ml_in_size inL1Size[nDataCachePorts]; //< the size of the data sent to the CPU 
	ml_in_enable inL1Enable[nDataCachePorts]; //< true if cache has a data to be sent to the load/store unit and it has been accepted 
	ml_in_valid inL1Valid[nDataCachePorts];   //< true if cache has a data to be sent to the load/store unit 
	ml_in_addr inL1Address[nDataCachePorts];
	ml_in_data<int> inL1Tag[nDataCachePorts];
	*/
        outport < memreq < InstructionPtr, nCPUDataPathSize > > outDL1[nDataCachePorts];   ///< To L1 Data Cache
	inport  < memreq < InstructionPtr, nCPUDataPathSize > > inDL1[nDataCachePorts];    ///< From L1 Data Cache


	/* To the common data bus */
	outport<InstructionPtr> outInstruction[nCDBPorts];

	/* From Reorder buffer */
	inport<InstructionPtr> inRetireInstruction[RetireWidth];

	//	ml_in_flush inFlush;
	inport<bool> inFlush;
	outport<bool> outFlush;

	/* To the reorder buffer */
	//	ml_out_data<bool> outBusy;			/*< true there is a pending load/store operation into the load/store queue */
  //	outport<bool> outBusy;


 bool misaligned(InstructionPtr load)
{
  bool res;
  address_t ea = load->ea;
  int size = load->operation->memory_access_size();
  address_t baseaddr = ea & (~((address_t)(DL1_nLineSize - 1)));
  res = (ea + size) > (baseaddr + DL1_nLineSize);
#ifdef DD_DEBUG_MISALIGNED_LOAD
  if (DD_DEBUG_TIMESTAMP < timestamp() )
    {
      if (res)
	{
	  cerr << "[Misaligned] ea           : " << hexa(ea) << endl;
	  cerr << "[Misaligned] baseaddr     : " << hexa(baseaddr) << endl;
	  cerr << "[Misaligned] ea+size      : " << hexa(ea+size) << endl;
	  cerr << "[Misaligned] ba+nLineSize : " << hexa(baseaddr+DL1_nLineSize) << endl;
	  cerr << "[Misaligned] size         : " << size << endl;
	  cerr << "[Misaligned] nLineSize    : " << DL1_nLineSize << endl;
	}
    }
#endif
  return res;
}

  UInt64 LongToF32bis(UInt32 val) {

    uint64_t raw_fp64;
    uint32_t raw_fp32 = val;
    
    SoftFloat fp32(raw_fp32);
    Flags flagsConvert;
    SoftDouble fp64(fp32, flagsConvert);
    
    raw_fp64 = fp64.queryValue();    
    return raw_fp64;
  }

  
  UInt32 F32ToLong(double f) {
    double f_double;
    int64_t f_copy;
    UInt32 result;
    
    f_double = f;
    f_copy = *(int64_t *)&f_double;
    result = (((f_copy >> 32) & 0x0c0000000) | ((f_copy >> 29) & 0x3fffffff));
    return result;
  }
  


	LoadStoreQueue(const char *name, endianess_t endianess) : module(name)
	{
		int i, j;

		class_name = " LoadStoreQueueClass";
		
		// Unisim port names ...
		for (i=0; i<Width; i++)
		  {
		    inInstruction[i].set_unisim_name(this,"inInstruction",i);
		  }		
		for (i=0; i<AllocateWidth; i++)
		  {
		    inAllocateLoadInstruction[i].set_unisim_name(this,"inAllocateLoadInstruction",i);
		  }		
		for (i=0; i<AllocateWidth; i++)
		  {
		    inAllocateStoreInstruction[i].set_unisim_name(this,"inAllocateStoreInstruction",i);
		  }		
		for (i=0; i<nDataCachePorts; i++)
		  {
		    inDL1[i].set_unisim_name(this,"inDL1",i);
		  }		
		for (i=0; i<nDataCachePorts; i++)
		  {
		    outDL1[i].set_unisim_name(this,"outDL1",i);
		  }		
		for (i=0; i<nCDBPorts; i++)
		  {
		    outInstruction[i].set_unisim_name(this,"outInstruction",i);
		  }		
		for (i=0; i<RetireWidth; i++)
		  {
		    inRetireInstruction[i].set_unisim_name(this,"inRetireInstruction",i);
		  }		
		
		inFlush.set_unisim_name(this,"inFlush");
		outFlush.set_unisim_name(this,"outFlush");
		//		outStateChanged(stateChanged);
		//		inStateChanged(stateChanged);
		/*
		SC_HAS_PROCESS(LoadStoreQueue);

		SC_METHOD(ExternalControl);
		sensitive << inStateChanged;
		for(i = 0; i < nDataCachePorts; i++)
			sensitive << inL1Accept[i] << inL1Enable[i] << inL1Instruction[i] << inL1Data[i] << inL1Address[i] << inL1Valid[i];
		for(i = 0; i < nCDBPorts; i++)
			sensitive << inAccept[i];
		for(i = 0; i < Width; i++)
			sensitive << inValid[i];
		for(i = 0; i < AllocateWidth; i++)
			sensitive << inAllocateLoadValid[i] << inAllocateStoreValid[i];

		SC_METHOD(InternalControl);
		sensitive_pos << inClock;
		*/

		for (i=0; i<nDataCachePorts; i++) {
		  sensitive_method(on_outDL1_Accept) << outDL1[i].accept;
		}
		for (i=0; i<Width; i++) {
		  sensitive_method(on_AGU_Data) << inInstruction[i].data;
		}
		for (i=0; i<nDataCachePorts; i++) {
		  sensitive_method(on_inDL1_Data)  << inDL1[i].data;
		}
		for (i=0; i<nCDBPorts; i++) {
		  sensitive_method(on_CDBA_Accept) << outInstruction[i].accept;
		}
		/*
		for (i=0; i<nDataCachePorts; i++) {
		  sensitive_method(on_inDL1_Enable) << inDL1[i].enable;
		}
		*/
		for (i=0; i<RetireWidth; i++) {
		  sensitive_method(on_RetireData) << inRetireInstruction[i].data;
		}
		/*
		for (i=0; i<RetireWidth; i++) {
		  sensitive_method(on_Retire_enable_and_onAllocLDST) << inRetireInstruction[i].enable;
		}
		for (i=0; i<AllocateWidth; i++) {
		  sensitive_method(on_Retire_enable_and_onAllocLDST) << inAllocateLoadInstruction[i].data;
		  sensitive_method(on_Retire_enable_and_onAllocLDST) << inAllocateStoreInstruction[i].data;
		}
		*/

		for (i=0; i<Width; i++) {
		  sensitive_method(on_Enables_and_onAllocLDST) << inInstruction[i].enable;
		}
		for (i=0; i<nDataCachePorts; i++) {
		  sensitive_method(on_Enables_and_onAllocLDST) << inDL1[i].enable;
		}
		for (i=0; i<RetireWidth; i++) {
		  sensitive_method(on_Enables_and_onAllocLDST) << inRetireInstruction[i].enable;
		}
		for (i=0; i<AllocateWidth; i++) {
		  sensitive_method(on_Enables_and_onAllocLDST) << inAllocateLoadInstruction[i].data;
		  sensitive_method(on_Enables_and_onAllocLDST) << inAllocateStoreInstruction[i].data;
		}
		


		sensitive_method(onDataFlush) << inFlush.data;
		sensitive_method(onAcceptFlush) << outFlush.accept;
		sensitive_method(onEnableFlush) << inFlush.enable;		

                sensitive_pos_method(start_of_cycle) << inClock;
                sensitive_neg_method(end_of_cycle) << inClock;

		this->endianess = endianess;

		/* Internal state */
		//		changed = true;
		
		/* Statistics */
		load_queue_cumulative_occupancy = 0;
		store_queue_cumulative_occupancy = 0;
		total_spec_loads = 0;

		load_to_remove = 0;
		store_to_remove = 0;
		store_to_commit = 0;

		for (int i=0; i<nDataCachePorts; i++)
		  inDL1_flushed_ports[i] = false;

	}

  /******************************************************************************
   * How LSQ work 
   ******************************************************************************
   *                                  LSQ 
   *                         +---------------------+
   *                         |                     |
   *            from AGU --->|                     |---> to CDBA
   *                         |                     |
   *                         |                     |
   *                         |                     |
   *                         |                     |
   *                         |                     |
   *      from Alloc (LD)--->|                     |
   *                         |                     |
   *                         |                     |
   *      from Alloc (ST)--->|                     |<--- from Retire
   *                         |                     |
   *                         +---------------------+
   *                             |             *  
   *                             |             | 
   *                             *             | 
   *                           to DL1      from DL1
   *                          (outDL1)      (inDL1)
   *     
   ************************************************************************************************************
   * 1 --- LSQ must accept all reponses coming from the first level data cache (from DL1)
   * 2 --- With conventional microarchitectures CDBA must accept enough request to satisfy 1.
   * 
   * This LSQ have to do his job with the maximum number of configurations
   * by processing with the following processes:
   *
   ************************************************************************************************************
   ************************************************************************************************************
   * THE LASTEST LOAD STORE QUEUE DESIGN
   ************************************************************************************************************
   * 0 -- At start of cycle : Send issued loads to outDL1.
   *                          Maybe we need to check for store-load forwarding before...
   *
   * 1 -- on outDL1 Accept  : We answer enable signals,
   *        1.1 - for loads we compute the number of loads to move in waiting_data_cache_load state.
   *        1.2 - for store we compute the number of store to remove.
   *
   * 2 -- on AGU Data       : always accept data from AGU (no problem entries in queue are already allocated)
   *                          Compute the number of loads/stores to move in issued state.
   *
   * 3 -- on AGU Enable     : nothing to do... (we will check enable signals at end of cycle...)
   *
   * 4 -- on inDL1 Data     : Forward data on CDBA...
   *
   * 5 -- on CDBA Accept    : forward accept to inDL1...
   *
   * 6 -- on inDL1 Enable   : forward enable to CDBA...
   *                          (We will check enable signals at end of cycle to move some load in finished state)
   *
   * 7 -- on Retirement and Allocation :
   *         7.1 -- Compute # of store to move to commited state
   *         7.2 -- Compute the new # of load to remove... 
   *         7.3 -- Compute free space in queue and eventualy accept Allocation request.
   *
   * 8 -- on Flush          : forward all flush signals...
   *
   * 9 -- At end_of_cycle   :
   *         9.0 -- if (Flush) then flush.
   *                else
   *         9.1 -- Remove store_to_remove from store queue
   *         9.2 -- Remove load_to_remove from load queue
   *         9.3 -- Check inDL1 enable signals and scan load queue to move load from waiting_data_cache_load state
   *                to finished_load state.
   *         9.5 -- Move waiting_data_cache load from issued_load state to waiting_data_cache_load state 
   *         9.6 -- Move issued load from allocated_load state to issued_load.
   *         9.7 -- Allocate and place in queue allocated instructions.
   *
   *
   ************************************************************************************************************
   ************************************************************************************************************/

  void on_Enables_and_onAllocLDST()
  {
    bool areallknown(true);
    int i;
    // AGU known ?
    for (i=0;i<Width;i++)
      {
	areallknown &= inInstruction[i].enable.known();
      }
    // inDL1 known?
    for (i=0;i<nDataCachePorts;i++)
      {
	areallknown &= inDL1[i].enable.known();
      }
    // Retire known?
    for (i=0;i<RetireWidth;i++)
      {
	areallknown &= inRetireInstruction[i].enable.known();
      }
    // Are Allocate data signals known?
    for (i=0;i<AllocateWidth;i++)
      {
	areallknown &= inAllocateLoadInstruction[i].data.known();
	areallknown &= inAllocateStoreInstruction[i].data.known();
      }

    if (areallknown)
      {	
	//	Instruction *InvalidLoad = NULL;
	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;

#ifdef DD_DEBUG_LSQ_VERB2
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  cerr << "[DD_DEBUG_LSQ]: ("<< timestamp() <<" In on_Enables... Are all known !!!" << endl;
#endif
#ifdef DD_DEBUG_LSCONFLICT_VERB2
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  { 
	    cerr << "[---DD_DEBUG_LSCONFLICT_VERB2---(Enables)("<<timestamp()<<")]" << endl;
	  }
#endif

	// Check for LoadStore Traps...
 	// -----------------------------
	for(i = 0; i < Width; i++)
	  {
	    if(!inInstruction[i].enable) break;
	    InstructionPtr instruction = inInstruction[i].data;
	    //	    const Instruction *instruction = &inst;
	
	    if(instruction->fn & FnStore)
	      {
		StoreQueueEntry<T, nSources> *store = SearchStore(instruction);
		
		if(store)// && store->state == issued_store)
		  {
		    /*
#ifdef DD_DEBUG_LSCONFLICT_VERB2
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  { 
	    cerr << "[---DD_DEBUG_LSCONFLICT_VERB2---(Enables)("<<timestamp()<<") Store Instruction : ]" << endl;
	    //  cerr << "\t\t\t"<< store->instruction<<endl;
	    cerr << "\t\t\t"<< *instruction<<endl;
	  }
#endif
		    */
		    for(load = loadQueue.SeekAtHead(); load; load++)
		      {
			/*
#ifdef DD_DEBUG_LSCONFLICT_VERB2
	if (DD_DEBUG_TIMESTAMP < timestamp())
	  { 
	    cerr << "[---DD_DEBUG_LSCONFLICT_VERB2---(Enables)("<<timestamp()<<") in For load loop...]" << endl;
	    cerr << "[---DD_DEBUG_LSCONFLICT_VERB2---(Enables)("<<timestamp()<<") Load Instruction : ]" << endl;
	    cerr << "\t\t\t"<< load->instruction<<endl;
	  }
#endif
			*/
			//			if(load->instruction.may_need_replay && (load->state == waiting_data_cache_load || load->state == finished_load))
			if(  (load->state == waiting_data_cache_load) ||
			     (load->state == finished_load) ||
			     ( (load->state == issued_load) && (load->dcachePort > -1) ) 
			     // case of load which will move from issued to waiting at end_of_cycle !!!
			     )
			  {
			    if(load->instruction > instruction)
			      {
				//if(LoadStoreConflict(&(load->instruction), &(store->instruction)))
				if(LoadStoreConflict(load->instruction, instruction))
				  {
#ifdef DD_DEBUG_LSCONFLICT_VERB2
				    if (DD_DEBUG_TIMESTAMP < timestamp())
				      { 
					cerr << "[---DD_DEBUG_LSCONFLICT---(on store issue)("<<timestamp()<<")]!!!! detected a replay trap" << endl;
					cerr << "!!!!!!!!!!!!!!!! \t\t LSQ: " << load->instruction << " and " << instruction 
					     << " conflict" << endl;
				      }
#endif
				    load->instruction->replay_trap = true;

				    if(load->state == finished_load)
				      load->state = invalid_load;
				    /*
				    if (!InvalidLoad)
				      {
					InvalidLoad = &(load->instruction);
				      }
				    */
				  }
			      }
			  }
		      }
		  }
		else
		  {
		    cerr << "Error : (" << timestamp() << ") --- Store not in store queue !!!" << endl;
		    abort();
		  }
	      }
	    else // Instruction is a load
	      {
		// Issue the load (nothing to do at all...)
	      }
	  }

	// If there is no traps forward load values to CDBA...
 	// -----------------------------
	//	if (!InvalidLoad)
	//	  {
	int cdbPort;
	int dataCachePort;
	//	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> first_invalid_load;
	//	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;
     
	// Accept as many data from DL1 as available CDBA prots
	for(dataCachePort = 0, cdbPort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	  {
	    //	    if (inDL1[dataCachePort].data.something())
	    //	      {
	    // Converting data format from Memory interface to pipeline data format...
	    //		outInstruction[cdbPort].data = inDL1[dataCachePort].data;
	    //for(dataCachePort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	    //  {
	    
	    // Search for the first invalid load in load queue :
	    for (first_invalid_load = loadQueue.SeekAtHead(); first_invalid_load; first_invalid_load++)
	      {
		if( (first_invalid_load->state == invalid_load) ) break;
	      }
	    //		if(inL1Valid[dataCachePort])
	    if(inDL1[dataCachePort].data.something())
	      {
		/* Got a data cache response and a common data bus arbiter accept */
		//load = SearchLoad(inL1Instruction[dataCachePort]);
		memreq< InstructionPtr, nCPUDataPathSize > tmpreq = inDL1[dataCachePort].data;
		//		    Instruction inst = inDL1[dataCachePort].data.instr;
		//		    load = SearchLoad(inDL1[dataCachePort].data.instruction);
		load = SearchLoad(tmpreq.instr);
		
		
		if(load)// && first_invalid_load && (load->instruction < first_invalid_load->instruction))
		  {
		    if(load->state == flushed_load)
		      {
			if ( first_invalid_load )// && (load->instruction < first_invalid_load->instruction))
			  {
			    outInstruction[cdbPort].data = first_invalid_load->instruction;
			  }
			else
			  {
			    inDL1_flushed_ports[dataCachePort] = true;
			    outInstruction[cdbPort].data.nothing();
			  }
		      }
		    else
		      {
			if(!first_invalid_load || 
			   (first_invalid_load && 
			    (load->instruction < first_invalid_load->instruction)
			    )
			   )
			{ 
			  if(load->state == waiting_data_cache_load)
			  {
			    memreq< InstructionPtr, nCPUDataPathSize > tmpreq = inDL1[dataCachePort].data;
			    InstructionPtr instruction = tmpreq.instr;
			    ByteArray<nCPUDataPathSize> vector = tmpreq.data;
			    T data=0;
			    // DD: Becarefull when copying data !!!
			    // DD: Here everything have to be rewritten... we must check destination type... 
			    if(instruction->fn == FnLoadFloat)
			      {
				switch(instruction->operation->memory_access_size())
				  {
				    uint64_t tmp64;
				    uint32_t tmp32;
				  case 4:
				    vector.Read(tmp32, 0, endianess);
				    data = tmp32 & 0xffffffff;
				    break;
				  case 8:
				    vector.Read(tmp64, 0, endianess);
				    data = tmp64;
				    break;
				  default:
				    cerr << "Warning unknow size !!!" << endl;
				    abort();
				    break;
				  }
			      }
			    else
			      {
				//switch(tmpreq.size)
				switch(instruction->operation->memory_access_size())
				  {
				    uint64_t tmp64;
				    uint32_t tmp32;
				    uint16_t tmp16;
				    uint8_t tmp8;
				    
				  case 1:
				    //vector.Read(((uint8_t&)data), 0, endianess);
				    vector.Read(tmp8, 0, endianess);
				    data = tmp8 & 0x000000ff;
				    break;
				  case 2:
				    //vector.Read(((uint16_t&)data), 0, endianess);
				    vector.Read(tmp16, 0, endianess);
				    data = tmp16 & 0x0000ffff;
				    break;
				  case 4:
				    //vector.Read(((uint32_t&)data), 0, endianess);
				    vector.Read(tmp32, 0, endianess);
				    data = tmp32 & 0xffffffff;
				    break;
				  case 8:
				    //vector.Read(((uint64_t&)data), 0, endianess);
				    vector.Read(tmp64, 0, endianess);
				    data = tmp64;
				    //data = data & 0x000000ff;
				    break;
				  default:
				    cerr << "Warning unknow size !!!" << endl;
				    abort();
				    break;
				  }
			      }
			    
			    //#ifdef DD_DEBUG_LSQ
#ifdef DD_DEBUG_FPLOAD
			    //Mourad Modifs
			    if (DD_DEBUG_TIMESTAMP < timestamp())
			      {
				cerr << "[DD_DEBUG_LSQ]: data: " << hexa(data) << endl;
			      }
#endif
			    //	if(instruction->fn & FnIntSextLoad)
			    //if(instruction->fn & FnIntExtended) // Faux !!!! // Creer une fonction booleene qui dit s'il faut faire une extension de signe...
			    if (instruction->operation->is_sign_extended())
			      {
				// do a sign extension
				instruction->destinations[0].data = SignExtension(data, instruction->operation->memory_access_size() * 8);
			      }
			    //				else if(instruction->fn & FnFpLoad)
			    //			else if(instruction->fn & FnLoadFloat)
			    else
			    {
			    if(instruction->fn == FnLoadFloat)
			      {
				if(instruction->operation->memory_access_size() == 4)
				  {
				    // do a single to double precision floating point conversion
				    //   double tmp = LongToF32((UInt32) data);
				    //				    double tmp = LongToF32((uint32_t) data);
				    /*
				    Flags tmp_flags;
				    SoftDouble tmp_res;
				    double tmp = (tmp_res.assign(SoftFloat((UInt32) data),tmp_flags)).queryValue();
				    */
				    //instruction->destinations[0].data = *(UInt64 *) &tmp;
				    //			    instruction->destinations[0].data = *(uint64_t *) &tmp;
				    instruction->destinations[0].data = LongToF32bis((uint32_t) data);
				  }
				else
				  {
				    // no floating point conversion needed
				    instruction->destinations[0].data = data;
				  }
#ifdef DD_DEBUG_FPLOAD
       if (DD_DEBUG_TIMESTAMP < timestamp() )
	 {
	   cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== [DD_FPLOAD] Floating Point Load ! ==== " << endl;
	   cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Instruction : "<< *instruction <<" ==== " << endl;
	   cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Converted Value : "<< hexa(instruction->destinations[0].data) <<" ==== " << endl;
	   cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Original Value(32) : "<< hexa((uint32_t)data) <<" ==== " << endl;
	   cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Original Value(64) : "<< hexa(data) <<" ==== " << endl;
	 }
#endif
			      }
			    else
			      {
				// no sign extension needed
				instruction->destinations[0].data = data;
#ifdef DD_DEBUG_LSQ
if (DD_DEBUG_TIMESTAMP < timestamp())
  cerr << "[DD_DEBUG_LSQ]: In else: data           : " << hexa(data) << endl;
  cerr << "[DD_DEBUG_LSQ]: In else destination.data: " << hexa(instruction->destinations[0].data) << endl;
#endif
			      }
			    }
			    if(load->instruction->replay_trap) instruction->replay_trap = true;
			    outInstruction[cdbPort].data = instruction;
			    load->cdbPort = cdbPort;
#ifdef DD_DEBUG_LSQ_VERB3
if (DD_DEBUG_TIMESTAMP < timestamp())
  cerr << "[DD_DEBUG_LSQ]: instruction sent: " << *instruction << endl;
 cerr << "[DD_DEBUG_LSQ]: destination.data: " << hexa(instruction->destinations[0].data) << endl;
#endif
			  }
			else
			  {
			    cerr << *this;
			    cerr << "time stamp = " << timestamp() << endl;
			    cerr << "ExternalControl: Load (" << load->instruction << ";" 
			      //<< inDL1[dataCachePort].data.instruction 
				 << ") into Load Queue" << endl;
			    abort();//exit(-1); //<< ") does not wait for DL1 response" << endl;
			  }
			}// end of "else" of "if is a flush"
			else
			{
			  outInstruction[cdbPort].data = first_invalid_load->instruction;
			}
		      }
		  } // end of if(load)
		else
		  {
		    cerr << *this;
		    cerr << "In on_inDL1_Data ...." << endl;
		    cerr << "time stamp = " << timestamp() << endl;
		    cerr << "in Load Store External Control while getting response from Data Cache" << endl;
		    //cerr << "Can't find load (" << inDL1[dataCachePort].data.instruction << ") into Load Queue" << endl;
		    cerr << "Cache Response : " << tmpreq << endl;
		    cerr << "Guilty Inst : " << tmpreq.instr << endl;
		    exit(-1);
		  }
		// DD Debuging accept signal cycles... Always accept response from DL1 !!! 
		//inDL1[dataCachePort].accept = true;
	      }
	    else
	      {
		if (first_invalid_load)
		  {
		    outInstruction[cdbPort].data = first_invalid_load->instruction;
		  }
		else
		  {
		    outInstruction[cdbPort].data.nothing();
		  }
		// DD Debuging accept signal cycles... Always accept response from DL1 !!! 
		//inDL1[dataCachePort].accept = false;
	      }
	  } // end of For (data...)
	//	  } // End of if (!InvalidLoad)
	/*
	else
	  { // Send the InvalidLoad...
	    int cdbPort=0;
	    outInstruction[cdbPort].data = *InvalidLoad;
	    for(cdbPort = 1; cdbPort < nCDBPorts; cdbPort++)
	      outInstruction[cdbPort].data.nothing();
	  }
	*/
	// Retire instruction ...
 	// -----------------------------
	for (i=0;i<RetireWidth;i++)
	  {
	    if(!inRetireInstruction[i].enable) break;
	    InstructionPtr instruction = inRetireInstruction[i].data;
	    //	    const Instruction *instruction = &inst;
	    
	    if(instruction->fn & FnStore)
	      {
		StoreQueueEntry<T, nSources> *store = SearchStore(instruction);
		
		if(store && store->state == issued_store)
		  {
		    // commit a store
		    /*
		    store->state = commited_store;
		    for(load = loadQueue.SeekAtHead(); load; load++)
		      {
			if(load->instruction->may_need_replay && (load->state == waiting_data_cache_load || load->state == finished_load))
			  {
			    if(load->instruction > store->instruction)
			      {
				if(LoadStoreConflict(load->instruction, store->instruction))
				  {
				    // 	cerr << "!!!!!!!!!!!!!!!!! detected a replay trap" << endl;
				    //	cerr << "!!!!!!! LSQ: " << load->instruction << " and " << store->instruction << " conflict" << endl;
				    //  cerr << *this;
				    //  exit(1);
				    load->instruction->replay_trap = true;
				  }
			      }
			  }
		      }
		    */
		    store_to_commit++;
		  }
	      }
	    else 
	      {
		if (instruction->fn & FnLoad)
		  {
		    LoadQueueEntry<T, nSources> *load = SearchLoad(instruction);
		
		    if(load && load->state == finished_load)
		      {
			// Remove the load
			load_to_remove++;
		      }
		  }
	      }
	  } // end of for
	//
	// Now Compute number of ld/st to alloc...
	//
	int loadQueueFreeSpace = loadQueue.FreeSpace();
	int storeQueueFreeSpace = storeQueue.FreeSpace();
	int load_to_add = 0;
	int store_to_add = 0;
	
	for (i=0;i<AllocateWidth;i++)
	  {
	    if (inAllocateLoadInstruction[i].data.something())
	      load_to_add++;
	    if (inAllocateStoreInstruction[i].data.something())
	      store_to_add++;
	  }
	
	int load_to_accept = MIN(loadQueueFreeSpace + load_to_remove, load_to_add);
	int store_to_accept = MIN(storeQueueFreeSpace + store_to_remove, store_to_add);
	
	//	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== LSQ ==== LD to accept = " << load_to_accept << endl;
	//	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== LSQ ==== ST to accept = " << store_to_accept << endl;

	for(i = 0; i < load_to_accept; i++)
	  inAllocateLoadInstruction[i].accept = true;
	for(; i < AllocateWidth; i++)
	  inAllocateLoadInstruction[i].accept = false;
	
	for(i = 0; i < store_to_accept; i++)
	  inAllocateStoreInstruction[i].accept = true;
	for(; i < AllocateWidth; i++)
	  inAllocateStoreInstruction[i].accept = false;

      } // end of : if (areallknow) ...
  } // end of : on_Enables_and_...




  /************************************************************************************************************
   ************************************************************************************************************/

  void on_RetireData()
  {
	  bool areallknown(true);
	  int i;
	  for (i=0;i<RetireWidth;i++)
	    {
	      areallknown &= inRetireInstruction[i].data.known();
	    }
	  if (areallknown)
	    {
	      for (i=0;i<RetireWidth;i++)
		{
		  if (inRetireInstruction[i].data.something())
		    {
		      inRetireInstruction[i].accept = true;
		    }
		  else
		    {
		      inRetireInstruction[i].accept = false;
		    }
		}
	    }
  }

  /************************************************************************************************************
   * 0 -- At start of cycle...
   ************************************************************************************************************/
  void start_of_cycle()
  {
    //    int i;
    //    int cdbPort;
    int dataCachePort;
    OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
    QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;
    
    load_to_remove = 0;
    store_to_remove = 0;
    store_to_commit = 0;

    for (int i=0; i<nDataCachePorts; i++)
      inDL1_flushed_ports[i] = false;
    // SOC
    /* Data Cache Access/Common Data Bus Access */
    dataCachePort = 0;
    if(!loadQueue.Empty() || !storeQueue.Empty())
      {
	load = loadQueue.SeekAtHead();
	store = storeQueue.SeekAtHead();
	
	do
	  {
	    if(load)
	      {
		do
		  {
		    /*
		    if(load->state == issued_load && !LoadStoreConflict(&(load->instruction)))
		      {
			// found a load that can access to the data cache
			break;
		      }
		    */
		    if(load->state == issued_load)
		      {
			//bool may_conflict = LoadStoreMayConflict(&(load->instruction));
			load->conflicted = LoadStoreConflict(load->instruction);
			//			if (!load->conflicted && !may_conflict)
			if (!load->conflicted)
			  {
			    // found a load that can access to the data cache
			    break;
			  }
		      }
		    load++;
		  } while(load);
	      }
	    
	    if(load)
	      {
		/*
		bool load_hit_store = false;
		// Does the load hit the store queue ?
		// Is it a store-load forwarding ?
		if(load->state == issued_load && !(load->instruction.fn & FnPrefetchLoad))
		  {
		    // scan the store queue to find a store producing a data used by a load
		    for(store = storeQueue.SeekAtTail(); store; store--)
		      {
			if(store->instruction < load->instruction)
			  {
			    // there is a store older than the load
			    if(! (store->state == allocated_store))
			      {
				// there is an unresolved store effective address so we can't forward a result to the load
				//break;
				//			      }
				//			    else 
				//			      {
				//				if((store->instruction->ea >= load->instruction->ea && store->instruction->ea < load->instruction->ea + load->instruction->operation->memory_access_size()) ||
				if((store->instruction.ea >= load->instruction.ea && store->instruction.ea < load->instruction.ea + load->instruction.operation->memory_access_size()) ||
				   (load->instruction.ea >= store->instruction.ea && load->instruction.ea < store->instruction.ea + store->instruction.operation->memory_access_size()))
				  {
				    // load and store memory access overlap
				    if(store->instruction.ea == load->instruction.ea &&
				       store->instruction.operation->memory_access_size() >= load->instruction.operation->memory_access_size())
				      {
					load_hit_store = true;
					load->state = finished_hit_sq;
				      }
				    else
				      {
					// partial store detected: load must go to the data cache
				      }
				    break;
				  }
			      }
			  }
		      }
		  }
		*/
		load->instruction->may_need_replay = LoadStoreMayConflict(load->instruction);
		if(load->instruction->may_need_replay) total_spec_loads++;
		
		// load can access to the data cache
		/*
		outL1Valid[dataCachePort] = true;
		outL1Instruction[dataCachePort] = load->instruction;
		outL1Size[dataCachePort] = load->instruction->operation->memory_access_size;
		outL1Address[dataCachePort] = load->instruction->ea;
		outL1Command[dataCachePort] = (load->instruction->fn & FnPrefetchLoad) ? PREFETCH_CACHE_COMMAND : READ_CACHE_COMMAND;
		outL1Tag[dataCachePort] = loadQueue.GetIdent(load);
		*/
				      /* Do a request to the instruction cache */
		// We disable store-load forwarding...
		//		if (!load_hit_store)
		  {
		    memreq < InstructionPtr, nCPUDataPathSize > tmp_memreq;
		    tmp_memreq.instr = load->instruction;
		    tmp_memreq.address = load->instruction->ea;
		    tmp_memreq.size = load->instruction->operation->memory_access_size();
		    tmp_memreq.command = memreq_types::cmd_READ;
		    tmp_memreq.uid = load->instruction->inum;
		    tmp_memreq.sender_type = memreq_types::sender_CPU;
		    tmp_memreq.message_type = memreq_types::type_REQUEST;
		    tmp_memreq.sender = this;
		    tmp_memreq.req_sender = this;
		    outDL1[dataCachePort].data = tmp_memreq;
		    load->dcachePort = dataCachePort;
		    dataCachePort++;

		    //		    load->state = waiting_data_cache_accept_load;
		  }
		//		dataCachePort++;
		load++;
	      }
	    else
	      {
		if(store && store->state == commited_store)
		  {
		    // committed store can access to the data cache
		    ByteArray<nCPUDataPathSize> data;

		    //if( store->instruction.function == FnStoreFloat ) // Bad
		    //if( store->instruction.fn == FnStoreFloat ) // Good
		    if( store->instruction->operation->function == FnStoreFloat )
		      {
#ifdef DD_DEBUG_FPSTORE
  if (DD_DEBUG_TIMESTAMP < timestamp() )
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== [DD_FPSTORE] Floating Point Store ! ==== " << endl;
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Instruction : "<< store->instruction <<" ==== " << endl;
    }
#endif
			// DD: Be carefull when copying data from registers !!!
			// For Single-Precision floating-point store we need to 
			switch( store->instruction->operation->memory_access_size())
			  {
			  case 4:
			    // For Single-Precision floating-point store we need to write into memory
			    // the true single-precision value.
			    //data.Write((uint32_t)(store->instruction.sources[0].data), 0, endianess);
#ifdef DD_DEBUG_FPSTORE
  if (DD_DEBUG_TIMESTAMP < timestamp() )
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== [DD_FPSTORE] Floating Point Store of size 4 ! ==== " << endl;
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Instruction : "<< store->instruction <<" ==== " << endl;
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Value : "<< store->instruction->singleprecision <<" ==== " << endl;
    }
#endif
  //data.Write((uint32_t)(store->instruction.operation->get_single(dummy_cpu)), 0, endianess);
                            if (store->instruction->operation->store_need_conversion())
			      {
				data.Write((uint32_t)(store->instruction->singleprecision), 0, endianess);
			      }
			    else
			      {
				data.Write((uint32_t)(store->instruction->sources[0].data), 0, endianess);      
			      }
			    break;
			  case 8:
			    data.Write((uint64_t)(store->instruction->sources[0].data), 0, endianess);
			    break;
			  default:
			    data.Write((uint32_t)(store->instruction->sources[0].data), 0, endianess);
			    cerr << "Warning unknown size !!!" << endl;
			    abort();
			    break;
			  }			
		      }
		    else
		      {

#ifdef DD_DEBUG_FPSTORE
  if (DD_DEBUG_TIMESTAMP < timestamp() )
    {
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== [DD_INTSTORE] Integer Store ! ==== " << endl;
      cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== Instruction : "<< store->instruction <<" ==== " << endl;
    }
#endif
			// DD: Be carefull when copying data from registers !!!
			switch( store->instruction->operation->memory_access_size())
			  {
			  case 1:
			    data.Write((uint8_t)(store->instruction->sources[0].data), 0, endianess);
			    break;
			  case 2:
			    data.Write((uint16_t)(store->instruction->sources[0].data), 0, endianess);
			    break;
			  case 4:
			    data.Write((uint32_t)(store->instruction->sources[0].data), 0, endianess);
			    break;
			  case 8:
			    data.Write((uint64_t)(store->instruction->sources[0].data), 0, endianess);
			    break;
			  default:
			    data.Write((uint32_t)(store->instruction->sources[0].data), 0, endianess);
			    cerr << "Warning unknown size !!!" << endl;
			    abort();
			    break;
			  }
		      }
		    /*
		    outL1Valid[dataCachePort] = true;
		    outL1Instruction[dataCachePort] = store->instruction;
		    outL1Size[dataCachePort] = store->instruction->operation->memory_access_size;
		    outL1Address[dataCachePort] = store->instruction->ea;
		    outL1Command[dataCachePort] = WRITE_CACHE_COMMAND;
		    outL1Data[dataCachePort].Write(data, store->instruction->operation->memory_access_size);
		    outL1Tag[dataCachePort] = -1;
		    */
		    memreq < InstructionPtr, nCPUDataPathSize > tmp_memreq;
		    tmp_memreq.instr = store->instruction;
		    tmp_memreq.address = store->instruction->ea;
		    tmp_memreq.size = store->instruction->operation->memory_access_size();
		    tmp_memreq.command = memreq_types::cmd_WRITE;
		    tmp_memreq.uid = store->instruction->inum;
		    tmp_memreq.sender_type = memreq_types::sender_CPU;
		    tmp_memreq.message_type = memreq_types::type_REQUEST;
		    tmp_memreq.sender = this;
		    tmp_memreq.req_sender = this;
		    		    tmp_memreq.data = data;		    
		    //		    tmp_memreq.data.Write(data);
		    outDL1[dataCachePort].data = tmp_memreq;
		    store->dcachePort = dataCachePort;
		    /*
		      if(store->instruction->ea == 0x000000011ff96160LL)
		      {
		      cerr << "time stamp = " << sc_time_stamp() << " : store at 0x000000011ff96160" << endl;
		      }
		    */
		    dataCachePort++;
		    store++;
		  }
		else
		  {
		    // no more load or store can access to the data cache
		    break;
		  }
	      }
	  } while(dataCachePort < nDataCachePorts && (load || store));
      }
    
    while(dataCachePort < nDataCachePorts)
      {
	//outL1Valid[dataCachePort] = false;
	outDL1[dataCachePort].data.nothing();
	dataCachePort++;
      }   

  }// end of : start_of_cycle

  /************************************************************************************************************
   * 1 -- on outDL1 Accept...
   ************************************************************************************************************/
  void on_outDL1_Accept()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<nDataCachePorts;i++)
      {
	areallknown &= outDL1[i].accept.known();
      }
    if (areallknown)
      {
	int cdbPort;
	int dataCachePort;
	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;
     
	//	for (i=0;i<nDataCachePorts;i++)
	//	  {
	// For stores compute the # of store to remove...
	/* Data Cache Accept for a load/store */
	dataCachePort = 0;
	load = loadQueue.SeekAtHead();
	store = storeQueue.SeekAtHead();
	
	do
	  {
	    if(load)
	      {
		do
		  {
		    if(load->state == issued_load && 
		    //		    if(load->state == waiting_data_cache_accept_load && 
		       ((load->instruction->fn & FnPrefetchLoad) || !LoadStoreConflict(load->instruction)))
		       //    ((load->instruction.fn & FnPrefetchLoad) || !load->conflicted))
		      {
			// found a load that accesses to the data cache
			break;
		      }
		    load++;
		  } while(load);
	      }
	    
	    if(load)
	      {
		// load accesses to the data cache
		//		if(inL1Accept[dataCachePort])
		if (load->dcachePort != dataCachePort)
		  {
		    cerr << "DD: Error, dcache port does not match !!!" << endl;
		    cerr << *load << endl;
		    abort();
		  }
		if (outDL1[dataCachePort].accept)
		  {
		    // data cache accepts the load
		    //		    outL1Enable[dataCachePort] = true;
		    outDL1[dataCachePort].enable = true;
		    if(load->instruction->fn == FnPrefetchLoad)
		      {
			// prefetch load will be removed from the load queue
			load_to_remove++;
		      }
		  }
		else
		  {
		    //		    outL1Enable[dataCachePort] = false;
		    load->dcachePort = -1;
		    outDL1[dataCachePort].enable = false;
		  }
		dataCachePort++;
		load++;
	      }
	    else
	      {
		if(store && store->state == commited_store)
		  {
		    // committed store accesses to the data cache
		    //		    if(inL1Accept[dataCachePort])
		    if (store->dcachePort != dataCachePort)
		      {
			cerr << "DD: Error, dcache port does not match !!!" << endl;
			cerr << *store << endl;
			abort();
		      }
		    if (outDL1[dataCachePort].accept)
		      {
			// data cache accepts a committed store
			store_to_remove++;
			outDL1[dataCachePort].enable = true;
		      }
		    else
		      {
			store->dcachePort = -1;
			outDL1[dataCachePort].enable = false;
		      }
		    dataCachePort++;
		    store++;
		  }
		else
		  {
		    // no more load or store access to the data cache
		    break;
		  }
	      }
	  } while(dataCachePort < nDataCachePorts && (load || store));
	
	while(dataCachePort < nDataCachePorts)
	  {
	    //outL1Enable[dataCachePort] = false;
	    outDL1[dataCachePort].enable = false;
	    dataCachePort++;
	  }
      } // end of: if (areallknow)
  } // end of: on_outDL1_Accept...

  /************************************************************************************************************
   * 2 -- on AGU Data...
   ************************************************************************************************************/
  void on_AGU_Data()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<Width;i++)
      {
	areallknown &= inInstruction[i].data.known();
      }
    if (areallknown)
      {
	// Always accept data from AGU because entries in queues are already reserved.
	// We will copy addresses at end of cycle.
	for (i=0;i<Width;i++)
	  {
	    inInstruction[i].accept = inInstruction[i].data.something();
	  }
      }
  }

  /************************************************************************************************************
   * 3 -- on AGU Enable     : nothing to do... (we will check enable signals at end of cycle...)
   ************************************************************************************************************/

  /************************************************************************************************************
   * 4 -- on inDL1 Data     : Forward data on CDBA... NO !!!!!!!!!!!!!!!!!! JUST ACCEPT DATA
   ************************************************************************************************************/
  void on_inDL1_Data()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<nDataCachePorts;i++)
      {
	areallknown &= inDL1[i].data.known();
      }
    if (areallknown)
      {
	for (i=0;i<nDataCachePorts;i++)
	  {
	    inDL1[i].accept = inDL1[i].data.something();
	  }
      }
  }

  void on_inDL1_Data_Old()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<nDataCachePorts;i++)
      {
	areallknown &= inDL1[i].data.known();
      }
    if (areallknown)
      {
	int cdbPort;
	int dataCachePort;
	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
	//	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;
     
	// Accept as many data from DL1 as available CDBA prots
	for(dataCachePort = 0, cdbPort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	  {
	    //	    if (inDL1[dataCachePort].data.something())
	    //	      {
	    // Converting data format from Memory interface to pipeline data format...
	    //		outInstruction[cdbPort].data = inDL1[dataCachePort].data;
	    //for(dataCachePort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	    //  {
		bool valid = false;
		bool enable = false;
		bool L1Accept = false;
		
		//		if(inL1Valid[dataCachePort])
		if(inDL1[dataCachePort].data.something())
		  {
		    /* Got a data cache response and a common data bus arbiter accept */
		    //load = SearchLoad(inL1Instruction[dataCachePort]);
		    memreq< InstructionPtr, nCPUDataPathSize > tmpreq = inDL1[dataCachePort].data;
		    //		    Instruction inst = inDL1[dataCachePort].data.instr;
		    //		    load = SearchLoad(inDL1[dataCachePort].data.instruction);
		    load = SearchLoad(&(tmpreq.instr));
		    //loadQueue.Retrieve(inL1Tag[dataCachePort]);
		    
		    if(load)
		      {
			if(load->state == flushed_load)
			  {
			    /*
			    L1Accept = true;
			    if(inL1Enable[dataCachePort])
			      {
				// flushed load will be removed from the load queue
				load_to_remove++;
			      }
			    */
			    //inDL1.accept = true;
			    //		for (int i=0; i<nDataCachePorts; i++)
			    inDL1_flushed_ports[dataCachePort] = true;
			    outInstruction[cdbPort].data.nothing();
	    

			  }
			else
			  {
			    if(load->state == waiting_data_cache_load)
			      {
				//InstructionPtr<T, nSources> instruction = inL1Instruction[dataCachePort];
				//Instruction inst;
				//Instruction *instruction = &inst;
				memreq< InstructionPtr, nCPUDataPathSize > tmpreq = inDL1[dataCachePort].data;
				//				Instruction inst= inDL1[dataCachePort].data.inst;
				//				Instruction *instruction = &inst;
				InstructionPtr instruction = &(tmpreq.instr);
				// remove the load from the load queue */
				//L1Accept = inAccept[cdbPort];
				//				L1Accept = outInstruction[cdbPort].accept;
				//				valid = true;
				
				//ByteArray<nCPUDataPathSize> vector = inDL1[dataCachePort].data.data;
				ByteArray<nCPUDataPathSize> vector = tmpreq.data;
				T data=0;
				// DD: Becarefull when copying data !!!
				// DD: Here everything have to be rewritten... we must check destination type... 
				if(instruction->fn == FnLoadFloat)
				  {
				    switch(instruction->operation->memory_access_size())
				      {
					uint64_t tmp64;
					uint32_t tmp32;
				      case 4:
					vector.Read(tmp32, 0, endianess);
					data = tmp32 & 0xffffffff;
					break;
				      case 8:
					vector.Read(tmp64, 0, endianess);
					data = tmp64;
					break;
				      default:
					cerr << "Warning unknow size !!!" << endl;
					abort();
					break;
				      }
				  }
				else
				  {
				    //switch(tmpreq.size)
				    switch(instruction->operation->memory_access_size())
				      {
					uint64_t tmp64;
					uint32_t tmp32;
					uint16_t tmp16;
					uint8_t tmp8;
					
				      case 1:
					//vector.Read(((uint8_t&)data), 0, endianess);
					vector.Read(tmp8, 0, endianess);
					data = tmp8 & 0x000000ff;
					break;
				      case 2:
					//vector.Read(((uint16_t&)data), 0, endianess);
					vector.Read(tmp16, 0, endianess);
					data = tmp16 & 0x0000ffff;
					break;
				      case 4:
					//vector.Read(((uint32_t&)data), 0, endianess);
					vector.Read(tmp32, 0, endianess);
					data = tmp32 & 0xffffffff;
					break;
				      case 8:
					//vector.Read(((uint64_t&)data), 0, endianess);
					vector.Read(tmp64, 0, endianess);
					data = tmp64;
					//data = data & 0x000000ff;
					break;
				      default:
					cerr << "Warning unknow size !!!" << endl;
					abort();
					break;
				      }
				  }

				//#define DD_DEBUG_LSQ
#ifdef DD_DEBUG_LSQ
				//Mourad Modifs
				if (DD_DEBUG_TIMESTAMP < timestamp())
				  {
				cerr << "[DD_DEBUG_LSQ]: data: " << hexa(data) << endl;
				  }
#endif
				//	if(instruction->fn & FnIntSextLoad)
				//if(instruction->fn & FnIntExtended) // Faux !!!! // Creer une fonction booleene qui dit s'il faut faire une extension de signe...
				//				if (0)
				if (instruction->operation->is_sign_extended())
				  {
				    // do a sign extension
				    instruction->destinations[0].data = SignExtension(data, instruction->operation->memory_access_size() * 8);
				  }
				//				else if(instruction->fn & FnFpLoad)
				//			else if(instruction->fn & FnLoadFloat)
				else if(instruction->fn == FnLoadFloat)
				  {
				    if(instruction->operation->memory_access_size() == 4)
				      {
					// do a single to double precision floating point conversion
					//double tmp = LongToF32((UInt32) data);
					double tmp = LongToF32bis((UInt32) data);
					instruction->destinations[0].data = *(UInt64 *) &tmp;
				      }
				    else
				      {
					// no floating point conversion needed
					instruction->destinations[0].data = data;
				      }
				  }
				else
				  {
				    // no sign extension needed
				    instruction->destinations[0].data = data;
				    //#define DD_DEBUG_LSQ
#ifdef DD_DEBUG_LSQ
if (DD_DEBUG_TIMESTAMP < timestamp())
				  {
				    cerr << "[DD_DEBUG_LSQ]: In else: data           : " << hexa(data) << endl;
				    cerr << "[DD_DEBUG_LSQ]: In else destination.data: " << hexa(instruction->destinations[0].data) << endl;
				  }
#endif
				  }
				
				if(load->instruction->replay_trap) instruction->replay_trap = true;
				outInstruction[cdbPort].data = *instruction;
				load->cdbPort = cdbPort;
				//#define DD_DEBUG_LSQ
#ifdef DD_DEBUG_LSQ
if (DD_DEBUG_TIMESTAMP < timestamp())
				  {
				cerr << "[DD_DEBUG_LSQ]: instruction sent: " << *instruction << endl;
				cerr << "[DD_DEBUG_LSQ]: destination.data: " << hexa(instruction->destinations[0].data) << endl;
				  }
#endif
				//if(inL1Enable[dataCachePort])
				//  enable = true;
				//inDL1.accept = true;
			      }
			    else
			      {
				cerr << *this;
				cerr << "time stamp = " << timestamp() << endl;
				cerr << "ExternalControl: Load (" << load->instruction << ";" 
				  //<< inDL1[dataCachePort].data.instruction 
				     << ") into Load Queue" << endl;
				abort();//exit(-1); //<< ") does not wait for DL1 response" << endl;
				//				exit(-1);
			      }
			  }
		      } // end of if(load)
		    else
		      {
			cerr << *this;
			cerr << "In on_inDL1_Data ...." << endl;
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "in Load Store External Control while getting response from Data Cache" << endl;
			//cerr << "Can't find load (" << inDL1[dataCachePort].data.instruction << ") into Load Queue" << endl;
			cerr << "Cache Response : " << tmpreq << endl;
			cerr << "Guilty Inst : " << tmpreq.instr << endl;
			exit(-1);
		      }

		    // DD Debuging accept signal cycles... Always accept response from DL1 !!! 
		    //inDL1[dataCachePort].accept = true;
		  }
		else
		  {
		    outInstruction[cdbPort].data.nothing();
		    // DD Debuging accept signal cycles... Always accept response from DL1 !!! 
		    //inDL1[dataCachePort].accept = false;

		  }
		/*		
			outL1Accept[dataCachePort] = L1Accept;
			outValid[cdbPort] = valid;
			outEnable[cdbPort] = enable;
		*/
	  } // end of For (data...)
	/*
	  while(dataCachePort < nDataCachePorts)
	  {
	  //		outL1Accept[dataCachePort] = false;
	  inDL1[dataCachePort].data
	  dataCachePort++;
	  }
	    while(cdbPort < nCDBPorts)
	    {
	    outValid[cdbPort] = false;
	    outEnable[cdbPort] = false;
	    cdbPort++;
	    }
	*/
      } // end of if(areallknown)...
    /*
	else
	{
	outInstruction[cdbPort].data.nothing();
	}
    */
} // end of on_inDL1_Data


  /************************************************************************************************************
   * 5 -- on CDBA Accept    : forward accept to inDL1...
   ************************************************************************************************************/
  void on_CDBA_Accept()
  {
    bool areallknown(true);
    int i;
    //    for (i=0;i<Width;i++)
    for (i=0;i<nCDBPorts;i++)
      {
	areallknown &= outInstruction[i].accept.known();
      }
    if (areallknown)
      {
	/*
	for(int dataCachePort = 0, cdbPort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	  {
	    // DD Debuging accept signal cycles... Always accept response from DL1 !!! 
	    // inDL1 data have already been accepted in on_inDL1_Data...
	    // Automatic accept !!!	    
	    if (inDL1_flushed_ports[dataCachePort])
	      {
		inDL1[dataCachePort].accept = true;
	      }
	    else
	      {
		inDL1[dataCachePort].accept = outInstruction[cdbPort].accept;
	      }
	    
	  }
	*/
	for(int cdbPort = 0; cdbPort < nCDBPorts; cdbPort++)
	  {
	    outInstruction[cdbPort].enable = outInstruction[cdbPort].accept;
	  }
      }
  }


  /************************************************************************************************************
   * 6 -- on inDL1 Enable   : forward enable to CDBA...
   ************************************************************************************************************/
  void on_inDL1_Enable()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<nDataCachePorts;i++)
      {
	areallknown &= inDL1[i].enable.known();
      }
    if (areallknown)
      {
	for(int dataCachePort = 0, cdbPort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
	  {
	    //	    outInstruction[cdbPort].enable = inDL1[dataCachePort].enable;
	    if (inDL1_flushed_ports[dataCachePort])
	      {
		//		inDL1[dataCachePort].accept = true;
		outInstruction[cdbPort].enable = false;
	      }
	    else
	      {
		//		inDL1[dataCachePort].accept = outInstruction[cdbPort].accept;
		outInstruction[cdbPort].enable = inDL1[dataCachePort].enable;
	      }
	    

	    
	  }
      }
  }


  /************************************************************************************************************
   * 7 -- on Retirement and Allocation :
   ************************************************************************************************************/
  void on_Retire_enable_and_onAllocLDST()
  {
    bool areallknown(true);
    int i;
    for (i=0;i<RetireWidth;i++)
      {
	areallknown &= inRetireInstruction[i].enable.known();
      }
    // Are Allocate data signals known ?
    for (i=0;i<AllocateWidth;i++)
      {
	areallknown &= inAllocateLoadInstruction[i].data.known();
	areallknown &= inAllocateStoreInstruction[i].data.known();
      }

    if (areallknown)
      {
	for (i=0;i<RetireWidth;i++)
	  {
	    if(!inRetireInstruction[i].enable) break;
	    InstructionPtr instruction = inRetireInstruction[i].data;
	    //	    const Instruction *instruction = &inst;
	    
	    if(instruction->fn & FnStore)
	      {
		StoreQueueEntry<T, nSources> *store = SearchStore(instruction);
		
		if(store && store->state == issued_store)
		  {
		    // commit a store
		    /*
		    store->state = commited_store;
		    for(load = loadQueue.SeekAtHead(); load; load++)
		      {
			if(load->instruction->may_need_replay && (load->state == waiting_data_cache_load || load->state == finished_load))
			  {
			    if(load->instruction > store->instruction)
			      {
				if(LoadStoreConflict(load->instruction, store->instruction))
				  {
				    // 	cerr << "!!!!!!!!!!!!!!!!! detected a replay trap" << endl;
				    //	cerr << "!!!!!!! LSQ: " << load->instruction << " and " << store->instruction << " conflict" << endl;
				    //  cerr << *this;
				    //  exit(1);
				    load->instruction->replay_trap = true;
				  }
			      }
			  }
		      }
		    */
		    store_to_commit++;
		  }
	      }
	    else 
	      {
		if (instruction->fn & FnLoad)
		  {
		    LoadQueueEntry<T, nSources> *load = SearchLoad(instruction);
		
		    if(load && load->state == finished_load)
		      {
			// Remove the load
			load_to_remove++;
		      }
		  }
	      }
	  } // end of for
	//
	// Now Compute number of ld/st to alloc...
	//
	int loadQueueFreeSpace = loadQueue.FreeSpace();
	int storeQueueFreeSpace = storeQueue.FreeSpace();
	int load_to_add = 0;
	int store_to_add = 0;
	
	for (i=0;i<AllocateWidth;i++)
	  {
	    if (inAllocateLoadInstruction[i].data.something())
	      load_to_add++;
	    if (inAllocateStoreInstruction[i].data.something())
	      store_to_add++;
	  }
	
	int load_to_accept = MIN(loadQueueFreeSpace + load_to_remove, load_to_add);
	int store_to_accept = MIN(storeQueueFreeSpace + store_to_remove, store_to_add);
	
	//	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== LSQ ==== LD to accept = " << load_to_accept << endl;
	//	cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== LSQ ==== ST to accept = " << store_to_accept << endl;

	for(i = 0; i < load_to_accept; i++)
	  inAllocateLoadInstruction[i].accept = true;
	for(; i < AllocateWidth; i++)
	  inAllocateLoadInstruction[i].accept = false;
	
	for(i = 0; i < store_to_accept; i++)
	  inAllocateStoreInstruction[i].accept = true;
	for(; i < AllocateWidth; i++)
	  inAllocateStoreInstruction[i].accept = false;

      } // end of : if (areallknow) ...
  } // end of : on_Retire_enable_and_...


/************************************************************************************************************
 * 8 -- on Flush          : forward all flush signals...
 ************************************************************************************************************/
void onDataFlush() { if (inFlush.data.something()) outFlush.data = inFlush.data; else outFlush.data.nothing(); }
void onAcceptFlush() { inFlush.accept = outFlush.accept; }
void onEnableFlush() { outFlush.enable = inFlush.enable; }

/************************************************************************************************************
 * 9 -- At end_of_cycle   :
 *       Here we redo all stuff from step 1 to 8 and we change the state of the module. 
 ************************************************************************************************************/
void end_of_cycle()
  {
    int i;
    int cdbPort;
    int dataCachePort;
    OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
    QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;
    //Mourad Modifs
#ifdef DD_DEBUG_LSQ_VERB2
    if (DD_DEBUG_TIMESTAMP < timestamp())
      {

    cerr << "[DD_DEBUG_LSQ]: Begin of EOC: loadQueue: " <<  endl << loadQueue << endl;
    cerr << "[DD_DEBUG_LSQ]: Begin of EOC: load_to_remove = " << load_to_remove << endl;
      }
#endif     


#ifdef DD_DEBUG_LSQ_VERB2
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
    cerr << "[DD_DEBUG_LSQ]: After Flsuh of EOC: loadQueue: " << endl << loadQueue << endl;
    cerr << "[DD_DEBUG_LSQ]: After Flush of EOC: load_to_remove = " << load_to_remove << endl;
      }
#endif     
    /////////////////////////////////////////////////////////////////////////
    // Store to remove
    /////////////////////////////////////////////////////////////////////////
    for (i=0; i<store_to_remove; i++)
      {
	// We want to remove splitted store information
	//	store = storeQueue.SeekAtHead();
	// if there is a store...
	/*
	if (store)
	  {
	    // if the store is a splitted store...
	    if (store->instruction.splitted_instruction)
	      {
		delete store->instruction.splitted_instruction;
		store->instruction.splitted_instruction = NULL;
	      }
	  }
	*/
	// We may need to check the store to remove... 
	storeQueue.RemoveHead();
      } 
    /////////////////////////////////////////////////////////////////////////
    // Load to remove
    /////////////////////////////////////////////////////////////////////////
    for (i=0; i<load_to_remove; i++)
      {
	// We may need to check the load to remove... 
	loadQueue.RemoveHead();
      }

#ifdef DD_DEBUG_LSQ_VERB2
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
    cerr << "[DD_DEBUG_LSQ]: After LS removing of EOC: loadQueue: " <<  endl << loadQueue << endl;
    cerr << "[DD_DEBUG_LSQ]: After LS removing of EOC: load_to_remove = " << load_to_remove << endl;
      }
#endif     

    /////////////////////////////////////////////////////////////////////////
    // Commit stores
    /////////////////////////////////////////////////////////////////////////
    /* Retire */
    for(i = 0; i < RetireWidth; i++)
      {
	if(!inRetireInstruction[i].enable) break;
	InstructionPtr instruction = inRetireInstruction[i].data;
	//	const Instruction *instruction = &inst;
	
	if(instruction->fn & FnStore)
	  {
	    StoreQueueEntry<T, nSources> *store = SearchStore(instruction);
	    
	    if(store && store->state == issued_store)
	      {
		// commit a store
		store->state = commited_store;
		// DDDD
		/*
		store->instruction = inst;
		for(load = loadQueue.SeekAtHead(); load; load++)
		  {
		    if(load->instruction.may_need_replay && (load->state == waiting_data_cache_load || load->state == finished_load))
		      {
			if(load->instruction > store->instruction)
			  {
			    if(LoadStoreConflict(&(load->instruction), &(store->instruction)))
			      {
#ifdef DD_DEBUG_LSCONFLICT
			    if (DD_DEBUG_TIMESTAMP < timestamp())
			      { 
				cerr << "[---DD_DEBUG_LSCONFLICT---(RETIRE)("<<timestamp()<<")]!!!! detected a replay trap" << endl;
				cerr << "!!!!!!! LSQ: " << load->instruction << " and " << store->instruction 
				     << " conflict" << endl;
			      }
#endif
				load->instruction.replay_trap = true;
			      }
			  }
		      }
		  }
		*/
	      }
	    else
	      {
		cerr << "Store do not have to commit !!!" << endl;
		abort();
	      }
	  }
      }

    //   // Accept as many data from DL1 as available CDBA prots
    /////////////////////////////////////////////////////////////////////////
    // Check inDL1 enables signals
    /////////////////////////////////////////////////////////////////////////
    for(dataCachePort = 0, cdbPort = 0; dataCachePort < nDataCachePorts && cdbPort < nCDBPorts; dataCachePort++, cdbPort++)
      {
	bool valid = false;
	bool enable = false;
	bool L1Accept = false;
	
	//		if(inL1Valid[dataCachePort])
	//	    if(inDL1[dataCachePort].data.something())
	if(inDL1[dataCachePort].enable)
	  {
	    /* Got a data cache response and a common data bus arbiter accept */
	    //load = SearchLoad(inL1Instruction[dataCachePort]);
	    //	    load = SearchLoad(inDL1[dataCachePort].data.instr);
	    memreq<InstructionPtr, nCPUDataPathSize> tmpreq = inDL1[dataCachePort].data;
	    load = SearchLoad(tmpreq.instr);
	    //loadQueue.Retrieve(inL1Tag[dataCachePort]);
	    
	    if(load)
	      {
		if(load->state == flushed_load)
		  {
		    loadQueue.Remove(load);
		  }
		else
		  {
		    if(load->state == waiting_data_cache_load)
		      {
			load->state = finished_load;
		      }
		    else
		      {
			cerr << *this;
			cerr << "time stamp = " << timestamp() << endl;
			//cerr << "ExternalControl: Load (" << load->instruction << ";" << inL1Instruction[dataCachePort] << ") does not wait for DL1 response" << endl;
			exit(-1);
		      }
		  }
	      }
	    else
	      {
		cerr << *this;
		cerr << "In EOC ...." << endl;
		cerr << "time stamp = " << timestamp() << endl;
		cerr << "in Load Store External Control while getting response from Data Cache" << endl;
		//cerr << "Can't find load (" << inL1Instruction[dataCachePort] << ") into Load Queue" << endl;
		//		exit(-1);
		abort();
	      }
	  }
      }

    /////////////////////////////////////////////////////////////////////////
    /* Store->Load forwarding */
    /////////////////////////////////////////////////////////////////////////
    // We disable store-load forwarding because this process doesn't send finished loads...
    // We have to correct this bug...
    /*
    if(!loadQueue.Empty())
      {
	load = loadQueue.SeekAtHead();
	do
	  {
	    if(load->state == issued_load && !(load->instruction.fn & FnPrefetchLoad))
	      {
		// scan the store queue to find a store producing a data used by a load
		for(store = storeQueue.SeekAtTail(); store; store--)
		  {
		    if(store->instruction < load->instruction)
		      {
			// there is a store older than the load
			if(store->state == allocated_store)
			  {
			    // there is an unresolved store effective address so we can't forward a result to the load
			    break;
			  }
			
			if((store->instruction.ea >= load->instruction.ea && store->instruction.ea < load->instruction.ea + load->instruction.operation->memory_access_size()) ||
			   (load->instruction.ea >= store->instruction.ea && load->instruction.ea < store->instruction.ea + store->instruction.operation->memory_access_size()))
			  {
			    // load and store memory access overlap
			    if(store->instruction.ea == load->instruction.ea &&
			       store->instruction.operation->memory_access_size() >= load->instruction.operation->memory_access_size())
			      {
				// load and store memory access match, so forward the store result to the load
				//	if(load->instruction.fn & FnIntSextLoad)
				//if(load->instruction.fn & (FnIntExtended)) //Faux!!!!
				if(0)
				  {
				    // do a sign extension
				    // DD: We assume that main destination is in destinations[0] and loads always have a destination... We don't deal yet with multiple loads...
				    load->instruction.destinations[0].data = SignExtension(store->instruction.sources[0].data & LSQMASK(load->instruction.operation->memory_access_size()), load->instruction.operation->memory_access_size() * 8);
				  }
				else if(load->instruction.fn & FnLoadFloat)
				  {
				    if(load->instruction.operation->memory_access_size() == 4)
				      {
					// do a single to double precision floating point conversion
					double tmp = LongToF32((UInt32) store->instruction.sources[0].data & LSQMASK(load->instruction.operation->memory_access_size()));
					load->instruction.destinations[0].data = *(UInt64 *) &tmp;
				      }
				    else
				      {
					// no floating point conversion needed
					// this complicated code sequence is for computing the same result as the emulator
					UInt64 tmp = store->instruction.sources[0].data & LSQMASK(load->instruction.operation->memory_access_size());
					double tmp2 = UInt64toDouble(tmp);
					load->instruction.destinations[0].data = *(UInt64 *) &tmp2;
				      }
				  }
				else
				  {
				    // no sign extension needed
				    load->instruction.destinations[0].data = store->instruction.sources[0].data & LSQMASK(load->instruction.operation->memory_access_size());
				  }
				
				// make the load finished
				load->state = finished_load;
			      }
			    else
			      {
				// partial store detected: load must go to the data cache
			      }
			    break;
			  }
		      }
		  }
	      }
	  } while(++load);
      } // end of: if (loadQueue.isEmpty()) ... // (end of store-load forwarding)

    */ // End of store-load forwarding disabling...

    /////////////////////////////////////////////////////////////////////////
    // Moving from issued to waiting_data_cache loads...
    /////////////////////////////////////////////////////////////////////////
    /* Data Cache Accept for a load/store */
    dataCachePort = 0;
    load = loadQueue.SeekAtHead();
    store = storeQueue.SeekAtHead();

    // Here is a bug: we don't know if the selected load is the load which sent a request...
    // Maybe now, yes know with the intermediate waiting_data_cache_accept_load state...
    do
      {
	if(load)
	  {
	    do
	      {
		/*
		if(load->state == issued_load )
		  {
		    load->conflicted = LoadStoreConflict(&(load->instruction));
		    if ( (load->instruction.fn & FnPrefetchLoad) || !load->conflicted )
		      {
			break;
		      }
		  }
		*/
		if(load->state == issued_load && 
		//		if(load->state == waiting_data_cache_accept_load && 
		   //		   ((load->instruction.fn & FnPrefetchLoad) || !LoadStoreConflict(&(load->instruction))))
		   // DD: WARNING
		   // Load conflict with a store in on_DL1_Accept() but the store may have been remove just before... 
		   ((load->instruction->fn & FnPrefetchLoad) || !load->conflicted))
		  {
		    // found a load that accesses to the data cache
		    break;
		  }
		load++;
	      } while(load);
	  }
	
	if(load)
	  {
	    // load accesses to the data cache
	    //		if(inL1Accept[dataCachePort])
	    if (outDL1[dataCachePort].accept)
	      {
		// data cache accepts the load
		//		    outL1Enable[dataCachePort] = true;
		//outDL1[dataCachePort].enable = true;
		/*
		if(load->instruction->fn & FnPrefetchLoad)
		      {
			// prefetch load will be removed from the load queue
			//load_to_remove++;
		      }
		*/
		if (load->state == issued_load)
		//		if (load->state == waiting_data_cache_accept_load)
		  load->state = waiting_data_cache_load;
	      }
	    /*
	    else
	      {
		//		    outL1Enable[dataCachePort] = false;
		outDL1[dataCachePort].enable = false;
	      }
	    */
	    dataCachePort++;
	    load++;
	  }
	else
	  {
	    if(store && store->state == commited_store)
	      {
		/*
		// committed store accesses to the data cache
		//		    if(inL1Accept[dataCachePort])
		if (outDL1[dataCachePort].accept)
		  {
		    // data cache accepts a committed store
		    store_to_remove++;
		    outDL1[dataCachePort].enable = true;
		  }
		else
		  {
		    outDL1[dataCachePort].enable = false;
		  }
		*/
		dataCachePort++;
		store++;
	      }
	    else
	      {
		// no more load or store access to the data cache
		break;
	      }
	  }
      } while(dataCachePort < nDataCachePorts && (load || store));
    /*
      while(dataCachePort < nDataCachePorts)
      {
      //outL1Enable[dataCachePort] = false;
      outDL1[dataCachePort].enable = false;
      dataCachePort++;
      }
    */
    //  } // end of: for (...)
   

    /////////////////////////////////////////////////////////////////////////
    // Moving loads and stores from allocated state to issued state... 
    /////////////////////////////////////////////////////////////////////////
    /* Get the effective address of load/store instructions that have been issued */
    for(i = 0; i < Width; i++)
      {
	//	if(inEnable[i])
	if(inInstruction[i].enable)
	  {
	    InstructionPtr instruction = inInstruction[i].data;
	    //	    Instruction *instruction = &inst;
	    
	    if(!instruction->must_reschedule)
	      {
		if(instruction->fn & FnLoad)
		  {
		    //////////////////////////////////////////////////////////////////////////////
		    
		    LoadQueueEntry<T, nSources> *entry = SearchLoad(instruction);

		    if(entry)
		      {
			if(entry->state == allocated_load)
			  {
			    // a load has its effective address ready
			    entry->state = issued_load;
			    entry->instruction = instruction;
			    //			    changed = true;
			    if (misaligned(entry->instruction))
			      {
#ifdef DD_DEBUG_MISALIGNED_LOAD
				if (DD_DEBUG_TIMESTAMP < timestamp() )
				  {
				    cerr << "time stamp = " << timestamp() << endl;
				    cerr << "load misaligned : " << *instruction << ")" << endl;
				    //cerr << loadQueue;
				    //abort();
				  }
#endif
				entry->state = finished_load;
				entry->instruction->exception = MISALIGNMENT_EXCEPTION;
			      }
			  }
			else
			  {
			    cerr << "time stamp = " << timestamp() << endl;
			    cerr << "load has already been issued (" << instruction << ")" << endl;
			    cerr << loadQueue;
			    abort();
			  }
		      }
		    else
		      {
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "in Load Store Internal Control while getting address from Address Generation Unit" << endl;
			cerr << "Can't find load (" << *instruction << ") into load queue" << endl;
			cerr << loadQueue;
			abort();
		      }
		    //////////////////////////////////////////////////////////////////////////////		    
		  }
		else if(instruction->fn & FnStore)
		  {
		    StoreQueueEntry<T, nSources> *entry = SearchStore(instruction);
		    
		    if(entry)
		      {
			if(entry->state == allocated_store)
			  {
#ifdef DD_DEBUG_LSQ_VERB3
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
	cerr << "[DD_DEBUG_LSQ_V3]("<<timestamp()<<"): Before Store copy !!! : " <<  endl << storeQueue << endl;
      }
#endif     
			    // a store has its effective address ready
			    entry->state = issued_store;
			    entry->instruction = instruction;

#ifdef DD_DEBUG_LSQ_VERB3
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
	cerr << "[DD_DEBUG_LSQ_V3]("<<timestamp()<<"): After Store copy !!! : " <<  endl << storeQueue << endl;
	cerr << "[DD_DEBUG_LSQ_V3]("<<timestamp()<<"): ===Instruction=== : " <<  endl << *instruction << endl;
      }
#endif     
                            //  Conversion is NOT needed for all floatingpoint memory access of size 4 !!!

// if(   (entry->instruction.fn == FnStoreFloat) &&
//       (entry->instruction.operation->memory_access_size() == 4)
// )
                            if ( entry->instruction->operation->store_need_conversion()) 
			      {
				// Conversion to single precision
				entry->instruction->sources[0].data = (UInt64) F32ToLong(*(double *) &entry->instruction->sources[0].data);
				// Oups get_single need a CPU as parameter :-(
				//entry->instruction.sources[0].data = (UInt64) entry->instruction.operation->get_single());
			      }

#ifdef DD_DEBUG_LSQ_VERB3
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
	cerr << "[DD_DEBUG_LSQ_V3]("<<timestamp()<<"): After Conversion !!! : " <<  endl << storeQueue << endl;
      }
#endif     
			    // The following commented code is no more usefull since we detect loadstore traps
			    // in on_Enables_... replay_trap load have just been sent to CDBA.

			    // DD_DEBUG : We need to check conflict with issued, waiting, and finished loads...
			    //LoadQueueEntry<T, nSources> *load=NULL;
			    //QueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
			    /*
			    OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;
			    for(load = loadQueue.SeekAtHead(); load; load++)
			      {
				if( (load->state == waiting_data_cache_load || load->state == finished_load) )
				  {
				    if(load->instruction > store->instruction)
				      {
#ifdef DD_DEBUG_LSCONFLICT
					if (DD_DEBUG_TIMESTAMP < timestamp())
					  { 
					    cerr << "[---DD_DEBUG_LSCONFLICT---(ISSUE)] Maybe ..." << endl;
					    cerr << " LD: " << load->instruction << " with ST: " << store->instruction << endl;
					  }
#endif
					if(LoadStoreConflict(&(load->instruction), &(store->instruction)))
					  {
#ifdef DD_DEBUG_LSCONFLICT
					    if (DD_DEBUG_TIMESTAMP < timestamp())
					      { 
						cerr << "[---DD_DEBUG_LSCONFLICT---(ISSUE)]!!!! detected a replay trap" << endl;
						cerr << "!!!!!!! LSQ: " << load->instruction << " and " << store->instruction 
						     << " conflict" << endl;
					      }
#endif
					    load->instruction.replay_trap = true;
					  }
				      }
				  }
			      }
			    */
			    // END DD_DEBUG...
			  }
			else
			  {
			    cerr << "time stamp = " << timestamp() << endl;
			    cerr << "store has already been issued (" << instruction << ")" << endl;
			    cerr << storeQueue;
			    //exit(-1);
			    abort();
			  }
		      }
		    else
		      {
			cerr << "time stamp = " << timestamp() << endl;
			cerr << "Can't find store (" << instruction << ") into store queue" << endl;
			cerr << storeQueue;
			//exit(-1);
			abort();
		      }
		  }
	      }
	  }
      }
    
    //////////////////////////////////////////////////////////
    // Get incoming allocate request and allocate load/store entries in queues...
    //////////////////////////////////////////////////////////
    /* Adding a load/store into the load queue/store queue */
    for(i = 0; i < AllocateWidth; i++)
      {
	if(!inAllocateLoadInstruction[i].enable) break;
	InstructionPtr instruction = inAllocateLoadInstruction[i].data;
	//	const Instruction *instruction = &inst; 
	
	if(instruction->fn & FnLoad)
	  {
	    // add a load into load queue
	    LoadQueueEntry<T, nSources> *entry = loadQueue.New();
	    
	    entry->state = allocated_load;
	    entry->instruction = instruction;
	    entry->cdbPort = -1;
	    entry->dcachePort = -1;
	    entry->conflicted = false;
	  }
      }
    
    /* Adding a load/store into the load queue/store queue */
    for(i = 0; i < AllocateWidth; i++)
      {
	if(!inAllocateStoreInstruction[i].enable) break;
	InstructionPtr instruction = inAllocateStoreInstruction[i].data;
	//	const Instruction *instruction = &inst;
	
	if(instruction->fn & FnStore)
	  {
	    // add a store into store queue
	    StoreQueueEntry<T, nSources> *entry = storeQueue.New();
	    entry->state = allocated_store;
	    entry->instruction = instruction;
	    entry->cdbPort = -1;
	    entry->dcachePort = -1;
	    entry->conflicted = false;
	  }
      }

    // We flush at the end of end_of_cycle...
    /////////////////////////////////////////////////////////////////////////
    // 8 on Flush...
    /////////////////////////////////////////////////////////////////////////
    //    if (inFlush.data && inFlush.enable)
    if (inFlush.enable && inFlush.data.something())
    {
      if (inFlush.data)
      {
#ifdef DD_DEBUG_LSQ_VERB2
if (DD_DEBUG_TIMESTAMP < timestamp())
      {
    cerr << "[DD_DEBUG_LSQ]: We are FLUSHING !!! " <<  endl << loadQueue << endl;
      }
#endif     
#ifdef DD_DEBUG_FLUSH
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ====  Flush !!!" << endl;
#endif
	/* Flush loads */
	for(load = loadQueue.SeekAtHead(); load;)
	  {
	    //if(load->state == issued_load || load->state == waiting_data_cache_load || load->state == flushed_load)
	    if(load->state == waiting_data_cache_load || load->state == flushed_load)
	      {
		load->state = flushed_load;
		load++;
	      }
	    else
	      {
		loadQueue.Remove(load);
	      }
	  }
	
	/* Flush non-committed stores */
	StoreQueueEntry<T, nSources> *store;
	for(store = storeQueue.GetTail(); store; store = storeQueue.GetTail())
	  {
	    if(store->state == commited_store) break;
	    storeQueue.RemoveTail();
	  }
      }
    }




  } // end of: enf_of_cycle

  // END OF THE LASTEST DESIGN

  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////

 
  //        void ExternalControl()
  // First expected signals are: 
  /*
	data of inInstruction[Width]
	data of inAllocateLoadInstruction[AllocateWidth]
	data of inAllocateStoreInstruction[AllocateWidth]
        accept of outDL1[nDataCachePorts]
	enable of inDL1[nDataCachePorts]
	none of outInstruction[nCDBPorts];
	data of inRetireInstruction[RetireWidth];
  */

  //	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> SearchLoad(const InstructionPtr<T, nSources>& instruction)
	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> SearchLoad(const InstructionPtr instruction)
	{
		OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> entry;

		for(entry = loadQueue.SeekAtHead(); entry; entry++)
		{
			if(entry->instruction->inum == instruction->inum)
				return entry;
		}
		return 0;
	}

  //	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> SearchStore(const InstructionPtr<T, nSources>& instruction)
	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> SearchStore(const InstructionPtr instruction)
	{
		QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> entry;

		for(entry = storeQueue.SeekAtHead(); entry; entry++)
		{
			if(entry->instruction->inum == instruction->inum)
				return entry;
		}
		return 0;
	}

	OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> SelectLoad(load_state_t state)
	{
		OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize> load;

		for(load = loadQueue.SeekAtHead(); load; load++)
		{
			if(load->state == state)
				return load;
		}
		return 0;
	}

	void SelectLoad(OoOQueuePointer<LoadQueueEntry<T, nSources>, LoadQueueSize>& load, load_state_t state)
	{
		for(load++; load; load++)
		{
			if(load->state == state) break;
		}
	}

	QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> SelectStore(store_state_t state)
	{
		QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;

		for(store = storeQueue.SeekAtHead(); store; store++)
		{
			if(store->state == state)
				return store;
		}
		return 0;
	}

	T SignExtension(T value, int bitsize)
	{
		return (int64_t)(value << (8 * sizeof(T) - bitsize)) >> (8 * sizeof(T) - bitsize);
	}
	
  //	bool LoadStoreConflict(const InstructionPtr<T, nSources>& load, const InstructionPtr<T, nSources>& store)
	bool LoadStoreConflict(const InstructionPtr load, const InstructionPtr store)
	{
		if(load > store)
		{
			if((store->ea >= load->ea && store->ea < load->ea + load->operation->memory_access_size()) ||
					(load->ea >= store->ea && load->ea < store->ea + store->operation->memory_access_size()))
			{
				return true;
			}
		}
		return false;
	}

	bool LoadStoreConflict(const InstructionPtr load)
	{
		QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;

		for(store = storeQueue.SeekAtHead(); store; store++)
		{
		  //			if(load > store->instruction)
			if(load > store->instruction)
			{
/*				if(store->state == allocated_store)
				{
					return true;
				}*/
			        if(store->state == allocated_store) continue;
				if((store->instruction->ea >= load->ea && store->instruction->ea < load->ea + load->operation->memory_access_size()) ||
			           (load->ea >= store->instruction->ea && load->ea < store->instruction->ea + store->instruction->operation->memory_access_size()))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool LoadStoreMayConflict(const InstructionPtr load)
	{
		QueuePointer<StoreQueueEntry<T, nSources>, StoreQueueSize> store;

		for(store = storeQueue.SeekAtHead(); store; store++)
		{
		  //			if(load > store->instruction)
			if(load > store->instruction)
			{
				if(store->state == allocated_store)
				{
					return true;
				}
			}
		}
		return false;
	}	

	friend ostream& operator << (ostream& os, const LoadStoreQueue& lsq)
	{
		os << "============ LOADSTOREQUEUE ===========" << endl;
		os << "load queue:" << endl;
		os << lsq.loadQueue;
		os << "store queue:" << endl;
		os << lsq.storeQueue;
		os << "=======================================" << endl;
		return os;
	}

	bool Check()
	{
		return loadQueue.Check() && storeQueue.Check();
	}

	/** Prints statistics into an output stream
		@param os an output stream
	*/
	void DumpStats(ostream& os, uint64_t sim_total_time, uint64_t sim_cycle)
	{
		os << "load_queue_cumulative_occupancy " << load_queue_cumulative_occupancy << " # load queue cumulative occupancy" << endl;
		os << "store_queue_cumulative_occupancy " << store_queue_cumulative_occupancy << " # store queue cumulative occupancy" << endl;
		os << "load_queue_occupancy " << ((double) load_queue_cumulative_occupancy / (double) sim_cycle) << " # " << " load queue occupancy (instructions/cycles)" << endl;
		os << "store_queue_occupancy " << ((double) store_queue_cumulative_occupancy / (double) sim_cycle) << " # " << " store queue occupancy (instructions/cycles)" << endl;
		os << "total_spec_loads " << total_spec_loads << " # " << name() << " total number of speculative loads" << endl;
	}
	
	void WarmRestart()
	{
		loadQueue.Reset();
		storeQueue.Reset();
		//		changed = true;
	}
	
	void ResetStats()
	{
		executed_refs = 0;
		executed_loads = 0;
		load_queue_cumulative_occupancy = 0;
		store_queue_cumulative_occupancy = 0;
		total_spec_loads = 0;
	}

private:
	OoOQueue<LoadQueueEntry<T, nSources> , LoadQueueSize> loadQueue;
	Queue<StoreQueueEntry<T, nSources>, StoreQueueSize> storeQueue;
	endianess_t endianess;
	UInt64 executed_refs;
	UInt64 executed_loads;
	UInt64 load_queue_cumulative_occupancy;
	UInt64 store_queue_cumulative_occupancy;
	UInt64 total_spec_loads;

	/*
	ml_out_data<bool> outStateChanged;
	ml_signal_data<bool> stateChanged;
	ml_in_data<bool> inStateChanged;
	bool changed;
	*/

  bool inDL1_flushed_ports[nDataCachePorts];

        int load_to_remove;
        int store_to_remove;
	int store_to_commit;

};

} // end of namespace execute
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
