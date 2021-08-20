import os.path
import sys
from xml.dom import minidom

class Simulator:
	def __init__(self, _filename, g_system):
		self.filename = os.path.expanduser(_filename)
		self.filename = os.path.abspath(self.filename)
		f = open(self.filename, 'r')
		dom = minidom.parse(f)
		reflist = dom.getElementsByTagName('Simulator')
		sim = reflist[0]
		# get the fileset 
		self.parseFileSet(sim, g_system)
		# get the modules instances
		self.parseModulesInstances(sim, g_system)
		# get the netlist
		self.parseNetList(sim, g_system)
		# get the name, version, author and author email of the simulator
		if not sim.hasAttribute('name'):
			print "ERROR: simulator name missing"
			print sim.toxml()
			sys.exit(-1)
		if not sim.hasAttribute('version'):
			print "ERROR: simulator version missing"
			print sim.toxml()
			sys.exit(-1)
		if not sim.hasAttribute('author'):
			print "ERROR: simulator author missing"
			print sim.toxml()
			sys.exit(-1)
		if not sim.hasAttribute('contact'):
			print "ERROR: simulator author contact missing"
			print sim.toxml()
			sys.exit(-1)
		self.name = sim.getAttribute('name')
		self.version = sim.getAttribute('version')
		self.author = sim.getAttribute('author')
		self.contact = sim.getAttribute('contact')

	def parseFileSet(self, sim, g_system):
		reflist = sim.getElementsByTagName('FileSet')
		self.file_set = []
		if len(reflist) != 0:
			file_set = reflist[0]
			reflist = file_set.getElementsByTagName('File')
			for file in reflist:
				filename = self.getText(file.childNodes)
				self.file_set.append(g_system.configuration.findFile(filename))
		# for file in self.file_set:
		# 	if file not in g_system.file_db:
		#		g_system.file_db.append(file)

	def parseModulesInstances(self, sim, g_system):
		reflist = sim.getElementsByTagName('ModulesInstances')
		self.modules_instances = []
		if len(reflist) != 0:
			m_i = reflist[0]
			reflist = m_i.getElementsByTagName('Module')
			for module in reflist:
				self.modules_instances.append(ModuleInstance(module, g_system))
	
	def parseNetList(self, sim, g_system):
		reflist = sim.getElementsByTagName('NetList')
		self.netlist = []
		if len(reflist) != 0:
			reflist = reflist[0].getElementsByTagName('Link')
			for link in reflist:
				self.netlist.append(Link(link, g_system))
	
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%sSimulator description:\n" % (pre,)
		str += "%s- filename: %s\n" % (pre, self.filename,)
		str += "%s- fileset: \n" % (pre, )
		for filename in self.file_set:
			str += "%s  - %s\n" % (pre, filename,)
		if len(self.modules_instances) != 0:
			str += "%s- modules instances:\n" % (pre,)
		for modinst in self.modules_instances:
			str += modinst.toString("%s  " % (pre,))
		if len(self.netlist) != 0:
			str += "%s- netlist:\n" % (pre,)
		for link in self.netlist:
			str += link.toString("%s  " % (pre,))
		return str

class ModuleInstance:
	def __init__(self, _modinst_dom, g_system):
		# get the module name and its instanceOf
		if not _modinst_dom.hasAttribute('name'):
			print "ERROR: found module with no 'name' attribute"
			print _modinst_dom.toxml()
			sys.exit(-1)
		if not _modinst_dom.hasAttribute('instanceOf'):
			print "ERROR: found module with no 'instanceOf' attribute"
			print _modinst_dom.toxml()
			sys.exit(-1)
		self.name = _modinst_dom.getAttribute('name')
		self.instanceOf = _modinst_dom.getAttribute('instanceOf')
		# get the module parameters
		self.parameters = {}
		reflist = _modinst_dom.getElementsByTagName('Parameter')
		for param in reflist:
			if not param.hasAttribute('name'):
				print "ERROR: attribute 'name' not found for parameter"
				print _modinst_dom.toxml()
			param_name = param.getAttribute('name')
			param_value = self.getText(param.childNodes)
			self.parameters[param_name] = param_value
	
	def toString(self, pre):
		str = "%s- module name='%s' instanceOf='%s'" % (pre, self.name, self.instanceOf,)
		if len(self.parameters) != 0:
			str += " parameters:\n"
		else:
			str += "\n"
		for k,v in self.parameters.iteritems():
			str += "%s    %s = %s\n" % (pre, k, v)
		return str
			
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

class Link:
	def __init__(self, _link_dom, g_system):
		reflist = _link_dom.getElementsByTagName('SourceModule')
		if len(reflist) == 0:
			print "ERROR: found link with no 'SourceModule' tag"
			print _link_dom.toxml()
			sys.exit(-1)
		self.source_module = self.getText(reflist[0].childNodes)
		reflist = _link_dom.getElementsByTagName('SourceSocket')
		if len(reflist) == 0:
			print "ERROR: found link with no 'SourceSocket' tag"
			print _link_dom.toxml()
			sys.exit(-1)
		self.source_socket = self.getText(reflist[0].childNodes)
		reflist = _link_dom.getElementsByTagName('DestModule')
		if len(reflist) == 0:
			print "ERROR: found link with no 'DestModule' tag"
			print _link_dom.toxml()
			sys.exit(-1)
		self.target_module = self.getText(reflist[0].childNodes)
		reflist = _link_dom.getElementsByTagName('DestSocket')
		if len(reflist) == 0:
			print "ERROR: found link with no 'DestSocket' tag"
			print _link_dom.toxml()
			sys.exit(-1)
		self.target_socket = self.getText(reflist[0].childNodes)

	def toString(self, pre):
		str = "%s  %s.%s -> %s.%s" % (pre, self.source_module, self.source_socket, self.target_module, self.target_socket,)
		return str

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

