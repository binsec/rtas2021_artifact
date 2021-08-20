# -*- coding: utf-8 -*-

import os
import os.path
import re
import puscomp.unit

class RepositoryClass:
	"""This class keeps track of all the components available in the
	   physical repositories given through the configuration class."""

	def __init__(self, configuration):
		self._status = True
		self._config = configuration
		self._units = dict()
		for (name, location) in self._config.getRepositories():
			print ('%s -> %s' % (name, location))
			self.addPhysicalRepository(name, location)
		# for entry in self._config.getRepositories():
		# 	self.addPhysicalRepository(entry['name'], entry['location'])
	
	def getStatus(self):
		return self._status

	def addPhysicalRepository(self, pr_name, pr_location):
		self._scanPhysicalRepository(pr_name, pr_location)

	def getUnit(self, unit_name):
		if unit_name not in self._units.keys():
			return None
		return self._units[unit_name]

	def dump(self, pre):
		new_pre = ""
		i = 0
		while i < len(pre):
			new_pre = new_pre + " "
			i = i + 1
		new_pre = new_pre + " - "
		for (key, unit) in self._units.items():
			print('%sUnit "%s"' % (pre, key))
			unit.dump(new_pre)

	def _scanPhysicalRepository(self, pr_name, pr_location):
		file_list = self._findUnitDescriptions(pr_location)
		for file in file_list:
			units = puscomp.unit.getUnitsFromFile(file, pr_name)
			for unit in units:
				if unit.getName() not in self._units:
					self._units[unit.getName()] = unit

	def _findUnitDescriptions(self, location):
		unit_list = []
		contents = os.listdir(location)
		for entry in contents:
			path = os.path.join(location, entry)
			if os.path.isdir(path):
				if not re.match('\.', entry):
					unit_list.extend(self._findUnitDescriptions(path))
			else:
				if re.search('\.uni$', entry):
					unit_list.append(path)
		return unit_list


