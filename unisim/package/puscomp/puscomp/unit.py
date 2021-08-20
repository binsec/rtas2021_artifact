# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET

def getUnitsFromFile(filename, rep_name):
	unit_list = []
	print("parsing package file: %s" % (filename))
	xml = ET.parse(filename)
	root = xml.getroot()
	unit_entries = root.findall('unit')
	for unit_entry in unit_entries:
		unit = UnitClass(unit_entry, filename, rep_name)
		unit_list.append(unit)
	return unit_list
	

class UnitClass:
	def __init__(self, xml_unit_entry, location, rep_name):
		# set the location and the repository of the unit
		self._location = location
		self._repository_name = rep_name
		# set the unit name
		name_entry = xml_unit_entry.find('./name')
		self._name = name_entry.text
		# set the unit files path in the repository
		file_path = xml_unit_entry.find('./path')
		if file_path is not None:
			self._path = file_path.text
		else:
			self._path = ''
		# set the unit files
		self._files = []
		files = xml_unit_entry.findall('./files/file')
		if files:
			for file in files:
				self._files.append(file.text)
		deps = xml_unit_entry.findall("./depends/dep")
		self._deps = []
		if deps:
			for dep in deps:
				self._deps.append(dep.text)
		cmake = xml_unit_entry.findall('./cmake')
		self._cmake = None
		if cmake:
			self._cmake = cmake[0].text

	def getName(self):
		return self._name

	def getDependencies(self):
		return self._deps

	def getFilesPath(self):
		return self._path

	def getFiles(self):
		return self._files
	
	def getCmake(self):
		return self._cmake
	
	def getLocation(self):
		return self._location

	def getRepositoryName(self):
		return self._repository_name

	def dump(self, pre):
		i = 0
		spaces = ''
		while i < len(pre):
			spaces = spaces + ' '
			i = i + 1
		print('%sName: %s' % (pre, self._name))
		print('%sRepository: %s' % (pre, self._repository_name))
		print('%sLocation: %s' % (pre, self._location))
		if self._path is not '':
			print ('%sPath: %s' % (pre, self._path))
		if len(self._files) is not 0:
			print('%sFiles:' % (pre))
			for file in self._files:
				print('%s - %s' % (spaces, file))
		if len(self._deps) is not 0:
			print('%sDependencies:' % (pre))
			for dep in self._deps:
				print('%s - %s' % (spaces, dep))
