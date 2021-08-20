# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
import sys
import os
import os.path
import shutil

class Builder:
	def __init__(self, root, repository):
		self._root = root
		self._units = dict()
		self._status = self._scanDependency(root, repository)
			

	def getStatus(self):
		return self._status
		
	def _scanDependency(self, dependency, repository):
		if dependency in self._units.keys():
			return True
		unit = repository.getUnit(dependency)
		if unit is None:
			print('\nERROR: could not find unit "%s"' % (dependency))
			return False
		self._units[dependency] = unit
		for dep in unit.getDependencies():
			# print("---> %s" % (unit.getName()))
			if not self._scanDependency(dep, repository):
				return False
		return True

	def generate(self, config, output_path):
		# copy all the files
		self._generateCopyFiles(config, output_path)
		if not self._status:
			return
		# generate the cmake
		# self._generateCmake(config, output_path)
		self._generateCopyCMakes(config, output_path)
		if not self._status:
			return

	def _generateCopyFiles(self, config, output_path):
		for unit in self._units.values():
			rep_location = config.getRepositoryLocationFromName(unit.getRepositoryName())
			src_path = unit.getFilesPath()
			rep_location = os.path.join(rep_location, src_path)
			for file in unit.getFiles():
				abs_filename = os.path.join(rep_location, file)
				abs_out_filename = os.path.join(output_path, file)
				abs_out_path = os.path.dirname(abs_out_filename)
				if not os.path.exists(abs_out_path):
					os.makedirs(abs_out_path)
				if not os.path.exists(abs_filename):
					print("ERROR: '%s' does not exist" % (abs_filename))
					self._status = False
					return
				if config.doLink():
					os.symlink(abs_filename, abs_out_filename)
				else:
					shutil.copyfile(abs_filename, abs_out_filename)
		return
	
	def _generateCopyCMakes(self, config, output_path):
		for unit in self._units.values():
			rep_location = config.getRepositoryLocationFromName(unit.getRepositoryName())
			rel_filename = unit.getCmake()
			if rel_filename is None:
				# print ( '+ %s (no cmake defined)' % (unit.getName()) )
				rel_filename = None
			else:
				print ( '- %s (cmake defined)' % (unit.getName()) )
				abs_filename = os.path.join(rep_location, rel_filename)
				if not os.path.exists(abs_filename):
					print ('WARNING: %s (cmake file does not exists)' % (unit.getName()))
				else:
					abs_out_filename = os.path.join(output_path, rel_filename)
					if not os.path.exists(os.path.dirname(abs_out_filename)):
						os.makedirs(os.path.dirname(abs_out_filename))
					if config.doLink():
						os.symlink(abs_filename, abs_out_filename)
					else:
						shutil.copyfile(abs_filename, abs_out_filename)

	def _generateCmake(self, config, output_path):
		# copy cmake header
		cmake_path = os.path.abspath(os.path.dirname(sys.argv[0]))
		cmake_filename = os.path.join(cmake_path, "CMakeList.txt.header")
		out_cmake_filename = os.path.join(output_path, "CMakeLists.txt")
		shutil.copyfile(cmake_filename, out_cmake_filename)
		out_cmake_file = open(out_cmake_filename, 'a')
		sorted_units = self._sortUnits()
		for unit_name in sorted_units:
			self._generateCMakeFromUnit(self._units[unit_name], config, out_cmake_file)
		if self._cmake is not None:
			abs_path = os.path.dirname(self._filename)
			abs_filename = os.path.join(abs_path, self._cmake)
			inf = open(abs_filename, 'r')
			for line in inf.readlines():
				out_cmake_file.write(line)
			inf.close()
		out_cmake_file.close()
		cmake_filename = os.path.join(cmake_path, "CMakeList.txt.tail")
		self._appendFileToFile(out_cmake_filename, cmake_filename)
		return

	def _sortUnits(self):
		sorted_units = []
		for (name, unit) in self._units.items():
			sorted_units = self._sortUnitsRec(name, sorted_units)
		return sorted_units

	def _sortUnitsRec(self, name, sorted_units):
		if name in sorted_units:
			return sorted_units
		for dep in self._units[name].getDependencies():
			sorted_units = self._sortUnitsRec(dep, sorted_units)
		pos = 0
		for dep in self._units[name].getDependencies():
			possible_pos = sorted_units.index(dep)
			if possible_pos > pos:
				pos = possible_pos
		if pos is 0:
			sorted_units.insert(0, name)
		else:
			sorted_units.insert(pos + 1, name)
		return sorted_units
			
	def _generateCMakeFromUnit(self, unit, config, out_cmake_file):
		print ("+ %s" % (unit.getName()), end = '')
		base_path = config.getRepositoryLocationFromName(unit.getRepositoryName())
		rel_path = unit.getCmake()
		if rel_path is None:
			print (' -')
			return
		abs_path = os.path.join(base_path, rel_path)
		if not os.path.exists(abs_path):
			print (' *')
			return
		inf = open(abs_path, 'r')
		for line in inf.readlines():
			out_cmake_file.write(line)
		inf.close()
		print (' +')
		return 

	def _appendFileToFile(self, filename, in_filename):
		rfile = open(in_filename, 'r')
		file = open(filename, 'a')
		for line in rfile.readlines():
			file.write(line)
		rfile.close()
		file.close()

	def dump(self, pre):
		print("%sRoot: %s" % (pre, self._root))
		print("%sUnits:" % (pre))
		for unit in self._units.keys():
			print("%s  %s" % (pre, unit))

