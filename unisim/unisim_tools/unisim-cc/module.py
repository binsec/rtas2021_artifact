import sys
from xml.dom import minidom

class Module:
	def __init__(self, _filename, g_system):
		self.filename = _filename
		f = open(self.filename, 'r')
		dom = minidom.parse(f)
		reflist = dom.getElementsByTagName('Module')
		if len(reflist) == 0:
			print "ERROR: 'Module' tag not found in module description"
			print dom.toxml()
			sys.exit(-1)
		module = reflist[0]
		if not module.hasAttribute('name'):
			print "ERROR: 'name' attribute missing in a module description"
			print dom.toxml()
			sys.exit(-1)
		self.name = module.getAttribute('name')
		# get the module parameters
		self.parseParameters(module, g_system)
		# get the module sockets
		self.parseSockets(module, g_system)
		# get the dependencies
		self.parseDependencies(module, g_system)

	def parseParameters(self, module, g_system):
		reflist = module.getElementsByTagName('Parameters')
		self.parameters = {}
		if len(reflist) == 0:
			return
		parameters = reflist[0]
		reflist = parameters.getElementsByTagName('Parameter')
		if len(reflist) == 0:
			return
		for parameter in reflist:
			param = Parameter(parameter, g_system)
			self.parameters[param.name] = param

	def parseSockets(self, module, g_system):
		reflist = module.getElementsByTagName('Sockets')
		self.sockets = {}
		if len(reflist) == 0:
			return
		sockets = reflist[0]
		reflist = sockets.getElementsByTagName('Socket')
		if len(reflist) == 0:
			return
		for socket in reflist:
			sock = Socket(socket, g_system)
			self.sockets[sock.name] = sock
	
	def parseDependencies(self, module, g_system):
		reflist = module.getElementsByTagName('Dependencies')
		self.sources = []
		self.libraries = []
		self.submodules = []
		if len(reflist) == 0:
			return
		dependencies = reflist[0]
		# getting the sources
		reflist = dependencies.getElementsByTagName('Sources')
		if len(reflist) != 0:
			sources = reflist[0]
			reflist = sources.getElementsByTagName('Source')
			if len(reflist) != 0:
				for source in reflist:
					src = Source(source, g_system)
					self.sources.append(src)
		# getting the libraries
		reflist = dependencies.getElementsByTagName('Libs')
		if len(reflist) != 0:
			libs = reflist[0]
			reflist = libs.getElementsByTagName('Lib')
			if len(reflist) != 0:
				for lib in reflist:
					library = Library(lib, g_system)
					self.libraries.append(library)
		# TODO: getting submodules
	
	def toString(self, pre):
		str  = "%sModule '%s':\n" % (pre, self.name,)
		str += "%s- filename: %s" % (pre, self.filename,)
		if len(self.parameters) != 0:
			str += "\n"
			str += "%s- parameters:" % (pre,)
			for k,v in self.parameters.iteritems():
				str += "\n"
				str += v.toString("%s  " % (pre,))
		if len(self.sockets) != 0:
			str += "\n"
			str += "%s- sockets:" % (pre,)
			for k,v in self.sockets.iteritems():
				str += "\n"
				str += v.toString("%s  " % (pre,))
		if len(self.sources) != 0:
			str += "\n"
			str += "%s- sources:" % (pre,)
			for source in self.sources:
				str += "\n"
				str += source.toString("%s  " % (pre,))
		if len(self.libraries) != 0:
			str += "\n"
			str += "%s- libraries:" % (pre,)
			for lib in self.libraries:
				str += "\n"
				str += lib.toString("%s  " % (pre,))
		return str

class Parameter:
	def __init__(self, _parameter, g_system):
		if not _parameter.hasAttribute('name'):
			print "ERROR: parameter 'name' tag missing in a parameter description"
			print _parameter.toxml()
			sys.exit(-1)
		self.name = _parameter.getAttribute('name')
		if _parameter.hasAttribute('use'):
			self.use = _parameter.getAttribute('use')
		else:
			self.use = "Runtime"
			self.order = 0
		if self.use == "Template": # we require an order number
			if _parameter.hasAttribute('order'):
				self.order = _parameter.getAttribute('order')
			else:
				print "ERROR: template parameter missing the order indicator"
				print _parameter.toxml()
				sys.exit(-1)
		self.default_value = self.getText(_parameter.childNodes)
	
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%s- %s:\n" % (pre, self.name,)
		str += "%s  - use: %s\n" % (pre, self.use,)
		if self.use == "Template":
			str += "%s  - order: %s\n" % (pre, self.order,)
		str += "%s  - default_value = %s" % (pre, self.default_value,)
		return str

class Socket:
	def __init__(self, _socket, g_system):
		if not _socket.hasAttribute('name'):
			print "ERROR: socket 'name' tag missing in a socket description"
			print _socket.toxml()
			sys.exit(-1)
		self.name = _socket.getAttribute('name')
		self.type = self.getText(_socket.childNodes)

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str  = "%s- %s type = %s" % (pre, self.name, self.type,)
		return str

class Source:
	def __init__(self, _source, g_system):
		self.nature = "Other"
		if _source.hasAttribute('nature'):
			self.nature = _source.getAttribute('nature')
		self.filename = self.getText(_source.childNodes)

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%s- %s (%s)" % (pre, self.filename, self.nature,)
		return str

class Library:
	def __init__(self, _lib, g_system):
		self.name = self.getText(_lib.childNodes)
	
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%s- %s" % (pre, self.name,) 
		return str

