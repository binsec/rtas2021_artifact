/* -*- c++ -*- */

/***************************************************************************
                          common.h  -  description
                             -------------------
    begin                : Thu Jan 23 2003
    copyright            : (C) 2003 CEA and Paris South University
    author               : Gilles Mouchard
    email                : mouchard@lri.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_INTERFACES_INSTRUCTION_INTERFACE_HH__
#define __UNISIM_COMPONENT_CLM_INTERFACES_INSTRUCTION_INTERFACE_HH__

//#include "system/cpu/powerpc405/iss_interface.h"
#include <unisim/component/clm/processor/ooosim/iss_interface.hh>
#include <unisim/component/cxx/processor/powerpc/powerpc.hh>

#include <unisim/component/clm/utility/common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace interfaces {

  //  using unisim::component::cxx::processor::powerpc::operand_type_t;
  /*
  using unisim::component::cxx::processor::powerpc::GPR_T;
  using unisim::component::cxx::processor::powerpc::FPR_T;
  using unisim::component::cxx::processor::powerpc::CRF_T;
  using unisim::component::cxx::processor::powerpc::CRB_T;
  using unisim::component::cxx::processor::powerpc::FPSCRF_T;
  using unisim::component::cxx::processor::powerpc::SR_T;
  using unisim::component::cxx::processor::powerpc::BO_T;
  using unisim::component::cxx::processor::powerpc::BD_T;
  using unisim::component::cxx::processor::powerpc::IMM_T;
  using unisim::component::cxx::processor::powerpc::CR_T;
  using unisim::component::cxx::processor::powerpc::FPSCR_T;
  using unisim::component::cxx::processor::powerpc::LR_T;
  using unisim::component::cxx::processor::powerpc::CTR_T;
  using unisim::component::cxx::processor::powerpc::XER_T;
  using unisim::component::cxx::processor::powerpc::NONE_REGISTER;
  */
  //using 
  using unisim::component::clm::utility::hexa;

  using unisim::component::clm::utility::branch_direction_t;
  using unisim::component::clm::utility::Taken;
  using unisim::component::clm::utility::NotTaken;
  using unisim::component::clm::utility::DontKnow;

  using unisim::component::clm::utility::exception_type_t;
  using unisim::component::clm::utility::NONE_EXCEPTION;
  using unisim::component::clm::utility::MISALIGNMENT_EXCEPTION;
  using unisim::component::clm::utility::INVALID_OPCODE_EXCEPTION;

  using unisim::component::clm::memory::address_t;

#undef GCC_INLINE


#if defined(__GNUC__) && defined(EXTENSIVE_INLINING)
#define GCC_INLINE __attribute__((always_inline))
#define INLINE inline
#else
#define GCC_INLINE
#define INLINE inline
#endif


/* Instruction source operand */
//template <class T>
class Source
{
	public:
                // DD We are now using emulator register types ...
                enum operand_type_t type; 
                //		enum reg_type_t type;
		int reg;	/*< architectural register number */
		int tag;	/*< physical register number */
		uint64_t data;		/*< value */

		bool ready;	/*< true if the value is ready to be bypassed or read from register file */
		bool valid;	/*< true if the value has been bypassed or read from register file */

		Source()
		{
		        type = NONE_REGISTER;
			reg = -1;
			tag = -1;
			data = 0;
		}


		friend ostream& operator << (ostream& os, const Source& s)
		{
			if(s.reg >= 0)
			{
				os << "source(";
				switch(s.type)
				{
					case GPR_T:
						os << "gpr";
						break;
					case FPR_T:
						os << "fpr";
						break;
					case CR_T:
						os << "cr";
						break;
					case FPSCR_T:
						os << "fpscr";
						break;
					case LR_T:
						os << "lr";
						break;
					case CTR_T:
						os << "ctr";
						break;
					case XER_T:
						os << "xer";
						break;
					default:
						os << "?";
				}
				os << s.reg;
				if(s.tag >= 0)
				{
				  os <<"::[vb"<<s.valid<<":rb"<<s.ready<<"]::";
					switch(s.type)
					{
						case GPR_T:
							os << "r";
							break;
						case FPR_T:
							os << "f";
							break;
					case CR_T:
						os << "Rcr";
						break;
					case FPSCR_T:
						os << "Rfpscr";
						break;
					case LR_T:
						os << "Rlr";
						break;
					case CTR_T:
						os << "Rctr";
						break;
					case XER_T:
						os << "Rxer";
						break;
						default:
							os << "?";
					}
					os << s.tag;
				}
				//	os << "=" << hexa(s.data);
				os << "=" << hexa(s.data);
				os << ")";
			}
			return os;
		}
};

/* Instruction destination operand */
//template <class T>
class Destination
{
	public:
                // DD We are now using emulator register types ...
                operand_type_t type; 
                //		enum reg_type_t type;
		int reg;	/*< architectural register number */
		int tag;	/*< physical register number */
		uint64_t data;

		Destination()
		{
			type = NONE_REGISTER;
			reg = -1;
			tag = -1;
			data = 0;
		}


		friend ostream& operator << (ostream& os, const Destination& d)
		{
			if(d.reg >= 0)
			{
				os << "dest(";
				switch(d.type)
				{
					case GPR_T:
						os << "gpr";
						break;
					case FPR_T:
						os << "fpr";
						break;
					case CR_T:
						os << "cr";
						break;
					case FPSCR_T:
						os << "fpscr";
						break;
					case LR_T:
						os << "lr";
						break;
					case CTR_T:
						os << "ctr";
						break;
					case XER_T:
						os << "xer";
						break;
					default:
						os << "?";
				}
				os << d.reg;
				if(d.tag >= 0)
				{
					os << ":";
					switch(d.type)
					{
						case GPR_T:
							os << "r";
							break;
						case FPR_T:
							os << "f";
							break;
					case CR_T:
						os << "Rcr";
						break;
					case FPSCR_T:
						os << "Rfpscr";
						break;
					case LR_T:
						os << "Rlr";
						break;
					case CTR_T:
						os << "Rctr";
						break;
					case XER_T:
						os << "Rxer";
						break;
						default:
							os << "?";
					}
					os << d.tag;
				}
				os << "=" << hexa(d.data);
				//				os << "=" << hex << (d.data) << dec;
				os << ")";
			}
			return os;
		}
};

class InstructionPtr;
ostream& operator << (ostream& os, const InstructionPtr& instructionPtr);
//inline InstructionPtr::~InstructionPtr();


class Instruction
{public:
  bool predecoded;
  bool decoded;
  Operation<CPU_CONFIG> *operation;                                 /*< decoded instruction of the emulator */
  int *operation_refcount;
  uint32_t binary;                                      /*< binary image of the instruction */
  address_t cia;                                        /*< current instruction address (i.e. the program counter) */
  address_t nia;                                        /*< next instruction address (kwown at commit) */
  address_t predicted_nia;                              /*< predicted next instruction address (know at fetch) */
  branch_direction_t branch_direction;
  branch_direction_t predicted_branch_direction;
  function_t fn;                                        /*< function of the instruction */
  int64_t  inum;                                         /*< unique number for the instruction (convenient for ordering instructions) */
  function_t function;
  int      tag;                                         /*< reorder buffer tag */
  uint32_t ea;                                          /*< effective address of a load/store instruction */
  uint32_t pred_ea;                                          /*< effective address of a load/store instruction */
  int      ls_reg_count;                                 // load/store counter for FnLoadMultiple/FnStoreMultiple
  int      ls_byte_count;
  int      load_reg;
  int      load_count;
  uint32_t store_reg_val;
  int      state;
  int      latency;
  bool     emul_executed;
  bool     verified;
  bool     mustbechecked;
  InstructionPtr *splitted_instruction;
  //  int      *splitted_instruction_refcount;
  CPU<CPU_CONFIG>     *cpu;                                       ///< the cpu to which the instruction belongs to

  // Extra information for OoO execution
  bool execution_serialized;			      /*< true if instruction is execution serialized */
  bool btb_miss;				     	/*< true if a btb miss occured */
  bool ras_miss;		       			/*< true if a ras miss occured */

  exception_type_t exception;
  bool must_reschedule;          /*< true if instruction must be rescheduled at the end of the read register pipeline stage */
  bool may_need_replay;          /*< true if instruction may need a replay */
  bool replay_trap;              /*< true if instruction need a replay */
  int obq_tag;

  bool valid;    //< Used with unisim signal array when something() can't be used
  uint32_t singleprecision;
  vector<Source> sources;
  //  Destination destination;
  vector<Destination> destinations;

	uint64_t timing_fetch_cycle;
	uint64_t timing_allocate_cycle;
	uint64_t timing_schedule_cycle;
	uint64_t timing_register_cycle;
	uint64_t timing_execute_cycle;
	uint64_t timing_writeback_cycle;
	uint64_t timing_retire_cycle;

  uint64_t alloc_stall_sched_reject;
  uint64_t alloc_stall_reord_reject;
  uint64_t alloc_stall_loadq_reject;
  uint64_t alloc_stall_storq_reject;


  Instruction()
  { 
    predecoded = false;
    decoded = false;
    operation = NULL;
    //    operation_refcount = new int(1);
    operation_refcount = NULL;
    binary = 0;
    cia = 0;
    nia = 0;
    predicted_nia = 0;
    branch_direction = DontKnow;
    predicted_branch_direction = DontKnow;
    fn = FnInvalid;
    inum = -1;
    function = FnInvalid;
    tag = -1;
    ea = 0;
    pred_ea = 0;
    ls_reg_count = 0;
    ls_byte_count = 0;
    load_reg = 0;
    load_count = 0;
    store_reg_val = 0;
    state = 0;
    latency = 0;
    emul_executed = false;
    verified = false;
    mustbechecked = true;
    splitted_instruction = NULL;
    //    splitted_instruction_refcount = NULL;
    cpu = NULL;

    // Extra information for OoO execution
    execution_serialized = false;
    btb_miss = false;
    ras_miss = false;
    exception = NONE_EXCEPTION;
    must_reschedule=false;
    may_need_replay=false;
    replay_trap = false;
    obq_tag = -1;

    valid = false;

    singleprecision = 0;
    sources.clear();
    destinations.clear();

	timing_fetch_cycle=0;
	timing_allocate_cycle=0;
	timing_schedule_cycle=0;
	timing_register_cycle=0;
	timing_execute_cycle=0;
	timing_writeback_cycle=0;
	timing_retire_cycle=0;

	alloc_stall_sched_reject=0;
	alloc_stall_reord_reject=0;
	alloc_stall_loadq_reject=0;
	alloc_stall_storq_reject=0;

 }

  ~Instruction()
  {
    sources.clear();
    destinations.clear();
    //assert(*operation_refcount>0);
    if (operation_refcount != NULL)
      {
#ifdef DD_DEBUG_OPERATIONREFCOUNT
#endif
	if ( *operation_refcount == 1 )
	  {
#ifdef DD_DEBUG_OPERATIONREFCOUNT
	    cerr << "[DD-ORC] Operation delete (RefC="<<*operation_refcount<<"): ";
	    operation->disasm(0,cerr);
	    cerr << endl;
#endif
	    delete operation;
	    delete operation_refcount;
	    operation = NULL;
	    operation_refcount = NULL;
	  }
	else
	  {
#ifdef DD_DEBUG_OPERATIONREFCOUNT
	    cerr << "[DD-ORC] Operation NOT delete (RefC="<<*operation_refcount<<"): ";
	    operation->disasm(0,cerr);
	    cerr << endl;
#endif
	    (*operation_refcount)--;
	  }
      }
    

    ////////
    /*
    if (splitted_instruction_refcount != NULL)
      {
#ifdef DD_DEBUG_SPLITTED_INSTRUCTIONREFCOUNT
#endif
	if ( *splitted_instruction_refcount == 1 )
	  {
#ifdef DD_DEBUG_SPLITTED_INSTRUCTIONREFCOUNT
	    cerr << "[DD-ORC] Splitted_Instruction delete (RefC="<<*splitted_instruction_refcount<<"): ";
	    //splitted_instruction->disasm(cerr);
	    cerr << endl;
#endif
	    delete splitted_instruction;
	    delete splitted_instruction_refcount;
	    splitted_instruction = NULL;
	    splitted_instruction_refcount = NULL;
	  }
	else
	  {
#ifdef DD_DEBUG_SPLITTED_INSTRUCTIONREFCOUNT
	    cerr << "[DD-ORC] Splitted_Instruction NOT delete (RefC="<<*splitted_instruction_refcount<<"): ";
	    //splitted_instruction->disasm(cerr);
	    cerr << endl;
#endif
	    (*splitted_instruction_refcount)--;
	  }
      }
    */
    //if (splitted_instruction != NULL) splitted_instruction->~InstructionPtr(); 
    //if (splitted_instruction != NULL) {delete splitted_instruction;} 
    //delete splitted_instruction;
  }
  
  Instruction(const Instruction& instruction) //GCC_INLINE
  {
    predecoded = instruction.predecoded;
    decoded = instruction.decoded;
    /*
    if (instruction.operation)
      operation = new Operation(instruction.operation->GetEncoding(),instruction.operation->GetAddr(),instruction.operation->GetName());
    else
      operation = NULL;
    */
    operation = instruction.operation;

    operation_refcount = instruction.operation_refcount;
    if (operation_refcount != NULL) (*operation_refcount)++;
    //    (*operation_refcount)++;

    binary = instruction.binary;
    cia = instruction.cia;
    nia = instruction.nia;
    predicted_nia = instruction.predicted_nia;
    branch_direction = instruction.branch_direction;
    predicted_branch_direction = instruction.predicted_branch_direction;
    fn = instruction.fn;
    inum = instruction.inum;
    function = instruction.function;
    tag = instruction.tag;
    ea = instruction.ea;
    pred_ea = instruction.pred_ea;
    ls_reg_count = instruction.ls_reg_count;
    ls_byte_count = instruction.ls_byte_count;
    load_reg = instruction.load_reg;
    load_count = instruction.load_count;
    store_reg_val = instruction.store_reg_val;
    state = instruction.state;
    latency = instruction.latency;
    emul_executed = instruction.emul_executed;
    verified = instruction.verified;
    
    mustbechecked = instruction.mustbechecked;
    splitted_instruction = instruction.splitted_instruction;
    //    if (splitted_instruction !=NULL) *splitted_instruction = *(instruction.splitted_instruction);
    //    splitted_instruction_refcount = instruction.splitted_instruction_refcount;
    //    if (splitted_instruction_refcount != NULL) (*splitted_instruction_refcount)++;

   cpu = instruction.cpu;

    // Extra information for OoO execution
    execution_serialized = instruction.execution_serialized;
    btb_miss = instruction.btb_miss;
    ras_miss = instruction.ras_miss;
    exception = instruction.exception;
    must_reschedule=instruction.must_reschedule;
    may_need_replay=instruction.may_need_replay;
    replay_trap = instruction.replay_trap;
    obq_tag = instruction.obq_tag;
    
    valid = instruction.valid;
    singleprecision = instruction.singleprecision;
    sources = instruction.sources;
    //    destination = instruction.destination;
    destinations = instruction.destinations;

    timing_fetch_cycle = instruction.timing_fetch_cycle;
    timing_allocate_cycle = instruction.timing_allocate_cycle;
    timing_schedule_cycle = instruction.timing_schedule_cycle;
    timing_register_cycle = instruction.timing_register_cycle;
    timing_execute_cycle = instruction.timing_execute_cycle;
    timing_writeback_cycle = instruction.timing_writeback_cycle;
    timing_retire_cycle = instruction.timing_retire_cycle;
    
    alloc_stall_sched_reject = instruction.alloc_stall_sched_reject;
    alloc_stall_reord_reject = instruction.alloc_stall_reord_reject;
    alloc_stall_loadq_reject = instruction.alloc_stall_loadq_reject;
    alloc_stall_storq_reject = instruction.alloc_stall_storq_reject;
 }

  Instruction &operator = (const Instruction& instruction) GCC_INLINE
  {
    predecoded = instruction.predecoded;
    decoded = instruction.decoded;
    /*
    if (instruction.operation)
      operation = new Operation(instruction.operation->GetEncoding(),instruction.operation->GetAddr(),instruction.operation->GetName());
    else
      operation = NULL;
    */
    if (operation_refcount != NULL)
      {
	(*operation_refcount)--;
	if (*operation_refcount == 0)
	  {
	    delete operation;
	    delete operation_refcount;
	  }
      }

    operation = instruction.operation;
    operation_refcount = instruction.operation_refcount;
    if (operation_refcount != NULL) (*operation_refcount)++;

    binary = instruction.binary;
    cia = instruction.cia;
    nia = instruction.nia;
    predicted_nia = instruction.predicted_nia;
    branch_direction = instruction.branch_direction;
    predicted_branch_direction = instruction.predicted_branch_direction;
    fn = instruction.fn;
    inum = instruction.inum;
    function = instruction.function;
    tag = instruction.tag;
    ea = instruction.ea;
    pred_ea = instruction.pred_ea;
    ls_reg_count = instruction.ls_reg_count;
    ls_byte_count = instruction.ls_byte_count;
    load_reg = instruction.load_reg;
    load_count = instruction.load_count;
    store_reg_val = instruction.store_reg_val;
    state = instruction.state;
    latency = instruction.latency;
    emul_executed = instruction.emul_executed;
    verified = instruction.verified;
    
    mustbechecked = instruction.mustbechecked;


    // 
    /*
    if (splitted_instruction_refcount != NULL)
      {
	(*splitted_instruction_refcount)--;
	if (*splitted_instruction_refcount == 0)
	  {
	    delete splitted_instruction;
	    delete splitted_instruction_refcount;
	  }
      }
    */
    splitted_instruction = instruction.splitted_instruction; 
    //    if (splitted_instruction !=NULL) *splitted_instruction = *(instruction.splitted_instruction);
    //    splitted_instruction_refcount = instruction.splitted_instruction_refcount;
    //    if (splitted_instruction_refcount != NULL) (*splitted_instruction_refcount)++;


    cpu = instruction.cpu;

    // Extra information for OoO execution
    execution_serialized = instruction.execution_serialized;
    btb_miss = instruction.btb_miss;
    ras_miss = instruction.ras_miss;
    exception = instruction.exception;
    must_reschedule=instruction.must_reschedule;
    may_need_replay=instruction.may_need_replay;
    replay_trap = instruction.replay_trap;
    obq_tag = instruction.obq_tag;

    valid = instruction.valid;
    singleprecision = instruction.singleprecision;
    
    sources = instruction.sources;
    //    destination = instruction.destination;
    destinations = instruction.destinations;

    timing_fetch_cycle = instruction.timing_fetch_cycle;
    timing_allocate_cycle = instruction.timing_allocate_cycle;
    timing_schedule_cycle = instruction.timing_schedule_cycle;
    timing_register_cycle = instruction.timing_register_cycle;
    timing_execute_cycle = instruction.timing_execute_cycle;
    timing_writeback_cycle = instruction.timing_writeback_cycle;
    timing_retire_cycle = instruction.timing_retire_cycle;
    
    alloc_stall_sched_reject = instruction.alloc_stall_sched_reject;
    alloc_stall_reord_reject = instruction.alloc_stall_reord_reject;
    alloc_stall_loadq_reject = instruction.alloc_stall_loadq_reject;
    alloc_stall_storq_reject = instruction.alloc_stall_storq_reject;

  }



  void Validate(const Instruction *writeBackInstruction)
  {
    int i;
    for (int j=0; j < writeBackInstruction->destinations.size(); j++)
      {
	int tag = writeBackInstruction->destinations[j].tag;
	if(tag >= 0)
	  {
	    //	    for(i = 0; i < nSources; i++)
	    for(i = 0; i < sources.size(); i++)
	      {
		if(sources[i].tag == tag && sources[i].type == writeBackInstruction->destinations[j].type)
		  {
		    sources[i].ready = true;
		  }
	      }
	  }
      }
  }

  int operator == (const Instruction& instruction) const GCC_INLINE;

  friend int operator < (const Instruction& a, const Instruction& b)
  { return a.inum < b.inum;
  }

  friend int operator <= (const Instruction& a, const Instruction& b)
  { return a.inum <= b.inum;
  }

  friend int operator > (const Instruction& a, const Instruction& b)
  { return a.inum > b.inum;
  }

  friend int operator >= (const Instruction& a, const Instruction& b)
  { return a.inum >= b.inum;
  }

  friend ostream& operator << (ostream& os, const Instruction& instruction)
  { 
    os << "inum=" << instruction.inum;
    os << ",tag=" << instruction.tag;
    os << ",cia=" << hexa(instruction.cia);
    os << ",disasm= [ ";

    if (instruction.operation != NULL)
      //      (instruction.operation)->disasm(instruction.cpu,os);
      (instruction.operation)->disasm(0,os);
    else
      os << "NULL";

    if (instruction.operation_refcount != NULL)
      os << " ,OpRefCount=" << *(instruction.operation_refcount);
    //    else
    //      os << " ,OpRefCount=NULL";

    os << " ],bin=" << hexa(instruction.binary);
    os << ",nia=" << hexa(instruction.nia);
    os << ",pred_nia=" << hexa(instruction.predicted_nia);
    os << ",direction=";
    if (instruction.branch_direction == Taken) os << "Taken ";
    if (instruction.branch_direction == NotTaken) os << "NotTaken ";
    if (instruction.branch_direction == DontKnow) os << "DontKnow ";
    os << ",func=" << hexa(instruction.fn);
    os << ",ea=" << hexa(instruction.ea);
    os << ",pred_ea=" << hexa(instruction.pred_ea);
    os << ",mbr=" << (instruction.must_reschedule?"true":"false");
    os << ",may-need-replay=" << (instruction.may_need_replay?"true":"false");
    os << ",replay_trap=" << (instruction.replay_trap?"true":"false");
    os << ",mbc=" << (instruction.mustbechecked?"true":"false");
    
    os << ",single =" << hexa(instruction.singleprecision);
    os << ",except =" << instruction.exception;
    os << ",sources:";
    for (int i=0; i < instruction.sources.size(); i++)
      { os << instruction.sources[i]; }
    os << ",dest:";
    for (int i=0; i < instruction.destinations.size(); i++)
      { os << instruction.destinations[i]; }
    //    os << ",dest.reg=" << instruction.destination.reg;
    //    os << ",dest.tag=" << instruction.destination.tag;
    //    os << ",dest.data=" << hexa(instruction.destination.data);
    if (instruction.splitted_instruction)
      { os << " --- S.I.: "<< *(instruction.splitted_instruction); }
    return os;
  }

};


//class Instruction;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///class InstructionPtr;
//inline InstructionPtr::~InstructionPtr();

///ostream& operator << (ostream& os, const InstructionPtr& instructionPtr);


template <class T>
struct PoolNode
{
	T data;
	int refcount;
	PoolNode<T> *next;
};

template <class T>
class Pool
{
public:
  	static Pool<T> pool;

	Pool()
	{
		freelist = 0;
		//		nodecount = 0;
		//		freelistsize = 0;
#ifdef DD_DEBUG_USEREF
		nodeCount=0;
#endif
	}

	~Pool()
	{
		PoolNode<T> *next;
		if(freelist)
		{
			do
			{
				next = freelist->next;
				delete freelist;
			} while((freelist = next));
		}
	}

        PoolNode<T> *New() GCC_INLINE;

        void Delete(PoolNode<T> *node) GCC_INLINE;
private:
	PoolNode<T> *freelist;
  //  int freelistsize;
  //  int nodecount;
#ifdef DD_DEBUG_USEREF
  static const int MAXLIVENODE = 5000;
  int nodeCount;
#endif
};

//Pool<Instruction> Pool<Instruction>::pool;

template <class T>
inline PoolNode<T> *Pool<T>::New()
{
	if(!freelist)
	{
		freelist = new PoolNode<T>();
		freelist->next = 0;
		/*
		nodecount++;
		freelistsize++;
		if (nodecount > MAXNODE)
		  {
		    cerr << "WARNING : Maximum node overlap !!!" << endl;
		    cerr << "freelistsize = " << freelistsize << endl;
		    abort();
		  }
		*/
#ifdef DD_DEBUG_USEREF
		nodeCount++;
		if (nodeCount > MAXLIVENODE)
		{
		  cerr << "Warning: maximum number of node overlap !" << endl;
		  abort();
		}
#endif
		
	}
	PoolNode<T> *node = freelist;
	freelist = freelist->next;
	//	freelistsize--;
	return node;
}

template <class T>
inline void Pool<T>::Delete(PoolNode<T> *node)
{
  if (node->data.operation_refcount != NULL)
    {
      // Splitted instruction have a refcount not null
      // We delete them to ensure no memory leak on there operation.
      delete node;
    }
  else
    {
      // For others instructions we use the freelist.
	node->next = freelist;
	freelist = node;
	//	freelistsize++;
    }
}

//Pool initialization !!!
template <class T1> Pool<T1> Pool<T1>::pool;
//Pool<Instruction> Pool<Instruction>::pool;


//template <class T, int nSources>
class InstructionPtr
{
public:
#ifdef USE_REF
	InstructionPtr() GCC_INLINE;

	InstructionPtr(const InstructionPtr& instructionPtr) GCC_INLINE;

	InstructionPtr& operator = (const InstructionPtr& instructionPtr) GCC_INLINE;

	~InstructionPtr() GCC_INLINE;
#endif

	operator const Instruction * () const GCC_INLINE;

	const Instruction * operator -> () const GCC_INLINE;

	operator Instruction * () GCC_INLINE;

	Instruction * operator -> () GCC_INLINE;

	int operator == (const InstructionPtr& instructionPtr) const GCC_INLINE;

	friend int operator < (const InstructionPtr& a, const InstructionPtr& b)
	{
#ifdef USE_REF
		return a.node->data < b.node->data;
#else
		return a.instr < b.instr;
#endif
	}

	friend int operator > (const InstructionPtr& a, const InstructionPtr& b)
	{
#ifdef USE_REF
		return a.node->data > b.node->data;
#else
		return a.instr > b.instr;
#endif
	}

	friend ostream& operator << (ostream& os, const InstructionPtr& instructionPtr)
	{
#ifdef USE_REF
#ifdef DD_DEBUG_USEREF
	  os << "@:" << instructionPtr.node << " RefCount:" << instructionPtr.node->refcount << " | ";
#endif
		os << instructionPtr.node->data;
#else
		os << instructionPtr.instr;
#endif
		return os;
	}

private:
#ifdef USE_REF
	PoolNode<Instruction > *node;
#else
	Instruction instr;
#endif

};

#ifdef USE_REF
//template <class T, int nSources>
inline InstructionPtr::InstructionPtr()
{
	node = Pool<Instruction >::pool.New();
	node->refcount = 1;
}

//template <class T, int nSources>
inline InstructionPtr::InstructionPtr(const InstructionPtr& instructionPtr)
{
	node = instructionPtr.node;
	node->refcount++;
}

//template <class T, int nSources>
inline InstructionPtr& InstructionPtr::operator = (const InstructionPtr& instructionPtr)
{
	if(--node->refcount == 0)
	{
		Pool<Instruction >::pool.Delete(node);
	}
	node = instructionPtr.node;
	node->refcount++;
	return *this;
}

//template <class T, int nSources>
inline InstructionPtr::~InstructionPtr()
{
#ifdef DD_DEBUG_USEREF
#endif
#if 0
#ifdef USE_REF
  if ((node->data).splitted_instruction != NULL)
    {
      node->data.splitted_instruction->~InstructionPtr();
      //      delete node->data.splitted_instruction;
      node->data.splitted_instruction = NULL;
    }
#else

  if (instr.splitted_instruction != NULL)
    {
      // It is not sure...
      //       delete instr.splitted_instruction;
      //       instr.splitted_instruction = NULL;
    }
#endif
#endif
	if(--node->refcount == 0)
	{
		Pool<Instruction >::pool.Delete(node);
	}
	node = 0;
}
#endif

//template <class T, int nSources>
inline InstructionPtr::operator const Instruction * () const
{
#ifdef USE_REF
	return &node->data;
#else
	return &instr;
#endif
}

//template <class T, int nSources>
inline const Instruction * InstructionPtr::operator -> () const
{
#ifdef USE_REF
	return &node->data;
#else
	return &instr;
#endif
}

//template <class T, int nSources>
inline InstructionPtr::operator Instruction * ()
{
#ifdef USE_REF
	return &node->data;
#else
	return &instr;
#endif
}

//template <class T, int nSources>
inline Instruction * InstructionPtr::operator -> ()
{
#ifdef USE_REF
	return &node->data;
#else
	return &instr;
#endif
}

//template <class T, int nSources>
inline int InstructionPtr::operator == (const InstructionPtr& instructionPtr) const
{
#ifdef USE_REF
	return node->data == instructionPtr.node->data;
#else
	return instr == instructionPtr.instr;
#endif
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
template class<int N>
class CollapseIPtr
{
public:
  InstructionPtr instr[N]; 
  InstructionPtr &operator [](int i) { return instr[i]; }
};
*/

#undef GCC_INLINE

} // end of namespace interfaces
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim


#endif
