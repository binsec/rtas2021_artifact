# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET

class ConfigClass:
	"""The configuration class that will be used populate the repositories"""
	def __init__(self, filename, do_link):
		self._status = True
		# self._data = []
		self._data = dict()
		config_tree = ET.parse(filename)
		root = config_tree.getroot()
		entries = root.findall('entry')
		for entry in entries:
			name_element = entry.find('name')
			location_element = entry.find('location')
			self._add(name_element.text, location_element.text)
		self._do_link = do_link

	def doLink(self):
		  return self._do_link;
		  
	def getStatus(self):
		return self._status
	
	def getRepositories(self):
		return self._data.items()
		# return self._data

	def getRepositoriesNames(self):
		return self._data.keys()
		# names = []
		# for entry in self._data:
		#	names.append(entry['name'])
		# return names

	def getRepositoryLocationFromName(self, name):
		if name in self._data.keys():
			return self._data[name]
		return None
		# for entry in self._data:
		#	if re.match(entry['name'], name):
		#		return entry['location']

	def dump(self, pre):
		for (name, location) in self._data.items():
			print('%s%s -> %s' % (pre, name, location))
		if self._do_link:
			print('%sOperation mode: creating link to files' % (pre,))
		else:
			print('%sOperation mode: copying files' % (pre,))
		# for entry in self._data:
		#	print('%s%s -> %s' % (pre, entry['name'], entry['location'],))
					
	def _add(self, name, location):
		self._data[name] = location
		# self._data.append({'name': name, 'location': location})

