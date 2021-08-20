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

#ifndef __UNISIM_KERNEL_LOGGER_HTTP_HTTP_WRITER_HH__
#define __UNISIM_KERNEL_LOGGER_HTTP_HTTP_WRITER_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger_server.hh>
#include <unisim/service/interfaces/http_server.hh>
#include <pthread.h>
#include <string>
#include <fstream>
#include <map>
#include <list>

namespace unisim {
namespace kernel {
namespace logger {
namespace http {

struct Writer
	: unisim::kernel::Service<unisim::service::interfaces::HttpServer>
	, Printer
{
	/** Http server export */
	unisim::kernel::ServiceExport<unisim::service::interfaces::HttpServer> http_server_export;
	
	Writer();
	virtual ~Writer();
	virtual void Print(mode_t mode, const char *name, const char *buffer);
	
	/** Serve an HTTP request
	  * HTTP server calls that interface method to ask for logger to serve an incoming HTTP request intended for HTTP writer
	  * 
	  * @param req HTTP request
	  * @param conn Connection with HTTP client (web browser)
	  */
	virtual bool ServeHttpRequest(unisim::util::hypapp::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
	virtual void ScanWebInterfaceModdings(unisim::service::interfaces::WebInterfaceModdingScanner& scanner);
private:
	bool enable;
	unsigned int max_log_size;
	double min_display_refresh_period;
	double max_display_refresh_period;
	
	unisim::kernel::variable::Parameter<bool>         param_enable;
	unisim::kernel::variable::Parameter<unsigned int> param_max_log_size;
	unisim::kernel::variable::Parameter<double>       param_min_display_refresh_period;
	unisim::kernel::variable::Parameter<double>       param_max_display_refresh_period;
	
	/* HTTP logs */
	struct HttpLogEntry
	{
		HttpLogEntry()
			: data(0)
		{
		}
		
		HttpLogEntry(mode_t mode, const char *name, const char *buffer)
			: data(new HttpLogEntryData(mode, name, buffer))
		{
			if(data) data->Catch();
		}
		
		HttpLogEntry(const HttpLogEntry& o)
			: data(0)
		{
			Copy(o);
		}
		
		HttpLogEntry& operator = (const HttpLogEntry& o)
		{
			Copy(o);
			return *this;
		}
		
		~HttpLogEntry()
		{
			if(data) data->Release();
			data = 0;
		}
		
		mode_t GetMode() const { return data ? data->GetMode() : NO_MODE; }
		const std::string& GetName() const { static std::string null_str; return data ? data->GetName() : null_str; }
		const std::string& GetMessage() const { static std::string null_str; return data ? data->GetMessage() : null_str; }
		
	private:
		struct HttpLogEntryData
		{
			HttpLogEntryData(mode_t _mode, const char *_name, const char *buffer)
				: mode(_mode)
				, name(_name)
				, msg(buffer)
				, ref_count(0)
			{
			}
			
			void Catch()
			{
				ref_count++;
			}
			
			void Release()
			{
				if(ref_count && (--ref_count == 0))
				{
					delete this;
				}
			}
			
			mode_t GetMode() const { return mode; }
			const std::string& GetName() const { return name; }
			const std::string& GetMessage() const { return msg; }
			
		private:
			mode_t mode;
			std::string name;
			std::string msg;
			unsigned int ref_count;
		};
		
		HttpLogEntryData *data;
		
		
		void Copy(const HttpLogEntry& o)
		{
			if(data != o.data)
			{
				if(data) data->Release();
				data = o.data;
				if(data) data->Catch();
			}
		}
	};
	
	typedef HttpLogEntry HTTP_LOG_ENTRY;
	typedef std::list<HTTP_LOG_ENTRY> HTTP_LOG;
	typedef std::map<std::string, HTTP_LOG *> HTTP_LOGS_PER_CLIENT;

	HTTP_LOG global_http_log;
	HTTP_LOGS_PER_CLIENT http_logs_per_client;
	bool activity;
	double http_refresh_period;
	
	pthread_mutex_t mutex;
	
	void PrintHttpLog(std::ostream& os, const HTTP_LOG& http_log, bool global);
};

} // end of namespace http
} // end of namespace logger
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_LOGGER_HTTP_HTTP_WRITER_HH__
