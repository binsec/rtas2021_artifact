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

#ifndef __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__
#define __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/hypapp/hypapp.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <unisim/service/interfaces/registers.hh>

namespace unisim {
namespace service {
namespace http_server {

class HttpServer
	: public unisim::kernel::Client<unisim::service::interfaces::HttpServer>
	, public unisim::kernel::Client<unisim::service::interfaces::Registers>
	, public unisim::util::hypapp::HttpServer
{
public:
	static const unsigned int MAX_IMPORTS = 256;
	
	unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *http_server_import[MAX_IMPORTS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *registers_import[MAX_IMPORTS];
	
	HttpServer(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~HttpServer();
	
	virtual bool EndSetup();
	virtual void Kill();
	bool Killed() const;
	
	virtual void Serve(unisim::util::hypapp::ClientConnection const& conn);
	

	void AddCSSFile(const unisim::service::interfaces::CSSFile& f);
	void AddJSFile(const unisim::service::interfaces::JSFile& f);
	void AddJSAction(const unisim::service::interfaces::BrowserDoAction& a);
	void AddJSAction(const unisim::service::interfaces::BrowserOpenTabAction& a);
	void AddJSAction(const unisim::service::interfaces::ToolbarDoAction& a);
	void AddJSAction(const unisim::service::interfaces::ToolbarOpenTabAction& a);
	void AddStatusBarItem(const unisim::service::interfaces::StatusBarItem& i);
	
private:
	unisim::kernel::logger::Logger logger;
	
	std::string sim_program_name;
	std::string sim_authors;
	std::string sim_copyright;
	std::string sim_version;
	std::string sim_description;
	std::string sim_license;
	
	unisim::kernel::variable::Parameter<bool> param_verbose;
	int http_port;
	unisim::kernel::variable::Parameter<int> param_http_port;
	int http_max_clients;
	unisim::kernel::variable::Parameter<int> param_http_max_clients;
	bool enable_cache;
	unisim::kernel::variable::Parameter<bool> param_enable_cache;
	double refresh_period;
	unisim::kernel::variable::Parameter<double> param_refresh_period;
	std::string config_file_format;
	unisim::kernel::variable::Parameter<std::string> param_config_file_format;
	
	
	std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::HttpServer> *> http_server_import_map;
	std::map<unisim::kernel::Object *, unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> *> registers_import_map;
	
	bool kernel_has_parameters;
	bool kernel_has_statistics;
	typedef std::vector<unisim::service::interfaces::CSSFile> CSSFiles;
	CSSFiles css_files;
	typedef std::vector<unisim::service::interfaces::JSFile> JSFiles;
	JSFiles js_files;
	typedef std::multimap<std::string, unisim::service::interfaces::BrowserAction *> BrowserActions;
	BrowserActions browser_actions;
	typedef std::vector<unisim::service::interfaces::ToolbarAction *> ToolbarActions;
	ToolbarActions toolbar_actions;
	typedef std::vector<unisim::service::interfaces::StatusBarItem *> StatusBarItems;
	StatusBarItems statusbar_items;
	
	unisim::kernel::Object *FindChildObject(unisim::kernel::Object *object, const std::string& child_hierarchical_name, std::size_t& pos);
	unisim::kernel::Object *FindObject(const std::string& hierarchical_name, std::size_t& pos);
	bool ServeFile(unisim::util::hypapp::HttpRequest const& req, const std::string& path, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeRootDocument(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	void Crawl(std::ostream& os, unisim::kernel::Object *object, unsigned int indent_level, bool last);
	void Crawl(std::ostream& os, unsigned int indent_level);
	bool ServeVariables(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, unisim::kernel::VariableBase::Type var_type);
	bool Serve404(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool Serve405(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn, const std::string& allow);
	bool Serve500(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool RouteHttpRequest(unisim::kernel::Object *object, unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeDefault(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeRegisters(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeRegister(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeExportConfigFile(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	bool ServeImportConfigFile(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
};

} // end of namespace http_server
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_HTTP_SERVER_HTTP_SERVER_HH__
