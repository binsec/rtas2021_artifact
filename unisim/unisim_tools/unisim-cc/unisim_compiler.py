import sys
import os.path
from xml.dom import minidom

class ConfigReader:
	def __init__(self, filename = 0):
		self.config_file = filename
		if self.config_file:
			self.parseConfigFile()
		else:
			self.defaultConfig()

	def defaultConfig(self):
		self.dirs = ['.']		

	def parseConfigFile(self):
		f = open(self.config_file, 'r')
		dom = minidom.parse(f)
		reflist = dom.getElementsByTagName('config')
		config = reflist[0]
		reflist = config.getElementsByTagName('directories')
		dirs = reflist[0]
		reflist = dirs.getElementsByTagName('name')
		if len(reflist) == 0:
			self.dirs = ['.']
		else:
			self.dirs = []
		for dir in reflist:
			dirname = self.getText(dir.childNodes)
			dirname = os.path.expanduser(dirname)
			if os.path.isdir(dirname):
				dirname = os.path.abspath(dirname)
				self.dirs.append(dirname)

	def getText(self, nodelist):
		rc = ""
		for node in nodelist:
			if node.nodeType == node.TEXT_NODE:
				rc = rc + node.data
		return rc

	def extendPath(self, file):
		l = [file]
		for dir in self.dirs:
			l.append(dir + "/" + file)
		return l

	def toString(self):
		if self.config_file:
			print "Configuration file: " + self.config_file
		else:
			print "No configuration file"
		print "  -search directories:"
		for dir in self.dirs:
			print "    " + dir
