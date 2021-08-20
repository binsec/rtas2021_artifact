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
 * Authors: Gilles Mouchard (gilles.mouchar@cea.fr)
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_KERNEL_SERVICE_SERVICE_HH__
#define __UNISIM_KERNEL_SERVICE_SERVICE_HH__

#include <unisim/util/inlining/inlining.hh>
#include <unisim/util/nat_sort/nat_sort.hh>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stdint.h>
#include <pthread.h>

#ifdef DEBUG_MEMORY_ALLOCATION
void *operator new(std::size_t size);
void *operator new[](std::size_t size);
void operator delete(void *p, std::size_t size);
void operator delete[](void *p, std::size_t size);
#endif

namespace unisim {
namespace kernel {

namespace variable
{
template <class TYPE> class Parameter;
template <class TYPE> class ParameterArray;
}

class Object;
class VariableBase;
class Simulator;
template <class SERVICE_IF> class Client;
template <class SERVICE_IF> class Service;
class ServiceImportBase;
class ServiceExportBase;
template <class SERVICE_IF> class ServiceImport;
template <class SERVICE_IF> class ServiceExport;
class SignalHandler;
class ConfigFileHelper;

std::string string_to_latex(const char *s, unsigned int cut = 0, const char *style = 0);

// Service import/export connection operators
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

//=============================================================================
//=                          VariableBaseListener                             =
//=============================================================================

class VariableBaseListener
{
public:
	virtual void VariableBaseNotify(const VariableBase *var) = 0;
	virtual ~VariableBaseListener() {};
};

//=============================================================================
//=                             VariableBase                                  =
//=============================================================================

class VariableBase
{
public:
	typedef enum { VAR_VOID, VAR_ARRAY, VAR_PARAMETER, VAR_STATISTIC, VAR_REGISTER, VAR_SIGNAL } Type;
	typedef enum { FMT_DEFAULT, FMT_HEX, FMT_DEC } Format;
	typedef enum { DT_USER, DT_BOOL, DT_SCHAR, DT_SHORT, DT_INT, DT_LONG, DT_LONG_LONG, DT_UCHAR, DT_USHORT, DT_UINT, DT_ULONG, DT_ULONG_LONG, DT_FLOAT, DT_DOUBLE, DT_STRING } DataType;

	VariableBase();
	VariableBase(const char *name, Object *owner, Type type, const char *description = 0);
	VariableBase(const char *name, VariableBase *container, Type type, const char *description = 0);
	virtual ~VariableBase();
	
	void Initialize();
	Object *GetOwner() const;
	VariableBase *GetContainer() const;
	static VariableBase &GetVoidVariable();
	const char *GetName() const;
	const char *GetVarName() const;
	const char *GetDescription() const;
	Type GetType() const;
	const char *GetTypeName() const;
	Format GetFormat() const;
	virtual const char *GetDataTypeName() const;
	virtual DataType GetDataType() const;
	bool HasEnumeratedValues() const;
	bool HasEnumeratedValue(const char *value) const;
	void GetEnumeratedValues(std::vector<std::string> &values) const;
	bool AddEnumeratedValue(const char *value);
	bool RemoveEnumeratedValue(const char *value);
	void SetDescription(const char *description);
	void SetFormat(Format fmt);
	bool IsVoid() const;

	virtual operator bool () const;
	operator signed char () const;
	operator short () const;
	operator int () const;
	operator long () const;
	virtual operator long long () const;
	operator unsigned char () const;
	operator unsigned short () const;
	operator unsigned int () const;
	operator unsigned long () const;
	virtual operator unsigned long long () const;
	operator float () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual VariableBase& operator = (bool value);
	VariableBase& operator = (char value);
	VariableBase& operator = (short value);
	VariableBase& operator = (int value);
	VariableBase& operator = (long value);
	virtual VariableBase& operator = (long long value);
	VariableBase& operator = (unsigned char value);
	VariableBase& operator = (unsigned short value);
	VariableBase& operator = (unsigned int value);
	VariableBase& operator = (unsigned long value);
	virtual VariableBase& operator = (unsigned long long value);
	VariableBase& operator = (float value);
	virtual VariableBase& operator = (double value);
	virtual VariableBase& operator = (const char * value);

	virtual VariableBase& operator [] (unsigned int index);
	virtual const VariableBase& operator [] (unsigned int index) const;
	virtual unsigned int GetLength() const;
	virtual unsigned int GetBitSize() const;
	
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual std::string GetSymbolicValue() const;
	void GenerateLatexDocumentation(std::ostream& os) const;
	
	bool IsMutable() const;
	bool IsVisible() const;
	bool IsSerializable() const;
	bool IsModified() const;
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);
	virtual void SetModified(bool is_modified);

	void AddListener(VariableBaseListener *listener);
	void RemoveListener(VariableBaseListener *listener);
	void NotifyListeners();

private:
	std::string name;
	std::string var_name;
	Object *owner;
	VariableBase *container;
	std::string description;
	std::vector<std::string> enumerated_values;
	Type type;
	Format fmt;
	bool is_mutable;
	bool is_visible;
	bool is_serializable;
	bool is_modified;
	std::set<VariableBaseListener*> listener_set;
};

//=============================================================================
//=                           ConfigFileHelper                                =
//=============================================================================

class ConfigFileHelper
{
public:
	virtual ~ConfigFileHelper();
	virtual const char *GetName() const = 0;
	virtual bool SaveVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
	virtual bool SaveVariables(std::ostream& os, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
	virtual bool LoadVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
	virtual bool LoadVariables(std::istream& is, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
	virtual bool Match(const char *filename) const;
protected:
	static bool MatchFilenameByExtension(const char *filename, const char *extension);
};

//=============================================================================
//=                                 Simulator                                 =
//=============================================================================

class Simulator
{
public:
	typedef enum
	{
		ST_OK_TO_START,
		ST_OK_DONT_START,
		ST_WARNING,
		ST_ERROR
	} SetupStatus;
	
	static inline Simulator *Instance() { return simulator; }
	
	Simulator(int argc, char **argv,
			void (*LoadBuiltInConfig)(Simulator *simulator) = 0);
	virtual ~Simulator();
	virtual SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);

	const VariableBase *FindVariable(const char *name, VariableBase::Type type = VariableBase::VAR_VOID) const;
	VariableBase *FindVariable(const char *name, VariableBase::Type type = VariableBase::VAR_VOID);
	const VariableBase *FindArray(const char *name) const;
	VariableBase *FindArray(const char *name);
	const VariableBase *FindParameter(const char *name) const;
	VariableBase *FindParameter(const char *name);
	const VariableBase *FindRegister(const char *name) const;
	VariableBase *FindRegister(const char *name);
	const VariableBase *FindStatistic(const char *name) const;
	VariableBase *FindStatistic(const char *name);
	Object *FindObject(const char *name) const;

	void GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	void GetRootVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	void GetArrays(std::list<VariableBase *>& lst);
	void GetParameters(std::list<VariableBase *>& lst);
	void GetRegisters(std::list<VariableBase *>& lst);
	void GetSignals(std::list<VariableBase *>& lst);
	void GetStatistics(std::list<VariableBase *>& lst);

	void Dump(std::ostream& os);
	void DumpVariables(std::ostream& os, VariableBase::Type filter_type = VariableBase::VAR_VOID);
	void DumpStatistics(std::ostream& os);
	void DumpParameters(std::ostream& os);
	void DumpRegisters(std::ostream& os);

	bool GetExecutablePath(const char *argv0, std::string& out_execute_path) const;
	bool GetBinPath(const char *argv0, std::string& out_bin_dir, std::string& out_bin_program) const;
	bool GetSharePath(const std::string& bin_dir, std::string& out_share_dir) const;
	const std::string& GetSharedDataDirectory() const;
	std::string SearchSharedDataFile(const char *filename) const;
	std::vector<std::string> const& GetCmdArgs() const;

	void GenerateLatexDocumentation(std::ostream& os) const;
	
	virtual double GetSimTime()	{ return (0);	}
	virtual double GetHostTime()	{ return (0);	}

	// ********** The following methods are added by Reda and used by PIMServer *****
	virtual unsigned long long   GetStructuredAddress(unsigned long long logicalAddress) { return (logicalAddress); }
	virtual unsigned long long   GetPhysicalAddress(unsigned long long logicalAddress) { return (logicalAddress); }
	// *******************************************************************************

	bool IsWarningEnabled() const;

	void Register(ConfigFileHelper *config_file_helper);
	
private:
	friend class Object;
	friend class VariableBase;
	template <class TYPE> friend class Variable;
	template <class TYPE> friend class VariableArray;
	friend class ServiceImportBase;
	friend class ServiceExportBase;
	friend class SignalHandler;

	static Simulator *simulator;
	VariableBase *void_variable;
	std::string shared_data_dir;
	std::map<std::string, std::string> set_vars;
	std::vector<std::string> get_config_filenames;
	std::string default_config_file_format;
	bool list_parms;
	bool generate_doc;
	std::string generate_doc_filename;
	bool enable_warning;
	bool enable_version;
	bool enable_help;
	bool warn_get_bin_path;
	bool warn_get_share_path;
	bool enable_press_enter_at_exit;
	std::string bin_dir;
	std::string program_binary;
	std::string program_name;
	std::string authors;
	std::string copyright;
	std::string description;
	std::string version;
	std::string license;
	std::string schematic;
	variable::Parameter<std::string> *var_program_name;
	variable::Parameter<std::string> *var_authors;
	variable::Parameter<std::string> *var_copyright;
	variable::Parameter<std::string> *var_description;
	variable::Parameter<std::string> *var_version;
	variable::Parameter<std::string> *var_license;
	variable::Parameter<std::string> *var_schematic;
	variable::Parameter<bool> *param_enable_press_enter_at_exit;
	variable::Parameter<std::string> *param_default_config_file_format;
	
	void Version(std::ostream& os) const;
	void Help(std::ostream& os) const;
	
	void Register(Object *object);
	void Register(ServiceImportBase *srv_import);
	void Register(ServiceExportBase *srv_export);
	void Register(VariableBase *variable);

	void Unregister(Object *object);
	void Unregister(ServiceImportBase *srv_import);
	void Unregister(ServiceExportBase *srv_export);
	void Unregister(VariableBase *variable);

	void Initialize(VariableBase *variable);

	ConfigFileHelper *GuessConfigFileHelper(const char *filename);
	ConfigFileHelper *FindConfigFileHelper(const std::string& config_file_format);
public:
	bool LoadVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID, const std::string& config_file_format = std::string());
	bool LoadVariables(std::istream& is, VariableBase::Type type = VariableBase::VAR_VOID, const std::string& config_file_format = std::string());
	bool SaveVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID, const std::string& config_file_format = std::string());
	bool SaveVariables(std::ostream& os, VariableBase::Type type = VariableBase::VAR_VOID, const std::string& config_file_format = std::string());

	void GetObjects(std::list<Object *>& lst) const;
	void GetRootObjects(std::list<Object *>& lst) const;

private:
	class CommandLineOption
	{
	public:
		CommandLineOption(char short_name, const char *long_name, const char *opt_description, const char *arg_description = 0);
		char GetShortName() const;
		const char *GetLongName() const;
		bool HasArgument() const;
		const char *GetArgumentDescription() const;
		const char *GetOptionDescription() const;
		int operator == (const char *arg) const;
	private:
		char short_name;
		const char *long_name;
		const char *arg_description;
		const char *opt_description;
	};

	std::string config_file_formats;
	std::vector<CommandLineOption> command_line_options;

	std::map<std::string, Object *, unisim::util::nat_sort::nat_ltstr> objects;
	std::map<std::string, ServiceImportBase *> imports;
	std::map<std::string, ServiceExportBase *> exports;
	std::map<std::string, VariableBase *, unisim::util::nat_sort::nat_ltstr> variables;
	std::map<std::string, ConfigFileHelper *> config_file_helpers;
	
	std::vector<std::string> cmd_args;
	variable::ParameterArray<std::string> *param_cmd_args;
	
public:
	template <typename T> T GetVariable(const char *variable_name, const T *t = 0) const;
	
	void SetVariable(const char *variable_name, const char *variable_value);
	void SetVariable(const char *variable_name, bool variable_value);
	void SetVariable(const char *variable_name, char variable_value);
	void SetVariable(const char *variable_name, unsigned char variable_value);
	void SetVariable(const char *variable_name, short variable_value);
	void SetVariable(const char *variable_name, unsigned short variable_value);
	void SetVariable(const char *variable_name, int variable_value);
	void SetVariable(const char *variable_name, unsigned int variable_value);
	void SetVariable(const char *variable_name, long variable_value);
	void SetVariable(const char *variable_name, unsigned long variable_value);
	void SetVariable(const char *variable_name, unsigned long long variable_value);
	void SetVariable(const char *variable_name, long long variable_value);
	void SetVariable(const char *variable_name, float variable_value);
	void SetVariable(const char *variable_name, double variable_value);
	
	virtual void SigInt();
	virtual void Kill();
private:
	pthread_mutex_t mutex;
	
	pthread_t sig_int_thrd;
	
	pthread_mutex_t sig_int_thrd_create_mutex;
	pthread_cond_t sig_int_thrd_create_cond;
	
	pthread_mutex_t sig_int_thrd_mutex;
	pthread_cond_t sig_int_thrd_cond;
	
	bool sig_int_cond;
	bool stop_sig_int_thrd;
	bool sig_int_thrd_alive;
	
	void SigIntThrd();
	static void *SigIntThrdEntryPoint(void *self);
	bool StartSigIntThrd();
	bool StopSigIntThrd();
	void MTSigInt();
	
	void BroadcastSigInt();
	void Lock();
	void Unlock();
};

//=============================================================================
//=                                 Object                                    =
//=============================================================================

class Object
{
public:
	Object(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Object();

	virtual void OnDisconnect();
	virtual bool BeginSetup(); // must not call any import. By contract it is called first.
	virtual bool Setup(ServiceExportBase *service_export); // must setup an export, can call any import it depends, see ServiceExportBase::SetupDependsOn.
	                                                       // By contract, BeginSetup has been called before.
	virtual bool EndSetup(); // can call any import
	                         // By contract, it is called after Setup(ServiceExportBase&)
	
	virtual void SigInt();
	virtual void Kill();
	bool Killed() const { return killed; }

	const char *GetName() const;
	const char *GetObjectName() const;
	std::string URI() const;

	void Add(ServiceImportBase& srv_import);
	void Remove(ServiceImportBase& srv_import);
	void Add(ServiceExportBase& srv_export);
	void Remove(ServiceExportBase& srv_export);
	void Add(Object& object);
	void Remove(Object& object);
	void Add(VariableBase& var);
	void Remove(VariableBase& var);
	const std::list<ServiceImportBase *>& GetServiceImports() const;
	const std::list<ServiceExportBase *>& GetServiceExports() const;
	const std::list<Object *>& GetLeafs() const;
	void GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID) const;
	Object *GetParent() const;
	void Disconnect();
	VariableBase& operator [] (const char *name);
	VariableBase& operator [] (const std::string& name);
	Simulator *GetSimulator() const;
	void GenerateLatexDocumentation(std::ostream& os, VariableBase::Type variable_type) const;
	const char *GetDescription() const;
	virtual void Stop(int exit_status, bool asynchronous = false);
	void SetDescription(const char *description);
	
private:
	std::string object_name;
	std::string object_base_name;
	std::string description;
	Object *parent;
	std::list<VariableBase *> variables;
	std::list<ServiceImportBase *> srv_imports;
	std::list<ServiceExportBase *> srv_exports;
	std::list<Object *> leaf_objects;
	bool killed;
};

//=============================================================================
//=                              ServiceInterface                             =
//=============================================================================

class ServiceInterface
{
public:
	virtual ~ServiceInterface() {}
};

//=============================================================================
//=                            Service<SERVICE_IF>                            =
//=============================================================================

template <class SERVICE_IF>
class Service : public SERVICE_IF, virtual public Object
{
public:
	Service(const char *name, Object *parent = 0, const char *description = 0);
};

template <class SERVICE_IF>
Service<SERVICE_IF>::Service(const char *_name, Object *_parent, const char *_description) :
	Object(_name, _parent, _description)
{
}

//=============================================================================
//=                           Client<SERVICE_IF>                              =
//=============================================================================

template <class SERVICE_IF>
class Client : virtual public Object
{
public:
	Client(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Client();
};

template <class SERVICE_IF>
Client<SERVICE_IF>::Client(const char *_name, Object *_parent, const char *_description) :
	Object(_name, _parent, _description)
{
}

template <class SERVICE_IF>
Client<SERVICE_IF>::~Client()
{
}

//=============================================================================
//=                           ServiceImportBase                               =
//=============================================================================

class ServiceImportBase
{
public:
	ServiceImportBase(const char *name, Object *owner);
	virtual ~ServiceImportBase();
	const char *GetName() const;
	virtual void Disconnect() = 0;
	virtual void DisconnectService() = 0;
	virtual void Dump(std::ostream& os) const = 0;
	virtual Object *GetService() const = 0;
	virtual ServiceExportBase *GetServiceExport() = 0;
protected:
	std::string name;
	Object *owner;
};

//=============================================================================
//=                           ServiceExportBase                               =
//=============================================================================

class ServiceExportBase
{
public:
	ServiceExportBase(const char *name, Object *owner);
	virtual ~ServiceExportBase();
	const char *GetName() const;
	virtual void Disconnect() = 0;
	virtual void DisconnectClient() = 0;
	virtual void Dump(std::ostream& os) const = 0;
	virtual Object *GetClient() const = 0;
	virtual Object *GetService() const = 0;
	void SetupDependsOn(ServiceImportBase& srv_import);
	virtual std::list<ServiceImportBase *>& GetSetupDependencies();

	friend void operator >> (ServiceExportBase& lhs, ServiceImportBase& rhs);
	friend void operator << (ServiceImportBase& lhs, ServiceExportBase& rhs);
protected:
	std::string name;
	Object *owner;
	std::list<ServiceImportBase *> setup_dependencies;
};

//=============================================================================
//=                       ServiceImport<SERVICE_IF>                           =
//=============================================================================

template <class SERVICE_IF>
class ServiceImport : public ServiceImportBase
{
public:
	ServiceImport(const char *name, Client<SERVICE_IF> *client);
	ServiceImport(const char *name, Object *owner);
	virtual ~ServiceImport();

 	inline operator SERVICE_IF * () const ALWAYS_INLINE;

	inline SERVICE_IF *operator -> () const ALWAYS_INLINE;

	// (import -> export) ==> export
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

	// (export <- import) ==> import
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (import1 -> import2) ==> import2
	friend ServiceImport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (import1 <- import2) ==> import2
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

	virtual void Disconnect();

	virtual void DisconnectService();

	void ResolveClient();

	void Unbind(ServiceExport<SERVICE_IF>& srv_export);

	virtual void Dump(std::ostream& os) const;

	virtual Object *GetService() const;

	virtual ServiceExportBase *GetServiceExport();

private:
	Service<SERVICE_IF> *service;
	ServiceExport<SERVICE_IF> *srv_export;
	ServiceImport<SERVICE_IF> *alias_import;
	std::list<ServiceImport<SERVICE_IF> *> actual_imports;
	Client<SERVICE_IF> *client;

	Service<SERVICE_IF> *ResolveService(Client<SERVICE_IF> *client);
	ServiceExport<SERVICE_IF> *ResolveServiceExport();
	void UnresolveService();
	void Bind(ServiceExport<SERVICE_IF>& srv_export); // myself -> export
	void Bind(ServiceImport<SERVICE_IF>& alias_import);  // myself -> import
	void Unbind(ServiceImport<SERVICE_IF>& alias_import);
};

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::ServiceImport(const char *_name, Client<SERVICE_IF> *_client) :
	ServiceImportBase(_name, _client),
	service(0),
	srv_export(0),
	alias_import(0),
	actual_imports(),
	client(_client)
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".ServiceImport(" << _name << ", client " << _client->GetName() << ")" << std::endl;
#endif
}

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::ServiceImport(const char *_name, Object *_owner) :
	ServiceImportBase(_name, _owner),
	service(0),
	srv_export(0),
	alias_import(0),
	actual_imports(),
	client(0)
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".ServiceImport(" << _name << ", object " << (_owner ? _owner->GetName() : "?") << ")" << std::endl;
#endif
}

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::~ServiceImport()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".~ServiceImport()" << std::endl;
#endif
	//ServiceImport<SERVICE_IF>::DisconnectService();
	ServiceImport<SERVICE_IF>::Disconnect();
}

template <class SERVICE_IF>
inline ServiceImport<SERVICE_IF>::operator SERVICE_IF * () const
{
	return (service);
}

template <class SERVICE_IF>
inline SERVICE_IF *ServiceImport<SERVICE_IF>::operator -> () const
{
#ifdef DEBUG_KERNEL
	if(!service)
	{
		std::cerr << "ERROR! " << GetName() << " interface can't be used because it is not bound to a service." << std::endl;
		owner->Object::Stop(-1);
	}
#endif
	return (service);
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& srv_export)
{
	if(this->alias_import)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->alias_import->GetName() << std::endl;
		return;
	}

	if(this->srv_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->srv_export->GetName() << std::endl;
		return;
	}

#ifdef DEBUG_KERNEL
	std::cerr << GetName() << " -> " << srv_export.GetName() << std::endl;
#endif
	this->srv_export = &srv_export;
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& alias_import)
{
	if(srv_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << srv_export->GetName() << std::endl;
		return;
	}

	if(this->alias_import)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << this->alias_import->GetName() << std::endl;
		return;
	}

#ifdef DEBUG_KERNEL
	std::cerr << GetName() << " -> " << alias_import.GetName() << std::endl;
#endif
	this->alias_import = &alias_import;
	alias_import.actual_imports.push_back(this);
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Unbind(ServiceExport<SERVICE_IF>& srv_export)
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".Unbind(" << srv_export.GetName() << ")" << std::endl;
#endif

	if(this->srv_export != &srv_export)
	{
		std::cerr << "ERROR! Can't disconnect " << GetName() << " from " << srv_export.GetName() << " because connection does not exist" << std::endl;
		return;
	}

	UnresolveService();

	this->srv_export = 0;
}

template <class SERVICE_IF>
Service<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveService(Client<SERVICE_IF> *_client)
{
	if(alias_import)
		return (alias_import->ResolveService(_client));
	else
		if(srv_export) return (srv_export->ResolveService(_client));

#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".ResolveService(" << _client->GetName() << ") failed" << std::endl;
#endif
	return (0);
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveServiceExport()
{
	if(alias_import)
		return (alias_import->ResolveServiceExport());
	else
		if(srv_export) return (srv_export->ResolveServiceExport());

	return (0);
}

template <class SERVICE_IF>
ServiceExportBase *ServiceImport<SERVICE_IF>::GetServiceExport()
{
	return (ResolveServiceExport());
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::ResolveClient()
{
	if(actual_imports.empty())
	{
		if(client && !service)
		{
			service = ResolveService(client);
		}
	}
	else
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			(*import_iter)->ResolveClient();
		}
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::UnresolveService()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".UnresolveService()" << std::endl;
#endif

	if(actual_imports.empty())
	{
		if(service)
		{
			//service->OnDisconnect(); // Gilles: That's dangerous
#ifdef DEBUG_KERNEL
			std::cerr << GetName() << ": Unresolving service " << service->GetName() << std::endl;
#endif
		}
	}
	else
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			(*import_iter)->UnresolveService();
		}
	}
	service = 0;
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Disconnect()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".Disconnect()" << std::endl;
#endif

	DisconnectService();

	if(alias_import)
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = alias_import->actual_imports.begin(); import_iter != alias_import->actual_imports.end(); import_iter++)
		{
			if(*import_iter == this)
			{
				alias_import->actual_imports.erase(import_iter);
				break;
			}
		}
		alias_import = 0;
	}

	if(!actual_imports.empty())
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			if((*import_iter)->alias_import == this)
			{
				(*import_iter)->alias_import = 0;
			}
		}
		actual_imports.clear();
	}

	if(srv_export)
	{
		srv_export->Unbind(*this);
		srv_export = 0;
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::DisconnectService()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".DisconnectService()" << std::endl;
#endif

	if(alias_import)
	{
		alias_import->DisconnectService();
	}

	if(srv_export)
	{
		UnresolveService();
		srv_export->Unbind(*this);
		srv_export = 0;
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Dump(std::ostream& os) const
{
	if(alias_import)
	{
		os << GetName() << " -> " << alias_import->GetName() << std::endl;
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::const_iterator import_iter;

	for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
	{
		os << "# " << (*import_iter)->GetName() << " -> " << GetName() << std::endl;
	}

	if(srv_export)
	{
		os << GetName() << " -> " << srv_export->GetName() << std::endl;
	}
}

template <class SERVICE_IF>
Object *ServiceImport<SERVICE_IF>::GetService() const
{
	return (service);
}

//=============================================================================
//=                          ServiceExport<SERVICE_IF>                        =
//=============================================================================

template <class SERVICE_IF>
class ServiceExport : public ServiceExportBase
{
public:
	ServiceExport(const char *name, Service<SERVICE_IF> *service);
	ServiceExport(const char *name, Object *owner);
	virtual ~ServiceExport();

	inline bool IsConnected() const ALWAYS_INLINE;

	// (import -> export) ==> export
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);
	
	// (export <- import) ==> import
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (export1 -> export2) ==> export2
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);
	
	// (export1 <- export2) ==> export2
	friend ServiceExport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

	virtual void Disconnect();
	virtual void DisconnectClient();

	void Unbind(ServiceImport<SERVICE_IF>& srv_import);

	Service<SERVICE_IF> *ResolveService(Client<SERVICE_IF> *client);
	ServiceExport<SERVICE_IF> *ResolveServiceExport();

	virtual void Dump(std::ostream& os) const;

	virtual Object *GetClient() const;
	virtual Object *GetService() const;
	
	virtual std::list<ServiceImportBase *>& GetSetupDependencies();

private:
	std::list<ServiceExport<SERVICE_IF> *> alias_exports;
	ServiceExport<SERVICE_IF> *actual_export;
	Service<SERVICE_IF> *service;
	std::list<ServiceImport<SERVICE_IF> *> srv_imports;
	Client<SERVICE_IF> *client;

	void UnresolveClient();
	void Bind(ServiceImport<SERVICE_IF>& srv_import);
	void Bind(ServiceExport<SERVICE_IF>& alias_export);
	void ResolveClient();
};

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>::ServiceExport(const char *_name, Service<SERVICE_IF> *_service) :
	ServiceExportBase(_name, _service),
	alias_exports(),
	actual_export(0),
	service(_service),
	srv_imports(),
	client(0)
{
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>::ServiceExport(const char *_name, Object *_owner) :
	ServiceExportBase(_name, _owner),
	alias_exports(),
	actual_export(0),
	service(0),
	srv_imports(),
	client(0)
{
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>::~ServiceExport()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".~ServiceExport()" << std::endl;
#endif
	//ServiceExport<SERVICE_IF>::DisconnectClient();
	ServiceExport<SERVICE_IF>::Disconnect();
}

template <class SERVICE_IF>
inline bool ServiceExport<SERVICE_IF>::IsConnected() const
{
	return (client != 0);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& srv_import)
{
	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			std::cerr << "WARNING! Can't connect again " << GetName() << " to " << srv_import.GetName() << " because it is already connected" << std::endl;
			return;
		}
	}

	srv_imports.push_back(&srv_import);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& alias_export)
{
	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		if(*export_iter == &alias_export)
		{
			std::cerr << "WARNING! Can't connect again " << GetName() << " to " << alias_export.GetName() << " because it is already" << std::endl;
			return;
		}
	}

	if(alias_export.actual_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_export.GetName() << " because " << alias_export.GetName() << " is already connected to " << alias_export.actual_export->GetName() << std::endl;
		return;
	}

	alias_exports.push_back(&alias_export);
	alias_export.actual_export = this;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Unbind(ServiceImport<SERVICE_IF>& srv_import)
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".Unbind(" << srv_import.GetName() << ")" << std::endl;
#endif

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			srv_imports.erase(import_iter);
			UnresolveClient();
			return;
		}
	}

	std::cerr << "WARNING! Unable to disconnect " << GetName() << " and " << srv_import.GetName() << " because connection does not exist" << std::endl;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::DisconnectClient()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".DisconnectClient()" << std::endl;
#endif

	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->DisconnectClient();
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	if(!srv_imports.empty())
	{
		UnresolveClient();

		for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
		{
			(*import_iter)->Unbind(*this);
		}
	
		srv_imports.clear();
	}
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Disconnect()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".Disconnect()" << std::endl;
#endif

	DisconnectClient();
	
	if(actual_export)
	{
		typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;
	
		for(export_iter = actual_export->alias_exports.begin(); export_iter != actual_export->alias_exports.end(); export_iter++)
		{
			if(*export_iter == this)
			{
				actual_export->alias_exports.erase(export_iter);
				break;
			}
		}
		actual_export = 0;
	}

	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->actual_export = 0;
	}

	alias_exports.clear();
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::ResolveClient()
{
	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->ResolveClient();
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		(*import_iter)->ResolveClient();
	}
}

template <class SERVICE_IF>
Service<SERVICE_IF> *ServiceExport<SERVICE_IF>::ResolveService(Client<SERVICE_IF> *_client)
{
	if(actual_export)
	{
		return (actual_export->ResolveService(_client));
	}

	if(service)
	{
		client = _client;
		return (service);
	}
	return (0);
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceExport<SERVICE_IF>::ResolveServiceExport()
{
	if(actual_export)
	{
		return (actual_export->ResolveServiceExport());
	}

	return (this);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::UnresolveClient()
{
#ifdef DEBUG_KERNEL
	std::cerr << GetName() << ".UnresolveClient()" << std::endl;
#endif

	if(actual_export)
	{
		client = 0;
		return (actual_export->UnresolveClient());
	}

	if(client)
	{
		//client->OnDisconnect(); // Gilles: that's dangerous
#ifdef DEBUG_KERNEL
		std::cerr << GetName() << ": Unresolving client " << client->GetName() << std::endl;
#endif
		client = 0;
	}
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Dump(std::ostream& os) const
{
	if(actual_export)
	{
		std::cerr << GetName() << " -> " << actual_export->GetName() << std::endl;
	}

	typename std::list<ServiceExport<SERVICE_IF> *>::const_iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		os << "# " << (*export_iter)->GetName() << " -> " << GetName() << std::endl;
	}
}

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetClient() const
{
	return (client);
}

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetService() const
{
	return (service);
}

template <class SERVICE_IF>
std::list<ServiceImportBase *>& ServiceExport<SERVICE_IF>::GetSetupDependencies()
{
	return actual_export ? actual_export->GetSetupDependencies() : ServiceExportBase::GetSetupDependencies();
}

//=============================================================================
//=                                Operators                                  =
//=============================================================================

// (import -> export) ==> export
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	rhs.Bind(lhs);
	lhs.ResolveClient();
	return (rhs);
}

// (export <- import) ==> import
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return (rhs);
}

// (import1 -> import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	lhs.ResolveClient();
	return (rhs);
}

// (import1 <- import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	rhs.ResolveClient();
	return (rhs);
}

// (export1 -> export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.ResolveClient();
	return (rhs);
}

// (export1 <- export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return (rhs);
}

} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_SERVICE_SERVICE_HH__
