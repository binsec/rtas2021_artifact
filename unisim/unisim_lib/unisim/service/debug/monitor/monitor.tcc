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
 
#ifndef __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_TCC__
#define __UNISIM_SERVICE_DEBUG_MONITOR_MONITOR_TCC__

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>

extern "C" int __u_start(int argc, char **argv, int (*callback)(void));
extern "C" int __u_set_breakpoint(const char *info, void (*callback)(int));
extern "C" int __u_set_watchpoint(const char *info, void (*callback)(int));
extern "C" int __u_delete_breakpoint(int);
extern "C" int __u_delete_watchpoint(int);
extern "C" int __u_lookup_data_object(const char *info, size_t size);
extern "C" int __u_val(int handler, void *value);
extern "C" int __u_stop(int);

namespace unisim {
namespace service {
namespace debug {
namespace monitor {

template <typename ADDRESS>
HardwareBreakpoint<ADDRESS>::HardwareBreakpoint(ADDRESS addr, int _handle, void (*_callback)(int))
	: unisim::util::debug::Breakpoint<ADDRESS>(addr)
	, handle(_handle)
	, callback(_callback)
{
}

template <typename ADDRESS>
HardwareBreakpoint<ADDRESS>::~HardwareBreakpoint()
{
}

template <typename ADDRESS>
void HardwareBreakpoint<ADDRESS>::Report() const
{
	(*callback)(handle);
}

template <typename ADDRESS>
HardwareWatchpoint<ADDRESS>::HardwareWatchpoint(unisim::util::debug::MemoryAccessType _mat, unisim::util::debug::MemoryType _mt, ADDRESS _addr, uint32_t _size, int _handle, void (*_callback)(int))
  : unisim::util::debug::Watchpoint<ADDRESS>(_mat, _mt, _addr, _size, true)
	, handle(_handle)
	, callback(_callback)
{
}

template <typename ADDRESS>
HardwareWatchpoint<ADDRESS>::~HardwareWatchpoint()
{
}

template <typename ADDRESS>
void HardwareWatchpoint<ADDRESS>::Report() const
{
	(*callback)(handle);
}

template <typename ADDRESS>
SourceCodeBreakpoint<ADDRESS>::SourceCodeBreakpoint(const char *source_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *_debug_event_trigger_if, typename unisim::service::interfaces::StatementLookup<ADDRESS> *stmt_lookup_if, int _handle, void (*callback)(int))
	: handle(_handle)
	, debug_event_trigger_if(_debug_event_trigger_if)
	, hw_breakpoints()
{
	std::string filename;
	unsigned int lineno;
	const char *s = source_location;
	while(*s && (*s != ':'))
	{
		filename += *s;
		s++;
	}
	if(*s != ':') return;
	
	s++;
	
	std::stringstream sstr_lineno(s);
	if(!(sstr_lineno >> lineno)) return;
	if(!sstr_lineno.eof()) return;
	
	std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
	
	stmt_lookup_if->FindStatements(stmts, filename.c_str(), lineno, 0);
	
	typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::size_type num_stmts = stmts.size();
	typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::size_type stmt_num;
	
	for(stmt_num = 0; stmt_num < num_stmts; stmt_num++)
	{
		const unisim::util::debug::Statement<ADDRESS> *stmt = stmts[stmt_num];
		
		ADDRESS hw_breakpoint_addr = stmt->GetAddress();
		
		HardwareBreakpoint<ADDRESS> *hw_breakpoint = new HardwareBreakpoint<ADDRESS>(hw_breakpoint_addr, handle, callback);
		
		if(debug_event_trigger_if->Listen(hw_breakpoint))
		{
			hw_breakpoints.push_back(hw_breakpoint);
		}
		else
		{
			delete hw_breakpoint;
		}
	}
}

template <typename ADDRESS>
SourceCodeBreakpoint<ADDRESS>::~SourceCodeBreakpoint()
{
	typename std::vector<HardwareBreakpoint<ADDRESS> *>::size_type num_hw_breakpoints = hw_breakpoints.size();
	typename std::vector<HardwareBreakpoint<ADDRESS> *>::size_type hw_breakpoint_num;
	
	for(hw_breakpoint_num = 0; hw_breakpoint_num < num_hw_breakpoints; hw_breakpoint_num++)
	{
		HardwareBreakpoint<ADDRESS> *hw_breakpoint = hw_breakpoints[hw_breakpoint_num];
		
		debug_event_trigger_if->Unlisten(hw_breakpoint);
	}
}

template <typename ADDRESS>
bool SourceCodeBreakpoint<ADDRESS>::Exists() const
{
	return !hw_breakpoints.empty();
}

template <typename ADDRESS>
void SourceCodeBreakpoint<ADDRESS>::Invalidate()
{
	hw_breakpoints.clear();
}

template <typename ADDRESS>
DataObjectWatchpoint<ADDRESS>::DataObjectWatchpoint(const char *data_location, typename unisim::service::interfaces::DebugEventTrigger<ADDRESS> *_debug_event_trigger_if, typename unisim::service::interfaces::SymbolTableLookup<ADDRESS> *symbol_table_lookup_if, int _handle, void (*callback)(int))
	: handle(_handle)
	, debug_event_trigger_if(_debug_event_trigger_if)
	, hw_watchpoint(0)
{
	const unisim::util::debug::Symbol<ADDRESS> *symbol = symbol_table_lookup_if->FindSymbolByName(data_location, unisim::util::debug::Symbol<ADDRESS>::SYM_OBJECT);
	
	if(symbol)
	{
		ADDRESS hw_watchpoint_addr = symbol->GetAddress();
		ADDRESS hw_watchpoint_size = symbol->GetSize();
		
		HardwareWatchpoint<ADDRESS> *hw_wp = new HardwareWatchpoint<ADDRESS>(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, hw_watchpoint_addr, hw_watchpoint_size, handle, callback);
		hw_wp->Catch();
		
		if(debug_event_trigger_if->Listen(hw_wp))
		{
			hw_watchpoint = hw_wp;
		}
		else
		{
			hw_wp->Release();
		}
	}
}

template <typename ADDRESS>
DataObjectWatchpoint<ADDRESS>::~DataObjectWatchpoint()
{
	if(hw_watchpoint)
	{
		debug_event_trigger_if->Unlisten(hw_watchpoint);
	}
}

template <typename ADDRESS>
bool DataObjectWatchpoint<ADDRESS>::Exists() const
{
	return hw_watchpoint != 0;
}

template <typename ADDRESS>
int DataObjectWatchpoint<ADDRESS>::GetHandle() const
{
	return handle;
}

template <typename ADDRESS>
void DataObjectWatchpoint<ADDRESS>::Invalidate()
{
	hw_watchpoint = 0;
}

template <typename ADDRESS>
DataObject<ADDRESS>::DataObject(const char *data_location, size_t _size, unisim::service::interfaces::Register *_reg_pc, typename unisim::service::interfaces::DataObjectLookup<ADDRESS> *data_object_lookup_if, int _handle)
	: handle(_handle)
	, size(_size)
	, reg_pc(_reg_pc)
	, data_object(0)
{
	ADDRESS cia = 0;
	reg_pc->GetValue(&cia);

	data_object = data_object_lookup_if->FindDataObject(data_location, cia);
}

template <typename ADDRESS>
DataObject<ADDRESS>::~DataObject()
{
	if(data_object)
	{
		delete data_object;
	}
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::Exists() const
{
	return data_object && data_object->Exists();
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::IsOptimizedOut() const
{
	return data_object && data_object->IsOptimizedOut();
}

template <typename ADDRESS>
bool DataObject<ADDRESS>::GetValue(void *value) const
{
	if(data_object)
	{
		ADDRESS cia = 0;
		reg_pc->GetValue(&cia);

		data_object->Seek(cia);
		
		if(data_object->Exists())
		{
			if(!data_object->IsOptimizedOut())
			{
				if(data_object->Fetch())
				{
					if(data_object->Read(0, value, 0, size * 8))
					{
						return true;
					}
					else
					{
						std::cerr << "data object \"" << data_object->GetName() << "\" can't be read" << std::endl;
					}
				}
				else
				{
					std::cerr << "data object \"" << data_object->GetName() << "\" can't be fetched" << std::endl;
				}
			}
			else
			{
				std::cerr << "data object \"" << data_object->GetName() << "\" is optimized out" << std::endl;
			}
		}
		else
		{
			std::cerr << "after seeking (PC change) data object \"" << data_object->GetName() << "\" no longer exists in current context" << std::endl;
		}
	}
	
	return false;
}

template <typename ADDRESS>
Monitor<ADDRESS>::Monitor(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, MonitorBase()
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(name, parent)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_event_trigger_import("debug-event", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, logger(*this)
	, reg_pc(0)
	, source_code_breakpoints()
	, data_object_watchpoints()
	, tracked_data_objects()
{
}

template <typename ADDRESS>
Monitor<ADDRESS>::~Monitor()
{
	typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type num_source_code_breakpoints = source_code_breakpoints.size();
	typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type source_code_breakpoint_num;
	
	for(source_code_breakpoint_num = 0; source_code_breakpoint_num < num_source_code_breakpoints; source_code_breakpoint_num++)
	{
		SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = source_code_breakpoints[source_code_breakpoint_num];
		
		if(source_code_breakpoint)
		{
			source_code_breakpoint->Invalidate();
			delete source_code_breakpoint;
		}
	}

	typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type num_data_object_watchpoints = data_object_watchpoints.size();
	typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type data_object_watchpoint_num;
	
	for(data_object_watchpoint_num = 0; data_object_watchpoint_num < num_data_object_watchpoints; data_object_watchpoint_num++)
	{
		DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = data_object_watchpoints[data_object_watchpoint_num];
		
		if(data_object_watchpoint)
		{
			data_object_watchpoint->Invalidate();
			delete data_object_watchpoint;
		}
	}

	typename std::vector<DataObject<ADDRESS> *>::size_type num_tracked_data_objects = tracked_data_objects.size();
	typename std::vector<DataObject<ADDRESS> *>::size_type tracked_data_object_num;
	
	for(tracked_data_object_num = 0; tracked_data_object_num < num_tracked_data_objects; tracked_data_object_num++)
	{
		DataObject<ADDRESS> *tracked_data_object = tracked_data_objects[tracked_data_object_num];
		
		delete tracked_data_object;
	}
}

template <typename ADDRESS>
bool Monitor<ADDRESS>::EndSetup()
{
	reg_pc = registers_import->GetRegister("pc");
	
	if(!reg_pc) return false;
	
	return Start();
}

template <typename ADDRESS>
int Monitor<ADDRESS>::SetBreakpoint(const char *info, void (*callback)(int))
{
	int handle = source_code_breakpoints.size() + 1;
	SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = new SourceCodeBreakpoint<ADDRESS>(info, debug_event_trigger_import, stmt_lookup_import, handle, callback);
	
	if(source_code_breakpoint)
	{
		if(source_code_breakpoint->Exists())
		{
			source_code_breakpoints.push_back(source_code_breakpoint);
			return handle;
		}
		
		delete source_code_breakpoint;
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::SetWatchpoint(const char *info, void (*callback)(int))
{
	int handle = data_object_watchpoints.size() + 1;
	DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = new DataObjectWatchpoint<ADDRESS>(info, debug_event_trigger_import, symbol_table_lookup_import, handle, callback);
	
	if(data_object_watchpoint)
	{
		if(data_object_watchpoint->Exists())
		{
			data_object_watchpoints.push_back(data_object_watchpoint);
			return handle;
		}
		
		delete data_object_watchpoint;
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::DeleteBreakpoint(int handle)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<SourceCodeBreakpoint<ADDRESS> *>::size_type) idx < source_code_breakpoints.size())
		{
			SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = source_code_breakpoints[idx];
			
			if(source_code_breakpoint)
			{
				delete source_code_breakpoint;
				source_code_breakpoints[idx] = 0;
				return 0;
			}
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::DeleteWatchpoint(int handle)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<DataObjectWatchpoint<ADDRESS> *>::size_type) idx < data_object_watchpoints.size())
		{
			DataObjectWatchpoint<ADDRESS> *data_object_watchpoint = data_object_watchpoints[idx];
			
			if(data_object_watchpoint)
			{
				delete data_object_watchpoint;
				data_object_watchpoints[idx] = 0;
				return 0;
			}
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::LookupDataObject(const char *info, size_t size)
{
	int handle = tracked_data_objects.size() + 1;

	DataObject<ADDRESS> *data_object = new DataObject<ADDRESS>(info, size, reg_pc, data_object_lookup_import, handle);
	
	if(data_object)
	{
		if(data_object->Exists())
		{
			tracked_data_objects.push_back(data_object);
			
			return handle;
		}
		else
		{
			delete data_object;
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::GetDataObjectValue(int handle, void *value)
{
	if(handle > 0)
	{
		int idx = handle - 1;
	
		if((typename std::vector<DataObject<ADDRESS> *>::size_type) idx < tracked_data_objects.size())
		{
			DataObject<ADDRESS> *tracked_data_object = tracked_data_objects[idx];
			
			if(tracked_data_object)
			{
				if(tracked_data_object->GetValue(value))
				{
					return 0;
				}
			}
		}
		else
		{
			logger << unisim::kernel::logger::DebugWarning << "invalid data object handle" << unisim::kernel::logger::EndDebugWarning;
		}
	}
	
	return -1;
}

template <typename ADDRESS>
int Monitor<ADDRESS>::StopMe(int exit_status)
{
	Object::Stop(exit_status);
	
	return -1;
}

template <typename ADDRESS>
void Monitor<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	const HardwareBreakpoint<ADDRESS> *hw_breakpoint = dynamic_cast<const HardwareBreakpoint<ADDRESS> *>(event);
	
	if(hw_breakpoint)
	{
		hw_breakpoint->Report();
	}
	else
	{
		const HardwareWatchpoint<ADDRESS> *hw_watchpoint = dynamic_cast<const HardwareWatchpoint<ADDRESS> *>(event);
		
		if(hw_watchpoint)
		{
			hw_watchpoint->Report();
		}
		else
		{
			logger << unisim::kernel::logger::DebugWarning << "Ignoring an event" << unisim::kernel::logger::EndDebugWarning;
		}
	}
}

} // end of namespace monitor
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
