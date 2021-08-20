/*
 *  Copyright (c) 2008,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr) 
 *          Daniel Gracia Pérez (daniel.gracia-perez@cea.fr)
 */

#include <unisim/kernel/logger/http/http_writer.hh>
#include <unisim/util/hypapp/hypapp.hh>

namespace unisim {
namespace kernel {
namespace logger {
namespace http {

Writer::Writer()
	: unisim::kernel::Object("http", Logger::StaticServerInstance(), "Logging service over HTTP")
	, unisim::kernel::Service<unisim::service::interfaces::HttpServer>("http", Logger::StaticServerInstance(), "Logging service over HTTP")
	, Printer()
	, http_server_export("http-server-export", this)
	, enable(false)
	, max_log_size(256)
	, min_display_refresh_period(0.040)
	, max_display_refresh_period(1.0)
	, param_enable("enable", this, enable, "Show logger output through HTTP")
	, param_max_log_size("max-log-size", this, max_log_size, "Maximum log size for HTTP output")
	, param_min_display_refresh_period("min-display-refresh-period", this, min_display_refresh_period, "Minimum refresh period of display for HTTP output (in seconds)")
	, param_max_display_refresh_period("max-display-refresh-period", this, max_display_refresh_period, "Maximum refresh period of display for HTTP output (in seconds)")
	, mutex()
{
	param_max_log_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	
	pthread_mutex_init(&mutex, NULL);
}

Writer::~Writer()
{
	pthread_mutex_lock(&mutex);
	for(HTTP_LOGS_PER_CLIENT::iterator it = http_logs_per_client.begin(); it != http_logs_per_client.end(); it++)
	{
		HTTP_LOG *http_log = (*it).second;
		delete http_log;
	}
	http_logs_per_client.clear();
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
}

void Writer::Print(mode_t mode, const char *name, const char *buffer)
{
	activity = true;
	
	if(enable)
	{
		pthread_mutex_lock(&mutex);
		HTTP_LOGS_PER_CLIENT::iterator it = http_logs_per_client.find(name);
		HTTP_LOG *http_log = 0;
		if(it != http_logs_per_client.end())
		{
			http_log = (*it).second;
		}
		else
		{
			http_logs_per_client[name] = http_log = new HTTP_LOG();
		}
		
		while(global_http_log.size() >= max_log_size)
		{
			global_http_log.pop_front();
		}
		while(http_log->size() >= max_log_size)
		{
			http_log->pop_front();
		}
		
		HTTP_LOG_ENTRY http_log_entry(mode, name, buffer);
		
		global_http_log.push_back(http_log_entry);
		http_log->push_back(http_log_entry);
		pthread_mutex_unlock(&mutex);
	}
}

bool Writer::ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn)
{
	unisim::util::hypapp::HttpResponse response;
	
	if(req.GetPath() == "")
	{
		switch(req.GetRequestType())
		{
			case unisim::util::hypapp::Request::OPTIONS:
				response.Allow("OPTIONS, GET, HEAD");
				break;
				
			case unisim::util::hypapp::Request::GET:
			case unisim::util::hypapp::Request::HEAD:
			{
				struct QueryDecoder : public unisim::util::hypapp::Form_URL_Encoded_Decoder
				{
					QueryDecoder()
						: object_name()
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
					
					std::string object_name;
				};

				unisim::kernel::Object *object = 0;
				
				if(req.HasQuery())
				{
					QueryDecoder query_decoder;
				
					if(query_decoder.Decode(req.GetQuery(), std::cerr))
					{
						object = GetSimulator()->FindObject(query_decoder.object_name.c_str());
					}
				}
				
				response << "<!DOCTYPE html>" << std::endl;
				response << "<html lang=\"en\">" << std::endl;
				response << "\t<head>" << std::endl;
				if(object)
				{
					response << "\t\t<title>Log of " << unisim::util::hypapp::HTML_Encoder::Encode(object->GetName()) << "</title>" << std::endl;
				}
				else
				{
					response << "\t\t<title>Log</title>" << std::endl;
				}
				response << "\t\t<meta name=\"description\" content=\"user interface for logs over HTTP\">" << std::endl;
				response << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
				response << "\t\t<link rel=\"stylesheet\" href=\"/unisim/kernel/logger/style.css\" type=\"text/css\" />" << std::endl;
				response << "\t\t<script>document.domain='" << req.GetDomain() << "';var activity = " << (activity ? "true" : "false") << ";</script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/uri.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/service/http_server/embedded_script.js\"></script>" << std::endl;
				response << "\t\t<script src=\"/unisim/kernel/logger/script.js\"></script>" << std::endl;
				response << "\t</head>" << std::endl;
				response << "\t<body onload=\"gui.auto_reload(" << (unsigned int)(http_refresh_period * 1000) << ", 'self-refresh-when-active')\">" << std::endl;
				http_refresh_period = activity ? min_display_refresh_period : std::min(http_refresh_period * 2.0, max_display_refresh_period);
				activity = false;
				response << "\t\t<div class=\"log\">" << std::endl;
				
				if(req.HasQuery())
				{
					if(object)
					{
						pthread_mutex_lock(&mutex);
						HTTP_LOGS_PER_CLIENT::const_iterator http_log_it = http_logs_per_client.find(object->GetName());
						if(http_log_it != http_logs_per_client.end())
						{
							HTTP_LOG *http_log = (*http_log_it).second;
							PrintHttpLog(response, *http_log, false);
						}
						pthread_mutex_unlock(&mutex);
					}
				}
				else
				{
					pthread_mutex_lock(&mutex);
					PrintHttpLog(response, global_http_log, true);
					pthread_mutex_unlock(&mutex);
				}

				response << "\t\t</div>" << std::endl;
				response << "\t</body>" << std::endl;
				response << "</html>" << std::endl;
				
				break;
			}
			
			default:
				response.SetStatus(unisim::util::hypapp::HttpResponse::METHOD_NOT_ALLOWED);
				response.Allow("OPTIONS, GET, HEAD");
				
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
				break;
		}
	}
	else
	{
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
	}
	
	return conn.Send(response.ToString((req.GetRequestType() == unisim::util::hypapp::Request::HEAD) || (req.GetRequestType() == unisim::util::hypapp::Request::OPTIONS)));
}

void Writer::ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner)
{
	scanner.Append(unisim::service::interfaces::ToolbarOpenTabAction(
		/* name */      GetName(), 
		/* label */     "<img src=\"/unisim/kernel/logger/icon.svg\" alt=\"LOG\">",
		/* tips */      "Log",
		/* tile */      unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
		/* uri */       URI()
	));

	struct ClientVisitor
	{
		Writer& writer;
		unisim::service::interfaces::WebInterfaceModdingScanner& scanner;
		
		ClientVisitor(Writer& _writer, unisim::service::interfaces::WebInterfaceModdingScanner& _scanner)
			: writer(_writer)
			, scanner(_scanner)
		{
		}
		
		void operator () (Logger const* client)
		{
			scanner.Append(unisim::service::interfaces::BrowserOpenTabAction(
				/* name        */ std::string(writer.GetName()) + "-" + client->GetName(),
				/* object name */ client->GetName(),
				/* label       */ "Show log",
				/* tile        */ unisim::service::interfaces::OpenTabAction::BOTTOM_TILE,
				/* uri         */ writer.URI() + std::string("?object=") + unisim::util::hypapp::URI_Encoder::EncodeComponent(client->GetName())
			));
		}
	};
	
	ClientVisitor client_visitor(*this, scanner);
	Logger::StaticServerInstance()->ForEachClient(client_visitor);
}

void Writer::PrintHttpLog(std::ostream& os, const HTTP_LOG& http_log, bool global)
{
	for(HTTP_LOG::const_iterator it = http_log.begin(); it != http_log.end(); it++)
	{
		std::string s;
		
		const HTTP_LOG_ENTRY& http_log_entry = *it;
		mode_t mode = http_log_entry.GetMode();
		const std::string& msg = http_log_entry.GetMessage();
		os << "\t\t\t<span";
		switch(mode)
		{
			case INFO_MODE   : os << " class=\"info\""; break;
			case WARNING_MODE: os << " class=\"warning\""; break;
			case ERROR_MODE  : os << " class=\"error\""; break;
			default          : break;
		}
		
		os << ">";
		if(global)
		{
			std::string s;
			
			const std::string& name = http_log_entry.GetName();
			s.append(name);
			s.append(1, ':');
			s.append(1, ' ');
			switch(mode)
			{
				case WARNING_MODE: s.append("WARNING! "); break;
				case ERROR_MODE  : s.append("ERROR! "); break;
				default          : break;
			}
			std::size_t prefix_length = name.length();
			std::string prefix(prefix_length, ' ');
			std::size_t base_pos = 0;
			for(std::size_t pos = msg.find_first_of('\n'); pos != std::string::npos; pos = msg.find_first_of('\n', base_pos))
			{
				s.append(msg, base_pos, pos - base_pos);
				s.append(1, '\n');
				s.append(prefix);
				base_pos = pos + 1;
			}
			
			s.append(msg, base_pos, std::string::npos);
			os << unisim::util::hypapp::HTML_Encoder::Encode(s);
		}
		else
		{
			switch(mode)
			{
				case WARNING_MODE: os << "WARNING!&nbsp;"; break;
				case ERROR_MODE  : os << "ERROR!&nbsp;"; break;
				default          : break;
			}
			os << unisim::util::hypapp::HTML_Encoder::Encode(msg) << "<br>";
		}
		
		os << "</span>" << std::endl;
	}
}

} // end of namespace http
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim
