/*
 *  Copyright (c) 2019,
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

#ifndef __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_TCC__
#define __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_TCC__

#include <unisim/service/debug/user_interface/user_interface.hh>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace service {
namespace debug {
namespace user_interface {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <typename ADDRESS>
UserInterface<ADDRESS>::UserInterface(const char *_name, unisim::kernel::Object *_parent)
	: unisim::kernel::Object(_name, _parent, "this service implements a debugger user interface over HTTP")
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(_name, _parent)
	, unisim::kernel::Service<unisim::service::interfaces::DebugEventListener<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugSelecting>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugYieldingRequest>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::StatementLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::BackTrace<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::Profiling<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DebugInfoLoading>(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(_name, _parent)
	, unisim::kernel::Client<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(_name, _parent)
	, http_server_export("http-server-export", this)
	, debug_yielding_export("debug-yielding-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, debug_yielding_request_import("debug-yielding-request-import", this)
	, debug_selecting_import("debug-selecting-import", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, registers_import("registers-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)
	, backtrace_import("backtrace-import", this)
	, profiling_import("profiling-import", this)
	, debug_info_loading_import("debug-info-loading-import", this)
	, data_object_lookup_import("data-object-lookup-import", this)
	, subprogram_lookup_import("subprogram-lookup-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, debug(false)
	, param_debug("debug", this, debug, "Enable/Disable debug (intended for developper)")
	, min_cont_refresh_period(0.125)
	, param_min_cont_refresh_period("min-cont-refresh-period", this, min_cont_refresh_period, "Minimum refresh period (host time) in seconds while continue")
	, max_cont_refresh_period(1.0)
	, param_max_cont_refresh_period("cont-refresh-period", this, max_cont_refresh_period, "Maximum refresh period (host time) in seconds while continue")
	, enable_cache(true)
	, param_enable_cache("enable-cache", this, enable_cache, "Enable/Disable web browser caching")
	, title(GetName())
	, param_title("title", this, title, "Title of this debugger user interface instance")
	, memory_atom_size(1)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, stepi(false)
	, cont(false)
	, intr(false)
	, halt(false)
	, refresh_period(0.125)
	, mutex()
	, mutex_post()
	, cond_cont()
	, mutex_cont()
	, num_processors(0)
	, curr_prc_num(0)
	, fetch_insn_events()
	, listening_fetch()
	, stop_events()
	, prc_trap()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_post, NULL);
	pthread_cond_init(&cond_cont, NULL);
	pthread_mutex_init(&mutex_cont, NULL);
}

template <typename ADDRESS>
UserInterface<ADDRESS>::~UserInterface()
{
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex_post);
	pthread_cond_destroy(&cond_cont);
	pthread_mutex_destroy(&mutex_cont);
	
	unsigned int num_fetch_insn_events = fetch_insn_events.size();
	for(unsigned int i = 0; i < num_fetch_insn_events; i++)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[i];
		fetch_insn_event->Release();
	}
}

template <typename ADDRESS>
bool UserInterface<ADDRESS>::EndSetup()
{
	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		logger << DebugError << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << EndDebugError;
		return false;
	}

	if(!registers_import) return false;
	if(!debug_selecting_import) return false;
	
	curr_prc_num = debug_selecting_import->DebugGetSelected();
	
	unsigned int prc_num;
	for(prc_num = 0; debug_selecting_import->DebugSelect(prc_num); prc_num++)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = new unisim::util::debug::FetchInsnEvent<ADDRESS>();
		fetch_insn_event->SetProcessorNumber(prc_num);
		fetch_insn_event->Catch();
		
		fetch_insn_events.push_back(fetch_insn_event);
	}
	
	num_processors = prc_num;
	listening_fetch.resize(num_processors);
	prc_trap.resize(num_processors);
	debug_selecting_import->DebugSelect(curr_prc_num);
	
	Interrupt();
	
	return true;
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::Kill()
{
	halt = true;
	Continue();
	unisim::kernel::Object::Kill();
}

// unisim::service::interfaces::HttpServer
template <typename ADDRESS>
bool UserInterface<ADDRESS>::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "toolbar_actions.js")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
				if(enable_cache)
				{
					response.EnableCache();
				}
				response.SetContentType("application/javascript");
				
				response << "InstrumenterToobarActions.prototype.bound_background_iframe_onload = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.pending_cmd = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.background_iframe = null;" << std::endl;
				response << "InstrumenterToobarActions.prototype.retry_count = 0;" << std::endl;
				response << "" << std::endl;
				response << "function InstrumenterToobarActions()" << std::endl;
				response << "{" << std::endl;
				response << "\tthis.bound_background_iframe_onload = new Map();" << std::endl;
				response << "\tthis.pending_cmd = null;" << std::endl;
				response << "\tthis.background_iframe = null;" << std::endl;
				response << "\tthis.retry_count = 0;" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.get_debug_ui_tab = function()" << std::endl;
				response << "{" << std::endl;
				response << "\treturn gui.find_tab_by_uri('" << URI() << "') || gui.open_tab('top-middle-tile','" << GetName() << "','" << URI() << "',true);" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.execute = function(cmd)" << std::endl;
				response << "{" << std::endl;
				response << "\tif(this.pending_cmd)" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\t// if new command is same as the pending command, let pending command execute" << std::endl;
				response << "\t\tif(this.pending_cmd == cmd) return;" << std::endl;
				response << "\t\t// cancel pending command" << std::endl;
				response << "\t\tif(this.bound_background_iframe_onload[this.pending_cmd])" << std::endl;
				response << "\t\t{" << std::endl;
				response << "\t\t\tthis.background_iframe.iframe_element.removeEventListener('load', this.bound_background_iframe_onload[this.pending_cmd]);" << std::endl;
				response << "\t\t}" << std::endl;
				response << "\t\tthis.pending_cmd = null;" << std::endl;
				response << "\t}" << std::endl;
				response << "\t" << std::endl;
				response << "\tvar tab = this.get_debug_ui_tab();" << std::endl;
				response << "\tif(tab)" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\tvar foreground_iframe = tab.get_foreground_iframe();" << std::endl;
				response << "\t\tif(foreground_iframe)" << std::endl;
				response << "\t\t{" << std::endl;
				response << "\t\t\tif(foreground_iframe.loaded)" << std::endl;
				response << "\t\t\t{" << std::endl;
				response << "\t\t\t\tvar cmd_button = foreground_iframe.iframe_element.contentWindow.document.getElementById(cmd);" << std::endl;
				response << "\t\t\t\tif(cmd_button)" << std::endl;
				response << "\t\t\t\t{" << std::endl;
				response << "\t\t\t\t\t// simulate a submit" << std::endl;
				response << "\t\t\t\t\tcmd_button.click();" << std::endl;
				response << "\t\t\t\t\tthis.retry_count = 0;" << std::endl;
				response << "\t\t\t\t\treturn true;" << std::endl;
				response << "\t\t\t\t}" << std::endl;
				response << "\t\t\t}" << std::endl;
				response << "\t\t\tif(this.retry_count < 2)" << std::endl;
				response << "\t\t\t{" << std::endl;
				response << "\t\t\t\t// debugging user interface page is either not yet loaded or command button is not available yet" << std::endl;
				response << "\t\t\t\t++this.retry_count;" << std::endl;
				response << "\t\t\t\tthis.background_iframe = tab.get_background_iframe();" << std::endl;
				response << "\t\t\t\tif(this.background_iframe)" << std::endl;
				response << "\t\t\t\t{" << std::endl;
				response << "\t\t\t\t\t// try to execute command once debugging user interface page is loaded" << std::endl;
				response << "\t\t\t\t\tthis.pending_cmd = cmd;" << std::endl;
				response << "\t\t\t\t\tthis.bound_background_iframe_onload[cmd] = this.onload.bind(this)" << std::endl;
				response << "\t\t\t\t\tthis.background_iframe.iframe_element.addEventListener('load', this.bound_background_iframe_onload[cmd]);" << std::endl;
				response << "\t\t\t\t}" << std::endl;
				response << "\t\t\t}" << std::endl;
				response << "\t\t}" << std::endl;
				response << "\t}" << std::endl;
				response << "\treturn false;" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "InstrumenterToobarActions.prototype.onload = function()" << std::endl;
				response << "{" << std::endl;
				response << "\tvar cmd = this.pending_cmd;" << std::endl;
				response << "\tif(this.bound_background_iframe_onload[cmd])" << std::endl;
				response << "\t{" << std::endl;
				response << "\t\tthis.background_iframe.iframe_element.removeEventListener('load', this.bound_background_iframe_onload[cmd]);" << std::endl;
				response << "\t}" << std::endl;
				response << "\tthis.pending_cmd = null;" << std::endl;
				response << "\t// debugging user interface page is loaded: retry execution of command" << std::endl;
				response << "\tthis.execute(cmd);" << std::endl;
				response << "}" << std::endl;
				response << "" << std::endl;
				response << "if((typeof GUI !== 'undefined') && GUI.prototype.constructor)" << std::endl;
				response << "{" << std::endl;
				response << "\tGUI.prototype.debug_ui_toolbar_actions = new InstrumenterToobarActions();" << std::endl;
				response << "}" << std::endl;
				
				break;
			
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				break;
		}
	}
	else if(req.GetPath() == "")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::POST:
			{
				LockPost();
				PropertySetter property_setter(*this);
				if(property_setter.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
				{
					Lock();
					property_setter.Apply();
					Unlock();
				}
				else
				{
					logger << DebugWarning << "parse error in POST data" << EndDebugWarning;
				}
				
				if(property_setter.cont || stepi || halt)
				{
					refresh_period = min_cont_refresh_period;
					Continue();
				}
				UnlockPost();

				// Post/Redirect/Get pattern: got Post, so do Redirect
				response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
				response.SetHeaderField("Location", URI());
				
				break;
			}
			
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				
				if(halt)
				{
					response << "\t<head>" << std::endl;
					response << "\t\t<title>" << title << "</title>" << std::endl;
					response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
					response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
					response << "\t\t<script type=\"text/javascript\">document.domain=\"" << req.GetDomain() << "\";</script>" << std::endl;
					if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
					{
						response << "\t\t<script>window.history.back();</script>" << std::endl;
					}
					response << "\t</head>" << std::endl;
					response << "\t<body>" << std::endl;
					response << "\t<p>Disconnected</p>" << std::endl;
					response << "\t<script>Reload = function() { window.location.href=window.location.href; }</script>" << std::endl;
					response << "\t<button onclick=\"Reload()\">Reconnect</button>" << std::endl;
					response << "\t</body>" << std::endl;
				}
				else
				{
					response << "\t<head>" << std::endl;
					response << "\t\t<title>" << title << "</title>" << std::endl;
					response << "\t\t<meta name=\"description\" content=\"debugging user interface over HTTP of virtual platform\">" << std::endl;
					response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
					response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
					response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
					response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/debug/user_interface/style.css\" type=\"text/css\" />" << std::endl;
					response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
					response << "\t\t<script>" << std::endl;
					response << "\t\t\tGUI.prototype.set_status = function()" << std::endl;
					response << "\t\t\t{" << std::endl;
					response << "\t\t\t\tvar status_item = gui.find_statusbar_item_by_name('status-" << GetName() << "');" << std::endl;
					response << "\t\t\t\tif(status_item)" << std::endl;
					response << "\t\t\t\t{" << std::endl;
					response << "\t\t\t\t\tvar child_nodes = status_item.div_element.childNodes;" << std::endl;
					response << "\t\t\t\t\tfor(var i = 0; i < child_nodes.length; i++)" << std::endl;
					response << "\t\t\t\t\t{" << std::endl;
					response << "\t\t\t\t\t\tvar child_node = child_nodes[i];" << std::endl;
					response << "\t\t\t\t\t\tif(child_node.className == 'state')" << std::endl;
					response << "\t\t\t\t\t\t{" << std::endl;
					response << "\t\t\t\t\t\t\tchild_node.innerHTML = '<span>" << (cont ? "running" : "paused") << "</span>';" << std::endl;
					response << "\t\t\t\t\t\t}" << std::endl;
					response << "\t\t\t\t\t}" << std::endl;
					response << "\t\t\t\t}" << std::endl;
					response << "\t\t\t}" << std::endl;
					response << "\t\t</script>" << std::endl;
					response << "\t\t<script src=\"/unisim/service/debug/user_interface/script.js\"></script>" << std::endl;
					response << "\t<body onload=\"";
					if(cont)
					{
						response << "gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh');";
						if(refresh_period < max_cont_refresh_period)
						{
							refresh_period = std::min(2.0 * refresh_period, max_cont_refresh_period);
						}
					}
					else
					{
						response << "gui.auto_reload(0);";
					}
					response << "\">" << std::endl;
					
					response << "\t\t<h2>Commands</h2>" << std::endl;
					response << "\t\t<table class=\"command-table\">" << std::endl;
					response << "\t\t\t<tbody>" << std::endl;
					response << "\t\t\t\t<tr>" << std::endl;
					response << "\t\t\t\t\t<td><form id=\"stepi-form\" action=\"" << URI() << "\" method=\"post\"><button title=\"Step instruction\" id=\"stepi\" type=\"submit\" name=\"stepi\" value=\"on\"" << (halt ? " disabled" : "") << ">Step instruction</button></form></td>" << std::endl;
					response << "\t\t\t\t\t<td><form id=\"step-form\" action=\"" << URI() << "\" method=\"post\"><button title=\"Step\" id=\"step\" type=\"submit\" name=\"step\" value=\"on\"" << (halt ? " disabled" : "") << ">Step</button></form></td>" << std::endl;
					response << "\t\t\t\t\t<td><form id=\"" << (cont ? "intr" : "cont") << "-form\" action=\"" << URI() << "\" method=\"post\"><button title=\"" << (cont ? "Interrupt" : "Continue") << "\" id=\"" << (cont ? "intr" : "cont") << "\" type=\"submit\" name=\"" << (cont ? "intr" : "cont") << "\" value=\"on\"" << (halt ? " disabled" : "") << ">" << (cont ? "Interrupt" : "Continue") << "</button></form></td>" << std::endl;
					response << "\t\t\t\t\t<td><form id=\"halt-form\" action=\"" << URI() << "\" method=\"post\"><button title=\"Halt\" id=\"halt\" type=\"submit\" name=\"halt\" value=\"on\"" << (halt ? " disabled" : "")  << ">Halt</button></form></td>" << std::endl;
					response << "\t\t\t\t</tr>" << std::endl;
					response << "\t\t\t</tbody>" << std::endl;
					response << "\t\t</table>" << std::endl;
				}
				
				response << "</html>" << std::endl;
				response << std::endl;
				break;
			}
			
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, POST, GET, HEAD");
				break;
				
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, POST, GET, HEAD");
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")</title>" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << " (" << unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED << ")\">" << std::endl;
				response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
				response << "\t\t<style>" << std::endl;
				response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
				response << "\t\t</style>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body>" << std::endl;
				response << "\t\t<p>Method not allowed</p>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
		}
	}
	else
	{
		response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
		
		response << "<!DOCTYPE html>" << std::endl;
		response << "<html lang=\"en\">" << std::endl;
		response << "\t<head>" << std::endl;
		response << "\t\t<title>Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")</title>" << std::endl;
		response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		response << "\t\t<meta name=\"description\" content=\"Error " << (unsigned int) unisim::util::hypapp::HttpResponse::NOT_FOUND << " (" << unisim::util::hypapp::HttpResponse::NOT_FOUND << ")\">" << std::endl;
		response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
		response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
		response << "\t\t<style>" << std::endl;
		response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
		response << "\t\t</style>" << std::endl;
		response << "\t</head>" << std::endl;
		response << "\t<body>" << std::endl;
		response << "\t\t<p>Unavailable</p>" << std::endl;
		response << "\t</body>" << std::endl;
		response << "</html>" << std::endl;
	}
	
	bool send_status = conn.Send(response.ToString(req.GetRequestType() == unisim::util::hypapp::Request::HEAD));

	if(send_status)
	{
		if(verbose)
		{
			logger << DebugInfo << "sending HTTP response: done" << EndDebugInfo;
		}
	}
	else
	{
		logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP response" << EndDebugWarning;
	}
	
	if(halt && (req.GetRequestType() == unisim::util::hypapp::Request::GET))
	{
		Stop(-1, /* asynchronous */ true);
	}
	
	return send_status;
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
	scanner.Append(unisim::service::interfaces::JSFile(URI() + "/toolbar_actions.js"));
	
	scanner.Append(unisim::service::interfaces::CSSFile("/unisim/service/debug/user_interface/status_style.css"));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("step-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/debug/user_interface/icon_stepi.svg\">",
		/* tips */      "Step instruction",
		/* js action */ "return gui && gui.debug_ui_toolbar_actions && gui.debug_ui_toolbar_actions.execute('stepi')"
	));

	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("step-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/debug/user_interface/icon_step.svg\">",
		/* tips */      "Step",
		/* js action */ "return gui && gui.debug_ui_toolbar_actions && gui.debug_ui_toolbar_actions.execute('step')"
	));

	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("cont-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/debug/user_interface/icon_cont.svg\">",
		/* tips */      "Continue",
		/* js action */ "return gui && gui.debug_ui_toolbar_actions && gui.debug_ui_toolbar_actions.execute('cont')"
	));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("intr-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/debug/user_interface/icon_intr.svg\">",
		/* tips */      "Interrupt",
		/* js action */ "return gui && gui.debug_ui_toolbar_actions && gui.debug_ui_toolbar_actions.execute('intr')"
	));
	
	scanner.Append(unisim::service::interfaces::ToolbarDoAction(
		/* name */      std::string("halt-") + GetName(), 
		/* label */     "<img src=\"/unisim/service/debug/user_interface/icon_halt.svg\">",
		/* tips */      "Halt",
		/* js action */ "return gui && gui.debug_ui_toolbar_actions && gui.debug_ui_toolbar_actions.execute('halt')"
	));
	
	std::ostringstream status_sstr;
	status_sstr << "<div class=\"name\"><span>Debugger:</span></div>";
	status_sstr << "<div class=\"state\" title=\"click play SW/interrupt SW buttons in toolbar to change debugged software state\"></div>";
	std::string status(status_sstr.str());
	
	scanner.Append(unisim::service::interfaces::StatusBarItem(
		/* name */       std::string("status-") + GetName(),
		/* class name */ "debug-ui-status",
		/* HTML */       status
	));
}

// unisim::service::interfaces::DebugYielding
template <typename ADDRESS>
void UserInterface<ADDRESS>::DebugYield()
{
	if(unlikely(halt)) return;
	
	if(unlikely(intr))
	{
		do
		{
			Lock();
			if(stepi)
			{
				UnlistenFetch(curr_prc_num);
				stepi = false;
			}
			ClearStopEvents();
			Unlock();
			WaitForUser();
			if(unlikely(halt))
			{
				return;
			}
			if(stepi)
			{
				ListenFetch(curr_prc_num);
			}
		}
		while(unlikely(intr));
	}
}

// unisim::service::interfaces::DebugEventListener<ADDRESS>
template <typename ADDRESS>
void UserInterface<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	
	unsigned int prc_num = event->GetProcessorNumber();
	
	if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_FETCH_INSN))
	{
		const unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<const unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);
		if(unlikely(verbose))
		{
			logger << DebugInfo << "/\\/\\/\\ " << *fetch_insn_event << EndDebugInfo;
		}
		
		prc_trap[prc_num] = true;
		intr = true;
		return;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_COMMIT_INSN))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<const unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *commit_insn_event << EndDebugInfo;
		}
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = static_cast<const unisim::util::debug::Breakpoint<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *breakpoint << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		intr = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = static_cast<const unisim::util::debug::Watchpoint<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *watchpoint << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		intr = true;
	}
	else if(likely(event_type == unisim::util::debug::Event<ADDRESS>::EV_TRAP))
	{
		if(unlikely(verbose))
		{
			const unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<const unisim::util::debug::TrapEvent<ADDRESS> *>(event);
			logger << DebugInfo << "/\\/\\/\\ " << *trap_event << EndDebugInfo;
		}
		event->Catch();
		stop_events.push_back(event);
		prc_trap[prc_num] = true;
		intr = true;
	}
	else
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "/\\/\\/\\ unknown event" << EndDebugInfo;
		}
		// ignore event
		return;
	}
}

template <typename ADDRESS>
bool UserInterface<ADDRESS>::ListenFetch(unsigned int prc_num)
{
	if(listening_fetch[prc_num]) return true;
	
	unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[prc_num];
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "prc #" << prc_num << ": stepi on" << EndDebugInfo;
	}
	if(!debug_event_trigger_import->Listen(fetch_insn_event))
	{
		logger << DebugError << "Can't track fetched instructions for Processor #" << prc_num << EndDebugError;
		return false;
	}
	
	listening_fetch[prc_num] = true;
	
	return true;
}

template <typename ADDRESS>
bool UserInterface<ADDRESS>::UnlistenFetch(unsigned int prc_num)
{
	if(!listening_fetch[prc_num]) return true;

	unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = fetch_insn_events[prc_num];
		
	if(unlikely(verbose))
	{
		logger << DebugInfo << "prc #" << prc_num << ": stepi off" << EndDebugInfo;
	}
	if(!debug_event_trigger_import->Unlisten(fetch_insn_event))
	{
		logger << DebugError << "Can't untrack fetched instructions for Processor #" << prc_num << EndDebugError;
		return false;
	}
	
	listening_fetch[prc_num] = false;

	return true;
}

template <class ADDRESS>
void UserInterface<ADDRESS>::ClearStopEvents()
{
	unsigned int prc_num;
	for(prc_num = 0; prc_num < num_processors; prc_num++)
	{
		prc_trap[prc_num] = false;
	}
	
	unsigned int num_stop_events = stop_events.size();
	unsigned int i;
	for(i = 0; i < num_stop_events; i++)
	{
		const unisim::util::debug::Event<ADDRESS> *event = stop_events[i];
		event->Release();
	}
	
	stop_events.clear();
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::WaitForUser()
{
// 	std::cerr << "WaitForUser: start" << std::endl;
	pthread_mutex_lock(&mutex_cont);
	intr = false;
	cont = false;
	do
	{
		pthread_cond_wait(&cond_cont, &mutex_cont);
	}
	while(!cont);
	pthread_mutex_unlock(&mutex_cont);
// 	std::cerr << "WaitForUser: end" << std::endl;
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::Continue()
{
// 	std::cerr << "Run" << std::endl;
	pthread_mutex_lock(&mutex_cont);
	intr = false;
	cont = true;
	pthread_cond_signal(&cond_cont);
	pthread_mutex_unlock(&mutex_cont);
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::Interrupt()
{
	intr = true;
	
	if(debug_yielding_request_import)
	{
		debug_yielding_request_import->DebugYieldRequest();
	}
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::Lock()
{
	pthread_mutex_lock(&mutex);
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::Unlock()
{
	pthread_mutex_unlock(&mutex);
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::LockPost()
{
	pthread_mutex_lock(&mutex_post);
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::UnlockPost()
{
	pthread_mutex_unlock(&mutex_post);
}

template <typename ADDRESS>
UserInterface<ADDRESS>::PropertySetter::PropertySetter(UserInterface<ADDRESS>& _user_interface)
	: user_interface(_user_interface)
	, stepi(false)
	, step(false)
	, cont(false)
	, intr(false)
	, halt(false)
{
}

template <typename ADDRESS>
bool UserInterface<ADDRESS>::PropertySetter::FormAssign(const std::string& name, const std::string& value)
{
	if((name.compare("stepi") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Step" << EndDebugInfo;
		}
		stepi = true;
	}
	else if((name.compare("step") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Step instruction" << EndDebugInfo;
		}
		step = true;
	}
	else if((name.compare("cont") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Continue" << EndDebugInfo;
		}
		cont = true;
	}
	else if((name.compare("intr") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Interrupt" << EndDebugInfo;
		}
		intr = true;
	}
	else if((name.compare("halt") == 0) && (value.compare("on") == 0))
	{
		if(user_interface.verbose)
		{
			user_interface.logger << DebugInfo << "Halt" << EndDebugInfo;
		}
		halt = true;
	}
	
	return true;
}

template <typename ADDRESS>
void UserInterface<ADDRESS>::PropertySetter::Apply()
{
	if(stepi)
	{
		user_interface.stepi = true;
	}
	if(intr)
	{
		user_interface.Interrupt();
	}
	if(halt)
	{
		user_interface.halt = true;
	}
	
}

} // end of namespace user_interface
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_DEBUG_USER_INTERFACE_USER_INTERFACE_TCC__
