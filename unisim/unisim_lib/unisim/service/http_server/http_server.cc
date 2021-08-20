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

#include <unisim/service/http_server/http_server.hh>
#include <unisim/util/endian/endian.hh>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace http_server {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

namespace {

bool ParseHex(void *buffer, unsigned int buf_size, const std::string& str)
{
	std::size_t l_pos = 0;
	std::size_t len = str.length();
	if(l_pos >= len) return false;
	
	char c = 0;
	do
	{
		c = str[l_pos++];
		if((c != ' ') && (c != '\t') && (c != '0'))
		{
			return false;
		}
	}
	while((l_pos < len) && (c != '0'));
	
	if(l_pos >= len) return false;
	
	c = str[l_pos++];
	
	if((c != 'x') && (c != 'X')) return false;
	
	if(l_pos >= len) return false;
	
	std::size_t r_pos = (len - 1);
	
	if(r_pos >= l_pos)
	{
		do
		{
			c = str[r_pos];
			if((c != ' ') && (c != '\t')) break; 
		}
		while(--r_pos >= l_pos);
	}
	
	if(l_pos > r_pos) return false;

#if BYTE_ORDER == BIG_ENDIAN
	uint8_t *buf = (uint8_t *) buffer + buf_size - 1;
	uint8_t *buf_end = (uint8_t *) buffer - 1;
#else
	uint8_t *buf = (uint8_t *) buffer;
	uint8_t *buf_end = (uint8_t *) buffer + buf_size;
#endif
	
	bool h, o;
	for(h = false, o = false; (r_pos >= l_pos); r_pos--, h = !h)
	{
		c = str[r_pos];
		
		char d;
		if((c >= '0') && (c <= '9'))
		{
			d = (c - '0');
		}
		else if((c >= 'a') && (c <= 'f'))
		{
			d = 10 + (c - 'a');
		}
		else if((c >= 'A') && (c <= 'F'))
		{
			d = 10 + (c - 'A');
		}
		else
		{
			return false;
		}
		
		if(!o)
		{
			if(h)
			{
				*buf |= d << 4;
#if BYTE_ORDER == BIG_ENDIAN
				buf--;
#else
				buf++;
#endif
				o = (r_pos > l_pos) && (buf == buf_end);
			}
			else
			{
				*buf = d;
			}
		}
	}
	
	while(buf != buf_end)
	{
		if(h)
		{
			h = false;
		}
		else
		{
			*buf = 0;
		}
#if BYTE_ORDER == BIG_ENDIAN
		buf--;
#else
		buf++;
#endif
	}
	
	return true;
}

template <typename T>
bool ParseHex(T& value, const std::string& str)
{
	return ParseHex(&value, sizeof(value), str);
}

}

HttpServer::HttpServer(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "HTTP server")
	, unisim::kernel::Client<unisim::service::interfaces::HttpServer>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::Registers>(name, parent)
	, unisim::util::hypapp::HttpServer()
	, http_server_import()
	, registers_import()
	, logger(*this)
	, sim_program_name(GetSimulator()->FindVariable("program-name")->operator std::string())
	, sim_authors(GetSimulator()->FindVariable("authors")->operator std::string())
	, sim_copyright(GetSimulator()->FindVariable("copyright")->operator std::string())
	, sim_version(GetSimulator()->FindVariable("version")->operator std::string())
	, sim_description(GetSimulator()->FindVariable("version")->operator std::string())
	, sim_license(GetSimulator()->FindVariable("license")->operator std::string())
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, http_port(0)
	, param_http_port("http-port", this, http_port, "HTTP port")
	, http_max_clients(10)
	, param_http_max_clients("http-max-clients", this, http_max_clients, "HTTP max clients")
	, enable_cache(true)
	, param_enable_cache("enable-cache", this, enable_cache, "Enable/Disable web browser caching for files")
	, refresh_period(1.0)
	, param_refresh_period("refresh-period", this, refresh_period, "Refresh period of views (in seconds)")
	, config_file_format()
	, param_config_file_format("config-file-format", this, config_file_format, "Configuration file format")
	, http_server_import_map()
	, registers_import_map()
	, kernel_has_parameters(false)
	, kernel_has_statistics(false)
	, css_files()
	, js_files()
	, browser_actions()
	, toolbar_actions()
	, statusbar_items()
{
	param_http_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_http_port.SetMutable(false);
	param_http_max_clients.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	param_http_max_clients.SetMutable(false);
	
	logger << DebugInfo << "HTTP server on port #" << http_port << EndDebugInfo;
	this->SetTCPPort(http_port);
	this->SetMaxClients(http_max_clients);
	this->SetLog(logger.DebugInfoStream());
	this->SetWarnLog(logger.DebugWarningStream());
	this->SetErrLog(logger.DebugErrorStream());
	
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		std::stringstream http_server_import_name_sstr;
		http_server_import_name_sstr << "http-server-import[" << i << "]";
		http_server_import[i] = new unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer>(http_server_import_name_sstr.str().c_str(), this);
		
		std::stringstream registers_import_name_sstr;
		registers_import_name_sstr << "registers-import[" << i << "]";
		registers_import[i] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>(registers_import_name_sstr.str().c_str(), this);
	}
}

HttpServer::~HttpServer()
{
	Kill();
	if(Running())
	{
		JoinLoopThread();
	}
	
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		delete http_server_import[i];
		delete registers_import[i];
	}
	
	for(ToolbarActions::const_iterator it = toolbar_actions.begin(); it != toolbar_actions.end(); it++)
	{
		const unisim::service::interfaces::ToolbarAction *a = *it;
		delete a;
	}

	
	for(BrowserActions::const_iterator it = browser_actions.begin(); it != browser_actions.end(); it++)
	{
		const unisim::service::interfaces::BrowserAction *a = (*it).second;
		delete a;
	}
	
	for(StatusBarItems::const_iterator it = statusbar_items.begin(); it != statusbar_items.end(); it++)
	{
		const unisim::service::interfaces::StatusBarItem *item = *it;
		delete item;
	}
}

bool HttpServer::EndSetup()
{
	if(config_file_format.empty())
	{
		config_file_format = std::string(*GetSimulator()->FindVariable("default-config-file-format"));
	}
	
	std::stringstream config_file_format_sstr(config_file_format);
	std::string cfg_file_format;
	
	while(config_file_format_sstr >> cfg_file_format)
	{
		if(cfg_file_format == "XML")
		{
			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("export-config-file-") + GetName(), 
				/* label */     "<a draggable=\"false\" ondragstart=\"return false\" href=\"/export-config-file?format=XML\" download=\"sim_config.xml\" target=\"_blank\"><img src=\"/unisim/service/http_server/icon_export_config_xml.svg\" alt=\".XML↓\"></a>",
				/* tips */      "Export configuration as .XML file"
			));
			
			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("import-config-file-") + GetName(), 
				/* label */     "<img src=\"/unisim/service/http_server/icon_import_config_xml.svg\" alt=\".XML↑\">",
				/* tips */      "Import configuration from .XML file",
				/* js action */ "new FileUploader('/import-config-file?format=XML', '.xml', true)"
			));
		}
		else if(cfg_file_format == "INI")
		{
			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("export-config-file-") + GetName(), 
				/* label */     "<a draggable=\"false\" ondragstart=\"return false\" href=\"/export-config-file?format=INI\" download=\"sim_config.ini\" target=\"_blank\"><img src=\"/unisim/service/http_server/icon_export_config_ini.svg\" alt=\".INI↓\"></a>",
				/* tips */      "Export configuration as .INI file"
			));

			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("import-config-file-") + GetName(), 
				/* label */     "<img src=\"/unisim/service/http_server/icon_import_config_ini.svg\" alt=\".INI↑\">",
				/* tips */      "Import configuration from .INI file",
				/* js action */ "new FileUploader('/import-config-file?format=INI', '.ini', true)"
			));
		}
		else if(cfg_file_format == "JSON")
		{
			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("export-config-file-") + GetName(), 
				/* label */     "<a draggable=\"false\" ondragstart=\"return false\" href=\"/export-config-file?format=JSON\" download=\"sim_config.json\" target=\"_blank\"><img src=\"/unisim/service/http_server/icon_export_config_json.svg\" alt=\".JSON↓\"></a>",
				/* tips */      "Export configuration as .JSON file"
			));

			AddJSAction(unisim::service::interfaces::ToolbarDoAction(
				/* name */      std::string("import-config-file-") + GetName(), 
				/* label */     "<img src=\"/unisim/service/http_server/icon_import_config_json.svg\" alt=\".JSON↑\">",
				/* tips */      "Import configuration from .JSON file",
				/* js action */ "new FileUploader('/import-config-file?format=JSON', '.json', true)"
			));
		}
	}

	std::list<unisim::kernel::VariableBase *> kernel_param_lst;
	std::list<unisim::kernel::VariableBase *> kernel_stat_lst;
	
	GetSimulator()->GetRootVariables(kernel_param_lst, unisim::kernel::VariableBase::VAR_PARAMETER);
	kernel_has_parameters = !kernel_param_lst.empty();
	
	GetSimulator()->GetRootVariables(kernel_stat_lst, unisim::kernel::VariableBase::VAR_STATISTIC);
	kernel_has_statistics = !kernel_stat_lst.empty();
	
	unsigned int i;

	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*http_server_import[i])
		{
			unisim::kernel::Object *http_server_object = http_server_import[i]->GetService();
		
			if(http_server_object)
			{
				http_server_import_map[http_server_object] = http_server_import[i];
			
				AddJSAction(
					unisim::service::interfaces::BrowserOpenTabAction(
					/* name        */ http_server_object->GetName(),
					/* object name */ http_server_object->GetName(),
					/* label       */ "Open",
					/* tile        */ unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
					/* uri         */ http_server_object->URI()
				));
			}
		}
	}
	
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*registers_import[i])
		{
			unisim::kernel::Object *object = registers_import[i]->GetService();
			
			if(object)
			{
				registers_import_map[object] = registers_import[i];
			}
		}
	}

	std::list<unisim::kernel::Object *> objects;
	GetSimulator()->GetObjects(objects);
	
	std::list<unisim::kernel::Object *>::const_iterator object_it;
	for(object_it = objects.begin(); object_it != objects.end(); object_it++)
	{
		unisim::kernel::Object *object = *object_it;
		
		std::list<unisim::kernel::VariableBase *> param_lst;
		std::list<unisim::kernel::VariableBase *> stat_lst;
		object->GetVariables(param_lst, unisim::kernel::VariableBase::VAR_PARAMETER);
		object->GetVariables(stat_lst, unisim::kernel::VariableBase::VAR_STATISTIC);

		if(!param_lst.empty())
		{
			AddJSAction(
				unisim::service::interfaces::BrowserOpenTabAction(
				/* name        */ std::string("config-") + object->GetName(),
				/* object name */ object->GetName(),
				/* label       */ "Configure",
				/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
				/* uri         */ std::string("/config?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
			));
		}
		
		if(!stat_lst.empty())
		{
			AddJSAction(
				unisim::service::interfaces::BrowserOpenTabAction(
				/* name        */ std::string("stats-") + object->GetName(),
				/* object name */ object->GetName(),
				/* label       */ "Show statistics",
				/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
				/* uri         */ std::string("/stats?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
			));
		}

		std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *>::iterator it = registers_import_map.find(object);
				
		if(it != registers_import_map.end())
		{
			unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *import = (*it).second;
			
			if(import)
			{
				AddJSAction(
					unisim::service::interfaces::BrowserOpenTabAction(
					/* name        */ std::string("registers-") + object->GetName(),
					/* object name */ object->GetName(),
					/* label       */ "Show registers",
					/* tile        */ unisim::service::interfaces::OpenTabAction::TOP_RIGHT_TILE,
					/* uri         */ std::string("/registers?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(object->GetName())
				));
			}
		}
	}
	
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(*http_server_import[i])
		{
			struct WebInterfaceModdingScanner : unisim::service::interfaces::WebInterfaceModdingScanner
			{
				WebInterfaceModdingScanner(HttpServer& _http_server)
					: http_server(_http_server)
				{
				}
				
				virtual void Append(const unisim::service::interfaces::CSSFile& f)
				{
					http_server.AddCSSFile(f);
				}
				
				virtual void Append(const unisim::service::interfaces::JSFile& f)
				{
					http_server.AddJSFile(f);
				}
				
				virtual void Append(const unisim::service::interfaces::BrowserDoAction& a)
				{
					http_server.AddJSAction(a);
				}
				
				virtual void Append(const unisim::service::interfaces::BrowserOpenTabAction& a)
				{
					http_server.AddJSAction(a);
				}
				
				virtual void Append(const unisim::service::interfaces::ToolbarDoAction& a)
				{
					http_server.AddJSAction(a);
				}

				virtual void Append(const unisim::service::interfaces::ToolbarOpenTabAction& a)
				{
					http_server.AddJSAction(a);
				}
				
				virtual void Append(const unisim::service::interfaces::StatusBarItem& i)
				{
					http_server.AddStatusBarItem(i);
				}
				
			private:
				HttpServer& http_server;
			};
			
			WebInterfaceModdingScanner scanner(*this);
			(*http_server_import[i])->ScanWebInterfaceModdings(scanner);
		}
	}
	
	if(verbose)
	{
		logger << DebugInfo << browser_actions.size() << " browser action(s) defined" << EndDebugInfo;
		BrowserActions::const_iterator it;
		for(it = browser_actions.begin(); it != browser_actions.end(); it++)
		{
			const unisim::service::interfaces::BrowserAction *a = (*it).second;
			logger << DebugInfo << (*a) << EndDebugInfo;
		}
	}
	
	StartLoopThread();
	logger << DebugInfo << "HTTP server started on TCP/IP port #" << http_port << EndDebugInfo;
	return true;
}

void HttpServer::Kill()
{
	unisim::kernel::Object::Kill();
	unisim::util::hypapp::HttpServer::Kill();
}

bool HttpServer::Killed() const
{
	return unisim::kernel::Object::Killed() || unisim::util::hypapp::HttpServer::Killed();
}

void HttpServer::AddCSSFile(const unisim::service::interfaces::CSSFile& f)
{
	css_files.push_back(f);
}

void HttpServer::AddJSFile(const unisim::service::interfaces::JSFile& f)
{
	js_files.push_back(f);
}

void HttpServer::AddJSAction(const unisim::service::interfaces::BrowserDoAction& a)
{
	browser_actions.insert(std::pair<std::string, unisim::service::interfaces::BrowserAction *>(a.GetObjectName(), new unisim::service::interfaces::BrowserDoAction(a)));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::BrowserOpenTabAction& a)
{
	browser_actions.insert(std::pair<std::string, unisim::service::interfaces::BrowserAction *>(
		a.GetObjectName(),
		new unisim::service::interfaces::BrowserOpenTabAction(a, std::string("return gui && gui.open_tab('") + unisim::service::interfaces::to_string(a.GetTile()) + "','" + a.GetName() + "','" + a.GetURI() + "')")
	));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::ToolbarDoAction& a)
{
	toolbar_actions.push_back(new unisim::service::interfaces::ToolbarDoAction(a));
}

void HttpServer::AddJSAction(const unisim::service::interfaces::ToolbarOpenTabAction& a)
{
	toolbar_actions.push_back(new unisim::service::interfaces::ToolbarOpenTabAction(a, std::string("return gui && gui.open_tab('") + unisim::service::interfaces::to_string(a.GetTile()) + "','" + a.GetName() + "','" + a.GetURI() + "')"));
}

void HttpServer::AddStatusBarItem(const unisim::service::interfaces::StatusBarItem& i)
{
	statusbar_items.push_back(new unisim::service::interfaces::StatusBarItem(i));
}

unisim::kernel::Object *HttpServer::FindChildObject(unisim::kernel::Object *object, const std::string& hierarchical_name, std::size_t& pos)
{
	std::size_t curr_pos = pos;
	if(curr_pos >= hierarchical_name.length()) return 0;

	if(hierarchical_name[curr_pos] == '/')
	{
		pos = curr_pos + 1;
		return (pos < hierarchical_name.length()) ? FindChildObject(object, hierarchical_name, pos) : 0; // eat "/"
	}
	
	std::size_t hierarchical_delimiter_pos = hierarchical_name.find_first_of('/', curr_pos);
	
	std::size_t len = ((hierarchical_delimiter_pos == std::string::npos) ? hierarchical_name.length() : hierarchical_delimiter_pos) - curr_pos;
	
	if((len == 1) && (hierarchical_name[curr_pos] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		return (pos < hierarchical_name.length()) ? FindChildObject(object, hierarchical_name, pos) : 0; // eat "/."
	}
	
	if((len == 2) && (hierarchical_name[pos] == '.') && (hierarchical_name[pos + 1] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		unisim::kernel::Object *parent = object->GetParent();
		return (pos < hierarchical_name.length()) ? (parent ? FindChildObject(parent, hierarchical_name, pos) : 0) : parent; // eat "../"
	}

	const std::list<unisim::kernel::Object *>& leaf_objects = object->GetLeafs();
	
	std::list<unisim::kernel::Object *>::const_iterator it;
	
	unisim::kernel::Object *found_child = 0;
	
	for(it = leaf_objects.begin(); it != leaf_objects.end(); it++)
	{
		unisim::kernel::Object *child = *it;
		
		if(hierarchical_name.compare(curr_pos, len, child->GetObjectName()) == 0)
		{
			// found child
			found_child = child;
			break;
		}
	}
	
	if(!found_child)
	{
		// not found
		return 0;
	}
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		pos = hierarchical_name.length();
		return found_child;
	}
	
	pos = hierarchical_delimiter_pos;
	
	unisim::kernel::Object *found_object = (pos < hierarchical_name.length()) ? FindChildObject(found_child, hierarchical_name, pos) : 0;
	
	return found_object ? found_object : found_child;
}

unisim::kernel::Object *HttpServer::FindObject(const std::string& hierarchical_name, std::size_t& pos)
{
	std::size_t curr_pos = pos;
	if(curr_pos >= hierarchical_name.length()) return 0;

	if(hierarchical_name[curr_pos] != '/') return 0;
	
	curr_pos++;
	
	std::size_t hierarchical_delimiter_pos = hierarchical_name.find_first_of('/', curr_pos);
	
	std::size_t len = ((hierarchical_delimiter_pos == std::string::npos) ? hierarchical_name.length() : hierarchical_delimiter_pos) - curr_pos;
	
	if((len == 1) && (hierarchical_name[curr_pos] == '.'))
	{
		pos = hierarchical_delimiter_pos;
		return (pos < hierarchical_name.length()) ? FindObject(hierarchical_name, pos) : 0; // eat "/."
	}
	
	if((len == 2) && (hierarchical_name[curr_pos] == '.') && (hierarchical_name[curr_pos + 1] == '.'))
	{
		return 0; // "../" is forbidden for root
	}

	std::list<unisim::kernel::Object *> root_objects;
	
	GetSimulator()->GetRootObjects(root_objects);
	
	std::list<unisim::kernel::Object *>::const_iterator it;
	
	unisim::kernel::Object *found_root_object = 0;
	
	for(it = root_objects.begin(); it != root_objects.end(); it++)
	{
		unisim::kernel::Object *root_object = *it;
		
		if(hierarchical_name.compare(curr_pos, len, root_object->GetObjectName()) == 0)
		{
			// found root object
			found_root_object = root_object;
			break;
		}
	}

	if(!found_root_object)
	{
		// not found
		return 0;
	}
	
	if(hierarchical_delimiter_pos == std::string::npos)
	{
		pos = hierarchical_name.length();
		return found_root_object;
	}
	
	pos = hierarchical_delimiter_pos;
	
	unisim::kernel::Object *found_object = (pos < hierarchical_name.length()) ? FindChildObject(found_root_object, hierarchical_name, pos) : 0;
	
	return found_object ? found_object : found_root_object;
}

bool HttpServer::ServeFile(unisim::util::hypapp::HttpRequest const& req, const std::string& path, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(Verbose())
	{
		logger << DebugInfo << "opening File \"" << path << "\"" << EndDebugInfo;
	}
	
	std::ifstream file(path.c_str(), std::fstream::binary);

	if(file.is_open())
	{
		if((req.GetRequestType() == unisim::util::hypapp::Request::GET) ||
		   (req.GetRequestType() == unisim::util::hypapp::Request::HEAD))
		{
			if(file.seekg(0, file.end))
			{
				std::streampos length = file.tellg();
				
				if(file.seekg(0, file.beg))
				{
					
					if(enable_cache)
					{
						response.EnableCache();
					}
					
					std::string ext = path.substr(path.find_last_of("/."));
					
					if((ext == ".htm") || (ext == ".html"))
						response.SetContentType("text/html; charset=utf-8");
					else if(ext == ".css")
						response.SetContentType("text/css");
					else if(ext == ".js")
						response.SetContentType("application/javascript");
					else if(ext == ".png")
						response.SetContentType("image/png");
					else if((ext == ".jpg") || (ext == ".jpeg"))
						response.SetContentType("image/jpeg");
					else if(ext == ".svg")
						response.SetContentType("image/svg+xml");
					else if(ext == ".ico")
						response.SetContentType("image/x-icon");
					else
						response.SetContentType("application/octet-stream");

					if(length > 0)
					{
						char buffer[4096];
						std::streampos count = length;
						do
						{
							std::streamsize n = file.readsome(buffer, sizeof(buffer));
							
							if(file.fail()) return false;
							
							if(n > 0)
							{
								response.write(buffer, n);
								count -= n;
							}
						}
						while(count > 0);
					}
				}
				else
				{
					logger << DebugWarning << "Can't seek at beginning of File \"" << path << "\"" << EndDebugWarning;
					return Serve500(req, conn);
				}
			}
			else
			{
				logger << DebugWarning << "Can't seek at end of File \"" << path << "\"" << EndDebugWarning;
				return Serve500(req, conn);
			}
		}
		else if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
		{
			response.Allow("OPTIONS, GET, HEAD");
		}
		else
		{
			logger << DebugWarning << "Method not allowed" << EndDebugWarning;
			return Serve405(req, conn, "OPTIONS, GET, HEAD");
		}
	}
	else
	{
		logger << DebugWarning << "Can' open File \"" << path << "\" for reading" << EndDebugWarning;
		return Serve404(req, conn);
	}
	
	bool send_status = conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));

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
	
	return send_status;
}

void HttpServer::Crawl(std::ostream& os, unisim::kernel::Object *object, unsigned int indent_level, bool last)
{
	const std::list<unisim::kernel::Object *>& leaf_objects = object->GetLeafs();
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *import = 0;
	std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *>::iterator it = http_server_import_map.find(object);
	if(it != http_server_import_map.end())
	{
		import = (*it).second;
	}
	
	std::pair<BrowserActions::const_iterator, BrowserActions::const_iterator> browser_actions_range = browser_actions.equal_range(object->GetName());
	
	if(import || (browser_actions_range.first != browser_actions_range.second) || !leaf_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<li";
		if(last)
		{
			os << " class=\"last\"";
		}
		os << ">" << std::endl;
		
		indent_level++;
		
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<span title=\"" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetDescription()) << "\" class=\"browser-item noselect\"";
		
		if(import)
		{
			os << " ondblclick=\"gui.open_tab('top-middle-tile','" << object->GetName() << "','" << object->URI() << "')\"";
		}

		if(browser_actions_range.first != browser_actions_range.second)
		{
			os << " oncontextmenu=\"event.preventDefault(); gui.create_context_menu(event.clientX, event.clientY, [";
			for(BrowserActions::const_iterator it = browser_actions_range.first; it != browser_actions_range.second; it++)
			{
				if(it != browser_actions_range.first) os << ",";
				const unisim::service::interfaces::BrowserAction *a = (*it).second;
				os << "{label:'" << a->GetLabel() << "', action:function() { " << a->GetJSCodeSnippet() << "; } }";
			}
			os << "])\"";
		}
		else
		{
			os << " oncontextmenu=\"return false\"";
		}
		os << ">" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetObjectName());
		os << "</span>" << std::endl;;
		
		if(!leaf_objects.empty())
		{
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "<ul class=\"tree\">" << std::endl;
			indent_level++;
			
			std::list<unisim::kernel::Object *>::const_iterator it;
			std::list<unisim::kernel::Object *>::const_iterator next_it;
			
			for(it = leaf_objects.begin(); it != leaf_objects.end(); it = next_it)
			{
				unisim::kernel::Object *child = *it;
				next_it = it;
				next_it++;
				
				Crawl(os, child, indent_level, next_it == leaf_objects.end());
			}
			
			indent_level--;
			for(unsigned int i = 0; i < indent_level; i++) os << '\t';
			os << "</ul>" << std::endl;
		}
		
		indent_level--;
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "</li>" << std::endl;
	}
}

void HttpServer::Crawl(std::ostream& os, unsigned int indent_level)
{
	for(unsigned int i = 0; i < indent_level; i++) os << '\t';
	
	os << "<span";
	if(!sim_program_name.empty() || !sim_authors.empty() || !sim_copyright.empty() || !sim_version.empty() || !sim_description.empty() || !sim_license.empty())
	{
		os << " title=\"";
		if(!sim_program_name.empty())
		{
			os << sim_program_name;
			if(!sim_version.empty())
			{
				os << " " << sim_version;
			}
			os << std::endl;
		}
		if(!sim_copyright.empty())
		{
			os << sim_copyright << std::endl;
		}
		if(!sim_license.empty())
		{
			os << "License: " << sim_license << std::endl;
		}
		if(!sim_authors.empty())
		{
			os << "Authors: " << sim_authors << std::endl;
		}
		os << "\"";
	}
	os << " class=\"browser-item noselect\"";
	if(kernel_has_parameters || kernel_has_statistics)
	{
		os << " oncontextmenu=\"event.preventDefault(); gui.create_context_menu(event.clientX, event.clientY, [";
		if(kernel_has_parameters)
		{
			os << "{label:'Configure', action:function() { gui.open_tab('bottom-tile','config-kernel','/config?object=kernel'); } }";
		}
		if(kernel_has_parameters && kernel_has_statistics)
		{
			os << ",";
		}
		if(kernel_has_statistics)
		{
			os << "{label:'Show statistics', action:function() { gui.open_tab('bottom-tile','stats-kernel','/stats?object=kernel'); } }";
		}
		os << "])\"";
	}
	os << ">" << unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name);
	os << "</span>" << std::endl;

	std::list<unisim::kernel::Object *> root_objects;
	
	GetSimulator()->GetRootObjects(root_objects);
	
	if(!root_objects.empty())
	{
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "<ul class=\"tree\">" << std::endl;
		indent_level++;
		
		std::list<unisim::kernel::Object *>::const_iterator it;
		std::list<unisim::kernel::Object *>::const_iterator next_it;
		
		for(it = root_objects.begin(); it != root_objects.end(); it = next_it)
		{
			unisim::kernel::Object *root_object = *it;
			next_it = it;
			next_it++;
			
			Crawl(os, root_object, indent_level, next_it == root_objects.end());
		}
		indent_level--;
		for(unsigned int i = 0; i < indent_level; i++) os << '\t';
		os << "</ul>" << std::endl;
	}
}

bool HttpServer::ServeVariables(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, unisim::kernel::VariableBase::Type var_type)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
	{
		response.Allow("OPTIONS, GET, HEAD, POST");
	}
	else
	{
		bool is_kernel = false;
		unisim::kernel::Object *object = 0;
		
		if(req.HasQuery())
		{
			struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
			{
				QueryDecoder(HttpServer& _http_server)
					: http_server(_http_server)
					, object_name()
				{
				}
				
				virtual bool FormAssign(const std::string& name, const std::string& value)
				{
					if(name == "object")
					{
						object_name = value;
						return true;
					}
					
					return false;
				}
				
				HttpServer& http_server;
				std::string object_name;
			};
			
			QueryDecoder query_decoder(*this);
		
			if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
			{
				is_kernel = (query_decoder.object_name == "kernel");
				object = is_kernel ? 0 : GetSimulator()->FindObject(query_decoder.object_name.c_str());
			}
		}
		else
		{
			if(verbose)
			{
				logger << DebugInfo << "missing query" << EndDebugInfo;
			}
		}

		if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
		{
			struct Form_URL_Encoded_Decoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
			{
				virtual bool FormAssign(const std::string& _name, const std::string& _value)
				{
					name = _name;
					value = _value;
					return true;
				}
				
				std::string name;
				std::string value;
			};

			Form_URL_Encoded_Decoder decoder;
			if(decoder.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
			{
				if(object)
				{
					(*object)[decoder.name] = decoder.value.c_str();
				}
				else
				{
					*GetSimulator()->FindVariable(decoder.name.c_str()) = decoder.value.c_str();
				}
			}
			
			// Post/Redirect/Get pattern: got Post, so do Redirect
			response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
			response.SetHeaderField("Location", req.GetRequestURI());
		}
		else if((req.GetRequestType() == unisim::util::hypapp::Request::GET) ||
				(req.GetRequestType() == unisim::util::hypapp::Request::HEAD))
		{
			response << "<!DOCTYPE html>" << std::endl;
			response << "<html lang=\"en\">" << std::endl;
			response << "\t<head>" << std::endl;
			response << "\t\t<title>";
			switch(var_type)
			{
				case unisim::kernel::VariableBase::VAR_PARAMETER:
					response << "Configuration of ";
					break;
					
				case unisim::kernel::VariableBase::VAR_STATISTIC:
					response << "Statistics of ";
					break;
					
				default:
					response << "Variables of unknown type of";
					break;
			}
			response << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : (is_kernel ? unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name): "an unknown object"));
			response << "</title>" << std::endl;
			response << "\t\t<meta name=\"description\" content=\"user interface for object variables over HTTP\">" << std::endl;
			response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
			response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
			response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/var_style.css\" type=\"text/css\" />" << std::endl;
			response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/var_script.js\"></script>" << std::endl;
			response << "\t</head>" << std::endl;
			response << "\t<body onload=\"gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
			
			if(object || is_kernel)
			{
				std::list<unisim::kernel::VariableBase *> var_lst;
				if(object)
				{
					object->GetVariables(var_lst, var_type);
				}
				else
				{
					GetSimulator()->GetRootVariables(var_lst, var_type);
				}
				
				if(!var_lst.empty())
				{
					response << "\t\t<table class=\"var-table\">" << std::endl;
					response << "\t\t\t<thead>" << std::endl;
					response << "\t\t\t\t<tr>" << std::endl;
					response << "\t\t\t\t\t<th class=\"var-name\">Name</th>" << std::endl;
					response << "\t\t\t\t\t<th class=\"var-value\">Value</th>" << std::endl;
					response << "\t\t\t\t\t<th class=\"var-data-type\">Data Type</th>" << std::endl;
					response << "\t\t\t\t\t<th class=\"var-description\">Description</th>" << std::endl;
					response << "\t\t\t\t</tr>" << std::endl;
					response << "\t\t\t</thead>" << std::endl;
					response << "\t\t\t<tbody>" << std::endl;
					std::list<unisim::kernel::VariableBase *>::const_iterator var_iter;
					unsigned int var_id;
					for(var_id = 0, var_iter = var_lst.begin(); var_iter != var_lst.end(); var_id++, var_iter++)
					{
						unisim::kernel::VariableBase *var = *var_iter;
						
						response << "\t\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t\t<td class=\"var-name\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetVarName()) << "</td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"var-value\">" << std::endl;
						response << "\t\t\t\t\t\t<form action=\"/";
						switch(var_type)
						{
							case unisim::kernel::VariableBase::VAR_PARAMETER: response << "config"; break;
							case unisim::kernel::VariableBase::VAR_STATISTIC: response << "stats"; break;
							default                                         : response << "unknown"; break;
						}
						response << "?object=";
						if(is_kernel) response << "kernel";
						else if(object) response << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName());
						response << "\" method=\"post\">" << std::endl;
						
						std::string current_value = std::string(*var);
						if(var->HasEnumeratedValues())
						{
							response << "\t\t\t\t\t\t\t<select onchange=\"this.form.submit()\"" << (var->IsMutable() ? " title=\"Choose a value\"" : "") << " class=\"var-value-select" << (var->IsMutable() ? "" : " disabled") << "\" name=\"" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetVarName()) << "\"" << (var->IsMutable() ? "" : " readonly") << ">" << std::endl;
							std::vector<std::string> values;
							var->GetEnumeratedValues(values);
							
							std::vector<std::string>::const_iterator it;
							for(it = values.begin(); it != values.end(); it++)
							{
								const std::string& value = (*it);
								response << "\t\t\t\t\t\t\t\t<option value=\"" << unisim::util::hypapp::HTML_Encoder::Encode(value) << "\""<< ((current_value == value) ? " selected": "") << (var->IsMutable() ? "" : " disabled") << ">" << unisim::util::hypapp::HTML_Encoder::Encode(value) << "</option>" << std::endl;
							}
							response << "\t\t\t\t\t\t\t</select>" << std::endl;
						}
						else
						{
							response << "\t\t\t\t\t\t\t<input" << (var->IsMutable() ? " title=\"Type a value then press enter\"" : "") << " class=\"var-value-text" << (var->IsMutable() ? "" : " disabled") << "\" type=\"text\" spellcheck=\"false\" name=\"" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetVarName()) << "\" value=\"" << unisim::util::hypapp::HTML_Encoder::Encode(current_value) << "\"" << (var->IsMutable() ? "" : " readonly") << ">" << std::endl;
						}
						
						response << "\t\t\t\t\t\t</form>" << std::endl;
						response << "\t\t\t\t\t</td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"var-data-type\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetDataTypeName()) << "</td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"var-description\">" << unisim::util::hypapp::HTML_Encoder::Encode(var->GetDescription()) << "</td>" << std::endl;
						response << "\t\t\t\t</tr>" << std::endl;
					}
					
					response << "\t\t\t</tbody>" << std::endl;
					response << "\t\t</table>" << std::endl;
				}
			}

			response << "\t</body>" << std::endl;
			response << "</html>" << std::endl;
		}
		else
		{
			logger << DebugWarning << "Method not allowed" << EndDebugWarning;
			return Serve405(req, conn, "OPTIONS, GET, HEAD, POST");
		}
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
	
	return send_status;
}

bool HttpServer::ServeRegisters(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
	{
		response.Allow("OPTIONS, GET, HEAD, POST");
	}
	else
	{
		unisim::kernel::Object *object = 0;
		unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *import = 0;
		
		if(req.HasQuery())
		{
			struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
			{
				QueryDecoder(HttpServer& _http_server)
					: http_server(_http_server)
					, object_name()
				{
				}
				
				virtual bool FormAssign(const std::string& name, const std::string& value)
				{
					if(name == "object")
					{
						object_name = value;
						return true;
					}
					
					return false;
				}
				
				HttpServer& http_server;
				std::string object_name;
			};
			
			QueryDecoder query_decoder(*this);
		
			if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
			{
				object = GetSimulator()->FindObject(query_decoder.object_name.c_str());
				
				if(object)
				{
					std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *>::iterator it = registers_import_map.find(object);
					
					if(it != registers_import_map.end())
					{
						import = (*it).second;
					}
				}
			}
		}
		
		if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
		{
			if(import)
			{
				struct Command
				{
					enum COMMAND_TYPE
					{
						CMD_NONE,
						CMD_SET_REGISTER,
						CMD_SET_FIELD
					};
					
					COMMAND_TYPE type;
					std::string reg_name;
					std::string field_name;
					std::string value_str;
					
					Command(COMMAND_TYPE _type, const std::string& _reg_name, const std::string& _value_str)
						: type(_type)
						, reg_name(_reg_name)
						, field_name()
						, value_str(_value_str)
					{
					}

					Command(COMMAND_TYPE _type, const std::string& _reg_name, const std::string& _field_name, const std::string& _value_str)
						: type(_type)
						, reg_name(_reg_name)
						, field_name(_field_name)
						, value_str(_value_str)
					{
					}
				};
				
				struct Form_URL_Encoded_Decoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					Form_URL_Encoded_Decoder(HttpServer& _http_server) : http_server(_http_server) {}
					
					virtual bool FormAssign(const std::string& name, const std::string& value)
					{
						std::size_t delim_pos1 = name.find_first_of('*');
						if(delim_pos1 != std::string::npos)
						{
							std::string command_type_str = name.substr(0, delim_pos1);
							Command::COMMAND_TYPE command_type = (command_type_str == "set-reg") ? Command::CMD_SET_REGISTER
							                                                                     : ((command_type_str == "set-field") ? Command::CMD_SET_FIELD
							                                                                                                          : Command::CMD_NONE);
							
							if(command_type == Command::CMD_SET_FIELD)
							{
								std::size_t delim_pos2 = name.find_first_of('*', delim_pos1 + 1);
								if(delim_pos2 != std::string::npos)
								{
									std::string reg_name = name.substr(delim_pos1 + 1, delim_pos2 - delim_pos1 - 1);
									std::string field_name = name.substr(delim_pos2 + 1);
									commands.push_back(Command(command_type, reg_name, field_name, value));
								}
								else
								{
									http_server.logger << DebugWarning << "Missing second separator ('*') in \"" << name << "\"" << EndDebugWarning;
								}
							}
							else if(command_type == Command::CMD_SET_REGISTER)
							{
								std::string reg_name = name.substr(delim_pos1 + 1);
								commands.push_back(Command(command_type, reg_name, value));
							}
							else
							{
								http_server.logger << DebugWarning << "Unknown command \"" << command_type_str << "\"" << EndDebugWarning;
							}
						}
						else
						{
							http_server.logger << DebugWarning << "Missing first separator ('*') in \"" << name << "\"" << EndDebugWarning;
						}
						return true;
					}
					
					HttpServer& http_server;
					typedef std::vector<Command> Commands;
					Commands commands;
				};

				Form_URL_Encoded_Decoder decoder(*this);
				if(decoder.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
				{
					for(Form_URL_Encoded_Decoder::Commands::const_iterator it = decoder.commands.begin(); it != decoder.commands.end(); it++)
					{
						const Command& command = *it;
						
						switch(command.type)
						{
							case Command::CMD_NONE:
								break;
							case Command::CMD_SET_REGISTER:
							{
								unisim::service::interfaces::Register *reg = (*import)->GetRegister(command.reg_name.c_str());
								if(reg)
								{
									unsigned int reg_size = reg->GetSize();
									uint8_t reg_value[reg_size];
									if(ParseHex(reg_value, reg_size, command.value_str))
									{
										reg->SetValue(reg_value);
									}
									else
									{
										logger << DebugWarning << "parse error in \"" << command.value_str << "\"" << EndDebugWarning;
									}
									
								}
								else
								{
									logger << DebugWarning << "Unknown Register \"" << command.reg_name << EndDebugWarning;
								}
								break;
							}
							case Command::CMD_SET_FIELD:
							{
								unisim::service::interfaces::Register *reg = (*import)->GetRegister(command.reg_name.c_str());
								if(reg)
								{
									uint64_t value;
									if(ParseHex(value, command.value_str))
									{
										unisim::service::interfaces::Field *field = reg->GetField(command.field_name.c_str());
										
										if(field)
										{
											field->SetValue(value);
										}
										else
										{
											logger << DebugWarning << "Unknown Field \"" << command.field_name << "\" in Register \"" << command.reg_name << EndDebugWarning;
										}
									}
									else
									{
										logger << DebugWarning << "parse error in \"" << command.value_str << "\"" << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "Unknown Register \"" << command.reg_name << EndDebugWarning;
								}
								break;
							}
						}
					}
				}
			}
			
			// Post/Redirect/Get pattern: got Post, so do Redirect
			response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
			response.SetHeaderField("Location", req.GetRequestURI());
		}
		else if((req.GetRequestType() == unisim::util::hypapp::Request::GET) ||
				(req.GetRequestType() == unisim::util::hypapp::Request::HEAD))
		{
			response << "<!DOCTYPE html>" << std::endl;
			response << "<html lang=\"en\">" << std::endl;
			response << "\t<head>" << std::endl;
			response << "\t\t<title>Registers of " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "an unknown object") << "</title>" << std::endl;
			response << "\t\t<meta name=\"description\" content=\"user interface for " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "object") << " registers over HTTP\">" << std::endl;
			response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
			response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
			response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/regs_style.css\" type=\"text/css\" />" << std::endl;
			response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/regs_script.js\"></script>" << std::endl;
			response << "\t</head>" << std::endl;
			response << "\t<body onload=\"gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
			
			if(object && import)
			{
				struct RegisterPrinter : unisim::service::interfaces::RegisterScanner
				{
					RegisterPrinter(unisim::kernel::Object *_object, std::ostream& _response) : object(_object), response(_response) {}
					
					virtual void Append(unisim::service::interfaces::Register *reg)
					{
						unsigned int reg_size = reg->GetSize();
						uint8_t reg_value[reg_size];
						reg->GetValue(&reg_value);
						
						struct FieldCounter : unisim::service::interfaces::FieldScanner
						{
							FieldCounter() : count(0) {}
							
							virtual void Append(unisim::service::interfaces::Field *field)
							{
								++count;
							}
							
							unsigned int count;
						};
						FieldCounter field_counter;
						reg->ScanFields(field_counter);
						
						response << "\t\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t\t<td class=\"name\">";
						if(field_counter.count)
						{
							response << "<span class=\"collapse-toggle unfold\" id=\"collapse-toggle-" << unisim::util::hypapp::URI_Encoder::EncodeComponent(reg->GetName()) << "\"></span>";
						}
						response << "<a href=\"/register?object=" << unisim::util::hypapp::URI_Encoder::Encode(object->GetName()) << "&register=" << unisim::util::hypapp::URI_Encoder::Encode(reg->GetName()) << "\">" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "</a></td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"range\">" << (reg->GetSize() * 8) << "-bit</td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"value\">" << std::endl;
						response << "\t\t\t\t\t\t<form action=\"/registers?object=" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "\" method=\"post\">" << std::endl;
						response << "\t\t\t\t\t\t\t<input title=\"Type a value then press enter\" class=\"value-text\" type=\"text\" spellcheck=\"false\" name=\"set-reg*" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "\" value=\"0x" << std::hex;
#if BYTE_ORDER == BIG_ENDIAN
						for(int i = 0; i < (int) reg_size; i++)
#else
						for(int i = (reg_size - 1); i >= 0; i--)
#endif
						{
							response << (reg_value[i] >> 4);
							response << (reg_value[i] & 15);
						}
						
						response << std::dec << "\">" << std::endl;
						
						response << "\t\t\t\t\t\t</form>" << std::endl;
						response << "\t\t\t\t\t</td>" << std::endl;
						response << "\t\t\t\t\t<td class=\"description\">" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetDescription()) << "</td>" << std::endl;
						response << "\t\t\t\t</tr>" << std::endl;
						if(field_counter.count)
						{
							response << "\t\t\t</tbody>" << std::endl;
							response << "\t\t\t<tbody>" << std::endl;
						}
						
						struct FieldPrinter : unisim::service::interfaces::FieldScanner
						{
							FieldPrinter(unisim::kernel::Object *_object, unisim::service::interfaces::Register *_reg, std::ostream& _response) : object(_object), reg(_reg), response(_response) {}
							
							virtual void Append(unisim::service::interfaces::Field *field)
							{
								response << "\t\t\t\t<tr>" << std::endl;
								response << "\t\t\t\t\t<td class=\"name\"><span class=\"field-indent\"></span>" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetName()) << "</td>" << std::endl;
								response << "\t\t\t\t\t<td class=\"range\">[";
								unsigned int bit_width = field->GetBitWidth();
								unsigned int bit_offset = field->GetBitOffset();
								if(bit_width)
								{
									if(bit_width != 1)
									{
										response << (bit_offset + bit_width - 1) << "..";
									}
									response << bit_offset;
								}
								response << "]</td>" << std::endl;
								response << "\t\t\t\t\t<td class=\"value\">" << std::endl;
								response << "\t\t\t\t\t\t<form action=\"/registers?object=" << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "\" method=\"post\">" << std::endl;
								response << "\t\t\t\t\t\t\t<input title=\"Type a value then press enter\" class=\"value-text\" type=\"text\" spellcheck=\"false\" name=\"set-field*" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "*" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetName()) << "\" value=\"0x" << std::hex << field->GetValue() << std::dec << "\">" << std::endl;
								response << "\t\t\t\t\t\t</form>" << std::endl;
								response << "\t\t\t\t\t</td>" << std::endl;
								response << "\t\t\t\t\t<td class=\"description\">" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetDescription()) << "</td>" << std::endl;
								response << "\t\t\t\t</tr>" << std::endl;
							}
								
							unisim::kernel::Object *object;
							unisim::service::interfaces::Register *reg;
							std::ostream& response;
							bool open;
						};
						
						FieldPrinter field_printer(object, reg, response);
						reg->ScanFields(field_printer);
						if(field_counter.count)
						{
							response << "\t\t\t</tbody>" << std::endl;
							response << "\t\t\t<tbody>" << std::endl;
						}
					}
				private:
					unisim::kernel::Object *object;
					std::ostream& response;
				};
						
				response << "\t\t<table class=\"reg-table\">" << std::endl;
				response << "\t\t\t<thead>" << std::endl;
				response << "\t\t\t\t<tr>" << std::endl;
				response << "\t\t\t\t\t<th class=\"name\">Name</th>" << std::endl;
				response << "\t\t\t\t\t<th class=\"range\">Range</th>" << std::endl;
				response << "\t\t\t\t\t<th class=\"value\">Value</th>" << std::endl;
				response << "\t\t\t\t\t<th class=\"description\">Description</th>" << std::endl;
				response << "\t\t\t\t</tr>" << std::endl;
				response << "\t\t\t</thead>" << std::endl;
				response << "\t\t\t<tbody>" << std::endl;
				RegisterPrinter reg_printer(object, response);
				(*import)->ScanRegisters(reg_printer);
				response << "\t\t\t</tbody>" << std::endl;
				response << "\t\t</table>" << std::endl;
			}
			
			response << "\t</body>" << std::endl;
			response << "</html>" << std::endl;
		}
		else
		{
			logger << DebugWarning << "Method not allowed" << EndDebugWarning;
			return Serve405(req, conn, "OPTIONS, GET, HEAD, POST");
		}
	}
		
	bool send_status = conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));

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
	
	return send_status;
}

bool HttpServer::ServeRootDocument(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	switch(req.GetRequestType())
	{
		case unisim::util::hypapp::Request::OPTIONS:
			response.Allow("OPTIONS, GET, HEAD, POST");
			break;
			
		case unisim::util::hypapp::Request::GET:
		case unisim::util::hypapp::Request::HEAD:
			response << "<!DOCTYPE html>" << std::endl;
			response << "<html lang=\"en\">" << std::endl;
			response << "\t<head>" << std::endl;
			response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
			response << "\t\t<meta name=\"description\" content=\"remote control interface over HTTP\">" << std::endl;
			response << "\t\t<title>" << unisim::util::hypapp::HTML_Encoder::Encode(sim_program_name) << "</title>" << std::endl;
			response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
			response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/style.css\" type=\"text/css\" />" << std::endl;
			
			for(CSSFiles::const_iterator it = css_files.begin(); it != css_files.end(); it++)
			{
				const unisim::service::interfaces::CSSFile& css_file = *it;
				response << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" << css_file.GetFilename() << "\" />" << std::endl;
			}
			
			response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/script.js\"></script>" << std::endl;
			
			for(JSFiles::const_iterator it = js_files.begin(); it != js_files.end(); it++)
			{
				const unisim::service::interfaces::JSFile& js_file = *it;
				response << "\t\t<script src=\"" << js_file.GetFilename() << "\"></script>" << std::endl;
			}
			
			response << "\t</head>" << std::endl;
			response << "\t<body";
#if 0
			response << " oncontextmenu=\"return false;\"" << std::endl; // this disables right-clicks
#endif
			response << ">" << std::endl;
		
			response << "\t<div id=\"toolbar-div\">" << std::endl;
			response << "\t\t<div class=\"toolbar\">" << std::endl;
			for(ToolbarActions::const_iterator it = toolbar_actions.begin(); it != toolbar_actions.end(); it++)
			{
				const unisim::service::interfaces::ToolbarAction *a = *it;
				const std::string& js_code_snippet = a->GetJSCodeSnippet();
				response << "\t\t\t<div class=\"toolbar-item\">" << std::endl;
				response << "\t\t\t\t<div id=\"" << a->GetName() << "\" title=\"" << a->GetTips() << "\" class=\"toolbar-button\"";
				if(!js_code_snippet.empty()) response << " onclick=\"" << a->GetJSCodeSnippet() << "\"";
				response << ">" << a->GetLabel() << "</div>" << std::endl;
				response << "\t\t\t</div>" << std::endl;
			}
			response << "\t\t</div>" << std::endl;
			response << "\t</div>" << std::endl;
			response << "\t<div id=\"content-div\">" << std::endl;
			response << "\t\t<div class=\"tile\" id=\"left-tile-div\">" << std::endl;
			response << "\t\t\t<div title=\"History of closed tabs\" class=\"tab-headers-history-shortcut\" id=\"left-tab-headers-history-shortcut\"></div>" << std::endl;
			response << "\t\t\t<div title=\"Scroll tab headers to the left\" class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"left-tab-headers-left-scroller\"></div>" << std::endl;
			response << "\t\t\t<div title=\"Scroll tab headers to the right\" class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"left-tab-headers-right-scroller\"></div>" << std::endl;
			response << "\t\t\t<div class=\"tab-headers\" id=\"left-tab-headers-div\">" << std::endl;
			response << "\t\t\t\t<div class=\"tab-header noselect left-tab-header-div\" id=\"browser-tab-header\">" << std::endl;
			response << "\t\t\t\t\t<span title=\"Browser of the hierarchy of simulation objects" << std::endl << std::endl << "Tips:" << std::endl << "- Hover over the object to display its description" << std::endl << "- Double click on the object to open it" << std::endl << "- Right click on the object to show a context menu\">Browser</span>" << std::endl;
			response << "\t\t\t\t</div>" << std::endl;
			response << "\t\t\t</div>" << std::endl;
			response << "\t\t\t<div class=\"tab-contents\" id=\"left-tab-contents-div\">" << std::endl;
			response << "\t\t\t\t<div class=\"tab-content left-tab-content-div\" id=\"browser-tab-content\">" << std::endl;
			response << "\t\t\t\t\t<div class=\"noselect\" id=\"browser\">" << std::endl;
			Crawl(response, 6);
			response << "\t\t\t\t\t</div>" << std::endl;
			response << "\t\t\t\t</div>" << std::endl;
			response << "\t\t\t</div>" << std::endl;
			response << "\t\t</div>" << std::endl;
			response << "\t\t<div title=\"drag horizontally to resize tiles on both sides\" class=\"resizer\" id=\"left-horiz-resizer-div\"></div>" << std::endl;
			response << "\t\t<div id=\"right-div\">" << std::endl;
			response << "\t\t\t<div id=\"top-div\">" << std::endl;
			response << "\t\t\t\t<div class=\"tile\" id=\"top-middle-tile-div\">" << std::endl;
			response << "\t\t\t\t\t<div title=\"History of closed tabs\" class=\"tab-headers-history-shortcut\" id=\"top-middle-tab-headers-history-shortcut\"></div>" << std::endl;
			response << "\t\t\t\t\t<div title=\"Scroll tab headers to the left\" class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"top-middle-tab-headers-left-scroller\"></div>" << std::endl;
			response << "\t\t\t\t\t<div title=\"Scroll tab headers to the right\" class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"top-middle-tab-headers-right-scroller\"></div>" << std::endl;
			response << "\t\t\t\t\t<div class=\"tab-headers\" id=\"top-middle-tab-headers-div\"></div>" << std::endl;
			response << "\t\t\t\t\t<div class=\"tab-contents\" id=\"top-middle-tab-contents-div\"></div>" << std::endl;
			response << "\t\t\t\t</div>" << std::endl;
			response << "\t\t\t\t<div title=\"drag horizontally to resize tiles on both sides\" class=\"resizer\" id=\"right-horiz-resizer-div\"></div>" << std::endl;
			response << "\t\t\t\t<div class=\"tile\" id=\"top-right-tile-div\">" << std::endl;
			response << "\t\t\t\t\t<div title=\"History of closed tabs\" class=\"tab-headers-history-shortcut\" id=\"top-right-tab-headers-history-shortcut\"></div>" << std::endl;
			response << "\t\t\t\t\t<div title=\"Scroll tab headers to the left\" class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"top-right-tab-headers-left-scroller\"></div>" << std::endl;
			response << "\t\t\t\t\t<div title=\"Scroll tab headers to the right\" class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"top-right-tab-headers-right-scroller\"></div>" << std::endl;
			response << "\t\t\t\t\t<div class=\"tab-headers\" id=\"top-right-tab-headers-div\"></div>" << std::endl;
			response << "\t\t\t\t\t<div class=\"tab-contents\" id=\"top-right-tab-contents-div\"></div>" << std::endl;
			response << "\t\t\t\t</div>" << std::endl;
			response << "\t\t\t</div>" << std::endl;
			response << "\t\t\t<div title=\"drag vertically to resize tiles on both sides\" class=\"resizer\" id=\"vert-resizer-div\"></div>" << std::endl;
			response << "\t\t\t<div class=\"tile\" id=\"bottom-tile-div\">" << std::endl;
			response << "\t\t\t\t<div title=\"History of closed tabs\" class=\"tab-headers-history-shortcut\" id=\"bottom-tab-headers-history-shortcut\"></div>" << std::endl;
			response << "\t\t\t\t<div title=\"Scroll tab headers to the left\" class=\"tab-headers-scroller tab-headers-left-scroller\" id=\"bottom-tab-headers-left-scroller\"></div>" << std::endl;
			response << "\t\t\t\t<div title=\"Scroll tab headers to the right\" class=\"tab-headers-scroller tab-headers-right-scroller\" id=\"bottom-tab-headers-right-scroller\"></div>" << std::endl;
			response << "\t\t\t\t<div class=\"tab-headers\" id=\"bottom-tab-headers-div\"></div>" << std::endl;
			response << "\t\t\t\t<div class=\"tab-contents\" id=\"bottom-tab-contents-div\"></div>" << std::endl;
			response << "\t\t\t</div>" << std::endl;
			response << "\t\t</div>" << std::endl;
			response << "\t</div>" << std::endl;
			response << "\t<div id=\"statusbar-div\">" << std::endl;
			response << "\t\t<div class=\"statusbar\">" << std::endl;
			for(StatusBarItems::const_iterator it = statusbar_items.begin(); it != statusbar_items.end(); it++)
			{
				const unisim::service::interfaces::StatusBarItem *item = *it;
				response << "\t\t\t<div class=\"statusbar-item";
				const std::string& class_name = item->GetClassName();
				if(!class_name.empty())
				{
					response << ' ' << class_name;
				}
				response << "\" name=\"" << item->GetName() << "\">" << std::endl;
				response << "\t\t\t\t" << item->GetHTMLCodeSnippet() << std::endl; 
				response << "\t\t\t</div>" << std::endl;
			}
			response << "\t\t</div>" << std::endl;
			response << "\t</div>" << std::endl;
			response << "\t</body>" << std::endl;
			response << "</html>" << std::endl;
			
			break;
			
		default:
			logger << DebugWarning << "Method not allowed" << EndDebugWarning;
			return Serve405(req, conn, "OPTIONS, GET, HEAD");
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
	
	return send_status;
}

bool HttpServer::ServeRegister(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
	{
		response.Allow("OPTIONS, GET, HEAD, POST");
	}
	else
	{
		unisim::kernel::Object *object = 0;
		unisim::service::interfaces::Register *reg = 0;
		unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *import = 0;
		
		if(req.HasQuery())
		{
			struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
			{
				QueryDecoder(HttpServer& _http_server)
					: http_server(_http_server)
					, object_name()
					, register_name()
				{
				}
				
				virtual bool FormAssign(const std::string& name, const std::string& value)
				{
					if(name == "object")
					{
						object_name = value;
						return true;
					}
					else if(name == "register")
					{
						register_name = value;
						return true;
					}
					
					return false;
				}
				
				HttpServer& http_server;
				std::string object_name;
				std::string register_name;
			};
			
			QueryDecoder query_decoder(*this);
		
			if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
			{
				object = GetSimulator()->FindObject(query_decoder.object_name.c_str());
				
				if(object)
				{
					std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *>::iterator it = registers_import_map.find(object);
					
					if(it != registers_import_map.end())
					{
						import = (*it).second;
					}
					
					if(import)
					{
						reg = (*import)->GetRegister(query_decoder.register_name.c_str());
					}
				}
				
			}
		}
		
		if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
		{
			if(reg)
			{
				struct Command
				{
					enum COMMAND_TYPE
					{
						CMD_NONE,
						CMD_SET_REGISTER,
						CMD_SET_FIELD
					};
					
					COMMAND_TYPE type;
					std::string field_name;
					std::string value_str;
					
					Command(COMMAND_TYPE _type, const std::string& _value_str)
						: type(_type)
						, field_name()
						, value_str(_value_str)
					{
					}

					Command(COMMAND_TYPE _type, const std::string& _field_name, const std::string& _value_str)
						: type(_type)
						, field_name(_field_name)
						, value_str(_value_str)
					{
					}
				};
				
				struct Form_URL_Encoded_Decoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					Form_URL_Encoded_Decoder(HttpServer& _http_server) : http_server(_http_server) {}
					
					virtual bool FormAssign(const std::string& name, const std::string& value)
					{
						std::size_t delim_pos = name.find_first_of('*');
						std::string command_type_str = name.substr(0, delim_pos);
						Command::COMMAND_TYPE command_type = (command_type_str == "set-reg") ? Command::CMD_SET_REGISTER
							                                                                     : ((command_type_str == "set-field") ? Command::CMD_SET_FIELD
							                                                                                                          : Command::CMD_NONE);
						if(command_type == Command::CMD_SET_FIELD)
						{
							if(delim_pos != std::string::npos)
							{
								std::string field_name = name.substr(delim_pos + 1);
								commands.push_back(Command(command_type, field_name, value));
							}
							else
							{
								http_server.logger << DebugWarning << "Missing separator ('*') in \"" << name << "\"" << EndDebugWarning;
							}
						}
						else if(command_type == Command::CMD_SET_REGISTER)
						{
							commands.push_back(Command(command_type, value));
						}
						else
						{
							http_server.logger << DebugWarning << "Unknown command \"" << command_type_str << "\"" << EndDebugWarning;
						}
						return true;
					}
					
					HttpServer& http_server;
					typedef std::vector<Command> Commands;
					Commands commands;
				};

				Form_URL_Encoded_Decoder decoder(*this);
				if(decoder.Decode(std::string(req.GetContent(), req.GetContentLength()), logger.DebugWarningStream()))
				{
					for(Form_URL_Encoded_Decoder::Commands::const_iterator it = decoder.commands.begin(); it != decoder.commands.end(); it++)
					{
						const Command& command = *it;
						
						switch(command.type)
						{
							case Command::CMD_NONE:
								break;
								
							case Command::CMD_SET_REGISTER:
							{
								unsigned int reg_size = reg->GetSize();
								uint8_t reg_value[reg_size];
								if(ParseHex(reg_value, reg_size, command.value_str))
								{
									reg->SetValue(reg_value);
								}
								else
								{
									logger << DebugWarning << "parse error in \"" << command.value_str << "\"" << EndDebugWarning;
								}
								break;
							}
							case Command::CMD_SET_FIELD:
							{
								uint64_t value;
								if(ParseHex(value, command.value_str))
								{
									unisim::service::interfaces::Field *field = reg->GetField(command.field_name.c_str());
												
									if(field)
									{
										field->SetValue(value);
									}
									else
									{
										logger << DebugWarning << "Unknown Field \"" << command.field_name << "\" in Register \"" << reg->GetName() << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "parse error in \"" << command.value_str << "\"" << EndDebugWarning;
								}
								break;
							}
						}
					}
				}
			}
			
			// Post/Redirect/Get pattern: got Post, so do Redirect
			response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
			response.SetHeaderField("Location", req.GetRequestURI());
		}
		else if((req.GetRequestType() == unisim::util::hypapp::Request::GET) ||
				(req.GetRequestType() == unisim::util::hypapp::Request::HEAD))
		{
			response << "<!DOCTYPE html>" << std::endl;
			response << "<html lang=\"en\">" << std::endl;
			response << "\t<head>" << std::endl;
			response << "\t\t<title>";
			if(reg)
			{
				response << "Register " << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName());
			}
			else
			{
				response << "Unknown register";
			}
			response << " of " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "an unknown object") << "</title>" << std::endl;
			response << "\t\t<meta name=\"description\" content=\"user interface for ";
			if(reg)
			{
				response << "Register " << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName());
			}
			else
			{
				response << "Unknown register";
			}
			response << " of " << (object ? unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) : "an unknown object") << " over HTTP\">" << std::endl;
			response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
			response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" << std::endl;
			response << "\t\t<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\" />" << std::endl;
			response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/service/http_server/reg_style.css\" type=\"text/css\" />" << std::endl;
			response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
			response << "\t\t<script src=\"/unisim/service/http_server/reg_script.js\"></script>" << std::endl;
			response << "\t</head>" << std::endl;
			response << "\t<body onload=\"gui.auto_reload(" << (unsigned int)(refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
			
			if(reg)
			{
				response << "\t\t<table class=\"field-table\">" << std::endl;
				response << "\t\t\t<tr>" << std::endl;
				response << "\t\t\t\t<th class=\"name\">Name</th>" << std::endl;
				response << "\t\t\t\t<th class=\"range\">Range</th>" << std::endl;
				response << "\t\t\t\t<th class=\"value\">Value</th>" << std::endl;
				response << "\t\t\t\t<th class=\"description\">Description</th>" << std::endl;
				response << "\t\t\t</tr>" << std::endl;
				response << "\t\t\t<tr>" << std::endl;
				response << "\t\t\t\t\t<td class=\"name\">" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetName()) << "</td>" << std::endl;
				response << "\t\t\t\t\t<td class=\"range\">" << (reg->GetSize() * 8) << "-bit</td>" << std::endl;
				response << "\t\t\t\t\t<td class=\"value\">" << std::endl;
				response << "\t\t\t\t\t\t<form action=\"/register?object=" << unisim::util::hypapp::URI_Encoder::Encode(object->GetName()) << "&register=" << unisim::util::hypapp::URI_Encoder::Encode(reg->GetName()) << "\" method=\"post\">" << std::endl;
				response << "\t\t\t\t\t\t\t<input title=\"Type a value then press enter\" class=\"value-text\" type=\"text\" spellcheck=\"false\" name=\"set-reg\" value=\"0x" << std::hex;
				
				unsigned int reg_size = reg->GetSize();
				uint8_t reg_value[reg_size];
				reg->GetValue(&reg_value);
#if BYTE_ORDER == BIG_ENDIAN
				for(int i = 0; i < (int) reg_size; i++)
#else
				for(int i = (reg_size - 1); i >= 0; i--)
#endif
				{
					response << (reg_value[i] >> 4);
					response << (reg_value[i] & 15);
				}
				
				response << std::dec << "\">" << std::endl;
				response << "\t\t\t\t\t\t</form>" << std::endl;
				response << "\t\t\t\t\t</td>" << std::endl;
				response << "\t\t\t\t\t<td class=\"description\">" << unisim::util::hypapp::HTML_Encoder::Encode(reg->GetDescription()) << "</td>" << std::endl;
				response << "\t\t\t\t</tr>" << std::endl;
						
				struct FieldPrinter : unisim::service::interfaces::FieldScanner
				{
					FieldPrinter(unisim::kernel::Object *_object, unisim::service::interfaces::Register *_reg, std::ostream& _response) : object(_object), reg(_reg), response(_response) {}
					
					virtual void Append(unisim::service::interfaces::Field *field)
					{
						response << "\t\t\t<tr>" << std::endl;
						response << "\t\t\t\t<td class=\"name\"><span class=\"field-indent\"></span>" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetName()) << "</td>" << std::endl;
						response << "\t\t\t\t<td class=\"range\">[";
						unsigned int bit_width = field->GetBitWidth();
						unsigned int bit_offset = field->GetBitOffset();
						if(bit_width)
						{
							if(bit_width != 1)
							{
								response << (bit_offset + bit_width - 1) << "..";
							}
							response << bit_offset;
						}
						response << "]</td>" << std::endl;
						response << "\t\t\t\t<td class=\"value\">" << std::endl;
						response << "\t\t\t\t\t<form action=\"/register?object=" << unisim::util::hypapp::URI_Encoder::Encode(object->GetName()) << "&register=" << unisim::util::hypapp::URI_Encoder::Encode(reg->GetName()) << "\" method=\"post\">" << std::endl;
						response << "\t\t\t\t\t\t<input title=\"Type a value then press enter\" class=\"value-text\" type=\"text\" spellcheck=\"false\" name=\"set-field*" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetName()) << "\" value=\"0x" << std::hex << field->GetValue() << std::dec << "\">" << std::endl;
						response << "\t\t\t\t\t</form>" << std::endl;
						response << "\t\t\t\t</td>" << std::endl;
						response << "\t\t\t\t<td class=\"description\">" << unisim::util::hypapp::HTML_Encoder::Encode(field->GetDescription()) << "</td>" << std::endl;
						response << "\t\t\t</tr>" << std::endl;
					}
						
					unisim::kernel::Object *object;
					unisim::service::interfaces::Register *reg;
					std::ostream& response;
				};
				
				FieldPrinter field_printer(object, reg, response);
				reg->ScanFields(field_printer);
				response << "\t\t</table>" << std::endl;
			}
			
			response << "\t</body>" << std::endl;
			response << "</html>" << std::endl;
		}
		else
		{
			logger << DebugWarning << "Method not allowed" << EndDebugWarning;
			return Serve405(req, conn, "OPTIONS, GET, HEAD, POST");
		}
	}
		
	bool send_status = conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));

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
	
	return send_status;
}

bool HttpServer::Serve404(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	// Note: When browsing option "Show friendly HTTP error messages" is enabled, IE5+ does not show 404 error document if it is less than 512 bytes long 
	unisim::util::hypapp::HttpResponse response;
	
	response.SetStatus(unisim::util::hypapp::HttpResponse::NOT_FOUND);
	
	response << "<!DOCTYPE html>" << std::endl;
	response << "<html lang=\"en\">" << std::endl;
	response << "\t<head>" << std::endl;
	response << "\t\t<title>Error 404 (Not Found)</title>" << std::endl;
	response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	response << "\t\t<meta name=\"description\" content=\"Error 404 (Not Found)\">" << std::endl;
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
	
	return conn.Send(response.ToString());
}

bool HttpServer::Serve405(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, const std::string& allow)
{
	unisim::util::hypapp::HttpResponse response;
	
	response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
	response.Allow(allow);
	
	response << "<!DOCTYPE html>" << std::endl;
	response << "<html lang=\"en\">" << std::endl;
	response << "\t<head>" << std::endl;
	response << "\t\t<title>Error 405 (Method Not Allowed)</title>" << std::endl;
	response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	response << "\t\t<meta name=\"description\" content=\"Error 405 (Method Not Allowed)\">" << std::endl;
	response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	response << "\t\t<style>" << std::endl;
	response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
	response << "\t\t</style>" << std::endl;
	response << "\t</head>" << std::endl;
	response << "\t<body>" << std::endl;
	response << "\t\t<p>HTTP Method not allowed</p>" << std::endl;
	response << "\t</body>" << std::endl;
	response << "</html>" << std::endl;
	
	return conn.Send(response.ToString());
}

bool HttpServer::Serve500(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	response.SetStatus(unisim::util::hypapp::HttpResponse::INTERNAL_SERVER_ERROR);
	
	response << "<!DOCTYPE html>" << std::endl;
	response << "<html lang=\"en\">" << std::endl;
	response << "\t<head>" << std::endl;
	response << "\t\t<title>Error 500 (Internal Server Error)</title>" << std::endl;
	response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	response << "\t\t<meta name=\"description\" content=\"Error 500 (Internal Server Error)\">" << std::endl;
	response << "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" << std::endl;
	response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	response << "\t\t<style>" << std::endl;
	response << "\t\t\tbody { font-family:Arial,Helvetica,sans-serif; font-style:normal; font-size:14px; text-align:left; font-weight:400; color:black; background-color:white; }" << std::endl;
	response << "\t\t</style>" << std::endl;
	response << "\t</head>" << std::endl;
	response << "\t<body>" << std::endl;
	response << "\t\t<p>Internal Server Error</p>" << std::endl;
	response << "\t</body>" << std::endl;
	response << "</html>" << std::endl;
	
	return conn.Send(response.ToString());
}

bool HttpServer::ServeExportConfigFile(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	if((req.GetRequestType() == unisim::util::hypapp::Request::GET) ||
	   (req.GetRequestType() == unisim::util::hypapp::Request::HEAD) ||
	   (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS))
	{
		unisim::util::hypapp::HttpResponse response;
		
		if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
		{
			response.Allow("OPTIONS, GET, HEAD");
		}
		else
		{
			std::string format;
			
			if(req.HasQuery())
			{
				struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					QueryDecoder(HttpServer& _http_server)
						: http_server(_http_server)
						, format()
					{
					}
					
					virtual bool FormAssign(const std::string& name, const std::string& value)
					{
						if(name == "format")
						{
							format = value;
							return true;
						}
						
						return false;
					}
					
					HttpServer& http_server;
					std::string format;
				};
				
				QueryDecoder query_decoder(*this);
			
				if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
				{
					format = query_decoder.format;
				}
			}

			response.SetContentType("application/octet-stream");
			if(format == "XML")
			{
				response.SetHeaderField("Content-Disposition", "attachment;filename=\"sim_config.xml\"");
			}
			else if(format == "INI")
			{
				response.SetHeaderField("Content-Disposition", "attachment;filename=\"sim_config.ini\"");
			}
			else if(format == "JSON")
			{
				response.SetHeaderField("Content-Disposition", "attachment;filename=\"sim_config.json\"");
			}
			
			GetSimulator()->SaveVariables(response, unisim::kernel::VariableBase::VAR_PARAMETER, format);
			
			return conn.Send(response.ToString(req.GetRequestType() == unisim::util::hypapp::Request::HEAD));
		}
	}
	else
	{
		logger << DebugWarning << "Method not allowed" << EndDebugWarning;
		return Serve405(req, conn, "OPTIONS, GET, HEAD");
	}
	
	return false;
}

bool HttpServer::ServeImportConfigFile(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	if((req.GetRequestType() == unisim::util::hypapp::Request::POST) ||
	   (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS))
	{
		unisim::util::hypapp::HttpResponse response;
		
		if(req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)
		{
			response.Allow("OPTIONS, POST");
		}
		else
		{
			std::string format;
			
			if(req.HasQuery())
			{
				struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					QueryDecoder(HttpServer& _http_server)
						: http_server(_http_server)
						, format()
					{
					}
					
					virtual bool FormAssign(const std::string& name, const std::string& value)
					{
						if(name == "format")
						{
							format = value;
							return true;
						}
						
						return false;
					}
					
					HttpServer& http_server;
					std::string format;
				};
				
				QueryDecoder query_decoder(*this);
			
				if(query_decoder.Decode(req.GetQuery(), logger.DebugWarningStream()))
				{
					format = query_decoder.format;
				}
			}
			
			unsigned int n = req.GetPartCount();
			for(unsigned int i = 0; i < n; i++)
			{
				unisim::util::hypapp::HttpRequestPart const& part = req.GetPart(i);

				std::stringstream content_part_sstr(std::string(part.GetContent(), part.GetContentLength()));
				if(GetSimulator()->LoadVariables(content_part_sstr, unisim::kernel::VariableBase::VAR_PARAMETER, format))
				{
					if(verbose)
					{
						logger << DebugInfo << "Successful loading parameters set from part #" << i << " of a multipart HTTP request" << EndDebugInfo;
					}
				}
				else
				{
					logger << DebugWarning << "Loading parameters set from part #" << i << " of a multipart HTTP request failed" << EndDebugWarning;
				}
			}

			unisim::util::hypapp::HttpResponse response;
			
			if(req.GetRequestType() == unisim::util::hypapp::Request::POST)
			{
				// Post/Redirect/Get pattern: got Post, so do Redirect
				response.SetStatus(unisim::util::hypapp::HttpResponse::SEE_OTHER);
				response.SetHeaderField("Location", std::string("http://") + req.GetHost());
			}
			
			return conn.Send(response.ToString(req.GetRequestType() == unisim::util::hypapp::Request::HEAD));
		}
	}
	else
	{
		logger << DebugWarning << "Method not allowed" << EndDebugWarning;
		return Serve405(req, conn, "OPTIONS, POST");
	}
	
	return false;
}

void HttpServer::Serve(unisim::util::hypapp::ClientConnection const& conn)
{
	struct MessageLoop : public unisim::util::hypapp::MessageLoop
	{
		MessageLoop(HttpServer& _http_server)
			: unisim::util::hypapp::MessageLoop(
				_http_server,
				_http_server.logger.DebugInfoStream(),
				_http_server.logger.DebugWarningStream(),
				_http_server.logger.DebugErrorStream())
			, http_server(_http_server)
		{
		}
		
		virtual bool SendResponse(unisim::util::hypapp::Request const& req, unisim::util::hypapp::ClientConnection const& conn)
		{
			unisim::util::hypapp::HttpRequest http_request(req, http_server.logger.DebugInfoStream(), http_server.logger.DebugWarningStream(), http_server.logger.DebugErrorStream());
			
			if(http_request.IsValid()) // http request is valid?
			{
				if(http_server.Verbose())
				{
					http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" is valid" << EndDebugInfo;
				}
				
				if(http_request.GetAbsolutePath() == "/") // URI refers to root document (directly or indirectly)
				{
					if(http_server.Verbose())
					{
						http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" refers to root" << EndDebugInfo;
					}
					
					return http_server.ServeRootDocument(http_request, conn);
				}
				else if(http_request.GetAbsolutePath() == "/favicon.ico")
				{
					return http_server.ServeFile(http_request, http_server.GetSimulator()->GetSharedDataDirectory() + "/unisim/service/http_server/favicon.ico", conn);
				}
				else if((http_request.GetAbsolutePath() == "/config") || (http_request.GetAbsolutePath() == "/config/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::VariableBase::VAR_PARAMETER);
				}
				else if((http_request.GetAbsolutePath() == "/stats") || (http_request.GetAbsolutePath() == "/stats/"))
				{
					return http_server.ServeVariables(http_request, conn, unisim::kernel::VariableBase::VAR_STATISTIC);
				}
				else if((http_request.GetAbsolutePath() == "/registers") || (http_request.GetAbsolutePath() == "/registers/"))
				{
					return http_server.ServeRegisters(http_request, conn);
				}
				else if((http_request.GetAbsolutePath() == "/register") || (http_request.GetAbsolutePath() == "/register/"))
				{
					return http_server.ServeRegister(http_request, conn);
				}
				else if((http_request.GetAbsolutePath() == "/kernel") || (http_request.GetAbsolutePath() == "/kernel/"))
				{
					return http_server.Serve404(http_request, conn);
				}
				else if(http_request.GetAbsolutePath() == "/export-config-file")
				{
					return http_server.ServeExportConfigFile(http_request, conn);
				}
				else if(http_request.GetAbsolutePath() == "/import-config-file")
				{
					return http_server.ServeImportConfigFile(http_request, conn);
				}
				else
				{
					std::string path = http_server.GetSimulator()->GetSharedDataDirectory() + http_request.GetAbsolutePath();
					
					if(http_server.Verbose())
					{
						http_server.logger << DebugInfo << "looking if \"" << path << "\" exists and is a regular file" << EndDebugInfo;
					}
					
					struct stat st;
					if((stat(path.c_str(), &st) == 0) && S_ISREG(st.st_mode))
					{
						if(http_server.ServeFile(http_request, path, conn)) return true;
					}
					else
					{
						if(http_server.Verbose())
						{
							http_server.logger << DebugInfo << "\"" << path << "\" does not exist or is not a regular file" << EndDebugInfo;
						}

						std::size_t pos = 0;
						unisim::kernel::Object *object = http_server.FindObject(http_request.GetAbsolutePath(), pos);
						
						if(object)
						{
							if(http_server.Verbose())
							{
								http_server.logger << DebugInfo << "delegating Http request to Object \"" << object->GetName() << "\"" << EndDebugInfo;
							}
							std::string object_http_request_server_root(http_request.GetAbsolutePath().substr(0, pos));
							std::string object_http_request_path(http_request.GetAbsolutePath().substr(pos));
							unisim::util::hypapp::HttpRequest object_http_request(object_http_request_server_root, object_http_request_path, http_request);
							return !http_server.RouteHttpRequest(object, object_http_request, conn) || !http_server.Killed();
						}
						else
						{
							if(http_server.Verbose())
							{
								http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" refers neither a regular file nor an object" << EndDebugInfo;
							}
						}
					}
				}
			}
			else
			{
				if(http_server.Verbose())
				{
					http_server.logger << DebugInfo << "URI \"" << req.GetRequestURI() << "\" is invalid" << EndDebugInfo;
				}
			}
			
			if(http_server.Verbose())
			{
				http_server.logger << DebugInfo << "sending HTTP response 404 Not Found" << EndDebugInfo;
			}
			
			if(!http_server.Serve404(http_request, conn))
			{
				http_server.logger << DebugWarning << "I/O error or connection closed by peer while sending HTTP 404 Not Found" << EndDebugWarning;
			}
			return false;
		}
		
	private:
		HttpServer& http_server;
	};
	
	MessageLoop msg_loop = MessageLoop(*this);
	msg_loop.Run(conn);
}

bool HttpServer::RouteHttpRequest(unisim::kernel::Object *object, unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *>::iterator it = http_server_import_map.find(object);
	if(it != http_server_import_map.end())
	{
		unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *import = (*it).second;
		if(import)
		{
			return (*import)->ServeHttpRequest(req, conn);
		}
	}
	
	return Serve404(req, conn);
}

bool HttpServer::ServeDefault(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	response << "<!DOCTYPE html>" << std::endl;
	response << "<html lang=\"en\">" << std::endl;
	response << "\t<head>" << std::endl;
	response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	response << "\t\t<script>document.domain='" << req.GetDomain() << "';</script>" << std::endl;
	response << "\t</head>" << std::endl;
	response << "\t<body>" << std::endl;
	response << "\t</body>" << std::endl;
	response << "</html>" << std::endl;

	return conn.Send(response.ToString(req.GetRequestType() == unisim::util::hypapp::Request::HEAD));
}


} // end of namespace http_server
} // end of namespace service
} // end of namespace unisim
