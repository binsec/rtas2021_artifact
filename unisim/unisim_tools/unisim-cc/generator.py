import sys
import os
import shutil
import filecmp

def getModules(simulator, g_system):
	# copy the modules sources
	modules = []
	if len(simulator.modules_instances) != 0:
		for modinst in simulator.modules_instances:
			if modinst.instanceOf not in g_system.module_db:
				print "ERROR: module instance not found in the module database"
				print "%s" % (modinst.toString(""),)
				sys.exit(-1)
			modules.append(modinst.instanceOf)
	finished = False
	while not finished:
		# TODO check for submodules
		finished = True
	return modules

def getLibraries(modules, g_system):
	# copy the libraries sources
	libs = []
	for module in modules:
		for lib in g_system.module_db[module].libraries:
			if lib.name not in g_system.lib_db:
				print "ERROR: library not found in the library database"
				print "%s" % (lib.name,)
				sys.exit(-1)
			if lib.name not in libs:
				libs.append(lib.name)
	finished = False
	while not finished:
		finished = True
		temp_libs = libs
		for lib in libs:
			if len(g_system.lib_db[lib].deps) != 0:
				for dep in g_system.lib_db[lib].deps:
					if dep.name not in g_system.lib_db:
						print "ERROR: library not found in the library database"
						print "%s" % (dep.name,)
						sys.exit(-1)
					if dep.name not in temp_libs:
						temp_libs.append(dep.name)
						finished = False
		libs = temp_libs
	return libs

def copySources(modules, libs, g_system):
	sources = []
	for module in modules:
		sources.extend(g_system.module_db[module].sources)
	for lib in libs:
		sources.extend(g_system.lib_db[lib].sources)

	for src in sources:
		filename = src.filename
		absfilename = g_system.configuration.findFile(filename)
		dir = './' + os.path.dirname(filename)
		if not os.path.isdir(dir):
			os.makedirs(dir)
		copy = False
		if os.path.exists(filename):
			if not filecmp.cmp(absfilename, filename):
				copy = True
		else:
			copy = True
		if copy:
			shutil.copyfile(absfilename, filename)

def generateModuleInstantiation(modinst, g_system):
	str = ""
	module = g_system.module_db[modinst.instanceOf]
	str += module.name
	# get the templates and sort them
	temp = {}
	for k, param in module.parameters.iteritems():
		if param.use == "Template":
			temp[param.order] = param
	templates = []
	while len(temp) != 0:
		first = True
		min = 0
		for k,v in temp.iteritems():
			if first:
				first = False
				min = k
			else:
				if k < min:
					min = k
		templates.append(temp.pop(min))
	# now templates is an ordered list of the template options
	if len(templates) != 0:
		str += "<"
		first = True
		for val in templates:
			if first:
				first = False
			else:
				str += ", "
			str += "/*%s*/" % (val.name,)
			if val.name in modinst.parameters:
				str += "%s" % (modinst.parameters[val.name],)
			else:
				str += "%s" % (val.default_value,)
		str += "> "
	str += "%s(\"%s\");" % (modinst.name, modinst.name,)
	return str

def generateModuleRuntimeParameters(pre, modinst, g_system):
	str = ""
	module = g_system.module_db[modinst.instanceOf]
	str += "%s// %s runtime parameters\n" % (pre, modinst.name)
	# get the runtime parameters
	params = []
	for k, param in module.parameters.iteritems():
		if param.use == "Runtime":
			params.append(param)
	# now create the code for each of the parameters
	for param in params:
		value = param.default_value
		if param.name in modinst.parameters:
			value = modinst.parameters[param.name]
		str += "%s%s[\"%s\"] = %s;\n" % (pre, modinst.name, param.name, value,)
	return str

def generateLink(link, g_system):
	str = ""
	str += "%s.%s(%s.%s);" % (link.source_module, link.source_socket, link.target_module, link.target_socket,)
	return str

def createMain(simulator, g_system):
	f = open("main.cc", "w")
	# we must include systemc because we are going to call sc_start
	str  = "#include <systemc.h>\n"
	#	# we must include the unisim::kernel::service if it is used by the application
	#	if "unisim::kernel::service" in g_system.lib_db:
	#		str += "#include \"unisim/kernel/kernel.hh\"\n"
	# check files that need to be included from the simulator instantiations
	modules = []
	if len(simulator.modules_instances) != 0:
		for modinst in simulator.modules_instances:
			modules.append(modinst.instanceOf)
	sources = []
	for module in modules:
		sources.extend(g_system.module_db[module].sources)
	filenames = []
	for source in sources:
		if source.nature == "Implementation" or source.nature == "Interface":
			filenames.append(source.filename)
	for filename in filenames:
		str += "#include \"%s\"\n" % (filename,)
	f.write(str)
	str  = "\n\n"
	str += "int sc_main(int argv, char** argc)\n"
	str += "{\n"
	# instantiate the components
	if len(simulator.modules_instances) != 0:
		for modinst in simulator.modules_instances:
			str += "\t"
			str += generateModuleInstantiation(modinst, g_system)
			str += "\n"
	f.write(str)
	# runtime parametrization of the components
	str = "\n"
	if len(simulator.modules_instances) != 0:
		for modinst in simulator.modules_instances:
			str += generateModuleRuntimeParameters("\t", modinst, g_system)
			str += "\n"
	f.write(str)
	# connect the components
	str  = "\n"
	str += "\t/* netlist start */\n"
	if len(simulator.netlist) != 0:
		for link in simulator.netlist: 
			str += "\t"
			str += generateLink(link, g_system)
			str += "\n"
	str += "\t/* netlist end */\n"
	f.write(str)
	# launch the setup if necessary
	if "unisim::kernel::service" in g_system.lib_db:
		str  = "\n"
		str += "\t*(unisim::kernel::ServiceManager::GetParameter(\"kernel_logger.std_out\")) = true;\n"
		str += "\t*(unisim::kernel::ServiceManager::GetParameter(\"kernel_logger.std_out_color\")) = true;\n"
		str += "\n"
		str += "\t/* launch initial setup start */\n"
		str += "\tif(!unisim::kernel::ServiceManager::Setup())\n"
		str += "\t\treturn -1;\n"
		str += "\t/* launch initial setup end */\n"
		f.write(str)
	# launch the simulation
	str  = "\n"
	str += "\t/* launch the simulation */\n"
	str += "\tsc_start();\n"
	str += "\n"
	str += "\treturn 0;\n"
	str += "}\n"
	f.write(str)
	f.close()

def createSimm4(libs, g_system):
	f = open("./m4/sim.m4", "w")
	for lib in libs:
		f.write("## %s\n" % (lib,))
		f.write(g_system.lib_db[lib].m4)
		f.write("\n")
		f.write("\n")
	f.close()

def createConfigure(simulator, libs, g_system):
	f = open("./configure.ac", "w")
	f.write("AC_INIT([%s], [%s],[%s <%s>], [%s])\n" % (simulator.name, simulator.version, simulator.author, simulator.contact, simulator.name,))
	f.write("AC_CONFIG_AUX_DIR(config)\n")
	f.write("AC_CONFIG_HEADERS([config.h])\n")
	f.write("AC_CANONICAL_BUILD\n")
	f.write("AC_CANONICAL_HOST\n")
	f.write("AC_CANONICAL_TARGET\n\n")
	f.write("AM_INIT_AUTOMAKE\n\n")
	f.write("AC_PATH_PROGS(SH, sh)\n")
	f.write("AC_PROG_CXX\n")
	f.write("AC_PROG_INSTALL\n")
	f.write("AC_PROG_RANLIB\n")
	f.write("AC_PROG_LN_S\n\n")
	f.write("AC_LANG([C++])\n\n")

	for lib in libs:
		f.write("## %s\n" % (lib,))
		f.write(g_system.lib_db[lib].configure)
		f.write("\n")
	
	f.write("# files to generate\n")
	f.write("#   this should include all the makefiles to generate\n")
	f.write("AC_CONFIG_FILES([Makefile])\n\n")

	f.write("# launch the generation of makefiles\n")
	f.write("#   this must be the last command of configure.ac\n")
	f.write("AC_OUTPUT\n")
	
	f.close()

def createMakefile(simulator, modules, libs, g_system):
	f = open("./Makefile.am", "w")
	sources = []
	for module in modules:
		sources.extend(g_system.module_db[module].sources)
	for lib in libs:
		sources.extend(g_system.lib_db[lib].sources)

	f.write("bin_PROGRAMS = %s\n" % (simulator.name,))
	f.write("%s_SOURCES = \\\n" % (simulator.name,))
	for src in sources:
		f.write("\t%s\\\n" % (src.filename,))
	f.write("\tmain.cc\n")
	f.close()

def createCompilation(simulator, modules, libs, g_system):
	m4dir = "./m4"
	if not os.path.isdir(m4dir):
		os.makedirs(m4dir)
	createSimm4(libs, g_system)
	createConfigure(simulator, libs, g_system)
	createMakefile(simulator, modules, libs, g_system)

def generateCode(simulator, g_system):
	modules = getModules(simulator, g_system)
	libs = getLibraries(modules, g_system)

	if g_system.verbose:
		str = "List of modules to copy sources:"
		for module in modules:
			str += " '%s'" % (module,)
		print "%s" % (str,)
		str = "List of libraries to copy sources:"
		for lib in libs:
			str += " '%s'" % (lib,)
		print "%s" % (str,)

	copySources(modules, libs, g_system)
	createMain(simulator, g_system)
	createCompilation(simulator, modules, libs, g_system)

