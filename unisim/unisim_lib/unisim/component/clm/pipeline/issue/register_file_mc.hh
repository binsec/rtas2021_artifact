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

#ifndef __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_REGISTER_FILE_MC_HH__
#define __UNISIM_COMPONENT_CLM_PIPELINE_ISSUE_REGISTER_FILE_MC_HH__

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
template <class T, int nSources, int nIntegerRegisters, int nFloatingPointRegisters, int nReadRegisterStages, int IntegerReadRegisterWidth, int FloatingPointReadRegisterWidth, int LoadStoreReadRegisterWidth, int nWriteBackStages, int WriteBackWidth,
	  uint32_t nConfig=2 >
class RegisterFile : public module
{
	public:
		inclock inClock;

		/* From/to the common data bus */
		inport<InstructionPtr, nConfig*WriteBackWidth > inWriteBackInstruction;

		/* From/to the integer issue queue */
		inport<InstructionPtr, nConfig*IntegerReadRegisterWidth > inIntegerInstruction;

		/* From/to the floating point issue queue */
		inport<InstructionPtr, nConfig*FloatingPointReadRegisterWidth > inFloatingPointInstruction;

		/* From/to the load/store issue queue */
		inport<InstructionPtr, nConfig*LoadStoreReadRegisterWidth > inLoadStoreInstruction;

		/* From/to the integer instruction broadcaster */
		outport<InstructionPtr, nConfig*IntegerReadRegisterWidth > outIntegerInstruction;

		/* From/to the floating point instruction broadcaster */
		outport<InstructionPtr, nConfig*FloatingPointReadRegisterWidth > outFloatingPointInstruction;

		/* From/to the load/store instruction broadcaster */
		outport<InstructionPtr, nConfig*LoadStoreReadRegisterWidth > outLoadStoreInstruction;

		/* To the allocator renamer and the reorder buffer */
		outport<InstructionPtr, nConfig*WriteBackWidth > outWriteBackReceived;

		/* From the reorder buffer */
		//		ml_in_flush inFlush;
		inport<bool, nConfig > inFlush;
		outport<bool, nConfig > outFlush;

		/** The register file constructor
			@param name the module name
		*/
		RegisterFile(const char *name) : module(name)
		{
			int i, j;

			class_name = " RegisterFileClass";
			// Unisim port names ...
			//for (i=0; i<WriteBackWidth; i++)
			//  {
			inWriteBackInstruction.set_unisim_name(this,"inWriteBackInstruction");
			//  }		
			//for (i=0; i<IntegerReadRegisterWidth; i++)
			//  {
			inIntegerInstruction.set_unisim_name(this,"inIntegerInstruction");
			//  }		
			//for (i=0; i<FloatingPointReadRegisterWidth; i++)
			//  {
			inFloatingPointInstruction.set_unisim_name(this,"inFloatingPointInstruction");
			//  }		
			//for (i=0; i<LoadStoreReadRegisterWidth; i++)
			//  {
			inLoadStoreInstruction.set_unisim_name(this,"inLoadStoreInstruction");
			//  }		
			//for (i=0; i<IntegerReadRegisterWidth; i++)
			//  {
			outIntegerInstruction.set_unisim_name(this,"outIntegerInstruction");
			//  }		
			//for (i=0; i<FloatingPointReadRegisterWidth; i++)
			//  {
			outFloatingPointInstruction.set_unisim_name(this,"outFloatingPointInstruction");
			//  }		
			//for (i=0; i<LoadStoreReadRegisterWidth; i++)
			//  {
			outLoadStoreInstruction.set_unisim_name(this,"outLoadStoreInstruction");
			//  }		
			//for (i=0; i<WriteBackWidth; i++)
			//  {
			outWriteBackReceived.set_unisim_name(this,"outWriteBackReceived");
			//  }		
			inFlush.set_unisim_name(this,"inFlush");
			outFlush.set_unisim_name(this,"outFlush");

			sensitive_pos_method(start_of_cycle) << inClock;
			sensitive_neg_method(end_of_cycle) << inClock;


			// on WB DATA
			//			for(i = 0; i < WriteBackWidth; i++)
			{ sensitive_method(onWBData) << inWriteBackInstruction.data; }
			// on WB ENABLE
			//			for(i = 0; i < WriteBackWidth; i++)
			{ sensitive_method(onWBEnable) << inWriteBackInstruction.enable; }
			
			//			SC_METHOD(ExternalControlIntegerPipe);
			//			sensitive << inStateChangedIntegerPipe;
			// on DATA
			/*
			for(i = 0; i < IntegerReadRegisterWidth; i++)
				{ sensitive_method(onIntegerData) << inIntegerInstruction.data; }
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
				{ sensitive_method(onFloatingData) << inFloatingPointInstruction.data; }
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
				{ sensitive_method(onLoadStoreData) << inLoadStoreInstruction.data; }
			*/
			// on ACCEPT
			/*
			for(i = 0; i < IntegerReadRegisterWidth; i++)
			  { 
			    sensitive_method(onIntegerAccept) << outIntegerInstruction.accept;
			    sensitive_method(onIntegerAccept) << inIntegerInstruction.data; 
			  }
			for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			  {
			    sensitive_method(onFloatingAccept) << outFloatingPointInstruction.accept;
			    sensitive_method(onFloatingAccept) << inFloatingPointInstruction.data;
			  }
			for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			  { 
			    sensitive_method(onLoadStoreAccept) << outLoadStoreInstruction.accept;
			    sensitive_method(onLoadStoreAccept) << inLoadStoreInstruction.data;
			  }
			*/
			// on inDATA and outACCEPT
			//for(i = 0; i < IntegerReadRegisterWidth; i++)
			//  {
			sensitive_method(onInteger) << inIntegerInstruction.data;
			sensitive_method(onInteger) << outIntegerInstruction.accept;
			//  }
			//for(i = 0; i < FloatingPointReadRegisterWidth; i++)
			//  {
			sensitive_method(onFloatingPoint) << inFloatingPointInstruction.data;
			sensitive_method(onFloatingPoint) << outFloatingPointInstruction.accept;
			//  }
			//for(i = 0; i < LoadStoreReadRegisterWidth; i++)
			//  {
			sensitive_method(onLoadStore) << inLoadStoreInstruction.data;
			sensitive_method(onLoadStore) << outLoadStoreInstruction.accept;
			//  }


			//for(i = 0; i < WriteBackWidth; i++)
			{ sensitive_method(onWriteBackReceivedAccept) << outWriteBackReceived.accept; }

			sensitive_method(onFlushData) << inFlush.data;
			sensitive_method(onFlushAccept) << outFlush.accept;
			sensitive_method(onFlushEnable) << inFlush.enable;


			for(int cfg=0; cfg<nConfig; cfg++)
			  {
			    memset(integerValid[cfg], true, sizeof(integerValid[cfg]));
			    memset(floatingPointValid[cfg], true, sizeof(floatingPointValid[cfg]));
			    memset(conditionValid[cfg], true, sizeof(conditionValid[cfg]));
			    memset(FPSCRValid[cfg], true, sizeof(FPSCRValid[cfg]));
			    memset(linkValid[cfg], true, sizeof(linkValid[cfg]));
			    memset(countValid[cfg], true, sizeof(countValid[cfg]));
			    memset(XERValid[cfg], true, sizeof(XERValid[cfg]));

			    WriteBacksReceived[cfg] = false;
			    integer_read_register_to_add[cfg] = 0;
			    floating_point_read_register_to_add[cfg] = 0;
			    load_store_read_register_to_add[cfg] = 0;

			  }
			integer_data_received = false;
			floating_point_data_received = false;
			load_store_data_received = false;

		}

	void onFlushData()
	  {
	    for(int cfg=0; cfg<nConfig; cfg++)
	      {
		//    inFlush.accept = inFlush.data.something();
		if ( inFlush.data[cfg].something() )
		  {
		    outFlush.data[cfg] = inFlush.data[cfg];
		  }
		else
		  {
		    outFlush.data[cfg].nothing();
		  }
	      }
	    outFlush.data.send();
	  }
	void onFlushAccept()
	  {
	    for(int cfg=0; cfg<nConfig; cfg++)
	      {
		inFlush.accept[cfg] = outFlush.accept[cfg];
	      }
	    inFlush.accept.send();
	  }
	void onFlushEnable()
	  {
	    for(int cfg=0; cfg<nConfig; cfg++)
	      {
		outFlush.enable[cfg] = inFlush.enable[cfg];
	      }
	    outFlush.enable.send();
	  }




  void onWBData()
  {
    /*
    bool areallknown(true);
    int i;
    for(i = 0; i < WriteBackWidth; i++)
      {
	if (!inWriteBackInstruction[i].data.known())
	  {
	    areallknown =false; break;
	  }
      }
    */
    //    if (areallknown)
    if ( inWriteBackInstruction.data.known() )
      {
	for(int cfg=0; cfg<nConfig; cfg++)
	{
	  for(int i = 0; i < WriteBackWidth; i++)
	  {
	    inWriteBackInstruction.accept[cfg*WriteBackWidth+i]
	      = inWriteBackInstruction.data[cfg*WriteBackWidth+i].something();
	  }
	}
	inWriteBackInstruction.accept.send();
      }
  }

  void onWBEnable()
  {
    /*
		    bool areallknown(true);
		    int i;
		    for(i = 0; i < WriteBackWidth; i++)
		      {
			if (!inWriteBackInstruction[i].enable.known())
			  {
			    areallknown =false; break;
			  }
		      }
    */
    //		    if (areallknown)
    if ( inWriteBackInstruction.enable.known() )
      {
	for (int cfg=0; cfg<nConfig; cfg++)
	  {
			int integerReadRegisterPort;
			int floatingPointReadRegisterPort;
			int loadStoreReadRegisterPort;

			WriteBacksReceived[cfg] = true;
			//////////////////////////////////////
			// We have to send all data ... 
			//////////////////////////////////////
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, IntegerReadRegisterWidth * nReadRegisterStages> integerReadRegisterPipelineEntry;

			/* For each instructions which have reach the end of the read pipeline */
			for(integerReadRegisterPort = 0, 
			      integerReadRegisterPipelineEntry = integerReadRegisterPipeline[cfg].SeekAtHead();
			    integerReadRegisterPort < IntegerReadRegisterWidth && integerReadRegisterPipelineEntry;
			    integerReadRegisterPipelineEntry++, integerReadRegisterPort++)
			{
				if(integerReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = integerReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(integerReadRegisterPipelineEntry, &instruction, cfg);
				/* request for a launch of the instruction */
				//				outIntegerValid[integerReadRegisterPort] = true;
				//  outIntegerInstruction[integerReadRegisterPort].data = instruction;
				outIntegerInstruction.data[cfg*IntegerReadRegisterWidth+integerReadRegisterPort]
				  = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
			  //	outIntegerValid[integerReadRegisterPort] = false;
				outIntegerInstruction.data[cfg*IntegerReadRegisterWidth+integerReadRegisterPort].nothing();
			}
			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, FloatingPointReadRegisterWidth * nReadRegisterStages> floatingPointReadRegisterPipelineEntry;

			/* For each instructions which have reach the end of the read pipeline */
			for(floatingPointReadRegisterPort = 0,
			      floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline[cfg].SeekAtHead();
			    floatingPointReadRegisterPort < FloatingPointReadRegisterWidth && floatingPointReadRegisterPipelineEntry;
			    floatingPointReadRegisterPipelineEntry++, floatingPointReadRegisterPort++)
			{
				if(floatingPointReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = floatingPointReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(floatingPointReadRegisterPipelineEntry, &instruction, cfg);
				
				/* request for a launch of the instruction */
				//				outFloatingPointValid[floatingPointReadRegisterPort] = true;
				outFloatingPointInstruction.data[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort]
				  = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
			  //				outFloatingPointValid[floatingPointReadRegisterPort] = false;
				outFloatingPointInstruction.data[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort].nothing();
			}

			QueuePointer<ReadRegisterPipelineEntry<T, nSources>, LoadStoreReadRegisterWidth * nReadRegisterStages> loadStoreReadRegisterPipelineEntry;
			/* For each instructions which have reach the end of the read pipeline */
			for(loadStoreReadRegisterPort = 0, loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline[cfg].SeekAtHead(); loadStoreReadRegisterPort < LoadStoreReadRegisterWidth && loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++, loadStoreReadRegisterPort++)
			{
				if(loadStoreReadRegisterPipelineEntry->delay > 0) break;
				/* Get the instruction in the pipeline entry */
				InstructionPtr instruction = loadStoreReadRegisterPipelineEntry->instruction;
				/* Bypass results being written back */
				/* If instruction is not ready, don't launch it and stop */
				instruction->must_reschedule = !ByPass(loadStoreReadRegisterPipelineEntry, &instruction, cfg);
				/* request for a launch of the instruction */
				//				outLoadStoreValid[loadStoreReadRegisterPort] = true;
				outLoadStoreInstruction.data[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort] = instruction;
			}
			/* Don't launch any instruction on the remaining ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
			  //				outLoadStoreValid[loadStoreReadRegisterPort] = false;
				outLoadStoreInstruction.data[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort].nothing();
			}
	  }// End of foreach Config.
	outIntegerInstruction.data.send();
	outFloatingPointInstruction.data.send();
	outLoadStoreInstruction.data.send();
      }// end of if(areallknown)...
		  
}


		void onInteger()
		{
		  /*
		  bool areallknown(true);
		  for(int i = 0; i < IntegerReadRegisterWidth; i++)
		    {
		      if (!inIntegerInstruction[i].data.known() || !outIntegerInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  */
		  //		  if (areallknown)
		  if ( inIntegerInstruction.data.known() && outIntegerInstruction.accept.known() )
		  {
		    for(int cfg=0; cfg<nConfig; cfg++)
		    {
			int integerReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				if(!inIntegerInstruction.data[cfg*IntegerReadRegisterWidth+integerReadRegisterPort].something()) break;
				integer_read_register_to_add[cfg]++;
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
				if(!outIntegerInstruction.accept[cfg*IntegerReadRegisterWidth+integerReadRegisterPort]) break;
				outIntegerInstruction.enable[cfg*IntegerReadRegisterWidth+integerReadRegisterPort] = true;
				/* Count accepted instruction as being removed */
				integer_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				outIntegerInstruction.enable[cfg*IntegerReadRegisterWidth+integerReadRegisterPort] = false;
			}

			/* Get the free space into the read pipeline */
			int integer_read_register_free_space = integerReadRegisterPipeline[cfg].FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int integer_read_register_to_accept = MIN(integer_read_register_free_space + integer_read_register_to_remove, integer_read_register_to_add[cfg]);

			/* Accepts instructions according to the number of instructions to accept */
			for(integerReadRegisterPort = 0; integerReadRegisterPort < integer_read_register_to_accept; integerReadRegisterPort++)
			{
				inIntegerInstruction.accept[cfg*IntegerReadRegisterWidth+integerReadRegisterPort] = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; integerReadRegisterPort < IntegerReadRegisterWidth; integerReadRegisterPort++)
			{
				inIntegerInstruction.accept[cfg*IntegerReadRegisterWidth+integerReadRegisterPort] = false;
			}
			// end Accept part
		    }//Endof foreach Config.
		    outIntegerInstruction.enable.send();
		    inIntegerInstruction.accept.send();
		  }//Endof areallknown
		}


		void onFloatingPoint()
		{
		  /*
		  bool areallknown(true);
		  for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
		    {
		      if (!inFloatingPointInstruction[i].data.known() || !outFloatingPointInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  */
		  //		  if (areallknown)
		  if ( inFloatingPointInstruction.data.known() && outFloatingPointInstruction.accept.known())
		  {
		    for (int cfg=0; cfg<nConfig; cfg++)
		    {
			int floatingPointReadRegisterPort;

			/* Count the number of instruction to add into the read pipeline */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				if(!inFloatingPointInstruction.data[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort].something()) break;
				/* Count accepted instruction as being removed */
				floating_point_read_register_to_add[cfg]++;
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
				if(!outFloatingPointInstruction.accept[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort]) break;
				outFloatingPointInstruction.enable[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort] = true;
				floating_point_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				outFloatingPointInstruction.enable[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort] = false;
			}

			/* Get the free space into the read pipeline */
			int floating_point_read_register_free_space = floatingPointReadRegisterPipeline[cfg].FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int floating_point_read_register_to_accept = MIN(floating_point_read_register_free_space + floating_point_read_register_to_remove, floating_point_read_register_to_add[cfg]);

			/* Accepts instructions according to the number of instructions to accept */
			for(floatingPointReadRegisterPort = 0; floatingPointReadRegisterPort < floating_point_read_register_to_accept; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction.accept[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort] = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; floatingPointReadRegisterPort < FloatingPointReadRegisterWidth; floatingPointReadRegisterPort++)
			{
				inFloatingPointInstruction.accept[cfg*FloatingPointReadRegisterWidth+floatingPointReadRegisterPort] = false;
			}
			// End of Accept part
		    }//Endof foreach Config.
		    outFloatingPointInstruction.enable.send();
		    inFloatingPointInstruction.accept.send();
		  }//End of areallknown
		}

		void onLoadStore()
		{
		  /*
		  bool areallknown(true);
		  for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
		    {
		      if (!inLoadStoreInstruction[i].data.known() || !outLoadStoreInstruction[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  */
		  //		  if (areallknown)
		  if ( inLoadStoreInstruction.data.known() && outLoadStoreInstruction.accept.known() )
		  {
		    for (int cfg=0; cfg<nConfig; cfg++)
		    {
			/* Count the number of instruction to add into the read pipeline */
			for(int loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				if(!inLoadStoreInstruction.data[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort].something()) break;
				load_store_read_register_to_add[cfg]++;
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
				if(!outLoadStoreInstruction.accept[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort]) break;
				outLoadStoreInstruction.enable[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort] = true;
				/* Count accepted instruction as being removed */
				load_store_read_register_to_remove++;
			}
			/* Don't enable any instruction on the remaing read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				outLoadStoreInstruction.enable[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort] = false;
			}

			/* Get the free space into the read pipeline */
			int load_store_read_register_free_space = loadStoreReadRegisterPipeline[cfg].FreeSpace();

			/* Compute the number of instructions to accept in the read pipeline, i.e., the minimum of the number of instructions to add and
			the sum of free space into the pipeline and the number of instructions to remove */
			int load_store_read_register_to_accept = MIN(load_store_read_register_free_space + load_store_read_register_to_remove, load_store_read_register_to_add[cfg]);

			/* Accepts instructions according to the number of instructions to accept */
			for(loadStoreReadRegisterPort = 0; loadStoreReadRegisterPort < load_store_read_register_to_accept; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction.accept[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort] = true;
			}
			/* Reject instructions on the remaining read ports */
			for(; loadStoreReadRegisterPort < LoadStoreReadRegisterWidth; loadStoreReadRegisterPort++)
			{
				inLoadStoreInstruction.accept[cfg*LoadStoreReadRegisterWidth+loadStoreReadRegisterPort] = false;
			}
			// End of Accept Part
		    }//Endof foreach Config.
		    outLoadStoreInstruction.enable.send();
		    inLoadStoreInstruction.accept.send();
		  }// Endof areallknown
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
		void ByPass(int cfg)
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
			for(integerReadRegisterPipelineEntry = integerReadRegisterPipeline[cfg].SeekAtHead(); integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++)
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
				  for(writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
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
			for(floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline[cfg].SeekAtHead(); floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++)
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
				for(writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
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
			for(loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline[cfg].SeekAtHead(); loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++)
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
				for(writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
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
		bool ByPass(ReadRegisterPipelineEntry<T, nSources> *entry, InstructionPtr *instruction, int cfg)
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
			  if(inWriteBackInstruction.enable[cfg*WriteBackWidth+j])
			    {
			      /* Get the instruction */
			      InstructionPtr writeBackInstruction = inWriteBackInstruction.data[cfg*WriteBackWidth+j];
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
		  //int i;
		  
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
		  for(int cfg=0; cfg<nConfig; cfg++)
		  {
			/* Flush ? */
			if(inFlush.enable[cfg] && inFlush.data[cfg].something())
			{
			  if(inFlush.data[cfg])
			  {
				/* Flush the write back pipeline */
				writeBackPipeline[cfg].Flush();

				/* Flush the integer read pipeline */
				integerReadRegisterPipeline[cfg].Flush();

				/* Flush the floating point read pipeline */
				floatingPointReadRegisterPipeline[cfg].Flush();

				/* Flush the load/store read pipeline */
				loadStoreReadRegisterPipeline[cfg].Flush();

				/* Reset the valid bits */
				/*
				memset(integerValid, true, sizeof(integerValid));
				memset(floatingPointValid, true, sizeof(floatingPointValid));
				memset(conditionValid, true, sizeof(conditionValid));
				memset(FPSCRValid, true, sizeof(FPSCRValid));
				memset(linkValid, true, sizeof(linkValid));
				memset(countValid, true, sizeof(countValid));
				memset(XERValid, true, sizeof(XERValid));
				*/
				memset(integerValid[cfg], true, sizeof(integerValid[cfg]));
				memset(floatingPointValid[cfg], true, sizeof(floatingPointValid[cfg]));
				memset(conditionValid[cfg], true, sizeof(conditionValid[cfg]));
				memset(FPSCRValid[cfg], true, sizeof(FPSCRValid[cfg]));
				memset(linkValid[cfg], true, sizeof(linkValid[cfg]));
				memset(countValid[cfg], true, sizeof(countValid[cfg]));
				memset(XERValid[cfg], true, sizeof(XERValid[cfg]));
				//return;
				continue;
			  }
			}//endof Flush...

			/* Remove launched integer instructions from the integer read pipeline */
			for(int i = 0; i < IntegerReadRegisterWidth; i++)
			{
				if(!outIntegerInstruction.accept[cfg*IntegerReadRegisterWidth+i]) break;
				integerReadRegisterPipeline[cfg].RemoveHead();
			}

			/* Remove launched floating point instructions from the floating point read pipeline */
			for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
			{
				if(!outFloatingPointInstruction.accept[cfg*FloatingPointReadRegisterWidth+i]) break;
				floatingPointReadRegisterPipeline[cfg].RemoveHead();
			}

			/* Remove launched load/store instructions from the load/store read pipeline */
			for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
			{
				if(!outLoadStoreInstruction.accept[cfg*LoadStoreReadRegisterWidth+i]) break;
				loadStoreReadRegisterPipeline[cfg].RemoveHead();
			}


			int writeBackPort;
			QueuePointer<WriteBackPipelineEntry<T, nSources>, WriteBackWidth * nWriteBackStages> writeBackPipelineEntry;

			/* Remove instructions  which finished to write back their results from the write back pipeline */
			for(writeBackPort = 0, writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPort < WriteBackWidth && writeBackPipelineEntry; writeBackPipelineEntry++, writeBackPort++)
			{
				if(writeBackPipelineEntry->delay > 0) break;
				if (outWriteBackReceived.accept[cfg*WriteBackWidth+writeBackPort])
				  {
				    WriteBack(writeBackPipelineEntry->instruction, cfg);
				    writeBackPipeline[cfg].RemoveHead();
				  }
				//				changed = true;
			}

			/* For each integer read register port */
			for(int i = 0; i < IntegerReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				//if(!inIntegerEnable[i]) break;
				if(!inIntegerInstruction.enable[cfg*IntegerReadRegisterWidth+i]) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = integerReadRegisterPipeline[cfg].New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * IntegerReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inIntegerInstruction.data[cfg*IntegerReadRegisterWidth+i]);
					/* Read the source operand of the instruction from the register file */
					Read(entry, cfg);
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
			for(int i = 0; i < FloatingPointReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				if(!inFloatingPointInstruction.enable[cfg*FloatingPointReadRegisterWidth+i]) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = floatingPointReadRegisterPipeline[cfg].New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * FloatingPointReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inFloatingPointInstruction.data[cfg*FloatingPointReadRegisterWidth+i]);
					/* Read the source operand of the instruction from the register file */
					Read(entry, cfg);
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
			for(int i = 0; i < LoadStoreReadRegisterWidth; i++)
			{
				/* Is there an instruction on the read port ? */
				if(!inLoadStoreInstruction.enable[cfg*LoadStoreReadRegisterWidth+i]) break;
				/* Allocate a pipeline entry */
				ReadRegisterPipelineEntry<T, nSources> *entry = loadStoreReadRegisterPipeline[cfg].New();

				if(entry)
				{
					/* Set the processing delay to the number of pipe stage */
					entry->delay = nReadRegisterStages * LoadStoreReadRegisterWidth;
					/* Records the instruction */
					entry->Initialize(inLoadStoreInstruction.data[cfg*LoadStoreReadRegisterWidth+i]);
					/* Read the source operand of the instruction from the register file */
					Read(entry, cfg);

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
			for(int i = 0; i < WriteBackWidth; i++)
			{
				/* Is there an instruction on the write back port ? */
				//if(!inWriteBackInstruction[i].enable) break;
			  if (inWriteBackInstruction.enable[cfg*WriteBackWidth+i])
			    {
				/* Get the instruction */
				const InstructionPtr writeBackInstruction = inWriteBackInstruction.data[cfg*WriteBackWidth+i];

				/* Allocate a write back pipeline entry */
				WriteBackPipelineEntry<T, nSources> *entry = writeBackPipeline[cfg].New();

				if(entry)
				{
					/* Set the processing delay to number of pipeline stage */
					entry->delay = nWriteBackStages * WriteBackWidth;
					/* Records the instruction */
					entry->instruction = writeBackInstruction;
					//changed = true;
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
			for(integerReadRegisterPipelineEntry = integerReadRegisterPipeline[cfg].SeekAtHead(); integerReadRegisterPipelineEntry; integerReadRegisterPipelineEntry++)
			{
				if(integerReadRegisterPipelineEntry->delay > 0)
				{
				  //integerPipeChanged = true;
					integerReadRegisterPipelineEntry->delay = (integerReadRegisterPipelineEntry->delay >= IntegerReadRegisterWidth) ? integerReadRegisterPipelineEntry->delay - IntegerReadRegisterWidth : 0;
				}
			}
			/* Decrement the processing delay in each floating point read register pipeline entry */
			for(floatingPointReadRegisterPipelineEntry = floatingPointReadRegisterPipeline[cfg].SeekAtHead(); floatingPointReadRegisterPipelineEntry; floatingPointReadRegisterPipelineEntry++)
			{
				if(floatingPointReadRegisterPipelineEntry->delay > 0)
				{
				  //floatingPointPipeChanged = true;
					floatingPointReadRegisterPipelineEntry->delay = (floatingPointReadRegisterPipelineEntry->delay >= FloatingPointReadRegisterWidth) ? floatingPointReadRegisterPipelineEntry->delay - FloatingPointReadRegisterWidth : 0;
				}
			}
			/* Decrement the processing delay in each load/store read register pipeline entry */
			for(loadStoreReadRegisterPipelineEntry = loadStoreReadRegisterPipeline[cfg].SeekAtHead(); loadStoreReadRegisterPipelineEntry; loadStoreReadRegisterPipelineEntry++)
			{
				if(loadStoreReadRegisterPipelineEntry->delay > 0)
				{
				  //loadStorePipeChanged = true;
					loadStoreReadRegisterPipelineEntry->delay = (loadStoreReadRegisterPipelineEntry->delay >= LoadStoreReadRegisterWidth) ? loadStoreReadRegisterPipelineEntry->delay - LoadStoreReadRegisterWidth : 0;
				}
			}

			/* Decrement the processing delay in each write back pipeline entry */
			for(writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPipelineEntry; writeBackPipelineEntry++)
			{
				if(writeBackPipelineEntry->delay > 0)
				{
				  //changed = true;
				  //writeBackPipelineEntry->delay--;
				  writeBackPipelineEntry->delay = (writeBackPipelineEntry->delay >= WriteBackWidth) ? writeBackPipelineEntry->delay - WriteBackWidth : 0;
				}
			}

			/* Bypass the result from the write back pipeline to the read register pipelines */
			ByPass(cfg);

#ifdef DD_DEBUG_PIPELINE_VERB1
		cerr << "["<<this->name()<<"("<<timestamp()<<")] ==== EOC ==== Pipeline Debug" << endl;
		cerr << this << endl;
#endif

		  }// Endof foreach Config.

		} // End of end_of_cycle...
		
		///////////////////////////////////
		// START OF CYCLE !!! YES !!! WARNING : NO
		// We don't have to send something at start_of_cycle ...
		// we have to wait that writeback complete (writeBack.enable)
		// to send instruction with Bypassed sources ...
		///////////////////////////////////
		void start_of_cycle()
		{
		  for (int cfg=0; cfg<nConfig; cfg++)
		  {
		    // Just reset counters ...
		    WriteBacksReceived[cfg] = false;
		    
		    integer_read_register_to_add[cfg] = 0;
		    floating_point_read_register_to_add[cfg] = 0;
		    load_store_read_register_to_add[cfg] = 0;
		    
		    int writeBackPort;
		    QueuePointer<WriteBackPipelineEntry<T, nSources>, WriteBackWidth * nWriteBackStages> writeBackPipelineEntry;
		    
		    // Sending written back instruction from previous cycle ...
		    /* Remove instructions  which finished to write back their results from the write back pipeline */
		    for(writeBackPort = 0, writeBackPipelineEntry = writeBackPipeline[cfg].SeekAtHead(); writeBackPort < WriteBackWidth && writeBackPipelineEntry; writeBackPipelineEntry++, writeBackPort++)
		      {
			if(writeBackPipelineEntry->delay > 0) break;
			outWriteBackReceived.data[cfg*WriteBackWidth+writeBackPort] = writeBackPipelineEntry->instruction;
		      }
		    while(writeBackPort<WriteBackWidth)
		      {
			outWriteBackReceived.data[cfg*WriteBackWidth+writeBackPort].nothing();
			writeBackPort++;
		      }
		    
		  }//Endof foreach Config.

		  outWriteBackReceived.data.send();

		  integer_data_received = false;
		  floating_point_data_received = false;
		  load_store_data_received = false;
		    
		}// end of start_of_cycle

                void onWriteBackReceivedAccept()
                {
		  /*
		  bool areallknown(true);
		  for(int i = 0; i < WriteBackWidth; i++)
		    {
		      if (!outWriteBackReceived[i].accept.known())
			{
			  areallknown =false; break;
			}
		    }
		  */
		  //		  if (areallknown)
		  if (outWriteBackReceived.accept.known())
		  {
		    for(int cfg=0; cfg<nConfig; cfg++)
		    {
		      for(int i = 0; i < WriteBackWidth; i++)
			{
			  /*
			  if (outWriteBackReceived[i].accept)
			    {
			      outWriteBackReceived[i].enable = true;
			    }
			  else
			    {
			      outWriteBackReceived[i].enable = false;
			    }
			  */
			  outWriteBackReceived.enable[cfg*WriteBackWidth+i]
			    = outWriteBackReceived.accept[cfg*WriteBackWidth+i];
			}
		    }//Endof foreach Config 
		    outWriteBackReceived.enable.send();
		  }//Endof areallknown
		}

		/** Reads an integer register (general purpose register)
			@param tag the register number
			@return the register value
		*/
		T ReadGPR(int tag, int cfg)
		{
			return integerRegisters[cfg].Read(tag);
		}

		/** Writes into an integer register (general purpose register)
			@param tag the register number
			@param value the value to write
		*/
		void WriteGPR(int tag, T value, int cfg)
		{
			integerRegisters[cfg].Write(tag, value);
		}

		/** Reads a floating point register
			@param tag the register number
			@return the register value
		*/
		T ReadFPR(int tag, int cfg)
		{
			return floatingPointRegisters[cfg].Read(tag);
		}

		/** Writes into a floating point register
			@param tag the register number
			@param value the value to write
		*/
		void WriteFPR(int tag, T value, int cfg)
		{
			floatingPointRegisters[cfg].Write(tag, value);
		}

		uint32_t ReadCR(int tag, int cfg)
		{
			return conditionRegisters[cfg].Read(tag);
		}
		void WriteCR(int tag, T value, int cfg)
		{
			conditionRegisters[cfg].Write(tag, value);
		}


		uint32_t ReadFPSCR(int tag, int cfg)
		{
			return FPSCRegisters[cfg].Read(tag);
		}
		void WriteFPSCR(int tag, T value, int cfg)
		{
			FPSCRegisters[cfg].Write(tag, value);
		}

		uint32_t ReadLR(int tag, int cfg)
		{
			return linkRegisters[cfg].Read(tag);
		}
		void WriteLR(int tag, T value, int cfg)
		{
			linkRegisters[cfg].Write(tag, value);
		}


		uint32_t ReadCTR(int tag, int cfg)
		{
			return countRegisters[cfg].Read(tag);
		}
		void WriteCTR(int tag, T value, int cfg)
		{
			countRegisters[cfg].Write(tag, value);
		}


		uint32_t ReadXER(int tag, int cfg)
		{
			return XERRegisters[cfg].Read(tag);
		}
		void WriteXER(int tag, T value, int cfg)
		{
			XERRegisters[cfg].Write(tag, value);
		}



  

		/** Reads the source operands of an instruction into the register files
			@param instruction an instruction
		*/
		void Read(ReadRegisterPipelineEntry<T, nSources> *entry, int cfg)
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
				      if(integerValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = integerRegisters[cfg].Read((*instruction)->sources[i].tag);
					}
				      break;
				      
				    case FPR_T:
				      /* If register is valid */
				      if(floatingPointValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = floatingPointRegisters[cfg].Read((*instruction)->sources[i].tag);
					}
				      break;
				    case CR_T:
				      /* If register is valid */
				      if(conditionValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = conditionRegisters[cfg].Read((*instruction)->sources[i].tag);
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
				      if(FPSCRValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = FPSCRegisters[cfg].Read((*instruction)->sources[i].tag);
					}
				      break;
				    case LR_T:
				      /* If register is valid */
				      if(linkValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = linkRegisters[cfg].Read((*instruction)->sources[i].tag);
					}
				      break;
				    case CTR_T:
				      /* If register is valid */
				      if(countValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = countRegisters[cfg].Read((*instruction)->sources[i].tag);
					}
				      break;
				    case XER_T:
				      /* If register is valid */
				      if(XERValid[cfg][(*instruction)->sources[i].tag])
					{
					  /* then read the register value */
					  (*instruction)->sources[i].valid = true;
					  (*instruction)->sources[i].ready = true;
					  (*instruction)->sources[i].data = XERRegisters[cfg].Read((*instruction)->sources[i].tag);
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
						integerValid[cfg][entry->instruction->destinations[j].tag] = false;
						break;

					case FPR_T:
						floatingPointValid[cfg][entry->instruction->destinations[j].tag] = false;
						break;

					case CR_T:
						conditionValid[cfg][entry->instruction->destinations[j].tag] = false;
						break;
					case FPSCR_T:
						FPSCRValid[cfg][entry->instruction->destinations[j].tag] = false;
							break;
					case LR_T:
						linkValid[cfg][entry->instruction->destinations[j].tag] = false;
							break;
					case CTR_T:
						countValid[cfg][entry->instruction->destinations[j].tag] = false;
							break;
					case XER_T:
						XERValid[cfg][entry->instruction->destinations[j].tag] = false;
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
		void WriteBack(InstructionPtr instruction, int cfg)
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
						integerRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;

					case FPR_T:
						/* Write back the value into the floating point register file */
						floatingPointRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;
					case CR_T:
						/* Write back the value into the condition register file */
						conditionRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;
					case FPSCR_T:
						/* Write back the value into the FPSC register file */
						FPSCRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;
					case LR_T:
						/* Write back the value into the link register file */
						linkRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;
					case CTR_T:
						/* Write back the value into the count register file */
						countRegisters[cfg].Write(tag, instruction->destinations[j].data);
						break;
					case XER_T:
						/* Write back the value into the XER register file */
						XERRegisters[cfg].Write(tag, instruction->destinations[j].data);
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
						integerValid[cfg][tag] = true;
						break;

					case FPR_T:
						floatingPointValid[cfg][tag] = true;
						break;
					case CR_T:
						conditionValid[cfg][tag] = true;
							break;
					case FPSCR_T:
						FPSCRValid[cfg][tag] = true;
							break;
					case LR_T:
						linkValid[cfg][tag] = true;
							break;
					case CTR_T:
						countValid[cfg][tag] = true;
							break;
					case XER_T:
						XERValid[cfg][tag] = true;
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
		  for (int cfg=0; cfg<nConfig; cfg++)
		    {
			os << "============= [Config::"<<cfg<<"] REGISTERFILE ============" << endl;
			os << "Integer Pipe:" << endl;
			os << registerFile.integerReadRegisterPipeline[cfg];
			os << "Floating Point Pipe:" << endl;
			os << registerFile.floatingPointReadRegisterPipeline[cfg];
			os << "Load/Store Pipe:" << endl;
			os << registerFile.loadStoreReadRegisterPipeline[cfg];
			os << "WriteBack:" << endl;
			os << registerFile.writeBackPipeline[cfg];
			//	os << "Integer Registers:" << endl;
			//	os << registerFile.integerRegisters;
			//	os << "Floating Point Registers:" << endl;
			//	os << registerFile.floatingPointRegisters;
			
			os << "Integer RegisterFile:" << endl;
			os << registerFile.integerRegisters[cfg] << endl;
			/*
			os << "Floating Point RegisterFile:" << endl;
			os << registerFile.floatingPointRegisters << endl;
			*/
			os << "Conditon RegisterFile:" << endl;
			os << registerFile.conditionRegisters[cfg] << endl;
			/*
			os << "FPSC RegisterFile:" << endl;
			os << registerFile.FPSCRegisters << endl;
			*/
			os << "Link RegisterFile:" << endl;
			os << registerFile.linkRegisters[cfg] << endl;
			os << "Count RegisterFile:" << endl;
			os << registerFile.countRegisters[cfg] << endl;
			os << "XER RegisterFile:" << endl;
			os << registerFile.XERRegisters[cfg] << endl;
			os << "=======================================" << endl;
		    }
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
		Queue<WriteBackPipelineEntry<T, nSources>, nWriteBackStages * WriteBackWidth> writeBackPipeline[nConfig];

		/* A queue modeling the integer read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * IntegerReadRegisterWidth> integerReadRegisterPipeline[nConfig];

		/* A queue modeling the floating point read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * FloatingPointReadRegisterWidth> floatingPointReadRegisterPipeline[nConfig];

		/* A queue modeling the load/store read register pipe */
		Queue<ReadRegisterPipelineEntry<T, nSources>, nReadRegisterStages * LoadStoreReadRegisterWidth> loadStoreReadRegisterPipeline[nConfig];

		/* The integer register file */
		RegisterFileBase<T, nSources, nIntegerRegisters> integerRegisters[nConfig];
		/* Valid bits for each integer registers */
		bool integerValid[nConfig][nIntegerRegisters];

		/* The floating point register file */
		RegisterFileBase<T, nSources, nFloatingPointRegisters> floatingPointRegisters[nConfig];
		/* Valid bits for each floating point registers */
		bool floatingPointValid[nConfig][nIntegerRegisters];

		/* The condition register file */
		RegisterFileBase<T, nSources, nConditionRegisters> conditionRegisters[nConfig];
		/* Valid bits for each condition registers */
		bool conditionValid[nConfig][nConditionRegisters];

		/* The count register file */
		RegisterFileBase<T, nSources, nCountRegisters> countRegisters[nConfig];
		/* Valid bits for each count registers */
		bool countValid[nConfig][nCountRegisters];

		/* The FPSCR register file */
		RegisterFileBase<T, nSources, nFPSCRegisters> FPSCRegisters[nConfig];
		/* Valid bits for each FPSC registers */
		bool FPSCRValid[nConfig][nFPSCRegisters];

		/* The Link register file */
		RegisterFileBase<T, nSources, nLinkRegisters> linkRegisters[nConfig];
		/* Valid bits for each Link registers */
		bool linkValid[nConfig][nLinkRegisters];

		/* The XER register file */
		RegisterFileBase<T, nSources, nXERRegisters> XERRegisters[nConfig];
		/* Valid bits for each XER registers */
		bool XERValid[nConfig][nXERRegisters];


		bool WriteBacksReceived[nConfig];

		int integer_read_register_to_add[nConfig];
		int floating_point_read_register_to_add[nConfig];
		int load_store_read_register_to_add[nConfig];

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
