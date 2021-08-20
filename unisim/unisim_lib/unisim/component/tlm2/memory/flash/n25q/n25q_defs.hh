/*
 *  Copyright (c) 2016,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_DEFS_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_DEFS_HH__

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

enum n25q_protocol_type
{
	N25Q_EXTENDED_PROTOCOL = 1,
	N25Q_DUAL_IO_PROTOCOL  = 2,
	N25Q_QUAD_IO_PROTOCOL  = 4
};

enum n25q_command_type
{
	N25Q_UNKNOWN_COMMAND = 0,
	// IDENTIFICATION Operations                                         
	N25Q_READ_ID_COMMAND,                                                
	N25Q_MULTIPLE_IO_READ_ID_COMMAND,                                    
	N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND,
	// READ Operations
	N25Q_READ_COMMAND,                                                   
	N25Q_FAST_READ_COMMAND,                                              
	N25Q_DUAL_OUTPUT_FAST_READ_COMMAND,                                  
	N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND,                            
	N25Q_QUAD_OUTPUT_FAST_READ_COMMAND,                                  
	N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND,                            
	// WRITE Operations                                               
	N25Q_WRITE_ENABLE_COMMAND,                                           
	N25Q_WRITE_DISABLE_COMMAND,                                          
	// REGISTER Operations
	N25Q_READ_STATUS_REGISTER_COMMAND,                                   
	N25Q_WRITE_STATUS_REGISTER_COMMAND,                                  
	N25Q_READ_LOCK_REGISTER_COMMAND,                                     
	N25Q_WRITE_LOCK_REGISTER_COMMAND,                                    
	N25Q_READ_FLAG_STATUS_REGISTER_COMMAND,                              
	N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND,                             
	N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND,                
	N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND,               
	N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND,                   
	N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND,                  
	N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND,          
	N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND,         
	// PROGRAM Operations
	N25Q_PAGE_PROGRAM_COMMAND,                                           
	N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND,                                
	N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND,                       
	N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND,                                
	N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND,                       
	// ERASE Operations
	N25Q_SUBSECTOR_ERASE_COMMAND,                                        
	N25Q_SECTOR_ERASE_COMMAND,                                           
	N25Q_BULK_ERASE_COMMAND,                                             
	N25Q_PROGRAM_ERASE_RESUME_COMMAND,                                   
	N25Q_PROGRAM_ERASE_SUSPEND_COMMAND,                                  
	// ONE-TIME PROGRAMMABLE (OTP) Operations
	N25Q_READ_OTP_ARRAY_COMMAND,                                         
	N25Q_PROGRAM_OTP_ARRAY_COMMAND                                       
};

enum n25q_signal_type
{
	NO_SIGNAL = 0,
	DQ0 = 1,
	DQ1 = 2,
	DQ1_0 = DQ1 + DQ0,
	DQ2 = 4,
	DQ3 = 8,
	DQ3_0 = DQ3 + DQ2 + DQ1 + DQ0
};

enum n25q_mode_type
{
	N25Q_STD_MODE,
	N25Q_XIP_MODE
};

inline std::ostream& operator << (std::ostream& os, const n25q_protocol_type& n25q_protocol)
{
	switch(n25q_protocol)
	{
		case N25Q_EXTENDED_PROTOCOL: os << "N25Q extended protocol"; break;
		case N25Q_DUAL_IO_PROTOCOL: os << "N25Q dual I/O protocol"; break;
		case N25Q_QUAD_IO_PROTOCOL: os << "N25Q quad I/O protocol"; break;
	}
	return os;
}

inline std::ostream& operator << (std::ostream& os, const n25q_command_type& n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_UNKNOWN_COMMAND: os << "unknown N25Q command"; break;
		case N25Q_READ_ID_COMMAND: os << "N25Q READ ID command"; break;
		case N25Q_MULTIPLE_IO_READ_ID_COMMAND: os << "N25Q MULTIPLE I/O READ ID command"; break;
		case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND: os << "N25Q SERIAL FLASH DISCOVERY PARAMETER command"; break;
		case N25Q_READ_COMMAND: os << "N25Q READ command"; break;
		case N25Q_FAST_READ_COMMAND: os << "N25Q FAST READ command"; break;
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND: os << "N25Q DUAL OUTPUT FAST READ command"; break;
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND: os << "N25Q DUAL INPUT/OUTPUT FAST READ command"; break;
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND: os << "N25Q QUAD OUTPUT FAST READ command"; break;
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND: os << "N25Q QUAD INPUT/OUTPUT FAST READ command"; break;
		case N25Q_WRITE_ENABLE_COMMAND: os << "N25Q WRITE ENABLE command"; break;
		case N25Q_WRITE_DISABLE_COMMAND: os << "N25Q WRITE DISABLE command"; break;
		case N25Q_READ_STATUS_REGISTER_COMMAND: os << "N25Q READ STATUS REGISTER command"; break;
		case N25Q_WRITE_STATUS_REGISTER_COMMAND: os << "N25Q WRITE STATUS REGISTER command"; break;
		case N25Q_READ_LOCK_REGISTER_COMMAND: os << "N25Q READ LOCK REGISTER command"; break;
		case N25Q_WRITE_LOCK_REGISTER_COMMAND: os << "N25Q WRITE LOCK REGISTER command"; break;
		case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND: os << "N25Q READ FLAG STATUS REGISTER command"; break;
		case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND: os << "N25Q CLEAR FLAG STATUS REGISTER command"; break;
		case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q READ NONVOLATILE CONFIGURATION REGISTER command"; break;
		case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q WRITE NONVOLATILE CONFIGURATION REGISTER command"; break;
		case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q READ VOLATILE CONFIGURATION REGISTER command"; break;
		case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q WRITE VOLATION CONFIGURATION REGISTER command"; break;
		case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q READ ENHANCED VOLATILE CONFIGURATION REGISTER command"; break;
		case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: os << "N25Q WRITE ENHANCED VOLATILE CONFIGURATION REGISTER command"; break;
		case N25Q_PAGE_PROGRAM_COMMAND: os << "N25Q PAGE PROGRAM command"; break;
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND: os << "DUAL INPUT FAST PROGRAM command"; break;
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND: os << "N25Q EXTENDED DUAL INPUT FAST PROGRAM command"; break;
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND: os << "N25Q QUAD INPUT FAST PROGRAM command"; break;
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: os << "N25Q EXTENDED QUAD INPUT FAST PROGRAM command"; break;
		case N25Q_SUBSECTOR_ERASE_COMMAND: os << "N25Q SUBSECTOR ERASE command"; break;
		case N25Q_SECTOR_ERASE_COMMAND: os << "N25Q SECTOR ERASE command"; break;
		case N25Q_BULK_ERASE_COMMAND: os << "N25Q BULK ERASE command"; break;
		case N25Q_PROGRAM_ERASE_RESUME_COMMAND: os << "N25Q PROGRAM/ERASE RESUME command"; break;
		case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND: os << "N25Q PROGRAM/ERASE SUSPEND command"; break;
		case N25Q_READ_OTP_ARRAY_COMMAND: os << "N25Q READ OTP ARRAY command"; break;
		case N25Q_PROGRAM_OTP_ARRAY_COMMAND: os << "N25Q PROGRAM OTP ARRAY command"; break;
	}
	
	return os;
}

inline std::ostream& operator << (std::ostream& os, const n25q_signal_type& s)
{
	switch(s)
	{
		case NO_SIGNAL: os << "no signal"; break;
		case DQ0: os << "DQ0"; break;
		case DQ1: os << "DQ1"; break;
		case DQ1_0: os << "DQ[1:0]"; break;
		case DQ2: os << "DQ2"; break;
		case DQ3: os << "DQ3"; break;
		case DQ3_0: os << "DQ[3:0]"; break;
	}
	return os;
}

inline unsigned int n25q_last_signal_of(n25q_signal_type s)
{
	switch(s)
	{
		case NO_SIGNAL: return 0;
		case DQ0: return 0;
		case DQ1: return 1;
		case DQ1_0: return 1;
		case DQ2: return 2;
		case DQ3: return 3;
		case DQ3_0: return 3;
	}
	return 0;
}

inline bool n25q_is_program_command(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_PAGE_PROGRAM_COMMAND:
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			return true;
		default:
			return false;
	}
	return false;
}

inline bool n25q_is_erase_command(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_SUBSECTOR_ERASE_COMMAND:
		case N25Q_SECTOR_ERASE_COMMAND:
		case N25Q_BULK_ERASE_COMMAND:
			return true;
		default:
			return false;
	}
	return false;
}

inline bool n25q_command_is_supported(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol)
{
	switch(n25q_cmd)
	{
		case N25Q_UNKNOWN_COMMAND:
			return false;
		// IDENTIFICATION Operations
		case N25Q_READ_ID_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return false; // unsupported
				case N25Q_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case N25Q_MULTIPLE_IO_READ_ID_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return false; // unsupported
				case N25Q_DUAL_IO_PROTOCOL: return true;
				case N25Q_QUAD_IO_PROTOCOL: return true;
			}
			break;
		case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
			return true;
		// READ Operations
		case N25Q_READ_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return false; // unsupported
				case N25Q_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case N25Q_FAST_READ_COMMAND:
			return true;
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return true;
				case N25Q_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return false; // unsupported
				case N25Q_QUAD_IO_PROTOCOL: return true;
			}
			break;
		// WRITE Operations                                               
		case N25Q_WRITE_ENABLE_COMMAND: return true;
		case N25Q_WRITE_DISABLE_COMMAND: return true;
		// REGISTER Operations
		case N25Q_READ_STATUS_REGISTER_COMMAND: return true;
		case N25Q_WRITE_STATUS_REGISTER_COMMAND: return true;
		case N25Q_READ_LOCK_REGISTER_COMMAND: return true;
		case N25Q_WRITE_LOCK_REGISTER_COMMAND: return true;
		case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND: return true;
		case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return true;
		case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return true;
		// PROGRAM Operations
		case N25Q_PAGE_PROGRAM_COMMAND: return true;
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return true;
				case N25Q_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return true;
				case N25Q_QUAD_IO_PROTOCOL: return false; // unsupported
			}
			break;
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return false; // unsupported
				case N25Q_QUAD_IO_PROTOCOL: return true;
			}
			break;
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL: return true;
				case N25Q_DUAL_IO_PROTOCOL: return false; // unsupported
				case N25Q_QUAD_IO_PROTOCOL: return true;
			}
			break;
		// ERASE Operations
		case N25Q_SUBSECTOR_ERASE_COMMAND: return true;
		case N25Q_SECTOR_ERASE_COMMAND: return true;
		case N25Q_BULK_ERASE_COMMAND: return true;
		case N25Q_PROGRAM_ERASE_RESUME_COMMAND: return true;
		case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND: return true;
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case N25Q_READ_OTP_ARRAY_COMMAND: return true;
		case N25Q_PROGRAM_OTP_ARRAY_COMMAND: return true;
	}
	return false;
}

inline bool n25q_command_has_address(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_UNKNOWN_COMMAND:
			return false;
		// IDENTIFICATION Operations
		case N25Q_READ_ID_COMMAND: return false;
		case N25Q_MULTIPLE_IO_READ_ID_COMMAND: return false;
		case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND: return true;
		// READ Operations
		case N25Q_READ_COMMAND: return true;
		case N25Q_FAST_READ_COMMAND: return true;
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND: return true;
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND: return true;
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND: return true;
		// WRITE Operations
		case N25Q_WRITE_ENABLE_COMMAND: return false;
		case N25Q_WRITE_DISABLE_COMMAND: return false;
		// REGISTER Operations
		case N25Q_READ_STATUS_REGISTER_COMMAND: return false;
		case N25Q_WRITE_STATUS_REGISTER_COMMAND: return false;
		case N25Q_READ_LOCK_REGISTER_COMMAND: return true;
		case N25Q_WRITE_LOCK_REGISTER_COMMAND: return true;
		case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND: return false;
		case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return false;
		case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return false;
		// PROGRAM Operations
		case N25Q_PAGE_PROGRAM_COMMAND: return true;
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND: return true;
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return true;
		// ERASE Operations
		case N25Q_SUBSECTOR_ERASE_COMMAND: return true;
		case N25Q_SECTOR_ERASE_COMMAND: return true;
		case N25Q_BULK_ERASE_COMMAND: return false;
		case N25Q_PROGRAM_ERASE_RESUME_COMMAND: return false;
		case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND: return false;
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case N25Q_READ_OTP_ARRAY_COMMAND: return true;
		case N25Q_PROGRAM_OTP_ARRAY_COMMAND: return true;
	}
	return false;
}

inline bool n25q_command_has_input_data(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_WRITE_STATUS_REGISTER_COMMAND:
		case N25Q_WRITE_LOCK_REGISTER_COMMAND:
		case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case N25Q_PAGE_PROGRAM_COMMAND:
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_PROGRAM_OTP_ARRAY_COMMAND: return true;
		default: return false;
	}
	return false;
}

inline bool n25q_command_has_output_data(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		// IDENTIFICATION Operations
		case N25Q_READ_ID_COMMAND:
		case N25Q_MULTIPLE_IO_READ_ID_COMMAND:
		case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
		// READ Operations
		case N25Q_READ_COMMAND:
		case N25Q_FAST_READ_COMMAND:
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
		// REGISTER Operations
		case N25Q_READ_STATUS_REGISTER_COMMAND:
		case N25Q_READ_LOCK_REGISTER_COMMAND:
		case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND:
		case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
		// ONE-TIME PROGRAMMABLE (OTP) Operations
		case N25Q_READ_OTP_ARRAY_COMMAND: return true;
		default: return false;
	}
	return false;
}

inline bool n25q_command_has_dummy_cycles(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
		case N25Q_FAST_READ_COMMAND:
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
		case N25Q_READ_OTP_ARRAY_COMMAND: return true;
		default: return false;
	}
	return false;
}

inline n25q_signal_type n25q_signal(unsigned int sig_num)
{
	return (n25q_signal_type)(1 << sig_num);
}

inline unsigned int n25q_signal_width(n25q_signal_type s)
{
	switch(s)
	{
		case NO_SIGNAL: return 0;
		case DQ0: return 1;
		case DQ1: return 1;
		case DQ2: return 1;
		case DQ3: return 1;
		case DQ1_0: return 2;
		case DQ3_0: return 4;
	}
	return 0;
}

inline unsigned int n25q_width(n25q_protocol_type n25q_protocol)
{
	return n25q_protocol;
}

inline n25q_signal_type n25q_input_signal(unsigned int w)
{
	switch(w)
	{
		case 1: return DQ0;
		case 2: return DQ1_0;
		case 4: return DQ3_0;
	}
	return NO_SIGNAL;
}

inline n25q_signal_type n25q_output_signal(unsigned int w)
{
	switch(w)
	{
		case 1: return DQ1;
		case 2: return DQ1_0;
		case 4: return DQ3_0;
	}
	return NO_SIGNAL;
}

inline n25q_signal_type n25q_address_signal(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	switch(n25q_mode)
	{
		case N25Q_STD_MODE:
			switch(n25q_cmd)
			{
				case N25Q_UNKNOWN_COMMAND:
					return NO_SIGNAL;
				case N25Q_READ_ID_COMMAND:
				case N25Q_MULTIPLE_IO_READ_ID_COMMAND:
				case N25Q_WRITE_ENABLE_COMMAND:
				case N25Q_WRITE_DISABLE_COMMAND:
				case N25Q_READ_STATUS_REGISTER_COMMAND:
				case N25Q_WRITE_STATUS_REGISTER_COMMAND:
				case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND:
				case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
				case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_BULK_ERASE_COMMAND:
				case N25Q_PROGRAM_ERASE_RESUME_COMMAND:
				case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND:
					return NO_SIGNAL; // N/A
				case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
				case N25Q_PAGE_PROGRAM_COMMAND:
					return DQ0;
				// READ Operations
				case N25Q_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_FAST_READ_COMMAND:
				case N25Q_READ_LOCK_REGISTER_COMMAND:
				case N25Q_WRITE_LOCK_REGISTER_COMMAND:
				case N25Q_SUBSECTOR_ERASE_COMMAND:
				case N25Q_SECTOR_ERASE_COMMAND:
				case N25Q_READ_OTP_ARRAY_COMMAND:
				case N25Q_PROGRAM_OTP_ARRAY_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1_0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ3_0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1_0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ0;
					}
					break;
				case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ3_0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
			}
			break;
			
		case N25Q_XIP_MODE:
			switch(n25q_cmd)
			{
				case N25Q_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1_0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ3_0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				default:
					return NO_SIGNAL; // N/A
			}
			break;
	}
	return NO_SIGNAL; // should never occur
}

inline n25q_signal_type n25q_data_signal(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	switch(n25q_mode)
	{
		case N25Q_STD_MODE:
			switch(n25q_cmd)
			{
				case N25Q_UNKNOWN_COMMAND:
					return NO_SIGNAL;
				// IDENTIFICATION Operations
				case N25Q_READ_ID_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_MULTIPLE_IO_READ_ID_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1;  // just a guess
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0; // just a guess
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0; // just a guess
					}
					break;
				case N25Q_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_FAST_READ_COMMAND:
				case N25Q_READ_STATUS_REGISTER_COMMAND:
				case N25Q_READ_LOCK_REGISTER_COMMAND:
				case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND:
				case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_READ_OTP_ARRAY_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_WRITE_STATUS_REGISTER_COMMAND:
				case N25Q_WRITE_LOCK_REGISTER_COMMAND:
				case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
				case N25Q_PROGRAM_OTP_ARRAY_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1_0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ3_0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
				case N25Q_WRITE_ENABLE_COMMAND:
				case N25Q_WRITE_DISABLE_COMMAND:
				case N25Q_SUBSECTOR_ERASE_COMMAND:
				case N25Q_SECTOR_ERASE_COMMAND:
				case N25Q_BULK_ERASE_COMMAND:
				case N25Q_PROGRAM_ERASE_RESUME_COMMAND:
				case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND:
					return NO_SIGNAL; // N/A
				case N25Q_PAGE_PROGRAM_COMMAND:
					return DQ0;
				case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
					return DQ1_0;
				case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					return DQ3_0;
			}
			break;
			
		case N25Q_XIP_MODE:
			switch(n25q_cmd)
			{
				case N25Q_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ1_0;
						case N25Q_DUAL_IO_PROTOCOL: return DQ1_0;
						case N25Q_QUAD_IO_PROTOCOL: return NO_SIGNAL; // unsupported
					}
					break;
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL: return DQ3_0;
						case N25Q_DUAL_IO_PROTOCOL: return NO_SIGNAL; // unsupported
						case N25Q_QUAD_IO_PROTOCOL: return DQ3_0;
					}
					break;
				default:
					return NO_SIGNAL; // N/A
			}
			break;
	}
	
	return NO_SIGNAL; // should not occur
}

inline unsigned int n25q_command_cycles(n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	if(n25q_mode == N25Q_XIP_MODE) return 0;
	
	unsigned int w = n25q_width(n25q_protocol);
	return 1 * (8 / w);
}

inline unsigned int n25q_address_width(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	return n25q_signal_width(n25q_address_signal(n25q_cmd, n25q_protocol, n25q_mode));
}

inline unsigned int n25q_data_width(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	return n25q_signal_width(n25q_data_signal(n25q_cmd, n25q_protocol, n25q_mode));
}

inline unsigned int n25q_address_cycles(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	unsigned int w = n25q_address_width(n25q_cmd, n25q_protocol, n25q_mode);
	return 3 * (8 / w);
}

inline unsigned int n25q_data_cycles(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode, unsigned int data_bit_length)
{
	unsigned int w = n25q_data_width(n25q_cmd, n25q_protocol, n25q_mode);
	return (data_bit_length + w - 1) / w;
}

inline unsigned int n25q_data_cycles_per_byte(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	unsigned int w = n25q_data_width(n25q_cmd, n25q_protocol, n25q_mode);
	return 1 * (8 / w);
}

inline unsigned int n25q_data_bits_per_cycles(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	return n25q_data_width(n25q_cmd, n25q_protocol, n25q_mode);
}

inline unsigned int n25q_dummy_cycles(n25q_command_type n25q_cmd, n25q_protocol_type n25q_protocol, n25q_mode_type n25q_mode)
{
	switch(n25q_mode)
	{
		case N25Q_STD_MODE:
			switch(n25q_cmd)
			{
				case N25Q_FAST_READ_COMMAND:
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
						return 15;
				case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
				case N25Q_READ_OTP_ARRAY_COMMAND:
					switch(n25q_protocol)
					{
						case N25Q_EXTENDED_PROTOCOL:
						case N25Q_DUAL_IO_PROTOCOL:
							return 8;
						case N25Q_QUAD_IO_PROTOCOL:
							return 10;
					}
					break;
				default:
					return 0;
			}
			break;
		case N25Q_XIP_MODE:
			switch(n25q_cmd)
			{
				case N25Q_FAST_READ_COMMAND:
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					return 15;
				default:
					return 0; // N/A
			}
			break;
	}
	return 0;
}

inline n25q_command_type n25q_decode_command_byte(uint8_t command_byte, n25q_protocol_type n25q_protocol)
{
	switch(command_byte)
	{
		case 0x9e: case 0x9f: return N25Q_READ_ID_COMMAND;
		case 0xaf: return N25Q_MULTIPLE_IO_READ_ID_COMMAND;
		case 0x5a: return N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND;
		case 0x03: return N25Q_READ_COMMAND;
		case 0x0b:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL:
					return N25Q_FAST_READ_COMMAND;
				case N25Q_DUAL_IO_PROTOCOL:
					return N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND;
				case N25Q_QUAD_IO_PROTOCOL:
					return N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND;
			}
			break;
		case 0x3b:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL:
					return N25Q_UNKNOWN_COMMAND; // bad encoding
				case N25Q_DUAL_IO_PROTOCOL:
					return N25Q_DUAL_OUTPUT_FAST_READ_COMMAND;
				case N25Q_QUAD_IO_PROTOCOL:
					return N25Q_UNKNOWN_COMMAND; // bad encoding
			}
			break;
		case 0x6b: return N25Q_QUAD_OUTPUT_FAST_READ_COMMAND;
		case 0xbb: return N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND;
		case 0xeb: return N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND;
		case 0x06: return N25Q_WRITE_ENABLE_COMMAND;
		case 0x04: return N25Q_WRITE_DISABLE_COMMAND;
		case 0x05: return N25Q_READ_STATUS_REGISTER_COMMAND;
		case 0x01: return N25Q_WRITE_STATUS_REGISTER_COMMAND;
		case 0xe8: return N25Q_READ_LOCK_REGISTER_COMMAND;
		case 0xe5: return N25Q_WRITE_LOCK_REGISTER_COMMAND;
		case 0x70: return N25Q_READ_FLAG_STATUS_REGISTER_COMMAND;
		case 0x50: return N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND;
		case 0xb5: return N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0xb1: return N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0x85: return N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0x81: return N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0x65: return N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0x61: return N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND;
		case 0x02:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL:
					return N25Q_PAGE_PROGRAM_COMMAND;
				case N25Q_DUAL_IO_PROTOCOL:
					return N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND;
				case N25Q_QUAD_IO_PROTOCOL:
					return N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND;
			}
			break;
		case 0xa2:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL:
					return N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND;
				case N25Q_DUAL_IO_PROTOCOL:
					return N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND;
				case N25Q_QUAD_IO_PROTOCOL:
					return N25Q_UNKNOWN_COMMAND; // bad encoding
			}
			break;
		case 0xd2: return N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND;
		case 0x32:
			switch(n25q_protocol)
			{
				case N25Q_EXTENDED_PROTOCOL:
					return N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND;
				case N25Q_DUAL_IO_PROTOCOL:
					return N25Q_UNKNOWN_COMMAND; // bad encoding
				case N25Q_QUAD_IO_PROTOCOL:
					return N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND;
			}
			break;
		case 0x12: return N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND;
		case 0x20: return N25Q_SUBSECTOR_ERASE_COMMAND;
		case 0xd8: return N25Q_SECTOR_ERASE_COMMAND;
		case 0xc7: return N25Q_BULK_ERASE_COMMAND;
		case 0x7a: return N25Q_PROGRAM_ERASE_RESUME_COMMAND;
		case 0x75: return N25Q_PROGRAM_ERASE_SUSPEND_COMMAND;
		case 0x4b: return N25Q_READ_OTP_ARRAY_COMMAND;
		case 0x42: return N25Q_PROGRAM_OTP_ARRAY_COMMAND;
	}
	
	return N25Q_UNKNOWN_COMMAND; // bad encoding
}

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_DEFS_HH__
