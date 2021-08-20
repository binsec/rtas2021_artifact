import sys
import os.path
from xml.dom import minidom

class Configuration:
	def __init__(self, filename = 0):
		self.config_file = filename
		if self.config_file:
			self.parseConfigFile()
		else:
			self.defaultConfig()

	def defaultConfig(self):
		self.dirs = []
		self.configure_ac = 'configure.ac'
		self.makefile_am = 'makefile.am'

	def parseConfigFile(self):
		self.defaultConfig()
		f = open(self.config_file, 'r')
		dom = minidom.parse(f)
		reflist = dom.getElementsByTagName('config')
		config = reflist[0]
		reflist = config.getElementsByTagName('directories')
		if len(reflist) != 0:
			dirs = reflist[0]
			reflist = dirs.getElementsByTagName('name')
			if len(reflist) != 0:
				for dir in reflist:
					dirname = self.getText(dir.childNodes)
					dirname = os.path.expanduser(dirname)
					if os.path.isdir(dirname):
						dirname = os.path.abspath(dirname)
						self.dirs.append(dirname)
		reflist = config.getElementsByTagName('output')
		if len(reflist) != 0:
			output = reflist[0]
			reflist = output.getElementsByTagName('configure_ac')
			if len(reflist) != 0:
				self.configure_ac = self.getText(reflist[0].childNodes)
			reflist = output.getElementsByTagName('makefile_am')
			if len(reflist) != 0:
				self.makefile_am = self.getText(reflist[0].childNodes)

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def extendPath(self, file):
		l = []
		for dir in self.dirs:
			l.append(dir + "/" + file)
		return l

	def findFile(self, file):
		possibilities = self.extendPath(file)
		found = ""
		for i in possibilities:
			if not found:
				if os.path.isfile(i):
					found = os.path.abspath(i)
			else:
				if os.path.isfile(i) and found != os.path.abspath(i):
					print "Warning: %s found in two different locations (\n%s, \n%s)" % (file, found, os.path.abspath(i),)
		if not found:
			print "Error: %s not found" % (file,)
			sys.exit(-1)
		return found
			

	def toString(self):
		if self.config_file:
			str = "Configuration file: " + self.config_file + "\n"
		else:
			str = "No configuration file\n"
		if len(self.dirs) == 0:
			str += "  - no search directory\n"
		else:
			str += "  - list of search directories:\n"
			for dir in self.dirs:
				str += "    " + dir + "\n"
		str += "  - configure.ac filename: %s\n" % (self.configure_ac,)
		str += "  - makefile.am filename: %s\n" % (self.makefile_am,)
		return str
