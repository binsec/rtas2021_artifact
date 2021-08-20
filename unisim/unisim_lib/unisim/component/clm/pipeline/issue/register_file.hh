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
                          RegisterFile.hh  -  description
 ***************************************************************************/

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_REGISTER_FILE_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_REGISTER_FILE_HH__

#include <unisim/component/clm/utility/common.hh>

namespace unisim {
namespace component {
namespace clm {
namespace pipeline {
namespace issue {

using unisim::component::clm::utility::hexa;

/** A register file base class */
template <class T, int nSources, int nRegisters>
class RegisterFileBase
{
	public:

		/** The constructor */
		RegisterFileBase()
		{
			int i;
			/* Reset all registers */
			for(i = 0; i < nRegisters; i++)
			{
				registers[i] = 0;
			}
		}

		/** Reads a register
			@param tag the register number
			@return the value of the register
		*/
		T Read(int tag)
		{
			return registers[tag];
		}

		/** Writes a register
			@param tag the register number
			@param data the value to write
		*/
		void Write(int tag, T data)
		{
			registers[tag] = data;
		}

		/** Print the register file content into an output stream
			@param os an output stream
			@param rf a register file base
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const RegisterFileBase<T, nSources, nRegisters>& rf)
		{
			int i;
			const int nRegistersPerLine = 8;
			for(i = 0; i < nRegisters; i++)
			{
			  os << "r" << i << "=" << hexa(rf.registers[i]);
			  //os << "r" << i << "=" << hex << rf.registers[i] << dec;
			  if((i % nRegistersPerLine) == nRegistersPerLine - 1) os << endl; else os << "  ";
			}
			return os;
		}

	private:
		/* Register values */
		T registers[nRegisters];
};

/** A Write back pipeline entry */
template <class T, int nSources>
class WriteBackPipelineEntry
{
public:
	int delay;		/*< processing delay of the write back */
	InstructionPtr instruction;	/*< the instruction being written back */


  WriteBackPipelineEntry(): delay(0) {}
	/** Print a write back pipeline entry into an output stream
		@param os an output stream
		@param entry a write back pipeline entry
		@return the output stream
	*/
	friend ostream& operator << (ostream& os, const WriteBackPipelineEntry& entry)
	{
		os << "delay=" << entry.delay << ",instruction=" << entry.instruction;
		return os;
	}

	/** Compares two write back pipeline entry
		@param entry the rsh
		@return non-zero if different
	*/
	int operator == (const WriteBackPipelineEntry& entry)
	{
		return entry.instruction->inum == instruction->inum;
	}
};

/** A read register pipeline entry */
template <class T, int nSources>
class ReadRegisterPipelineEntry
{
public:
	int delay;			/*< processing delay for reading */
	InstructionPtr instruction;	/*< instruction reading the register file */
	bool ready;

	/** Prints a read register pipeline entry into an output stream
		@param os an output stream
		@param entry a read register pipeline entry
		@return the output stream
	*/
	friend ostream& operator << (ostream& os, const ReadRegisterPipelineEntry& entry)
	{
		os << "delay=" << entry.delay << ",instruction=" << entry.instruction;
		return os;
	}

	/** Compares two read register pipeline entry
		@param entry the rhs
		@return non-zero if different
	*/
	int operator == (const ReadRegisterPipelineEntry& entry)
	{
		return entry.instruction->inum == instruction->inum;
	}

	void Initialize(const InstructionPtr instruction)
	{
		this->instruction = instruction;
		(this->instruction)->must_reschedule = false;
		ready = false;
	}

	bool Ready()
	{
		if(ready) return true;
		int i;
		/* For each source operands, we check if it is available */
		/* If not, instruction can't be launched */
		//		for(i = 0; i < nSources; i++)
		for(i = 0; i < instruction->sources.size(); i++)
		{
		  if(instruction->sources[i].tag >= 0 && !instruction->sources[i].valid) return false;
		  /*
			if(instruction.sources[i].tag >= 0 && 
			   ( !instruction.sources[i].valid &&
			     !instruction.sources[i].ready )
			     ) return false;
		  */
		}
		ready = true;
		return true;
	}
};

/** A SystemC module implementing a register file, i.e., the integer pipe, the floating point pipe, and the load/store pipe, and one pipe for writing, each pipe having several ports */
template <class T, int nSources, int nIntegerRegisters, int nFloatingPointRegisters, int nReadRegisterStages, int IntegerReadRegisterWidth, int FloatingPointReadRegisterWidth, int LoadStoreReadRegisterWidth, int nWriteBackStages, int WriteBackWidth>
class RegisterFile : public module
{
	public:
		inclock inClock;

		/* From/to the common data bus */
		inport<InstructionPtr > inWriteBackInstruction[WriteBackWidth];

		/* From/to the integer issue queue */
		inport<InstructionPtr > inIntegerInstruction[IntegerReadRegisterWidth];

		/* From/to the floating point issue queue */
		inport<InstructionPtr > inFloatingPointInstruction[FloatingPointReadRegisterWidth];

		/* From/to the load/store issue queue */
		inport<InstructionPtr > inLoadStoreInstruction[LoadStoreReadRegisterWidth];

		/* From/to the integer instruction broadcaster */
		outport<InstructionPtr > outIntegerInstruction[IntegerReadRegisterWidth];

		/* From/to the floating point instruction broadcaster */
		outport<InstructionPtr > outFloatingPointInstruction[FloatingPointReadRegisterWidth];

		/* From/to the load/store instruction broadcaster */
		outport<InstructionPtr > outLoadStoreInstruction[LoadStoreReadRegisterWidth];

		/* To the allocator renamer and the reorder buffer */
		outport<InstructionPtr > outWriteBackReceived[WriteBackWidth];

		/* From the reorder buffer */
		//		ml_in_flush inFlush;
		inport<bool> inFlush;
		outport<bool> outFlush;

		/** The register file constructor
			@param name the module name
		*/
		RegisterFile(const char *name) : module(name)
		{
			int i, j;

			class_name = " RegisterFileClass";
			// Unisim port names ...
			for (i=0; i<WriteBackWidth; i++)
			  {
			    inWriteBackInstruction[i].set_unisim_name(this,"inWriteBackInstruction",i);
			  }		
			for (i=0; i<IntegerReadRegisterWidth; i++)
			  {
			    inIntegerInstruction[i].set_unisim_name(this,"inIntegerInstruction",i);
			  }		
			for (i=0; i<FloatingPointReadRegisterWidth; i++)
			  {
			    inFloatingPointInstruction[i].set_unisim_name(this,"inFloatingPointInstruction",i);
			  }		
			for (i=0; i<LoadStoreReadRegisterWidth; i++)
			  {
			    inLoadStoreInstruction[i].set_unisim_name(this,"inLoadStoreInstruction",i);
			  }		
			for (i=0; i<IntegerReadRegisterWidth; i++)
			  {
			    outIntegerInstruction[i].set_unisim_name(this,"outIntegerInstruction",i);
			  }		
			for (i=0; i<FloatingPointReadRegisterWidth; i++)
			  {
			    outFloatingPointInstruction[i].set_unisim_name(this,"outFloatingPointInstruction",i);
			  }		
			for (i=0; i<LoadStoreReadRegisterWidth; i++)
			  {
			    outLoadStoreInstruction[i].set_unisim_name(this,"outLoadStoreInstruction",i);
			  }		
			for (i=0; i<WriteBackWidth; i++)
			  {
			    outWriteBackReceived[i].set_unisim_name(this,"outWriteBackReceived",i);
			  }		
			inFlush.set_unisim_name(this,"inFlush");
			outFlush.set_unisim_name(this,"outFlush");
			/*
			outStateChangedIntegerPipe(stateChangedIntegerPipe);
			inStateChangedIntegerPipe(stateChangedIntegerPipe);
			outStateChangedFloatingPointPipe(stateChangedFloatingPointPipe);
			inStateChangedFloatingPointPipe(stateChangedFloatingPointPipe);
			outStateChangedLoadStorePipe(stateChangedLoadStorePipe);
			inStateChangedLoadStorePipe(stateChangedLoadStorePipe);
			*/

			//			SC_HAS_PROCESS(RegisterFile);

			sensitive_pos_method(start_of_cycle) << inClock;
			sensitive_neg_method(end_of_cycle) << inClock;


			// on WB DATA
			for(i = 0; i < WriteBackWidth; i++)
			        { sensitive_method(onWBData) << inWriteBackInstruction[i].data; }
			// on WB ENABLE
			for(i = 0; i < WriteBackWidth; i++)
			        { sensitive_method(onWBEnable) << inWriteBackInstruction[i].enable; }
			
			//			SC_METHOD(ExternalControlIntegerPipe);
			//			sensitive << inStateChangedIntegerPipe;
			// on DATA
			/*
			for(i = 0; i < IntegerReadRegisterWidth; i++)
				{ sensitive_method(onIntegerData) << inIntegerInstruction[i].data; }
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
				{ sensitive_method(onFloatingData) << inFloatingPointInstruction[i].data; }
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
				{ sensitive_method(onLoadStoreData) << inLoadStoreInstruction[i].data; }
			*/
			// on ACCEPT
			/*
			for(i = 0; i < IntegerReadRegisterWidth; i++)
			  { 
			    sensitive_method(onIntegerAccept) << outIntegerInstruction[i].accept;
			    sensitive_method(onIntegerAccept) << inIntegerInstruction[i].data; 
			  }
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			  {
			    sensitive_method(onFloatingAccept) << outFloatingPointInstruction[i].accept;
			    sensitive_method(onFloatingAccept) << inFloatingPointInstruction[i].data;
			  }
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			  { 
			    sensitive_method(onLoadStoreAccept) << outLoadStoreInstruction[i].accept;
			    sensitive_method(onLoadStoreAccept) << inLoadStoreInstruction[i].data;
			  }
			*/
			// on inDATA and outACCEPT
			for(i = 0; i < IntegerReadRegisterWidth; i++)
			  {
			    sensitive_method(onInteger) << inIntegerInstruction[i].data;
			    sensitive_method(onInteger) << outIntegerInstruction[i].accept;
			  }
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			  {
			    sensitive_method(onFloatingPoint) << inFloatingPointInstruction[i].data;
			    sensitive_method(onFloatingPoint) << outFloatingPointInstruction[i].accept;
			  }
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			  {
			    sensitive_method(onLoadStore) << inLoadStoreInstruction[i].data;
			    sensitive_method(onLoadStore) << outLoadStoreInstruction[i].accept;
			  }


			for(i = 0; i < WriteBackWidth; i++)
			  { sensitive_method(onWriteBackReceivedAccept) << outWriteBackReceived[i].accept; }

			sensitive_method(onFlushData) << inFlush.data;
			sensitive_method(onFlushAccept) << outFlush.accept;
			sensitive_method(onFlushEnable) << inFlush.enable;



			/*
			for(i = 0; i < IntegerReadRegisterWidth; i++)
				sensitive_method(onIntegerAccept) << outIntegerInstruction[i].accept;
			for(i = 0; i < IntegerReadRegisterWidth; i++)
				sensitive_method(onFloatingAccept) << outIntegerInstruction[i].accept;
			for(i = 0; i < IntegerReadRegisterWidth; i++)
				sensitive_method(onLoadStoreAccept) << outIntegerInstruction[i].accept;
			*/
			//			for(i = 0; i < WriteBackWidth; i++)
			//				sensitive << inWriteBackEnable[i];
			// on ENABLE ... not needed
			/*
			SC_METHOD(ExternalControlFloatingPointPipe);
			sensitive << inStateChangedFloatingPointPipe;
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
				sensitive << inFloatingPointValid[i] << inFloatingPointAccept[i];
			for(i = 0; i < WriteBackWidth; i++)
				sensitive << inWriteBackEnable[i];

			SC_METHOD(ExternalControlLoadStorePipe);
			sensitive << inStateChangedLoadStorePipe;
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
				sensitive << inLoadStoreValid[i] << inLoadStoreAccept[i];
			for(i = 0; i < WriteBackWidth; i++)
				sensitive << inWriteBackEnable[i];

			SC_METHOD(ExternalControl);
			for(i = 0; i < WriteBackWidth; i++)
				sensitive << inWriteBackValid[i];
			*/
			//			SC_METHOD(InternalControl);
			//			sensitive_pos << inClock;

			memset(integerValid, true, sizeof(integerValid));
			memset(floatingPointValid, true, sizeof(floatingPointValid));
			memset(conditionValid, true, sizeof(conditionValid));
			memset(FPSCRValid, true, sizeof(FPSCRValid));
			memset(linkValid, true, sizeof(linkValid));
			memset(countValid, true, sizeof(countValid));
			memset(XERValid, true, sizeof(XERValid));
			/*
			integerPipeChanged = true;
			floatingPointPipeChanged = true;
			loadStorePipeChanged = true;
			//			changed = true;
			*/
			WriteBacksReceived = false;
			integer_read_register_to_add = 0;
			floating_point_read_register_to_add = 0;
			load_store_read_register_to_add = 0;

		    integer_data_received = false;
		    floating_point_data_received = false;
		    load_store_data_received = false;
  

		}

	void onFlushData()
	  {
	    //    inFlush.accept = inFlush.data.something();
	    if ( inFlush.data.something() )
	      {
		outFlush.data = inFlush.data;
	      }
	    else
	      {
		outFlush.data.nothing();
	      }
	  }
	void onFlushAccept()
	  {
	    inFlush.accept = outFlush.accept;
	  }
	void onFlushEnable()
	  {
	    outFlush.enable = inFlush.enable;
	  }




  void onWBData()
  {
    bool areallknown(true);
    int i;
    for(i = 0; i < WriteBackWidth; i++)
      {
	if (!inWriteBackInstruction[i].data.known())
	  {
	    areallknown =false; break;
	  }
      }
    if (areallknown)
      {
	for(i = 0; i < WriteBackWidth; i++)
	  {
	    inWriteBackInstruction[i].accept = inWriteBackInstruction[i].data.something();
	  }
      }
  }

  void onWBEnable()
		{
		    bool areallknown(true);
		    int i;
		    for(i = 0; i < WriteBackWidth; i++)
		      {
			if (!inWriteBackInstruction[i].enable.known())
			  {
			    areallknown =false; break;
			  }
		      }
		    if (areallknown)
		      {
			int integerReadRegisterPort;
			int floatingPointReadRegisterPort;
			int loadStoreReadRegisterPort;

			WriteBacksReceived = true;
			//////////////////////////////////////
			// We have to send all data ... 
			//////////////////////////////////////
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> integerReadRegisterPipelineEntry;

			/* For each instructions which have reach the end of the read pipeline */
			for(integerReadRegisterPort = 0, integerReadRegisterPipelineEntry = integerReadRegisterPipeline.SeekAtHead(); integerReadRegisterPort < IntegerReadRegisterWidth && integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++, integerReadRegisterPort++)
			{
				if(integerReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = integerReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(integerReadRegisterPipelineEntry, &instruction);
				/* request for a launch of the instruction */
				//				outIntegerValid[integerReadRegisterPort] = true;
				outIntegerInstruction[integerReadRegisterPort].data = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
			  //	outIntegerValid[integerReadRegisterPort] = false;
				outIntegerInstruction[integerReadRegisterPort].data.nothing();
			}
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, FloatingPointReadRegisterWidth * nReadRegisterStages> floatingPointReadRegisterPipelineEntry;

			/* For each instructions which have reach the end of the read pipeline */
			for(floatingPointReadRegisterPort = 0, floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline.SeekAtHead(); floatingPointReadRegisterPort < FloatingPointReadRegisterWidth && floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++, floatingPointReadRegisterPort++)
			{
				if(floatingPointReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = floatingPointReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(floatingPointReadRegisterPipelineEntry, &instruction);
				
				/* request for a launch of the instruction */
				//				outFloatingPointValid[floatingPointReadRegisterPort] = true;
				outFloatingPointInstruction[floatingPointReadRegisterPort].data = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
			  //				outFloatingPointValid[floatingPointReadRegisterPort] = false;
				outFloatingPointInstruction[floatingPointReadRegisterPort].data.nothing();
			}

			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, LoadStoreReadRegisterWidth * nReadRegisterStages> loadStoreReadRegisterPipelineEntry;
			/* For each instructions which have reach the end of the read pipeline */
			for(loadStoreReadRegisterPort = 0, loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline.SeekAtHead(); loadStoreReadRegisterPort < LoadStoreReadRegisterWidth && loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++, loadStoreReadRegisterPort++)
			{
				if(loadStoreReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = loadStoreReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(loadStoreReadRegisterPipelineEntry, &instruction);
				/* request for a launch of the instruction */
				//				outLoadStoreValid[loadStoreReadRegisterPort] = true;
				outLoadStoreInstruction[loadStoreReadRegisterPort].data = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
			  //				outLoadStoreValid[loadStoreReadRegisterPort] = false;
				outLoadStoreInstruction[loadStoreReadRegisterPort].data.nothing();
			}

		      }// end of if(areallknown)...
		  
		}

		/** A SystemC process handling valid, accept and enable hand-shaking for integer pipe */
		//		void ExternalControlIntegerPipe()
		void onIntegerData()
		{
		  bool areallknown(true);
		  for(int i = 0; i < IntegerReadRegisterWidth; i++)
		    {
		      if (!inIntegerInstruction[i].data.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			int integerReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				if(!inIntegerInstruction[integerReadRegisterPort].data.something()) break;
				integer_read_register_to_add++;
			}
			integer_data_received = true;
			//		    floating_point_data_received = false;
			//		    load_store_data_received = false;
  		    }
		}

		void onIntegerAccept()
		{
		  bool areallknown(true);
		  for(int i = 0; i < IntegerReadRegisterWidth; i++)
		    {
		      if (!outIntegerInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown && integer_data_received)
		    {
			int integerReadRegisterPort;
			//			int integer_read_register_to_add = 0;
			int integer_read_register_to_remove = 0;

			/* Count the number of instruction to add into the read pipeline */
			/* Enable accepted instructions */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				if(!outIntegerInstruction[integerReadRegisterPort].accept) break;
				outIntegerInstruction[integerReadRegisterPort].enable = true;
				/* Count accepted instruction as being removed */
				integer_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				outIntegerInstruction[integerReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int integer_read_register_free_space = integerReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int integer_read_register_to_accept = MIN(integer_read_register_free_space + integer_read_register_to_remove, integer_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < integer_read_register_to_accept; integerReadRegisterPort++)
			{
				inIntegerInstruction[integerReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				inIntegerInstruction[integerReadRegisterPort].accept = false;
			}
		    }
		}

		void onInteger()
		{
		  bool areallknown(true);
		  for(int i = 0; i < IntegerReadRegisterWidth; i++)
		    {
		      if (!inIntegerInstruction[i].data.known() || !outIntegerInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			int integerReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				if(!inIntegerInstruction[integerReadRegisterPort].data.something()) break;
				integer_read_register_to_add++;
			}
			integer_data_received = true;
			//		    floating_point_data_received = false;
			//		    load_store_data_received = false;

			// Accept part
			//			int integerReadRegisterPort;
			//			int integer_read_register_to_add = 0;
			int integer_read_register_to_remove = 0;

			/* Count the number of instruction to add into the read pipeline */
			/* Enable accepted instructions */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				if(!outIntegerInstruction[integerReadRegisterPort].accept) break;
				outIntegerInstruction[integerReadRegisterPort].enable = true;
				/* Count accepted instruction as being removed */
				integer_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				outIntegerInstruction[integerReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int integer_read_register_free_space = integerReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int integer_read_register_to_accept = MIN(integer_read_register_free_space + integer_read_register_to_remove, integer_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < integer_read_register_to_accept; integerReadRegisterPort++)
			{
				inIntegerInstruction[integerReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				inIntegerInstruction[integerReadRegisterPort].accept = false;
			}
			// end Accept part
  		    }
		}


		/** A SystemC process handling valid, accept and enable hand-shaking for floating point pipe */
		//		void ExternalControlFloatingPointPipe()
		void onFloatingData()
		{
		  bool areallknown(true);
		  for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
		    {
		      if (!inFloatingPointInstruction[i].data.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			int floatingPointReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				if(!inFloatingPointInstruction[floatingPointReadRegisterPort].data.something()) break;
				/* Count accepted instruction as being removed */
				floating_point_read_register_to_add++;
			}
			//		    integer_data_received = false;
			floating_point_data_received = true;
			//		    load_store_data_received = false;

		    }
		}

		void onFloatingAccept()
		{
		  bool areallknown(true);
		  for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
		    {
		      if (!outFloatingPointInstruction[i].accept.known())
			{
			  areallknown = false; break;
			}
		    }
		  if (areallknown && floating_point_data_received)
		    {
			int floatingPointReadRegisterPort;
			int floating_point_read_register_to_remove = 0;

			/* Enable accepted instructions */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				if(!outFloatingPointInstruction[floatingPointReadRegisterPort].accept) break;
				outFloatingPointInstruction[floatingPointReadRegisterPort].enable = true;
				floating_point_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				outFloatingPointInstruction[floatingPointReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int floating_point_read_register_free_space = floatingPointReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int floating_point_read_register_to_accept = MIN(floating_point_read_register_free_space + floating_point_read_register_to_remove, floating_point_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < floating_point_read_register_to_accept; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction[floatingPointReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction[floatingPointReadRegisterPort].accept = false;
			}
		    }
		}

		void onFloatingPoint()
		{
		  bool areallknown(true);
		  for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
		    {
		      if (!inFloatingPointInstruction[i].data.known() || !outFloatingPointInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			int floatingPointReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				if(!inFloatingPointInstruction[floatingPointReadRegisterPort].data.something()) break;
				/* Count accepted instruction as being removed */
				floating_point_read_register_to_add++;
			}
			//		    integer_data_received = false;
			floating_point_data_received = true;
			//		    load_store_data_received = false;

			// Accept part
			//			int floatingPointReadRegisterPort;
			int floating_point_read_register_to_remove = 0;

			/* Enable accepted instructions */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				if(!outFloatingPointInstruction[floatingPointReadRegisterPort].accept) break;
				outFloatingPointInstruction[floatingPointReadRegisterPort].enable = true;
				floating_point_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				outFloatingPointInstruction[floatingPointReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int floating_point_read_register_free_space = floatingPointReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int floating_point_read_register_to_accept = MIN(floating_point_read_register_free_space + floating_point_read_register_to_remove, floating_point_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < floating_point_read_register_to_accept; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction[floatingPointReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction[floatingPointReadRegisterPort].accept = false;
			}
			// End of Accept part
		    }
		}

		/** A SystemC process handling valid, accept and enable hand-shaking for load/store pipe */
		//		void ExternalControlLoadStorePipe()
		void onLoadStoreData()
		{
		  bool areallknown(true);
		  for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
		    {
		      if (!inLoadStoreInstruction[i].data.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			/* Count the number of instruction to add into the read pipeline */
			for(int loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				if(!inLoadStoreInstruction[loadStoreReadRegisterPort].data.something()) break;
				load_store_read_register_to_add++;
			}
			//		    integer_data_received = false;
			//		    floating_point_data_received = false;
			load_store_data_received = true;
		    }
		}

		void onLoadStoreAccept()
		{
		  bool areallknown(true);
		  for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
		    {
		      if (!outLoadStoreInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  //		  if (areallknown && load_store_data_received)
		  if (areallknown && load_store_data_received)
		    {
			int loadStoreReadRegisterPort;
			int load_store_read_register_to_remove = 0;

			/* Enable accepted instructions */
			for(loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				if(!outLoadStoreInstruction[loadStoreReadRegisterPort].accept) break;
				outLoadStoreInstruction[loadStoreReadRegisterPort].enable = true;
				/* Count accepted instruction as being removed */
				load_store_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				outLoadStoreInstruction[loadStoreReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int load_store_read_register_free_space = loadStoreReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int load_store_read_register_to_accept = MIN(load_store_read_register_free_space + load_store_read_register_to_remove, load_store_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < load_store_read_register_to_accept; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction[loadStoreReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction[loadStoreReadRegisterPort].accept = false;
			}
		    }
		}

		void onLoadStore()
		{
		  bool areallknown(true);
		  for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
		    {
		      if (!inLoadStoreInstruction[i].data.known() || !outLoadStoreInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
			/* Count the number of instruction to add into the read pipeline */
			for(int loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				if(!inLoadStoreInstruction[loadStoreReadRegisterPort].data.something()) break;
				load_store_read_register_to_add++;
			}
			//		    integer_data_received = false;
			//		    floating_point_data_received = false;
			//load_store_data_received = true;

			// Accept part
			int loadStoreReadRegisterPort;
			int load_store_read_register_to_remove = 0;

			/* Enable accepted instructions */
			for(loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				if(!outLoadStoreInstruction[loadStoreReadRegisterPort].accept) break;
				outLoadStoreInstruction[loadStoreReadRegisterPort].enable = true;
				/* Count accepted instruction as being removed */
				load_store_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				outLoadStoreInstruction[loadStoreReadRegisterPort].enable = false;
			}

			/* Get the free space into the read pipeline */
			int load_store_read_register_free_space = loadStoreReadRegisterPipeline.FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int load_store_read_register_to_accept = MIN(load_store_read_register_free_space + load_store_read_register_to_remove, load_store_read_register_to_add);

			/* Accepts instructions according to the number of instructions to accept */
			for(loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < load_store_read_register_to_accept; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction[loadStoreReadRegisterPort].accept = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction[loadStoreReadRegisterPort].accept = false;
			}
			// End of Accept Part
		    }
		}

		/** A SystemC process for handling the valid, accept and enable hand-shaking for the write back pipe */
		/*
		void ExternalControl()
		{
			int writeBackPort;
			// Accepts instructions on all write back ports
			for(writeBackPort = 0; writeBackPort < WriteBackWidth; writeBackPort++)
			{
				outWriteBackAccept[writeBackPort] = inWriteBackValid[writeBackPort];
			}
		}
		*/

		/** For each read pipe stage, bypass results being written back */
		void ByPass()
		{
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> integerReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, FloatingPointReadRegisterWidth * nReadRegisterStages> floatingPointReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, LoadStoreReadRegisterWidth * nReadRegisterStages> loadStoreReadRegisterPipelineEntry;
			QueuePointer<WriteBackPipelineEntry<T, nSources>, WriteBackWidth * nWriteBackStages> writeBackPipelineEntry;
			/*
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> conditionReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> FPSCRReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> linkReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> countReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> XERReadRegisterPipelineEntry;
			*/

			/* For each read register pipeline entry, bypass results being written back */
			for(integerReadRegisterPipelineEntry = integerReadRegisterPipeline.SeekAtHead(); integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++)
			{
			  int i;
			  if(!integerReadRegisterPipelineEntry->Ready())
			  {
			    /* For each source operand */
			    for(i = 0; i < integerReadRegisterPipelineEntry->instruction->sources.size(); i++)
			      {
				if(integerReadRegisterPipelineEntry->instruction->sources[i].tag >= 0 && !integerReadRegisterPipelineEntry->instruction->sources[i].valid)
				{
				  /* For each write back pipeline entry */
				  for(writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
				  {
				    for (int j=0; j<writeBackPipelineEntry->instruction->destinations.size();j++)
				    {
				      /* Check if tags match */
				      if(integerReadRegisterPipelineEntry->instruction->sources[i].tag == writeBackPipelineEntry->instruction->destinations[j].tag &&
					 integerReadRegisterPipelineEntry->instruction->sources[i].type == writeBackPipelineEntry->instruction->destinations[j].type)
				      {
					/* Bypass the value */
					//integerPipeChanged = true;
					integerReadRegisterPipelineEntry->instruction->sources[i].valid = true;
					integerReadRegisterPipelineEntry->instruction->sources[i].ready = true;
					integerReadRegisterPipelineEntry->instruction->sources[i].data = writeBackPipelineEntry->instruction->destinations[j].data;
				      }
				    }
				  }
				}
			      }
			  }
			}

			/* For each read register pipeline entry, bypass results being written back */
			for(floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline.SeekAtHead(); floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++)
			{
			  if(!floatingPointReadRegisterPipelineEntry->Ready())
			  {
			    int i;
			    /* For each source operand */
			    for(i = 0; i < floatingPointReadRegisterPipelineEntry->instruction->sources.size(); i++)
			    {
			      if(floatingPointReadRegisterPipelineEntry->instruction->sources[i].tag >= 0 && !floatingPointReadRegisterPipelineEntry->instruction->sources[i].valid)
			      {
				/* For each write back pipeline entry */
				for(writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
				{
				  for (int j=0; j<writeBackPipelineEntry->instruction->destinations.size();j++)
				  {
				    /* Check if tags match */
				    if(floatingPointReadRegisterPipelineEntry->instruction->sources[i].tag == writeBackPipelineEntry->instruction->destinations[j].tag &&
				       floatingPointReadRegisterPipelineEntry->instruction->sources[i].type == writeBackPipelineEntry->instruction->destinations[j].type)
				    {
				      /* Bypass the value */
				      //floatingPointPipeChanged = true;
				      floatingPointReadRegisterPipelineEntry->instruction->sources[i].valid = true;
				      floatingPointReadRegisterPipelineEntry->instruction->sources[i].ready = true;
				      floatingPointReadRegisterPipelineEntry->instruction->sources[i].data = writeBackPipelineEntry->instruction->destinations[j].data;
				    }
				  }
				}
			      }
			    }
			  }
			}
			/* For each read register pipeline entry, bypass results being written back */
			for(loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline.SeekAtHead(); loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++)
			{
			  if(!loadStoreReadRegisterPipelineEntry->Ready())
			  {
			    int i;
			    /* For each source operand */
			    for(i = 0; i < loadStoreReadRegisterPipelineEntry->instruction->sources.size(); i++)
			    {
			      if(loadStoreReadRegisterPipelineEntry->instruction->sources[i].tag >= 0 && !loadStoreReadRegisterPipelineEntry->instruction->sources[i].valid)
			      {
				/* For each write back pipeline entry */
				for(writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
				{
				  for (int j=0; j<writeBackPipelineEntry->instruction->destinations.size();j++)
				  {
				    /* Check if tags match */
				    if(loadStoreReadRegisterPipelineEntry->instruction->sources[i].tag == writeBackPipelineEntry->instruction->destinations[j].tag &&
				       loadStoreReadRegisterPipelineEntry->instruction->sources[i].type == writeBackPipelineEntry->instruction->destinations[j].type)
				      {
					/* Bypass the value */
					//loadStorePipeChanged = true;
					loadStoreReadRegisterPipelineEntry->instruction->sources[i].valid = true;
					loadStoreReadRegisterPipelineEntry->instruction->sources[i].ready = true;
					loadStoreReadRegisterPipelineEntry->instruction->sources[i].data = writeBackPipelineEntry->instruction->destinations[j].data;
				      }
				  }
				}
			      }
			    }
			  }
			}



		}
		/** Bypass results being written back. It does not modifies the pipeline entry.
			@param entry an pipeline entry
			@param instruction a copy of the instruction in the pipeline entry
			@return true if instruction can be launched
		*/
		bool ByPass(ReadRegisterPipelineEntry<T, nSources> *entry, InstructionPtr *instruction)
		{
		  if(entry->ready) return true;
		  //		if(entry->Ready()) return true;
			int i;
			int needed = 0;
			int ready = 0;
			/* For each source operand */
			for(i = 0; i < (*instruction)->sources.size(); i++)
			{
			  if((*instruction)->sources[i].tag >= 0)
			  {
			    needed++;
			    //if(!entry->instruction->sources[i].valid)
			    if(!entry->instruction->sources[i].valid)
			    {
			      int j;
			      /* For each write back port */
			      for(j = 0; j < WriteBackWidth; j++)
			      {
				/* Is an instruction being written back ? */
				//if(inWriteBackEnable[j])
				if(inWriteBackInstruction[j].enable)
				  {
				    /* Get the instruction */
				    InstructionPtr writeBackInstruction = inWriteBackInstruction[j].data;
				    //    const Instruction *writeBackInstruction = &inst;
				    /* Check if tags match */
				    for (int k=0; k<writeBackInstruction->destinations.size();k++)
				    if((*instruction)->sources[i].tag == writeBackInstruction->destinations[k].tag &&
				       (*instruction)->sources[i].type == writeBackInstruction->destinations[k].type)
				    {
				      /* Bypass the value */
				      ready++;
				      (*instruction)->sources[i].valid = true;
				      (*instruction)->sources[i].ready = true;
				      (*instruction)->sources[i].data = writeBackInstruction->destinations[k].data;
				    }
				  }
			      }
			    }
			    else
			      {
				ready++;
			      }
			  }
			}
			return needed == ready;
		}


		/** A SystemC process managing the state of the register file and its pipes */
		//		void InternalControl()
		void end_of_cycle()
		{
			int i;

			/*
			if(!inFlush && !inIntegerEnable[0] && !inFloatingPointEnable[0] && !inLoadStoreEnable[0] &&
			   !inWriteBackEnable[0] &&
			   !inIntegerAccept[0] && !inFloatingPointAccept[0] && !inLoadStoreAccept[0] &&
			   !changed && !integerPipeChanged && !floatingPointPipeChanged && !loadStorePipeChanged) return;
			integerPipeChanged = false;
			floatingPointPipeChanged = false;
			loadStorePipeChanged = false;
			changed = false;
			*/
			/* Flush ? */
			if(inFlush.enable && inFlush.data.something())
			{
			  if(inFlush.data)
			  {
			  //				changed = true;
				/* Flush the write back pipeline */
				writeBackPipeline.Flush();

				/* Flush the integer read pipeline */
				integerReadRegisterPipeline.Flush();

				/* Flush the floating point read pipeline */
				floatingPointReadRegisterPipeline.Flush();

				/* Flush the load/store read pipeline */
				loadStoreReadRegisterPipeline.Flush();

				/* Reset all output ports */
				/*
				for(i = 0; i < WriteBackWidth; i++)
				{
					outWriteBackAccept[i] = false;
					outWriteBackEnable[i] = false;
				}
				for(i = 0; i < IntegerReadRegisterWidth; i++)
				{
					outIntegerAccept[i] = false;
					outIntegerValid[i] = false;
				}
				for(i = 0; i < FloatingPointReadRegisterWidth; i++)
				{
					outFloatingPointAccept[i] = false;
					outFloatingPointValid[i] = false;
				}
				for(i = 0; i < LoadStoreReadRegisterWidth; i++)
				{
					outLoadStoreAccept[i] = false;
					outLoadStoreValid[i] = false;
				}
				*/
				/* Reset the valid bits */
				memset(integerValid, true, sizeof(integerValid));
				memset(floatingPointValid, true, sizeof(floatingPointValid));
				memset(conditionValid, true, sizeof(conditionValid));
				memset(FPSCRValid, true, sizeof(FPSCRValid));
				memset(linkValid, true, sizeof(linkValid));
				memset(countValid, true, sizeof(countValid));
				memset(XERValid, true, sizeof(XERValid));
				return;
			  }
			}

			/* Remove launched integer instructions from the integer read pipeline */
			for(i = 0; i < IntegerReadRegisterWidth; i++)
			{
			  //				if(!inIntegerAccept[i]) break;
			  //   	if(!inIntegerInstruction[i].accept) break;
				if(!outIntegerInstruction[i].accept) break;
				integerReadRegisterPipeline.RemoveHead();
				//				integerPipeChanged = true;
			}

			/* Remove launched floating point instructions from the floating point read pipeline */
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			{
			  //	if(!inFloatingPointInstruction[i].accept) break;
				if(!outFloatingPointInstruction[i].accept) break;
				floatingPointReadRegisterPipeline.RemoveHead();
				//				floatingPointPipeChanged = true;
			}

			/* Remove launched load/store instructions from the load/store read pipeline */
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			{
			  //	if(!inLoadStoreInstruction[i].accept) break;
				if(!outLoadStoreInstruction[i].accept) break;
				loadStoreReadRegisterPipeline.RemoveHead();
				//				loadStorePipeChanged = true;
			}


			int writeBackPort;
			QueuePointer<WriteBackPipelineEntry<T, nSources>, WriteBackWidth * nWriteBackStages> writeBackPipelineEntry;

			/* Remove instructions  which finished to write back their results from the write back pipeline */
			for(writeBackPort = 0, writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPort < WriteBackWidth && writeBackPipelineEntry; writeBackPipelineEntry++, writeBackPort++)
			{
				if(writeBackPipelineEntry->delay > 0) break;
				if (outWriteBackReceived[writeBackPort].accept)
				  {
				    WriteBack(writeBackPipelineEntry->instruction);
				    writeBackPipeline.RemoveHead();
				  }
				//				changed = true;
			}

			/* For each integer read register port */
			for(i = 0; i < IntegerReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				//if(!inIntegerEnable[i]) break;
				if(!inIntegerInstruction[i].enable) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = integerReadRegisterPipeline.New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * IntegerReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inIntegerInstruction[i].data);
					/* Read the source operand of the instruction from the register file */
					Read(entry);
                                        // For Dump
                                        entry->instruction->timing_register_cycle = timestamp();
					//					if (timestamp()>380)
					//					  cerr << "DDDD: " << *entry << endl;
					/* Invalidate destination operand register of the instruction */
					/*
					if(entry->instruction->destination.tag >= 0)
					{
						switch(entry->instruction->destination.type)
						{
							case GPR_T:
								integerValid[entry->instruction->destination.tag] = false;
								break;

							case FPR_T:
								floatingPointValid[entry->instruction->destination.tag] = false;
								break;

							default:
								cerr << "register is neither an integer nor a floating point register" << endl;
								exit(-1);
						}
					}
					*/
					//integerPipeChanged = true;
				}
				else
				{
					/* throw an error if there is not enough space into the pipeline */
					cerr << "time stamp = " << timestamp() << endl;
					cerr << "Integer Read Register pipeline overflow" << endl;
					//exit(-1);
					abort();
				}
			}

			/* For each floating point read register port */
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				if(!inFloatingPointInstruction[i].enable) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = floatingPointReadRegisterPipeline.New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * FloatingPointReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inFloatingPointInstruction[i].data);
					/* Read the source operand of the instruction from the register file */
					Read(entry);
                                        // For Dump
                                        entry->instruction->timing_register_cycle = timestamp();

					/* Invalidate destination operand register of the instruction */
					/*
					if(entry->instruction->destination.tag >= 0)
					{
						switch(entry->instruction->destination.type)
						{
							case GPR_T:
								integerValid[entry->instruction->destination.tag] = false;
								break;

							case FPR_T:
								floatingPointValid[entry->instruction->destination.tag] = false;
								break;

							default:
								cerr << "register is neither an integer nor a floating point register" << endl;
								exit(-1);
						}
					}
					*/
					//floatingPointPipeChanged = true;
				}
				else
				{
					/* throw an error if there is not enough space into the pipeline */
					cerr << "time stamp = " << timestamp() << endl;
					cerr << "Floating Point Read Register pipeline overflow" << endl;
					//exit(-1);
					abort();
				}
			}

			/* For each load/store read register port */
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				if(!inLoadStoreInstruction[i].enable) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = loadStoreReadRegisterPipeline.New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * LoadStoreReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inLoadStoreInstruction[i].data);
					/* Read the source operand of the instruction from the register file */
					Read(entry);
                                        // For Dump
                                        entry->instruction->timing_register_cycle = timestamp();

					/*
					if(entry->instruction->destination.tag >= 0)
					{
						switch(entry->instruction->destination.type)
						{
							case GPR_T:
								integerValid[entry->instruction->destination.tag] = false;
								break;

							case FPR_T:
								floatingPointValid[entry->instruction->destination.tag] = false;
								break;

							default:
								cerr << "register is neither an integer nor a floating point register" << endl;
								exit(-1);
						}
					}
					*/
					//changed = true;
					//loadStorePipeChanged = true;
				}
				else
				{
					/* throw an error if there is not enough space into the pipeline */
					cerr << "time stamp = " << timestamp() << endl;
					cerr << "Load/Store Read Register pipeline overflow" << endl;
					exit(-1);
				}
			}

			/* Adds the instructions on the common data bus into the write back pipeline */
			/* For each write back port */
			for(i = 0; i < WriteBackWidth; i++)
			{
				/* Is there an instruction on the write back port ? */
				//if(!inWriteBackInstruction[i].enable) break;
			  if (inWriteBackInstruction[i].enable)
			    {
				/* Get the instruction */
				const InstructionPtr writeBackInstruction = inWriteBackInstruction[i].data;

				/* Allocate a write back pipeline entry */
				WriteBackPipelineEntry<T, nSources> *entry = writeBackPipeline.New();

				if(entry)
				{
					/* Set the processing delay to number of pipeline stage */
					entry->delay = nWriteBackStages * WriteBackWidth;
					/* Records the instruction */
					entry->instruction = writeBackInstruction;
					//changed = true;
                                        // For Dump
                                        entry->instruction->timing_writeback_cycle = timestamp();

				}
				else
				{
					/* Throw an error if there is not enough space into the pipeline */
					cerr << "time stamp = " << timestamp() << endl;
					cerr << "Write Back pipeline overflow" << endl;
					exit(-1);
				}
			    }
			}

			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> integerReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, FloatingPointReadRegisterWidth * nReadRegisterStages> floatingPointReadRegisterPipelineEntry;
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, LoadStoreReadRegisterWidth * nReadRegisterStages> loadStoreReadRegisterPipelineEntry;

			/* Decrement the processing delay in each integer read register pipeline entry */
			for(integerReadRegisterPipelineEntry = integerReadRegisterPipeline.SeekAtHead(); integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++)
			{
				if(integerReadRegisterPipelineEntry->delay > 0)
				{
				  //integerPipeChanged = true;
					integerReadRegisterPipelineEntry->delay = (integerReadRegisterPipelineEntry->delay >= IntegerReadRegisterWidth) ? integerReadRegisterPipelineEntry->delay - IntegerReadRegisterWidth : 0;
				}
			}
			/* Decrement the processing delay in each floating point read register pipeline entry */
			for(floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline.SeekAtHead(); floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++)
			{
				if(floatingPointReadRegisterPipelineEntry->delay > 0)
				{
				  //floatingPointPipeChanged = true;
					floatingPointReadRegisterPipelineEntry->delay = (floatingPointReadRegisterPipelineEntry->delay >= FloatingPointReadRegisterWidth) ? floatingPointReadRegisterPipelineEntry->delay - FloatingPointReadRegisterWidth : 0;
				}
			}
			/* Decrement the processing delay in each load/store read register pipeline entry */
			for(loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline.SeekAtHead(); loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++)
			{
				if(loadStoreReadRegisterPipelineEntry->delay > 0)
				{
				  //loadStorePipeChanged = true;
					loadStoreReadRegisterPipelineEntry->delay = (loadStoreReadRegisterPipelineEntry->delay >= LoadStoreReadRegisterWidth) ? loadStoreReadRegisterPipelineEntry->delay - LoadStoreReadRegisterWidth : 0;
				}
			}

			/* Decrement the processing delay in each write back pipeline entry */
			for(writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
			{
				if(writeBackPipelineEntry->delay > 0)
				{
				  //changed = true;
				  //writeBackPipelineEntry->delay--;
				  writeBackPipelineEntry->delay = (writeBackPipelineEntry->delay >= WriteBackWidth) ? writeBackPipelineEntry->delay - WriteBackWidth : 0;
				}
			}

			/* Bypass the result from the write back pipeline to the read register pipelines */
			ByPass();

#ifdef DD_DEBUG_PIPELINE_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
#endif

		} // End of end_of_cycle...
		
		///////////////////////////////////
		// START OF CYCLE !!! YES !!! WARNING : NO
		// We don't have to send something at start_of_cycle ...
		// we have to wait that writeback complete (writeBack.enable)
		// to send instruction with Bypassed sources ...
		///////////////////////////////////
		void start_of_cycle()
		{
		  // Just reset counters ...
		  WriteBacksReceived = false;
		  
		  integer_read_register_to_add = 0;
		  floating_point_read_register_to_add = 0;
		  load_store_read_register_to_add = 0;
		  
		  int writeBackPort;
		  QueuePointer<WriteBackPipelineEntry<T, nSources>, WriteBackWidth * nWriteBackStages> writeBackPipelineEntry;

		  // Sending written back instruction from previous cycle ...
		  /* Remove instructions  which finished to write back their results from the write back pipeline */
		  for(writeBackPort = 0, writeBackPipelineEntry = writeBackPipeline.SeekAtHead(); writeBackPort < WriteBackWidth && writeBackPipelineEntry; writeBackPipelineEntry++, writeBackPort++)
		    {
		      if(writeBackPipelineEntry->delay > 0) break;
		      outWriteBackReceived[writeBackPort].data = writeBackPipelineEntry->instruction;
		    }
		  while(writeBackPort<WriteBackWidth)
		    {
		      outWriteBackReceived[writeBackPort].data.nothing();
		      writeBackPort++;
		    }
		  
		    integer_data_received = false;
		    floating_point_data_received = false;
		    load_store_data_received = false;
  
		}// end of start_of_cycle

                void onWriteBackReceivedAccept()
                {
		  bool areallknown(true);
		  for(int i = 0; i < WriteBackWidth; i++)
		    {
		      if (!outWriteBackReceived[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  if (areallknown)
		    {
		      for(int i = 0; i < WriteBackWidth; i++)
			{
			  if (outWriteBackReceived[i].accept)
			    {
			      outWriteBackReceived[i].enable = true;
			    }
			  else
			    {
			      outWriteBackReceived[i].enable = false;
			    }
			}
		      
		    }
		}

		/** Reads an integer register (general purpose register)
			@param tag the register number
			@return the register value
		*/
		T ReadGPR(int tag)
		{
			return integerRegisters.Read(tag);
		}

		/** Writes into an integer register (general purpose register)
			@param tag the register number
			@param value the value to write
		*/
		void WriteGPR(int tag, T value)
		{
			integerRegisters.Write(tag, value);
		}

		/** Reads a floating point register
			@param tag the register number
			@return the register value
		*/
		T ReadFPR(int tag)
		{
			return floatingPointRegisters.Read(tag);
		}

		/** Writes into a floating point register
			@param tag the register number
			@param value the value to write
		*/
		void WriteFPR(int tag, T value)
		{
			floatingPointRegisters.Write(tag, value);
		}

		uint32_t ReadCR(int tag)
		{
			return conditionRegisters.Read(tag);
		}
		void WriteCR(int tag, T value)
		{
			conditionRegisters.Write(tag, value);
		}


		uint32_t ReadFPSCR(int tag)
		{
			return FPSCRegisters.Read(tag);
		}
		void WriteFPSCR(int tag, T value)
		{
			FPSCRegisters.Write(tag, value);
		}

		uint32_t ReadLR(int tag)
		{
			return linkRegisters.Read(tag);
		}
		void WriteLR(int tag, T value)
		{
			linkRegisters.Write(tag, value);
		}


		uint32_t ReadCTR(int tag)
		{
			return countRegisters.Read(tag);
		}
		void WriteCTR(int tag, T value)
		{
			countRegisters.Write(tag, value);
		}


		uint32_t ReadXER(int tag)
		{
			return XERRegisters.Read(tag);
		}
		void WriteXER(int tag, T value)
		{
			XERRegisters.Write(tag, value);
		}



  

		/** Reads the source operands of an instruction into the register files
			@param instruction an instruction
		*/
		void Read(ReadRegisterPipelineEntry<T, nSources> *entry)
		{
			int i;
			InstructionPtr *instruction = &(entry->instruction);
			/* For each source operands */
			for(i = 0; i < (*instruction)->sources.size(); i++)
			{
				if((*instruction)->sources[i].tag >= 0 && !(*instruction)->sources[i].valid)
				{
				  switch((*instruction)->sources[i].type)
				    {
				    case GPR_T:
				      /* If register is valid */
				      if(integerValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = integerRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				      
				    case FPR_T:
				      /* If register is valid */
				      if(floatingPointValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = floatingPointRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				    case CR_T:
				      /* If register is valid */
				      if(conditionValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = conditionRegisters.Read((*instruction)->sources[i].tag);
					}
				      /*
				      else
					{
					  if(timestamp() > 380) { cerr << "DDDD("<<timestamp()
								       <<": Condition register not Valid !!! " 
								       <<"register tag: " << (*instruction)->sources[i].tag
								       << endl; }
					}
				      */
				      break;
				    case FPSCR_T:
				      /* If register is valid */
				      if(FPSCRValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = FPSCRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				    case LR_T:
				      /* If register is valid */
				      if(linkValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = linkRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				    case CTR_T:
				      /* If register is valid */
				      if(countValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = countRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				    case XER_T:
				      /* If register is valid */
				      if(XERValid[(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = XERRegisters.Read((*instruction)->sources[i].tag);
					}
				      break;
				      
				    default:
				      cerr << "register is neither an integer nor a floating point register" << endl;
				      //exit(-1);
				      abort();
				    }
				}
			}
			/* Invalidate destination operand register of the instruction */
			//			if(entry->(*instruction)->destination.tag >= 0)
			for (int j=0; j<entry->instruction->destinations.size();j++)
			{
				switch(entry->instruction->destinations[j].type)
				{
					case GPR_T:
						integerValid[entry->instruction->destinations[j].tag] = false;
						break;

					case FPR_T:
						floatingPointValid[entry->instruction->destinations[j].tag] = false;
						break;

					case CR_T:
						conditionValid[entry->instruction->destinations[j].tag] = false;
						break;
					case FPSCR_T:
						FPSCRValid[entry->instruction->destinations[j].tag] = false;
							break;
					case LR_T:
						linkValid[entry->instruction->destinations[j].tag] = false;
							break;
					case CTR_T:
						countValid[entry->instruction->destinations[j].tag] = false;
							break;
					case XER_T:
						XERValid[entry->instruction->destinations[j].tag] = false;
							break;

					default:
						cerr << "register is neither an integer nor a floating point register" << endl;
						//exit(-1);
						abort();
				}
			}
		}

		/** Write back results of an instruction
			@param instruction an instruction
		*/
		void WriteBack(InstructionPtr instruction)
		{
		  for (int j=0; j<instruction->destinations.size();j++)
		  {
			int tag = instruction->destinations[j].tag;
			//			reg_type_t type = instruction->destination.type;
			operand_type_t type = instruction->destinations[j].type;
			/* Check whether there is a destination operand */
			if(tag >= 0)
			{
				switch(type)
				{
					case GPR_T:
						/* Write back the value into the integer register file */
						integerRegisters.Write(tag, instruction->destinations[j].data);
						break;

					case FPR_T:
						/* Write back the value into the floating point register file */
						floatingPointRegisters.Write(tag, instruction->destinations[j].data);
						break;
					case CR_T:
						/* Write back the value into the condition register file */
						conditionRegisters.Write(tag, instruction->destinations[j].data);
						break;
					case FPSCR_T:
						/* Write back the value into the FPSC register file */
						FPSCRegisters.Write(tag, instruction->destinations[j].data);
						break;
					case LR_T:
						/* Write back the value into the link register file */
						linkRegisters.Write(tag, instruction->destinations[j].data);
						break;
					case CTR_T:
						/* Write back the value into the count register file */
						countRegisters.Write(tag, instruction->destinations[j].data);
						break;
					case XER_T:
						/* Write back the value into the XER register file */
						XERRegisters.Write(tag, instruction->destinations[j].data);
						break;

					default:
						cerr << "register is neither an integer nor a floating point register" << endl;
						//exit(-1);
						abort();
				}
				
				/*
				QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> integerReadRegisterPipelineEntry;
				QueuePointer<ReadRegisterPipelineEntry<T, nSources>, FloatingPointReadRegisterWidth * nReadRegisterStages> floatingPointReadRegisterPipelineEntry;
				QueuePointer<ReadRegisterPipelineEntry<T, nSources>, LoadStoreReadRegisterWidth * nReadRegisterStages> loadStoreReadRegisterPipelineEntry;
				
				for(integerReadRegisterPipelineEntry = integerReadRegisterPipeline.SeekAtHead(); integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++)
				{
					int i;
					for(i = 0; i < nSources; i++)
					{
						if(integerReadRegisterPipelineEntry->instruction->sources[i].tag == tag &&
						   integerReadRegisterPipelineEntry->instruction->sources[i].type == type)
						{
							integerReadRegisterPipelineEntry->instruction->sources[i].valid = true;
						}
					}
				}
				for(floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline.SeekAtHead(); floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++)
				{
					int i;
					for(i = 0; i < nSources; i++)
					{
						if(floatingPointReadRegisterPipelineEntry->instruction->sources[i].tag == tag &&
						   floatingPointReadRegisterPipelineEntry->instruction->sources[i].type == type)
						{
							floatingPointReadRegisterPipelineEntry->instruction->sources[i].valid = true;
						}
					}
				}
				for(loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline.SeekAtHead(); loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++)
				{
					int i;
					for(i = 0; i < nSources; i++)
					{
						if(loadStoreReadRegisterPipelineEntry->instruction->sources[i].tag == tag &&
						   loadStoreReadRegisterPipelineEntry->instruction->sources[i].type == type)
						{
							loadStoreReadRegisterPipelineEntry->instruction->sources[i].valid = true;
						}
					}
				}
				*/
				/* Validate the destination operand register of the instruction */
				switch(type)
				{
					case GPR_T:
						integerValid[tag] = true;
						break;

					case FPR_T:
						floatingPointValid[tag] = true;
						break;
					case CR_T:
						conditionValid[tag] = true;
							break;
					case FPSCR_T:
						FPSCRValid[tag] = true;
							break;
					case LR_T:
						linkValid[tag] = true;
							break;
					case CTR_T:
						countValid[tag] = true;
							break;
					case XER_T:
						XERValid[tag] = true;
							break;

					default:
						cerr << "register is neither an integer nor a floating point register" << endl;
						//exit(-1);
						abort();
				}
			}// end of if (tag>0)...
			/********************************************************************
			 * Problem with instruction which access Special Purpose Registers 
			 * Tags are invalid ...
			 ********************************************************************/
			/*
			else
			  {
			    cerr << "register file : invalid tag !!!" << endl;
			    cerr << "Inst: " << *instruction << endl;
 			    exit(-1);
			    
			  }
			*/
		  }
		}

		/** Prints the register files content, the read pipes and the write back pipe into a output stream
			@param os an output stream
			@param registerFile a register file module
			@return the output stream
		*/
		friend ostream& operator << (ostream& os, const RegisterFile& registerFile)
		{
			os << "============= REGISTERFILE ============" << endl;
			os << "Integer Pipe:" << endl;
			os << registerFile.integerReadRegisterPipeline;
			os << "Floating Point Pipe:" << endl;
			os << registerFile.floatingPointReadRegisterPipeline;
			os << "Load/Store Pipe:" << endl;
			os << registerFile.loadStoreReadRegisterPipeline;
			os << "WriteBack:" << endl;
			os << registerFile.writeBackPipeline;
			//	os << "Integer Registers:" << endl;
			//	os << registerFile.integerRegisters;
			//	os << "Floating Point Registers:" << endl;
			//	os << registerFile.floatingPointRegisters;
			os << "Integer RegisterFile:" << endl;
			os << registerFile.integerRegisters << endl;
			os << "Floating Point RegisterFile:" << endl;
			os << registerFile.floatingPointRegisters << endl;
			os << "Conditon RegisterFile:" << endl;
			os << registerFile.conditionRegisters << endl;
			os << "FPSC RegisterFile:" << endl;
			os << registerFile.FPSCRegisters << endl;
			os << "Link RegisterFile:" << endl;
			os << registerFile.linkRegisters << endl;
			os << "Count RegisterFile:" << endl;
			os << registerFile.countRegisters << endl;
			os << "XER RegisterFile:" << endl;
			os << registerFile.XERRegisters << endl;
			os << "=======================================" << endl;
			return os;
		}

		/** Performs a sanity check of the register file module
			@return true if ok
		*/
		bool Check()
		{
			return integerReadRegisterPipeline.Check() &&
			       floatingPointReadRegisterPipeline.Check() &&
			       loadStoreReadRegisterPipeline.Check() &&
			       writeBackPipeline.Check();
		}
		
		void WarmRestart()
		{
			integerReadRegisterPipeline.Reset();
			loadStoreReadRegisterPipeline.Reset();
			floatingPointReadRegisterPipeline.Reset();
			writeBackPipeline.Reset();
			memset(integerValid, true, sizeof(integerValid));
			memset(floatingPointValid, true, sizeof(floatingPointValid));
			//			integerPipeChanged = true;
			//			floatingPointPipeChanged = true;
			//			loadStorePipeChanged = true;
			//			changed = true;
			WriteBacksReceived = false;
		}
		
		void ResetStats()
		{
		}

	private:
		/* A queue modeling the write back pipeline */
		Queue<WriteBackPipelineEntry<T, nSources>, nWriteBackStages * WriteBackWidth> writeBackPipeline;

		/* A queue modeling the integer read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * IntegerReadRegisterWidth> integerReadRegisterPipeline;

		/* A queue modeling the floating point read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * FloatingPointReadRegisterWidth> floatingPointReadRegisterPipeline;

		/* A queue modeling the load/store read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * LoadStoreReadRegisterWidth> loadStoreReadRegisterPipeline;

		/* The integer register file */
		RegisterFileBase<T, nSources, nIntegerRegisters> integerRegisters;
		/* Valid bits for each integer registers */
		bool integerValid[nIntegerRegisters];

		/* The floating point register file */
		RegisterFileBase<T, nSources, nFloatingPointRegisters> floatingPointRegisters;
		/* Valid bits for each floating point registers */
		bool floatingPointValid[nIntegerRegisters];

		/* The condition register file */
		RegisterFileBase<T, nSources, nConditionRegisters> conditionRegisters;
		/* Valid bits for each condition registers */
		bool conditionValid[nConditionRegisters];

		/* The count register file */
		RegisterFileBase<T, nSources, nCountRegisters> countRegisters;
		/* Valid bits for each count registers */
		bool countValid[nCountRegisters];

		/* The FPSCR register file */
		RegisterFileBase<T, nSources, nFPSCRegisters> FPSCRegisters;
		/* Valid bits for each FPSC registers */
		bool FPSCRValid[nFPSCRegisters];

		/* The Link register file */
		RegisterFileBase<T, nSources, nLinkRegisters> linkRegisters;
		/* Valid bits for each Link registers */
		bool linkValid[nLinkRegisters];

		/* The XER register file */
		RegisterFileBase<T, nSources, nXERRegisters> XERRegisters;
		/* Valid bits for each XER registers */
		bool XERValid[nXERRegisters];


		bool WriteBacksReceived;

		int integer_read_register_to_add;
		int floating_point_read_register_to_add;
		int load_store_read_register_to_add;

  bool integer_data_received;
  bool floating_point_data_received;
  bool load_store_data_received;
		/* Some ports to make the SystemC scheduler call the ExternalControl process on state changes */
		/*
		ml_out_data<bool> outStateChangedIntegerPipe;
		ml_signal_data<bool> stateChangedIntegerPipe;
		ml_in_data<bool> inStateChangedIntegerPipe;
		ml_out_data<bool> outStateChangedFloatingPointPipe;
		ml_signal_data<bool> stateChangedFloatingPointPipe;
		ml_in_data<bool> inStateChangedFloatingPointPipe;
		ml_out_data<bool> outStateChangedLoadStorePipe;
		ml_signal_data<bool> stateChangedLoadStorePipe;
		ml_in_data<bool> inStateChangedLoadStorePipe;
		*/
		/*
		bool integerPipeChanged;
		bool floatingPointPipeChanged;
		bool loadStorePipeChanged;
		bool changed;
		*/
};

} // end of namespace issue
} // end of namespace pipeline
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim


#endif
