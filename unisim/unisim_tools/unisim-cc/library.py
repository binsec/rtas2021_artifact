import sys
from xml.dom import minidom

class Library:
	def __init__(self, _filename, g_system):
		self.filename = _filename
		f = open(self.filename, 'r')
		dom = minidom.parse(f)
		reflist = dom.getElementsByTagName('Library')
		if len(reflist) == 0:
			print "ERROR: 'Library' tag not found in library description"
			print dom.toxml()
			sys.exit(-1)
		lib = reflist[0]
		if not lib.hasAttribute('name'):
			print "ERROR: 'name' attribute missing in a library description"
			print dom.toxml()
			sys.exit(-1)
		self.name = lib.getAttribute('name')
		# get any library sources
		self.parseSources(lib, g_system)
		# get any library dependencies
		self.parseLibs(lib, g_system)
		# get any M4 section necessary
		self.parseM4(lib, g_system)
		# get any configure section necessary
		self.parseConfigure(lib, g_system)

	def parseSources(self, lib, g_system):
		reflist = lib.getElementsByTagName('Sources')
		self.sources = []
		if len(reflist) != 0:
			sources = reflist[0]
			reflist = sources.getElementsByTagName('Source')
			if len(reflist) != 0:
				for source in reflist:
					src = Source(source, g_system)
					self.sources.append(src)
	
	def parseLibs(self, lib, g_system):
		reflist = lib.getElementsByTagName('Libs')
		self.deps = []
		if len(reflist) != 0:
			deps = reflist[0]
			reflist = deps.getElementsByTagName('Lib')
			if len(reflist) != 0:
				for dep in reflist:
					d = Dependency(dep, g_system)
					self.deps.append(d)

	def parseM4(self, lib, g_system):
		self.m4 = ""
		reflist = lib.getElementsByTagName('M4')
		if len(reflist) != 0:
			self.m4 = self.getText(reflist[0].childNodes)
	
	def parseConfigure(self, lib, g_system):
		self.configure = ""
		reflist = lib.getElementsByTagName('Configure')
		if len(reflist) != 0:
			self.configure = self.getText(reflist[0].childNodes)
	
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str  = "%sLibrary '%s':\n" % (pre, self.name,)
		str += "%s- filename: %s" % (pre, self.filename,)
		if len(self.sources) != 0:
			str += "\n"
			str += "%s- sources:" % (pre,)
			for source in self.sources:
				str += "\n"
				str += source.toString("%s  " % (pre,))
		if len(self.deps) != 0:
			str += "\n"
			str += "%s- dependencies:" % (pre,)
			for dep in self.deps:
				str += "\n"
				str += dep.toString("%s  " % (pre,))
		# if self.m4 != "":
		# 	str += "\n"
		# 	str += "%s- m4 text:" % (pre,)
		# 	str += "%s" % (self.m4)
		return str

class Source:
	def __init__(self, _source, g_system):
		self.filename = self.getText(_source.childNodes)

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%s- %s" % (pre, self.filename,)
		return str

class Dependency:
	def __init__(self, dep, g_system):
			self.name = self.getText(dep.childNodes)
	
	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def toString(self, pre):
		str = "%s- %s" % (pre, self.name,)
		return str

